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

#include "data_structures/string_c.h"
#include "data_structures/vector_c.h"
#include "data_structures/hashtable.h"

/**
 * @brief A macro that gets the containing structure of a member.
 *
 * This macro is used to retrieve the containing structure of a member variable.
 * It's a common idiom in C programming, especially when dealing with data structures
 * and type abstraction.\n
 * It calculates the address of the structure by subtracting
 * the offset of the member within the structure from the address of the member itself.\n
 *
 * This definition was taken from the Linux kernel source code.
 *
 * @param ptr Pointer to the member.
 * @param type Type of the container struct this is embedded in.
 * @param member Name of the member within the struct.
 * @return Pointer to the containing structure.
 */
#define container_of(ptr, type, member) ({                  \
    const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
    (type *)( (char *)__mptr - offsetof(type, member) );})

// C Constexpr is supported in GCC 13+ and Clang 19+ (not sure about other compilers)
#if __GNUC__ >= 13 || __clang_major__ >= 19
constexpr size_t k_max_msg = 4096;  ///< The maximum message size
constexpr size_t k_max_args = 1024; ///< The maximum number of arguments
#else
enum : size_t {
    k_max_msg = 4096, ///< The maximum message size
    k_max_args = 1024 ///< The maximum number of arguments
};

#define constexpr const
#endif

/**
 * @brief Enum representing the state of a connection.
 *
 * This enum is used to track the state of a connection in the server.
 */
enum {
    STATE_REQ = 0, ///< Waiting for a request from the client.
    STATE_RES = 1, ///< Sending a response to the client.
    STATE_END = 2  ///< Connection is closed or needs to be deleted.
};

/**
 * @brief Enum representing the result of a command.
 *
 * This enum is used to represent the result of a command in the server.
 */
enum {
    RES_OK = 0,  ///< The command was successful.
    RES_ERR = 1, ///< There was an error executing the command.
    RES_NX = 2   ///< The command was not executed.
};

/**
 * @brief Structure representing a connection.
 *
 * This structure is used to manage a connection in the server.
 */
struct Conn {
    int fd;                      ///< The file descriptor of the connection.
    uint32_t state;              ///< The current state of the connection.

    size_t rbuf_size;            ///< The size of the read buffer.
    uint8_t rbuf[4 + k_max_msg]; ///< The read buffer, used to store incoming data.

    size_t wbuf_size;            ///< The size of the write buffer.
    size_t wbuf_sent;            ///< The amount of data already sent from the write buffer.
    uint8_t wbuf[4 + k_max_msg]; ///< The write buffer, used to store outgoing data.
};

typedef struct Conn Conn;

/**
 * @brief Initializes a connection structure.
 *
 * This function is used to initialize a connection structure with default values.
 *
 * @param conn Pointer to the connection structure to be initialized.
 */
static void conn_init(Conn *conn) {
    conn->fd = -1;
    conn->state = STATE_END;
    conn->rbuf_size = 0;
    conn->wbuf_size = 0;
    conn->wbuf_sent = 0;

    memset(conn->rbuf, 0, sizeof(conn->rbuf));
    memset(conn->wbuf, 0, sizeof(conn->wbuf));
}

static void report_error(const char *msg) {
    perror(msg);
}

static void die(const char *msg) {
    report_error(msg);
    exit(1);
}

/**
 * @brief Sets a file descriptor to non-blocking mode.
 *
 * @param fd The file descriptor to be set to non-blocking mode.
 */
static void fd_set_nb(const int fd) {
    // get the current flags
    const int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) die("fcntl error");

    // set the non-blocking flag
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
        die("fcntl error");
}

/**
 * @brief Adds a connection to the connection vector.
 *
 * @param fd2conn Pointer to the vector of connections.
 * @param conn Pointer to the connection to be added.
 */
static void conn_put(ptr_vector *fd2conn, Conn *conn) {
    // resize the vector if necessary
    if (ptr_vector_size(fd2conn) <= (size_t) conn->fd) {
        ptr_vector_expand(fd2conn, conn->fd + 1);
    }
    ptr_vector_set(fd2conn, conn->fd, conn);
}

