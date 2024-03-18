#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>

#include "string_c.h"
#include "vector_c.h"
#include "hashtable.h"

// Gets the containing structure of a member
#define container_of(ptr, type, member) ({                  \
    const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
    (type *)( (char *)__mptr - offsetof(type, member) );})

// C Constexpr is supported in GCC 13+ and Clang 19+ (not sure about other compilers)
#if __GNUC__ >= 13 || __clang_major__ >= 19
constexpr size_t k_max_msg = 4096;
constexpr size_t k_max_args = 1024;
#else
enum : size_t {
    k_max_msg = 4096,
    k_max_args = 1024
};

#define constexpr const
#endif

enum {
    STATE_REQ = 0,
    STATE_RES = 1,
    STATE_END = 2 // for deletion
};

enum {
    RES_OK = 0,
    RES_ERR = 1,
    RES_NX = 2
};

typedef struct Conn {
    // file descriptor
    int fd;
    // either STATE_REQ or STATE_RES
    uint32_t state;

    // buffer for reading
    size_t rbuf_size;
    uint8_t rbuf[4 + k_max_msg];

    // buffer for writing
    size_t wbuf_size;
    size_t wbuf_sent;
    uint8_t wbuf[4 + k_max_msg];
} Conn;

// Initialize a new connection
static void conn_init(Conn *conn) {
    conn->fd = -1;
    conn->state = STATE_END;
    conn->rbuf_size = 0;
    conn->wbuf_size = 0;
    conn->wbuf_sent = 0;

    memset(conn->rbuf, 0, sizeof(conn->rbuf)); // initialize rbuf
    memset(conn->wbuf, 0, sizeof(conn->wbuf)); // initialize wbuf
}

static void report_error(const char *msg) {
    perror(msg);
}

static void die(const char *msg) {
    report_error(msg);
    exit(1);
}

static void fd_set_nb(const int fd) {
    errno = 0;
    int flags = fcntl(fd, F_GETFL, 0);
    if (errno) {
        die("fcntl error");
    }

    flags |= O_NONBLOCK;

    errno = 0;
    (void) fcntl(fd, F_SETFL, flags);
    if (errno) {
        die("fcntl error");
    }
}

static void conn_put(ptr_vector *fd2conn, Conn *conn) {
    if (ptr_vector_size(fd2conn) <= (size_t) conn->fd) {
        ptr_vector_resize_expand(fd2conn, conn->fd + 1);
    }
    ptr_vector_set(fd2conn, conn->fd, conn);
}

static int32_t accept_new_conn(ptr_vector *fd2conn, const int fd) {
    // accept
    struct sockaddr_in client_addr = {};
    socklen_t socklen = sizeof(client_addr);
    const int connfd = accept(fd, (struct sockaddr *) &client_addr, &socklen);
    if (connfd < 0) {
        report_error("accept() error");
        return -1; // error
    }

    // set the new connection fd to nonblocking mode
    fd_set_nb(connfd);
    // creating the struct Conn
    Conn *conn = malloc(sizeof(Conn));
    if (conn) {
        conn_init(conn);

        conn->fd = connfd;
        conn->state = STATE_REQ;
        conn_put(fd2conn, conn);
        return 0;
    }
    close(connfd);
    return -1;
}

static void state_req(Conn *conn);

static void state_res(Conn *conn);

static int32_t parse_req(const uint8_t *data, const size_t len, ptr_vector *out) {
    if (len < 4) {
        return -1;
    }
    uint32_t n = 0;
    memcpy(&n, &data[0], 4);
    if (n > k_max_args) {
        return -1;
    }

    size_t pos = 4;
    int j = 0;

    while (n--) {
        if (pos + 4 > len) {
            return -1;
        }
        uint32_t sz = 0;
        memcpy(&sz, &data[pos], 4);
        if (pos + 4 + sz > len) {
            return -1;
        }
        ptr_vector_push_back(out, string_new());

        string_append_cstr_range(ptr_vector_at(out, j++), (char *) &data[pos + 4], sz);

        pos += 4 + sz;
    }

    if (pos != len) {
        for (size_t i = 0; i < ptr_vector_size(out); ++i)
            string_free(ptr_vector_at(out, i));
        return -1; // trailing garbage
    }
    return 0;
}

