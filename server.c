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

static int32_t one_request(int conn_fd) {
    // 4 bytes header
    char buf[4 + k_max_msg + 1];
    errno = 0;

    int32_t err = read_full(conn_fd, buf, 4);
    if (err) {
        if (errno != 0) report_error("read() failure");
        return err;
    }

    uint32_t len = 0;
    memcpy(&len, buf, 4);  // assume little endian
    if (len > k_max_msg) {
        report_error("message too long");
        return -1;
    }

    // request body
    err = read_full(conn_fd, &buf[4], len);
    if (err) {
        report_error("read() error");
        return err;
    }

    buf[4 + len] = '\0';
    printf("client says: %s\n", &buf[4]);

    // reply using the same protocol
    const char reply[] = "world";
    char w_buf[4 + sizeof(reply)];

    len = (uint32_t) strlen(reply);
    memcpy(w_buf, &len, 4);
    memcpy(&w_buf[4], reply, len);

    return write_all(conn_fd, w_buf, 4 + len);
}

int main() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) die("socket() failure");

    int val = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    // bind
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(1234);
    addr.sin_addr.s_addr = ntohl(0);    // wildcard address 0.0.0.0

    int rv = bind(fd, (const struct sockaddr *) &addr, sizeof(addr));
    if (rv) die("bind() failure");

    // listen
    rv = listen(fd, SOMAXCONN);
    if (rv) die("listen() failure");

    while (true) {
        // accept
        struct sockaddr_in client_addr = {};
        socklen_t socklen = sizeof client_addr;
        int conn_fd = accept(fd, (struct sockaddr *) &client_addr, &socklen);

        if (conn_fd < 0) continue;   // error

        while (!one_request(conn_fd)) {}

        close(conn_fd);
        break;
    }

    return 0;
}