/**
 * @brief Accepts a new connection and adds it to the connection vector.
 *
 * @param fd2conn Pointer to the vector of connections.
 * @param fd The file descriptor of the server socket.
 * @return 0 if the operation was successful, -1 otherwise.
 */
static int32_t accept_new_conn(ptr_vector *fd2conn, const int fd) {
    // Accept a new client connection
    struct sockaddr_in client_addr = {};
    socklen_t socklen = sizeof(client_addr);
    const int connfd = accept(fd, (struct sockaddr *) &client_addr, &socklen);
    if (connfd < 0) {
        report_error("accept() error");
        return -1; // error
    }
    // Set the new connection file descriptor to non-blocking mode
    fd_set_nb(connfd);
    // Create a new connection structure
    Conn *conn = malloc(sizeof(Conn));
    if (conn) {
        // Initialize the connection structure
        conn_init(conn);

        // Set the file descriptor and state of the connection
        conn->fd = connfd;
        conn->state = STATE_REQ;
        // Add the connection to the connection vector
        conn_put(fd2conn, conn);
        return 0;
    }
    // If the connection structure could not be created, close the connection file descriptor
    close(connfd);
    return -1;
}

static void state_req(Conn *conn);

static void state_res(Conn *conn);

/**
 * @brief Parses a request from a client.
 *
 * @param data Pointer to the data to be parsed.
 * @param len The length of the data to be parsed.
 * @param out Pointer to the vector where the parsed arguments will be stored.
 * @return 0 if the operation was successful, -1 otherwise.
 *
 * @note The request is expected to be in a specific format:\n
 * - The first 4 bytes represent the number of arguments in the request.\n
 * - Each argument is represented by 4 bytes for the length of the argument, followed by the argument itself.\n
 *
 * @note If the request is not in the expected format, or if there is trailing data after the last argument, the function will return -1.\n
 * If the request is successfully parsed, the function will return 0 and the arguments will be stored in the output vector.\n
 */
static int32_t parse_req(const uint8_t *data, const size_t len, ptr_vector *out) {
    if (len < 4) return -1;

    // The first 4 bytes represent the number of arguments
    uint32_t n = 0;
    memcpy(&n, &data[0], 4);
    if (n > k_max_args) return -1;

    // Parse each argument
    size_t pos = 4;
    int j = 0;

    while (n--) {
        // Get the length of the argument
        if (pos + 4 > len) return -1;
        uint32_t sz = 0;
        memcpy(&sz, &data[pos], 4);
        if (pos + 4 + sz > len) return -1;

        // Create a new string for the argument and append it to the output vector
        ptr_vector_push_back(out, string_new());
        string_append_cstr_range(ptr_vector_at(out, j++), (char *) &data[pos + 4], sz);
        pos += 4 + sz;
    }
    if (pos != len) {
        // There is trailing data after the last argument
        for (size_t i = 0; i < ptr_vector_size(out); ++i)
            string_free(ptr_vector_at(out, i));
        return -1;
    }
    return 0;
}

/**
 * @brief Global data structure for the server.
 *
 * This structure contains the database (hash map) used by the server to store key-value pairs.\n
 * The database is represented as an instance of the HMap data structure.
 */
static struct {
    HMap db; ///< The hash map used by the server to store key-value pairs.
} g_data;

/**
 * @brief Structure representing an entry in the hash map.
 *
 * This structure is used to manage an entry in the hash map used by the server to store key-value pairs.\n
 * Each entry consists of a node (used by the hash map), a key, and a value.\n
 * The key and value are represented as instances of the \p string_c data structure.
 */
struct Entry {
    HNode node;        ///< The node used by the hash map.
    string_c *key;     ///< The key of the hash map entry.
    string_c *value;   ///< The value of the hash map entry.
};

typedef struct Entry Entry;

/**
 * @brief Initializes an Entry structure.
 *
 * @param entry Pointer to the Entry structure to be initialized.
 */
static void entry_init(Entry *entry) {
    entry->node = (HNode) {};
    entry->key = string_new();
    entry->value = string_new();
}

/**
 * @brief Frees an Entry structure.
 *
 * @param entry Pointer to the Entry structure to be freed.
 */
static void entry_free(const Entry *entry) {
    string_free(entry->key);
    string_free(entry->value);
}

