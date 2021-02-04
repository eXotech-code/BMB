#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <cstring>
#include <iostream>

int main() {
    /* This module will listen for incoming connections
     * from React and handle them.
     * The system calls to start listening are as follows:
     * 1. getaddrinfo()
     * 2. socket()
     * 3. bind()
     * 4. listen()
     * 5. accept() - if there are any connections waiting
     * to be accepted. */

    // Error code and socket file descriptor
    int status, sockfd;
    struct addrinfo hints, *res; // *res is the result of getaddrinfo()

    // Reset the value of hints
    std::memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // Either IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // Socket that keeps a connection
    hints.ai_flags = AI_PASSIVE; // localhost

    // Pack the *res with getaddrinfo() and lookup the error code on failure
    if ((status = getaddrinfo(NULL, "8080", &hints, &res)) != 0) {
        std::cerr << "getaddrinfo error: " << gai_strerror(status) << "\n";
        return 1;
    }

    // Get the socket file descriptor
    if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
       status = errno;
       std::cerr << "error while getting the socket file descriptor: " << status << "\n";
       return 1;
    }

    // Bind socket to port specified in "getaddrinfo"
    if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
        int err = errno;
        std::cerr << "bind error: " << err << "\n";
    }

    freeaddrinfo(res); // Free the linked list
    return 0;
}
