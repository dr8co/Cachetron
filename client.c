#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <assert.h>
#include <errno.h>

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

static int32_t send_req(const int fd, const char *text) {
    const uint32_t len = strlen(text);
    if (len > k_max_msg) return -1;

    char wbuf[4 + k_max_msg];
    memcpy(wbuf, &len, 4); // assume little endian
    memcpy(&wbuf[4], text, len);
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

    // do something
    rbuf[4 + len] = '\0';
    printf("server says: %s\n", &rbuf[4]);
    return 0;
}

int main() {
    const int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) die("socket() failure");

    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(1234);
    addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK); // 127.0.0.1

    const int rv = connect(fd, (const struct sockaddr *) &addr, sizeof addr);
    if (rv) die("connect() failure");

    // multiple pipelined requests
    const char *query_list[3] = {"hello1", "hello2", "hello3"};
    for (size_t i = 0; i < 3; ++i) {
        const int32_t err = send_req(fd, query_list[i]);
        if (err) goto L_DONE;
    }
    for (size_t i = 0; i < 3; ++i) {
        const int32_t err = read_res(fd);
        if (err) goto L_DONE;
    }

L_DONE:
    close(fd);
    return 0;
}