// Ignore the warning about statement expressions in macros (the 'container_of' macro)
#if __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-statement-expression-from-macro-expansion"
#elif __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

/**
 * @brief Compares two hash nodes for equality.
 *
 * @param lhs Pointer to the first hash node.
 * @param rhs Pointer to the second hash node.
 * @return True if the keys of the hash nodes are equal, false otherwise.
 */
static bool entry_eq(const HNode *lhs, const HNode *rhs) {
    return string_compare(container_of(lhs, Entry, node)->key, container_of(rhs, Entry, node)->key);
}


/**
 * @brief Calculates a hash value for a given data using the FNV-1a hash algorithm.
 *
 * FNV-1a has excellent dispersion for short strings and is relatively fast on modern processors.
 *
 * @param data Pointer to the data to be hashed.
 * @param len The length of the data to be hashed.
 * @return The calculated hash value (64-bit).
 */
static uint64_t fnv1a_hash(const uint8_t *data, const size_t len) {
    // FNV offset basis and FNV prime are two constants used in the FNV-1a hash algorithm.
    constexpr uint64_t FNV_offset_basis = 0xcbf29ce484222325ULL;
    constexpr uint64_t FNV_prime[[maybe_unused]] = 0x00000100000001b3ULL;

    // Initialize the hash to the FNV offset basis
    uint64_t hash = FNV_offset_basis;

    // Hash each byte in the buffer
    for (size_t i = 0; i < len; ++i) {
        // XOR the hash with the current byte
        hash ^= (uint64_t) data[i];
        // Multiply the hash by the FNV prime
#if __NO_FNV_GCC_OPTIMIZATION__
        // If FNV GCC optimization is not enabled, multiply the hash by the FNV prime
        hash *= FNV_prime;
#else
        // If FNV GCC optimization is enabled, perform an equivalent operation that is faster on some processors
        hash += (hash << 1) + (hash << 4) + (hash << 5) +
                (hash << 7) + (hash << 8) + (hash << 40);
#endif
    }

    return hash;
}

/**
 * @brief Enum representing various error codes.
 */
enum {
    ERR_UNKNOWN = 1, ///< Represents an unknown error.
    ERR_2BIG = 2,    ///< Represents an error when the data is too big.
};

/**
 * @brief Enum representing various serialization constants.
 */
enum {
    SER_NIL = 0, ///< Represents a nil value.
    SER_ERR = 1, ///< Represents an error message.
    SER_STR = 2, ///< Represents a string value.
    SER_INT = 3, ///< Represents an integer value.
    SER_ARR = 4, ///< Represents an array.
};

bool string_append_cstr_range_bin(string_c *const restrict s, const char *const restrict cstr, const size_t count) {
    if (s) {
        if (count == 0) return true;
        if (cstr) {
            // Resize the string if necessary
            if (string_reserve(s, s->size + count)) {
                // Copy the C-string into the string
                memcpy(s->data + s->size * sizeof(char), cstr, count * sizeof(char));
                s->size += count;
                return true;
            }
        }
    }
    return false;
}

bool string_push_back_bin(string_c *const restrict s, const char c) {
    if (s) {
        if (s->size == s->capacity) {
            if (!string_reserve(s, s->capacity * 2)) return false;
        }
        s->data[s->size++] = c;
        return true;
    }
    return false;
}

/**
 * @brief Appends a nil value to the output string.
 *
 * @param out Pointer to the output string.
 */
static void out_nil(string_c *out) {
    string_push_back_bin(out, SER_NIL);
}

/**
 * @brief Appends a string value to the output string.
 *
 * @param out Pointer to the output string.
 * @param val Pointer to the string value to be appended.
 */
static void out_str(string_c *out, const string_c *val) {
    string_push_back_bin(out, SER_STR);
    uint32_t len = string_length(val);
    string_append_cstr_range_bin(out, (char *) &len, 4);

    string_append(out, val);
}

/**
 * @brief Appends an integer value to the output string.
 *
 * @param out Pointer to the output string.
 * @param val The integer value to be appended.
 */
static void out_int(string_c *out, int64_t val) {
    string_push_back_bin(out, SER_INT);
    string_append_cstr_range_bin(out, (char *) &val, 8);
}

