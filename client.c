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

// C23 constexpr support
#if __GNUC__ >= 13 || __clang_major__ >= 19
constexpr size_t k_max_msg = 4096;
#else
enum : size_t {
    k_max_msg = 4096
};

#define constexpr const
#endif

/**
 * @brief Reports an error message.
 *
 * This function is used to report an error message to the user.
 *
 * @param msg The error message to be reported.
 */
static void report_error(const char *msg) {
    perror(msg);
}

/**
 * @brief Terminates the program and prints an error message.
 * 
 * This function is used to gracefully terminate the program and display an error message.
 * 
 * @param msg The error message to be displayed.
 */
static void die(const char *msg) {
    report_error(msg);
    exit(1);
}

/**
 * @brief Reads data from a file descriptor into a buffer.
 *
 * @param fd The file descriptor to read from.
 * @param buf The buffer to store the read data.
 * @param n The number of bytes to read.
 * @return The number of bytes read, or -1 if an error occurred.
 */
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

/**
 * @brief Writes the specified number of bytes from the buffer to the file descriptor.
 *
 * @param fd The file descriptor to write to.
 * @param buf The buffer containing the data to be written.
 * @param n The number of bytes to write.
 * @return Returns the number of bytes written on success, or -1 on failure.
 */
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

/**
 * @brief Sends a request to the specified file descriptor with the given command vector.
 *
 * @param fd The file descriptor to send the request to.
 * @param cmd The command vector containing the commands to be sent.
 * @return The number of bytes written on success, or -1 on failure.
 */
static int32_t send_req(const int fd, const ptr_vector *cmd) {
    uint32_t len = 4;

    // Calculate the length of the message
    for (size_t i = 0; i < ptr_vector_size(cmd); ++i) {
        len += 4 + string_length(ptr_vector_at(cmd, i));
    }
    // Check if the message is too long
    if (len > k_max_msg) return -1;

    // Write the message
    char wbuf[4 + k_max_msg];
    memcpy(&wbuf[0], &len, 4); // assume little endian
    const uint32_t n = ptr_vector_size(cmd);
    memcpy(&wbuf[4], &n, 4);
    size_t cur = 8; // current position in the buffer

    // Write the commands
    char strbuf[k_max_msg];
    for (size_t i = 0; i < ptr_vector_size(cmd); ++i) {
        const uint32_t p = string_length(ptr_vector_at(cmd, i));
        string_copy_buffer(ptr_vector_at(cmd, i), strbuf);

        memcpy(&wbuf[cur], &p, 4);
        memcpy(&wbuf[cur + 4], strbuf, p);

        cur += 4 + p;
    }

    return write_all(fd, wbuf, 4 + len);
}

/**
 * @brief Reads a response from the specified file descriptor.
 *
 * @param fd The file descriptor to read from.
 * @return The number of bytes read on success, or -1 on failure.
 */
static int32_t read_res(const int fd) {
    // 4 bytes header
    char rbuf[4 + k_max_msg + 1];
    errno = 0;
    // Read the length of the reply
    int32_t err = read_full(fd, rbuf, 4);
    if (err) {
        // If an error occurred, report it
        report_error(errno == 0 ? "read_full() error" : "read() error");
        return err;
    }

    // Get the length of the reply
    uint32_t len = 0;
    memcpy(&len, rbuf, 4); // Assume little endian
    if (len > k_max_msg) {
        report_error("too long"); // The message is too long
        return -1;
    }

    // Read the reply
    err = read_full(fd, &rbuf[4], len);
    if (err) {
        report_error("read() error");
        return err;
    }

    // If the reply is too short, report an error
    if (len < 4) {
        report_error("bad response");
        return -1;
    }
    // Get the response code and the message
    uint32_t rescode = 0;
    memcpy(&rescode, &rbuf[4], 4);
    printf("server says: [%u] %.*s\n", rescode, len - 4, &rbuf[8]);
    return 0;
}

int main(const int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <command> [args...]\n", argv[0]);
        return 1;
    }

    // Create a socket
    const int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) die("socket() failure");

    // Connect to the server
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(1234);
    addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK); // 127.0.0.1

    const int rv = connect(fd, (const struct sockaddr *) &addr, sizeof addr);
    if (rv) die("connect() failure");

    // Send the request
    ptr_vector *cmd = ptr_vector_new();
    for (int i = 1; i < argc; ++i) {
        ptr_vector_push_back(cmd, string_new());
        string_append_cstr(ptr_vector_at(cmd, i - 1), argv[i]);
    }
    const int32_t err = send_req(fd, cmd);
    if (!err) {
        // Read the response
        read_res(fd);
    }
    // Clean up
    close(fd);
    for (size_t i = 0; i < ptr_vector_size(cmd); ++i)
        string_free(ptr_vector_at(cmd, i));

    ptr_vector_free(cmd);
    return 0;
}