// The data structure for the key space.
static struct {
    HMap db;
} g_data;

// The structure for the key
typedef struct Entry {
    HNode node;
    string_c *key;
    string_c *value;
} Entry;

// initialize an Entry
static void entry_init(Entry *entry) {
    entry->node = (HNode){};
    entry->key = string_new();
    entry->value = string_new();
}

// free an Entry
static void entry_free(const Entry *entry) {
    string_free(entry->key);
    string_free(entry->value);
}

#if __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-statement-expression-from-macro-expansion"
#elif __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif
static bool entry_eq(const HNode *lhs, const HNode *rhs) {
    return string_compare(container_of(lhs, Entry, node)->key, container_of(rhs, Entry, node)->key);
}

static uint64_t str_hash(const uint8_t *data, const size_t len) {
    uint32_t hash = 0x811C9DC5;
    for (size_t i = 0; i < len; ++i) {
        hash = (hash + data[i]) * 0x01000193;
    }
    return hash;
}

static uint32_t do_get(const ptr_vector *cmd, uint8_t *res, uint32_t *reslen) {
    Entry key;
    entry_init(&key);

    string_swap(key.key, ptr_vector_at(cmd, 1));
    key.node.hcode = str_hash((uint8_t *) key.key->data, string_length(key.key));

    const HNode *node = hm_lookup(&g_data.db, &key.node, (bool (*)(HNode *, HNode *)) &entry_eq);
    if (node) {
        const string_c *val = container_of(node, Entry, node)->value;
        assert(string_length(val) <= k_max_msg);
        memcpy(res, val->data, string_length(val));
        *reslen = (uint32_t) string_length(val);

        entry_free(&key);
        return RES_OK;
    }
    entry_free(&key);
    return RES_NX;
}

static uint32_t do_set(const ptr_vector *cmd, const uint8_t *res, const uint32_t *reslen) {
    (void) res;
    (void) reslen;

    Entry key;
    entry_init(&key);

    string_swap(key.key, ptr_vector_at(cmd, 1));
    key.node.hcode = str_hash((uint8_t *) key.key->data, string_length(key.key));

    const HNode *node = hm_lookup(&g_data.db, &key.node, (bool (*)(HNode *, HNode *)) &entry_eq);
    if (node) {
        string_swap(container_of(node, Entry, node)->value, ptr_vector_at(cmd, 2));
    } else {
        Entry *new_entry = malloc(sizeof(Entry));
        if (new_entry == nullptr) {
            entry_free(&key);
            return RES_ERR;
        }
        new_entry->key = string_new();
        new_entry->value = string_new();

        string_swap(new_entry->key, key.key);
        new_entry->node.hcode = key.node.hcode;
        string_swap(new_entry->value, ptr_vector_at(cmd, 2));
        hm_insert(&g_data.db, &new_entry->node);
    }
    entry_free(&key);
    return RES_OK;
}

static uint32_t do_del(const ptr_vector *cmd, const uint8_t *res, const uint32_t *reslen) {
    (void) res;
    (void) reslen;

    Entry key;
    entry_init(&key);

    string_swap(key.key, ptr_vector_at(cmd, 1));
    key.node.hcode = str_hash((uint8_t *) key.key->data, string_length(key.key));

    const HNode *node = hm_pop(&g_data.db, &key.node, (bool (*)(HNode *, HNode *)) &entry_eq);
    if (node) {
        Entry *entry = container_of(node, Entry, node);
        string_free(entry->key);
        string_free(entry->value);
        free(entry);
    }
    entry_free(&key);
    return RES_OK;
}