/**
 * @brief Appends an error message to the output string.
 *
 * @param out Pointer to the output string.
 * @param code The error code.
 * @param msg Pointer to the error message.
 */
static void out_err(string_c *out, int32_t code, const string_c *msg) {
    string_push_back_bin(out, SER_ERR);
    string_append_cstr_range_bin(out, (char *) &code, 4);

    uint32_t len = string_length(msg);
    string_append_cstr_range_bin(out, (char *) &len, 4);

    string_append(out, msg);
}

/**
 * @brief Appends an array to the output string.
 *
 * @param out Pointer to the output string.
 * @param n The number of elements in the array.
 */
static void out_arr(string_c *out, uint32_t n) {
    string_push_back_bin(out, SER_ARR);
    string_append_cstr_range_bin(out, (char *) &n, 4);
}


/**
 * @brief Handles the "get" command.
 *
 * It retrieves the value associated with the provided key from the database. \n
 * If the key is found, it returns the associated value. Otherwise, it returns nil.
 *
 * @param cmd Pointer to the command vector. The command vector contains the command and its arguments.
 * @param out Pointer to the string where the response will be stored.
 */
static void do_get(const ptr_vector *cmd, string_c *out) {
    // Create a new Entry structure and initialize it
    Entry key;
    entry_init(&key);

    // Copy the key from the command vector to the Entry structure
    string_swap(key.key, ptr_vector_at(cmd, 1));
    key.node.hcode = fnv1a_hash((uint8_t *) key.key->data, string_length(key.key));

    // Look up the key in the hash map
    const HNode *node = hm_lookup(&g_data.db, &key.node, (bool (*)(HNode *, HNode *)) &entry_eq);
    if (node) {
        const string_c *val = container_of(node, Entry, node)->value;
        out_str(out, val);
    } else out_nil(out);

    entry_free(&key);
}

/**
 * @brief Handles the "set" command.
 *
 * It sets the value associated with the provided key in the database.\n
 * If the key already exists, it updates the associated value.
 * Otherwise, it creates a new key-value pair.
 *
 * @param cmd Pointer to the command vector. The command vector contains the command and its arguments.
 * @param out Pointer to the string where the response will be stored.
 */
static void do_set(const ptr_vector *cmd, string_c *out) {
    // Create a new Entry structure and initialize it
    Entry key;
    entry_init(&key);

    // Copy the key and value from the command vector to the Entry structure
    string_swap(key.key, ptr_vector_at(cmd, 1));
    key.node.hcode = fnv1a_hash((uint8_t *) key.key->data, string_length(key.key));

    // Look up the key in the hash map
    const HNode *node = hm_lookup(&g_data.db, &key.node, (bool (*)(HNode *, HNode *)) &entry_eq);
    if (node) {
        // If the key is found, update the value
        string_swap(container_of(node, Entry, node)->value, ptr_vector_at(cmd, 2));
    } else {
        // If the key is not found, create a new Entry structure and insert it into the hash map
        Entry *new_entry = malloc(sizeof(Entry));
        if (new_entry) {
            new_entry->key = string_new();
            new_entry->value = string_new();

            string_swap(new_entry->key, key.key);
            new_entry->node.hcode = key.node.hcode;
            string_swap(new_entry->value, ptr_vector_at(cmd, 2));
            hm_insert(&g_data.db, &new_entry->node);
        }
    }
    out_nil(out);
    entry_free(&key);
}


/**
 * @brief Handles the "del" command.
 *
 * It removes the key-value pair associated with the provided key from the database.\n
 * If the key is found and successfully removed, it returns 1. Otherwise, it returns 0.
 *
 * @param cmd Pointer to the command vector. The command vector contains the command and its arguments.
 * @param out Pointer to the string where the response will be stored.
 */
static void do_del(const ptr_vector *cmd, string_c *out) {
    // Create a new Entry structure and initialize it
    Entry key;
    entry_init(&key);

    // Copy the key from the command vector to the Entry structure
    string_swap(key.key, ptr_vector_at(cmd, 1));
    key.node.hcode = fnv1a_hash((uint8_t *) key.key->data, string_length(key.key));

    // Look up the key in the hash map and remove it if found
    const HNode *node = hm_pop(&g_data.db, &key.node, (bool (*)(HNode *, HNode *)) &entry_eq);
    if (node) {
        // If the key is found, free the memory allocated for the key and value
        Entry *entry = container_of(node, Entry, node);
        string_free(entry->key);
        string_free(entry->value);
        free(entry);
    }
    // Free the memory allocated for the key in the Entry structure
    entry_free(&key);

    out_int(out, node ? 1 : 0);
}

