#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

inline static void report_error(const char *msg) {
    perror(msg);
}

static void die(const char *msg) {
    report_error(msg);
    exit(1);
}

static void send_text(int conn_fd) {
    char buf[64] = {};
    ssize_t n = read(conn_fd, buf, sizeof(buf) - 1);
    if (n < 0) {
        report_error("read() failure");
        return;
    }
    printf("client says: %s\n", buf);

    char w_buf[] = "world";
    write(conn_fd, w_buf, strlen(w_buf));
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

        send_text(conn_fd);
        close(conn_fd);
        break;
    }

    return 0;
}