#if __clang__
#pragma clang diagnostic pop
#elif __GNUC__
#pragma GCC diagnostic pop
#endif

static bool cmd_is(const string_c *word, const char *cmd) {
    return string_case_compare_cstr(word, cmd);
}

static int32_t do_request(const uint8_t *req, const uint32_t reqlen,
                          uint32_t *rescode, uint8_t *res, uint32_t *reslen) {
    ptr_vector *cmd = ptr_vector_new();

    if (parse_req(req, reqlen, cmd) != 0) {
        report_error("bad request");
        ptr_vector_free(cmd);
        return -1;
    }
    if (ptr_vector_size(cmd) == 2 && cmd_is(ptr_vector_at(cmd, 0), "get")) {
        *rescode = do_get(cmd, res, reslen);
    } else if (ptr_vector_size(cmd) == 3 && cmd_is(ptr_vector_at(cmd, 0), "set")) {
        *rescode = do_set(cmd, res, reslen);
    } else if (ptr_vector_size(cmd) == 2 && cmd_is(ptr_vector_at(cmd, 0), "del")) {
        *rescode = do_del(cmd, res, reslen);
    } else {
        // cmd is not recognized
        *rescode = RES_ERR;
        const char *msg = "Unknown cmd";
        strcpy((char *) res, msg);
        *reslen = strlen(msg);
    }
    for (size_t i = 0; i < ptr_vector_size(cmd); ++i)
        string_free(ptr_vector_at(cmd, i));

    ptr_vector_free(cmd);
    return 0;
}

static bool try_one_request(Conn *conn) {
    // try to parse a request from the buffer
    if (conn->rbuf_size < 4) {
        // not enough data in the buffer. Will retry in the next iteration
        return false;
    }
    uint32_t len = 0;
    memcpy(&len, &conn->rbuf[0], 4);
    if (len > k_max_msg) {
        report_error("too long");
        conn->state = STATE_END;
        return false;
    }
    if (4 + len > conn->rbuf_size) {
        // not enough data in the buffer. Will retry in the next iteration
        return false;
    }

    // got one request, generate the response
    uint32_t rescode = 0;
    uint32_t wlen = 0;
    const int32_t err = do_request(&conn->rbuf[4], len, &rescode, &conn->wbuf[4 + 4], &wlen);

    if (err) {
        conn->state = STATE_END;
        return false;
    }
    wlen += 4;
    memcpy(&conn->wbuf[0], &wlen, 4);
    memcpy(&conn->wbuf[4], &rescode, 4);
    conn->wbuf_size = 4 + wlen;

    // remove the request from the buffer.
    const size_t remain = conn->rbuf_size - 4 - len;
    if (remain) {
        memmove(conn->rbuf, &conn->rbuf[4 + len], remain);
    }
    conn->rbuf_size = remain;

    // change state
    conn->state = STATE_RES;
    state_res(conn);

    // continue the outer loop if the request was fully processed
    return conn->state == STATE_REQ;
}

static bool try_fill_buffer(Conn *conn) {
    // try to fill the buffer
    assert(conn->rbuf_size < sizeof(conn->rbuf));
    ssize_t rv;
    do {
        const size_t cap = sizeof(conn->rbuf) - conn->rbuf_size;
        rv = read(conn->fd, &conn->rbuf[conn->rbuf_size], cap);
    } while (rv < 0 && errno == EINTR);
    if (rv < 0 && errno == EAGAIN) {
        // got EAGAIN, stop.
        return false;
    }
    if (rv < 0) {
        report_error("read() error");
        conn->state = STATE_END;
        return false;
    }
    if (rv == 0) {
        if (conn->rbuf_size > 0) {
            report_error("unexpected EOF");
        } else {
            fputs("EOF\n", stderr);
        }
        conn->state = STATE_END;
        return false;
    }

    conn->rbuf_size += (size_t) rv;
    assert(conn->rbuf_size <= sizeof(conn->rbuf));

    // Try to process requests one by one.
    while (try_one_request(conn)) {
    }
    return conn->state == STATE_REQ;
}