/**
 * @brief Scans a hash node and applies a function to it.
 *
 * @param node Pointer to the hash node to be scanned.
 * @param arg Pointer to the argument to be passed to the function.
 */
static void cb_scan(const HNode *node, void *arg) {
    auto *out = (string_c *) arg;
    out_str(out, container_of(node, Entry, node)->key);
}

// Restore the warning about statement expressions in macros
#if __clang__
#pragma clang diagnostic pop
#elif __GNUC__
#pragma GCC diagnostic pop
#endif


/**
 * @brief Scans a hash table and applies a function to each node.
 *
 * @param tab Pointer to the hash table to be scanned.
 * @param f Pointer to the function to be applied to each node. The function should take a pointer to a hash node and a pointer to an argument.
 * @param arg Pointer to the argument to be passed to the function.
 */
static void h_scan(const HTab *tab, void (*f)(HNode *, void *), void *arg) {
    if (tab->size == 0) return;

    for (size_t i = 0; i < tab->mask + 1; ++i) {
        HNode *node = tab->tab[i];
        while (node) {
            f(node, arg);
            node = node->next;
        }
    }
}

/**
 * @brief Handles the "keys" command.
 *
 * It sends back an array of all keys in the database.
 *
 * @param cmd Pointer to the command vector. The command vector contains the command and its arguments.
 * This parameter is not used in this function.
 * @param out Pointer to the string where the response will be stored.
 */
static void do_keys(const ptr_vector *cmd[[maybe_unused]

],
string_c *out
) {
out_arr(out, hm_size(
&g_data.db));
h_scan(&g_data.db.ht1, (void (*)(HNode *, void *)) &cb_scan, out);
h_scan(&g_data.db.ht2, (void (*)(HNode *, void *)) &cb_scan, out);
}

/**
 * @brief Compares a string with a command string, ignoring case.
 *
 * @param word Pointer to the string to be compared.
 * @param cmd The command string to compare with.
 * @return True if the strings are equal (ignoring case), false otherwise.
 */
static bool cmd_is(const string_c *word, const char *cmd) {
    return string_case_compare_cstr(word, cmd);
}


/**
 * @brief Processes a client request and generates a response.
 *
 * @param cmd Pointer to the command vector. The command vector contains the command and its arguments.
 * @param out Pointer to the string where the response will be stored.
 */
static void do_request(const ptr_vector *cmd, string_c *out) {
    // Check the command vector to determine the type of command
    if (ptr_vector_size(cmd) == 1 && cmd_is(ptr_vector_at(cmd, 0), "keys")) {
        do_keys(cmd, out);
    } else if (ptr_vector_size(cmd) == 2 && cmd_is(ptr_vector_at(cmd, 0), "get")) {
        do_get(cmd, out);
    } else if (ptr_vector_size(cmd) == 3 && cmd_is(ptr_vector_at(cmd, 0), "set")) {
        do_set(cmd, out);
    } else if (ptr_vector_size(cmd) == 2 && cmd_is(ptr_vector_at(cmd, 0), "del")) {
        do_del(cmd, out);
    } else {
        // cmd is not recognized
        string_c *tmp = string_new();
        string_append_cstr(tmp, "Unknown cmd");
        out_err(out, ERR_UNKNOWN, tmp);
        string_free(tmp);
    }
    // Free the memory allocated for the command vector and the parsed arguments
    // for (size_t i = 0; i < ptr_vector_size(cmd); ++i)
    // string_free(ptr_vector_at(cmd, i));
}

/**
 * @brief Tries to process one request from the client.
 *
 * @param conn Pointer to the connection structure.
 * @return True if the request was fully processed and the connection's state is \p STATE_REQ, false otherwise.
 *
 * @note This function attempts to parse a request from the client's buffer.
 * If the buffer does not contain enough data, the function will return false,
 * indicating that it will retry in the next iteration.\n
 * If the buffer contains a complete request, the function will generate a response, update the connection's state,
 * and remove the processed request from the buffer.
 */
