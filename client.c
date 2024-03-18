#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <assert.h>
#include <errno.h>

#include "data_structures/string_c.h"
#include "data_structures/vector_c.h"

#if __GNUC__ >= 13 || __clang_major__ >= 19
constexpr size_t k_max_msg = 4096;
#else
enum : size_t { k_max_msg = 4096 };

#define constexpr const
#endif

static void report_error(const char *msg) {
    perror(msg);
}

static void die(const char *msg) {
    report_error(msg);
    exit(1);
}

static int32_t read_full(const int fd, char *buf, size_t n) {
    while (n > 0) {
        const ssize_t rv = read(fd, buf, n);

        if (rv <= 0) return -1; // error, or unexpected EOF

        assert((size_t) rv <= n);

        n -= (size_t) rv;
        buf += rv;
    }
    return 0;
}

static int32_t write_all(const int fd, const char *buf, size_t n) {
    while (n > 0) {
        const ssize_t rv = write(fd, buf, n);

        if (rv <= 0) return -1; // error

        assert((size_t) rv <= n);

        n -= (size_t) rv;
        buf += rv;
    }
    return 0;
}

static int32_t send_req(const int fd, const ptr_vector *cmd) {
    uint32_t len = 4;

    for (size_t i = 0; i < ptr_vector_size(cmd); ++i) {
        len += 4 + string_length(ptr_vector_at(cmd, i));
    }
    if (len > k_max_msg) return -1;

    char wbuf[4 + k_max_msg];
    memcpy(&wbuf[0], &len, 4); // assume little endian
    const uint32_t n = ptr_vector_size(cmd);
    memcpy(&wbuf[4], &n, 4);
    size_t cur = 8;

    for (size_t i = 0; i < ptr_vector_size(cmd); ++i) {
        char *c_str = string_cstr(ptr_vector_at(cmd, i));
        uint32_t p = string_length(ptr_vector_at(cmd, i));
        memcpy(&wbuf[cur], &p, 4);

        memcpy(&wbuf[cur + 4], c_str, string_length(ptr_vector_at(cmd, i)));
        cur += 4 + string_length(ptr_vector_at(cmd, i));
        free(c_str);
    }

    return write_all(fd, wbuf, 4 + len);
}

static int32_t read_res(const int fd) {
    // 4 bytes header
    char rbuf[4 + k_max_msg + 1];
    errno = 0;
    int32_t err = read_full(fd, rbuf, 4);
    if (err) {
        report_error(errno == 0 ? "read_full() error" : "read() error");
        return err;
    }

    uint32_t len = 0;
    memcpy(&len, rbuf, 4); // assume little endian
    if (len > k_max_msg) {
        report_error("too long");
        return -1;
    }

    // reply body
    err = read_full(fd, &rbuf[4], len);
    if (err) {
        report_error("read() error");
        return err;
    }

    // print the result
    uint32_t rescode = 0;
    if (len < 4) {
        report_error("bad response");
        return -1;
    }
    memcpy(&rescode, &rbuf[4], 4);
    printf("server says: [%u] %.*s\n", rescode, len - 4, &rbuf[8]);
    return 0;
}

int main(const int argc, char **argv) {
    const int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) die("socket() failure");

    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(1234);
    addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK); // 127.0.0.1

    const int rv = connect(fd, (const struct sockaddr *) &addr, sizeof addr);
    if (rv) die("connect() failure");

    ptr_vector *cmd = ptr_vector_new();

    for (int i = 1; i < argc && (size_t) i < ptr_vector_capacity(cmd); ++i) {
        ptr_vector_push_back(cmd, string_new());
        string_append_cstr(ptr_vector_at(cmd, i - 1), argv[i]);
    }
    const int32_t err = send_req(fd, cmd);
    if (err) {
        goto L_DONE;
    }
    read_res(fd);

L_DONE:
    close(fd);
    for (size_t i = 0; i < ptr_vector_size(cmd); ++i)
        string_free(ptr_vector_at(cmd, i));

    ptr_vector_free(cmd);
    return 0;
}