static void state_req(Conn *conn) {
    while (try_fill_buffer(conn)) {
    }
}

static bool try_flush_buffer(Conn *conn) {
    ssize_t rv;
    do {
        const size_t remain = conn->wbuf_size - conn->wbuf_sent;
        rv = write(conn->fd, &conn->wbuf[conn->wbuf_sent], remain);
    } while (rv < 0 && errno == EINTR);
    if (rv < 0 && errno == EAGAIN) {
        // got EAGAIN, stop.
        return false;
    }
    if (rv < 0) {
        report_error("write() error");
        conn->state = STATE_END;
        return false;
    }
    conn->wbuf_sent += (size_t) rv;
    assert(conn->wbuf_sent <= conn->wbuf_size);
    if (conn->wbuf_sent == conn->wbuf_size) {
        // response was fully sent, change state back
        conn->state = STATE_REQ;
        conn->wbuf_sent = 0;
        conn->wbuf_size = 0;
        return false;
    }
    // still got some data in wbuf, could try to write again
    return true;
}

static void state_res(Conn *conn) {
    while (try_flush_buffer(conn)) {
    }
}

static void connection_io(Conn *conn) {
    if (conn->state == STATE_REQ) {
        state_req(conn);
    } else if (conn->state == STATE_RES) {
        state_res(conn);
    } else {
        assert(0); // not expected
    }
}

int main() {
    const int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) die("socket() failure");

    constexpr int val = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    // bind
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(1234);
    addr.sin_addr.s_addr = ntohl(0); // wildcard address 0.0.0.0

    int rv = bind(fd, (const struct sockaddr *) &addr, sizeof(addr));
    if (rv) die("bind() failure");

    // listen
    rv = listen(fd, SOMAXCONN);
    if (rv) die("listen() failure");

    // set the listen fd to nonblocking mode
    fd_set_nb(fd);

    // a map of all client connections, keyed by fd
    ptr_vector *fd2conn = ptr_vector_new();

    vector_c *poll_args = vector_new(sizeof(struct pollfd));

    // event loop
    while (true) {
        // prepare the arguments of the poll()
        vector_clear(poll_args);

        // for convenience, the listening fd is put in the first position
        struct pollfd pfd = {fd, POLLIN, 0};
        vector_push_back(poll_args, &pfd);

        // connection fds
        for (size_t i = 0; i < ptr_vector_size(fd2conn); ++i) {
            const Conn *conn = ptr_vector_at(fd2conn, i);
            if (conn == nullptr) continue;

            struct pollfd pfd2 = {.fd = -1};
            pfd2.fd = conn->fd;
            pfd2.events = conn->state == STATE_REQ ? POLLIN : POLLOUT;
            pfd2.events = pfd2.events | POLLERR;
            vector_push_back(poll_args, &pfd2);
        }

        // poll for active fds
        rv = poll(vector_data(poll_args), vector_size(poll_args), 1000);
        if (rv < 0) die("poll");

        // process active connections
        for (size_t i = 1; i < vector_size(poll_args); ++i) {
            if (((struct pollfd *) vector_at(poll_args, i))->revents) {
                Conn *conn = ptr_vector_at(fd2conn, ((struct pollfd *) vector_at(poll_args, i))->fd);
                if (conn == nullptr) continue;

                connection_io(conn);
                if (conn->state == STATE_END) {
                    // client closed normally, or something bad happened.
                    // destroy this connection
                    ptr_vector_set(fd2conn, conn->fd, nullptr);
                    (void) close(conn->fd);
                    free(conn);
                }
            }
        }

        // try to accept a new connection if the listening fd is active
        if (((struct pollfd *) vector_at(poll_args, 0))->revents)
            (void) accept_new_conn(fd2conn, fd);
    }

    vector_free(poll_args);
    ptr_vector_free(fd2conn);

    return 0;
}