static bool try_one_request(Conn *conn) {
    if (conn->rbuf_size < 4) {
        // The buffer does not contain enough data to parse the request
        return false;
    }
    // The first 4 bytes represent the length of the request
    uint32_t len = 0;
    memcpy(&len, &conn->rbuf[0], 4);
    if (len > k_max_msg) {
        // The request is too long
        report_error("too long");
        conn->state = STATE_END;
        return false;
    }
    if (4 + len > conn->rbuf_size) {
        // The buffer does not contain the complete request
        return false;
    }
    // Try to process the request and generate a response
    ptr_vector *cmd = ptr_vector_new();
    if (parse_req(&conn->rbuf[4], len, cmd) != 0) {
        report_error("bad request");
        conn->state = STATE_END;
        return false;
    }
    // Got one request, generate the response.
    string_c *out = string_new();
    do_request(cmd, out);

    // pack the response into the buffer
    if (4 + string_length(out) > k_max_msg) {
        string_clear(out);
        string_c *tmp = string_new();
        string_append_cstr(tmp, "Response is too big");
        out_err(out, ERR_2BIG, tmp);
        string_free(tmp);
    }

    // Copy the response length to the response buffer
    const uint32_t wlen = string_length(out);
    memcpy(&conn->wbuf[0], &wlen, 4);
    memcpy(&conn->wbuf[4], out->data, string_length(out));
    conn->wbuf_size = 4 + wlen;

    // Remove the processed request from the buffer
    const size_t remain = conn->rbuf_size - 4 - len;
    if (remain) {
        // There is remaining data in the buffer, move it to the beginning
        memmove(conn->rbuf, &conn->rbuf[4 + len], remain);
    }
    // Update the buffer size and the connection state
    conn->rbuf_size = remain;
    conn->state = STATE_RES;
    state_res(conn);

    string_free(out);
    for (size_t i = 0; i < ptr_vector_size(cmd); ++i) {
        const auto ptr = (string_c *) ptr_vector_at(cmd, i);
        if (ptr) string_free(ptr);
    }
    ptr_vector_free(cmd);

    return conn->state == STATE_REQ;
}

/**
 * @brief Tries to fill the read buffer with data from the client.
 *
 * @param conn Pointer to the connection structure.
 * @return True if the connection's state is \p STATE_REQ after processing the requests, false otherwise.
 */
static bool try_fill_buffer(Conn *conn) {
    // Read data from the client and fill the read buffer
    assert(conn->rbuf_size < sizeof(conn->rbuf));
    ssize_t rv;
    do {
        const size_t cap = sizeof(conn->rbuf) - conn->rbuf_size;
        rv = read(conn->fd, &conn->rbuf[conn->rbuf_size], cap);
    } while (rv < 0 && errno == EINTR);
    // If the read operation was interrupted, retry in the next iteration
    if (rv < 0 && errno == EAGAIN) {
        return false;
    }
    // If an error occurred during the read operation, set the connection state to STATE_END
    if (rv < 0) {
        report_error("read() error");
        conn->state = STATE_END;
        return false;
    }
    // If the client closed the connection, set the connection state to STATE_END
    if (rv == 0) {
        if (conn->rbuf_size > 0) {
            report_error("unexpected EOF");
        } else {
            fputs("EOF\n", stderr);
        }
        conn->state = STATE_END;
        return false;
    }

    // Update the buffer size with the number of bytes read
    conn->rbuf_size += (size_t) rv;
    assert(conn->rbuf_size <= sizeof(conn->rbuf));

    // Try to process the requests and update the connection state
    while (try_one_request(conn)) {
    }
    return conn->state == STATE_REQ;
}

/**
 * @brief Handles the request state of a connection.
 *
 * @param conn Pointer to the connection structure.
 */
static void state_req(Conn *conn) {
    // Try to fill the read buffer with data from the client
    while (try_fill_buffer(conn)) {
    }
}

/**
 * @brief Tries to flush the write buffer to the client.
 *
 * @param conn Pointer to the connection structure.
 * @return True if there is more data to be sent, false otherwise.
 */
