#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

static inline void die(const char *msg) {
    perror(msg);
    exit(1);
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

    const char msg[] = "hello";
    write(fd, msg, strlen(msg));

    char buf[64] = {};
    ssize_t n = read(fd, buf, sizeof(buf) - 1);
    if (n < 0) die("read() failure");

    printf("server says: %s\n", buf);
    close(fd);
    return 0;
}