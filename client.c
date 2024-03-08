#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <assert.h>
#include <errno.h>

const size_t k_max_msg = 4096;

inline static void report_error(const char *msg) {
    perror(msg);
}

static inline void die(const char *msg) {
    report_error(msg);
    exit(1);
}

static int32_t read_full(const int fd, char *buf, size_t n) {
    while (n > 0) {
        ssize_t rv = read(fd, buf, n);

        if (rv <= 0) return -1;  // error, or unexpected EOF

        assert((size_t) rv <= n);

        n -= (size_t) rv;
        buf += rv;
    }
    return 0;
}

static int32_t write_all(const int fd, const char *buf, size_t n) {
    while (n > 0) {
        ssize_t rv = write(fd, buf, n);

        if (rv <= 0) return -1;  // error

        assert((size_t) rv <= n);

        n -= (size_t) rv;
        buf += rv;
    }
    return 0;
}

static int32_t query(const int fd, const char *text) {
    uint32_t len = (uint32_t) strlen(text);
    if (len > k_max_msg) {
        return -1;
    }

    char w_buf[4 + k_max_msg];
    memcpy(w_buf, &len, 4);  // assume little endian
    memcpy(&w_buf[4], text, len);

    int32_t err = write_all(fd, w_buf, 4 + len);
    if (err) return err;

    // 4 bytes header
    char buf[4 + k_max_msg + 1];
    errno = 0;

    err = read_full(fd, buf, 4);
    if (err) {
        report_error(errno == 0 ? "read_full() error" : "read() error");
        return err;
    }

    memcpy(&len, buf, 4);  // assume little endian
    if (len > k_max_msg) {
        report_error("too long");
        return -1;
    }

    // reply body
    err = read_full(fd, &buf[4], len);
    if (err) {
        report_error("read() error");
        return err;
    }

    buf[4 + len] = '\0';
    printf("server says: %s\n", &buf[4]);

    return 0;
}

int main() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) die("socket() failure");

    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(1234);
    addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK);  // 127.0.0.1

    int rv = connect(fd, (const struct sockaddr *) &addr, sizeof addr);
    if (rv) die("connect() failure");

    // multiple requests
    if (query(fd, "hello1")) goto L_DONE;
    if (query(fd, "hello2")) goto L_DONE;
    if (query(fd, "hello3")) goto L_DONE;

    L_DONE:
    close(fd);
    return 0;
}