static bool try_flush_buffer(Conn *conn) {
    ssize_t rv;
    // Write data from the write buffer to the client
    do {
        const size_t remain = conn->wbuf_size - conn->wbuf_sent;
        rv = write(conn->fd, &conn->wbuf[conn->wbuf_sent], remain);
    } while (rv < 0 && errno == EINTR);

    // If the write operation was interrupted, retry in the next iteration
    if (rv < 0 && errno == EAGAIN) {
        return false;
    }
    // If an error occurred during the write operation, set the connection state to STATE_END
    if (rv < 0) {
        report_error("write() error");
        conn->state = STATE_END;
        return false;
    }
    // Update the amount of data sent from the write buffer
    conn->wbuf_sent += (size_t) rv;
    assert(conn->wbuf_sent <= conn->wbuf_size);
    if (conn->wbuf_sent == conn->wbuf_size) {
        conn->state = STATE_REQ;
        conn->wbuf_sent = 0;
        conn->wbuf_size = 0;
        return false;
    }
    return true;
}

/**
 * @brief Handles the response state of a connection.
 *
 * @param conn Pointer to the connection structure.
 */
static void state_res(Conn *conn) {
    // Try to flush the write buffer to the client
    while (try_flush_buffer(conn)) {
    }
}

/**
 * @brief Handles the I/O operations for a connection based on its state.
 *
 * This function checks the state of the connection and calls the appropriate function to handle the I/O operations.
 *
 * @param conn Pointer to the connection structure.
 */
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
    // Create a socket
    const int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) die("socket() failure");

    // Set the SO_REUSEADDR option for the socket
    constexpr int val = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    // Bind the socket to the address and port
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(1234);
    addr.sin_addr.s_addr = ntohl(0); // wildcard address 0.0.0.0

    int rv = bind(fd, (const struct sockaddr *) &addr, sizeof(addr));
    if (rv) die("bind() failure");

    // Listen for incoming connections
    rv = listen(fd, SOMAXCONN);
    if (rv) die("listen() failure");

    // Set the server socket to non-blocking mode
    fd_set_nb(fd);

    // A map of all client connections, keyed by fd
    ptr_vector *fd2conn = ptr_vector_new();

    vector_c *poll_args = vector_new(sizeof(struct pollfd));

    // The event loop
    while (true) {
        // Clear the poll arguments
        vector_clear(poll_args);

        // Add the listening fd to the poll arguments
        struct pollfd pfd = {fd, POLLIN, 0};
        vector_push_back(poll_args, &pfd);

        // Add the client connections to the poll arguments
        for (size_t i = 0; i < ptr_vector_size(fd2conn); ++i) {
            const Conn *conn = ptr_vector_at(fd2conn, i);
            if (conn == nullptr) continue;

            struct pollfd pfd2 = {.fd = -1};
            pfd2.fd = conn->fd;
            // Set the events based on the connection state
            pfd2.events = conn->state == STATE_REQ ? POLLIN : POLLOUT;
            pfd2.events = pfd2.events | POLLERR;
            vector_push_back(poll_args, &pfd2);
        }
        // Poll for events
        rv = poll(vector_data(poll_args), vector_size(poll_args), 1000);
        if (rv < 0) die("poll");

        // Process the events
        for (size_t i = 1; i < vector_size(poll_args); ++i) {
            if (((struct pollfd *) vector_at(poll_args, i))->revents) {
                Conn *conn = ptr_vector_at(fd2conn, ((struct pollfd *) vector_at(poll_args, i))->fd);
                if (conn == nullptr) continue;

                connection_io(conn);
                if (conn->state == STATE_END) {
                    // Remove the connection from the connection vector and close the connection
                    ptr_vector_set(fd2conn, conn->fd, nullptr);
                    (void) close(conn->fd);
                    free(conn);
                }
            }
        }
        // Accept new connections
        if (((struct pollfd *) vector_at(poll_args, 0))->revents)
            (void) accept_new_conn(fd2conn, fd);
    }
    // Free the memory used by the connection vector and the poll arguments
    vector_free(poll_args);
    ptr_vector_free(fd2conn);

    return 0;
}
