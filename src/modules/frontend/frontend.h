#include <sys/types.h>
#include <sys/socket.h>
#include <poll.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <iostream>
#include <string>
#include <unistd.h>

#define PORT "6969"
#define BACKLOG 10

/* My third attempt to make this fucking server work
 * This time we will use OOP since this is C++ roght?
 * "Why didn't you do it like that from the start"
 * you may ask.
 * Well, I am a moron, but anyway. Let's get to it. */

void raise_error(std::string type);
// Get sockaddr_in or in6 depending on the ip version.
void *get_in_addr(struct sockaddr *sa);

/* So this is the listener class. It represents
 * the socket that listens for new connections. */
class Listener {
    public:
        Listener();
        int get_fd();
    private:
        int fd; // File descriptor for listener socket.
        struct addrinfo *ai; // "ai" is the result of getaddrinfo.
};

// Connections on which we listen for new events using "poll()".
class Connections {
    public:
        Connections();
        void add_new(int new_fd);
        void delete_con(int i);
        struct pollfd* get_fds();
        int get_fd_count();
    private:
        /* Array containing file descriptors and
         * events for which we should listen. */
        struct pollfd *fds;
        int fd_size = 4; // Start with space for 4 connections.
        int fd_count = 0;
};
