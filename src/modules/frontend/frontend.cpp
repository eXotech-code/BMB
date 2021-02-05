#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <cstring>
#include <iostream>
#include <cstdlib>
#include <string>

#define BACKLOG 10
#define PORT "8080"

/* Wait for terminated childern processes to release the system resources
 * associated with them. See man wait(2) for details. */
void sigchld_handler(int s) {
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    // "waitpid()" returns 0 if there are no children left to wait for.
    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}

/* Get socket's address either in IPv4 or IPv6.
 * We are returning a void pointer because
 * we don't know if this is "sockaddr_in6" or
 * "sockaddr_in" in advance. */
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

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
    int status; // Status of getaddrinfo
    int sockfd, new_fd; // socket file descriptor and accepted socket's FD
    struct addrinfo hints, *res; // *res is the result of getaddrinfo()
    struct addrinfo *p; // *p is used to loop through linked list of addresses in *res
    struct sockaddr_storage client_addr; // "sockaddr_storage" is big enough for both address types.
    socklen_t sin_size; // Size of the address.
    int yes = 1; // This is used to set option value in "setsockopt()".
    struct sigaction sa; // Eliminate zombie processes. See man sigaction(2) and man wait(2).
    char client_addr_c[INET6_ADDRSTRLEN]; // Client address converted from binary.

    // Reset the value of hints
    std::memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // Either IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // Socket that keeps a connection
    hints.ai_flags = AI_PASSIVE; // localhost

    // Pack the *res with getaddrinfo() and lookup the error code on failure
    if ((status = getaddrinfo(NULL, PORT, &hints, &res)) != 0) {
        std::cerr << "getaddrinfo error: " << gai_strerror(status) << "\n";
        return 1;
    }

    // Get the socket file descriptor. Try first address that works.
    for (p = res; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            int err = errno;
            std::cerr << "error while getting the socket file descriptor: " << err << "trying on next address.\n";
            continue;
        }

        /* Set options for that socket.
         * "SOL_SOCKET" means that "setsockopt" sets options at socket level. */
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) != 0) {
            int err = errno;
            std::cerr << "setsockopt error: " << err << "\n";
            exit(1); // Close all file descriptors and terminate program.
        }

        // Bind socket to port specified in "getaddrinfo"
        if (bind(sockfd, res->ai_addr, res->ai_addrlen) != 0) {
            close(sockfd);
            int err = errno;
            std::cerr << "bind error: " << err << "\n";
            continue;
        }

        // Stop looking if the address has been found.
        break;
    }

    freeaddrinfo(res); // Free the linked list

    if (p == NULL) {
        std::cerr << "failed to bind socket to port.\n";
        exit(1);
    }

    // Listen for incoming connecting
    if (listen(sockfd, BACKLOG) != 0) {
        int err = errno;
        std::cerr << "listen error: " << err << "\n";
        exit(1);
    }

    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART; // Make system calls restartable. See man sigaction(2) and man signal(7).
    if (sigaction(SIGCHLD, &sa, NULL) != 0) {
        int err = errno;
        std::cerr << "sigaction error" << errno << "\n";
        exit(1);
    }

    std::cout << "Server ready. Listening on port: " << PORT << ".\n";

    // Now the fun begins!
    while(1) {
        sin_size = sizeof client_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size);
        if (new_fd == -1) {
            int err = errno;
            std::cerr << "accept error: " << err << "\n";
            continue;
        }

        // Convert client's address from binary to text form.
        inet_ntop(client_addr.ss_family, get_in_addr((struct sockaddr *)&client_addr), client_addr_c, sizeof client_addr_c);
        std::cout << "Client connected from: " << client_addr_c << "\n";
    }

    return 0;
}
