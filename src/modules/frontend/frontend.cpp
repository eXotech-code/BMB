#include "frontend.h"

void raise_error(std::string type) {
   int error_code = errno;
   std::cerr << type << " error: " << errno;
}

void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// Listener class member definitions
Listener::Listener() {
    int status; // Status of getaddrinfo.
    struct addrinfo hints;
    struct addrinfo *p; // // *p is used to loop through linked list of addresses at *ai
    int yes = 1; // This is used to set option value in "setsockopt()".

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // Use either IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // localhost

    // Pack the *ai with getaddrinfo() and lookup the error code on failure
    if ((status = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
       std::cerr << "getaddrinfo error for listener: " << gai_strerror(status) << "\n";
       exit(1); // Close all file descriptors and terminate program.
    }

    // Use first result from getaddrinfo that works.
    for (p = ai; p != NULL; p = p->ai_next) {
       fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
       if (fd == -1) {
           continue;
       }

       /* Set options for that socket.
        * "SOL_SOCKET" means that "setsockopt" sets options at socket level. */
       if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) != 0) {
           raise_error("setsockopt");
       }

       if (bind(fd, p->ai_addr, p->ai_addrlen) != 0) {
           raise_error("bind");
           continue;
       }

       // We found the address. That's it!
       break;
    }

    freeaddrinfo(ai);

    // If the socket couldn't bind. Set fd to -1. That will raise an error.
    if (p == NULL) {
        fd = -1;
    }

    // Listen for incoming connections.
    if (listen(fd, BACKLOG) != 0) {
        raise_error("listen");
        fd = -1;
    }
}

int Listener::get_fd() {
    return fd;
}

// Connections class member declarations.
Connections::Connections() {
    // Prepare space for array.
    fds = static_cast<struct pollfd *>(malloc(sizeof *fds * fd_size));
}

void Connections::add_new(int new_fd) {
    fds[fd_count].fd = new_fd;
    fds[fd_count].events = POLLIN; // Check if this socket is ready to be read from.

    fd_count++;
}

struct pollfd* Connections::get_fds() {
    return fds;
}

int Connections::get_fd_count() {
    return fd_count;
}

int main() {
    int new_fd; // Newly accepted socket descriptor
    struct sockaddr_storage client_addr; // New client address
    socklen_t addrlen;
    char client_ip[INET6_ADDRSTRLEN];

    Listener obj_listener; // Instantiate listener object.
    std::cout << "Listening on socket: " << obj_listener.get_fd() << " port: " << PORT << "\n";

    if (obj_listener.get_fd() == -1) {
        std::cerr << "Error while trying to get listener socket.\n";
        exit(1);
    }

    // Instantiate connections object.
    Connections obj_connections;

    // Add listener to connections.
    obj_connections.add_new(obj_listener.get_fd());

    // This is where the fun begins.
    while (1) {
        // Poll() returns number of sockets ready to be read from.
        int poll_count = poll(obj_connections.get_fds(), obj_connections.get_fd_count(), -1);

        if (poll_count == -1) {
            raise_error("poll");
            exit(1);
        }

        // Run through all the all the existing connections to see if there's data ready.
        for (int i = 0; i < obj_connections.get_fd_count(); i++) {
            // Check if data is ready to read.
            if (obj_connections.get_fds()[i].revents == POLLIN) {

                // If listener is ready get new connections.
                if (obj_connections.get_fds()[i].fd == obj_listener.get_fd()) {
                    addrlen = sizeof client_addr;
                    new_fd = accept(obj_listener.get_fd(), (struct sockaddr *)&client_addr, &addrlen);

                    if (new_fd == -1) {
                        raise_error("accept");
                    } else {
                        obj_connections.add_new(new_fd);

                        std::cout << "Got new connection from " << inet_ntop(client_addr.ss_family, get_in_addr((struct sockaddr *)&client_addr), client_ip, INET6_ADDRSTRLEN) << " on socket: " << new_fd << "\n";
                    }
                }
            }
        }
    }

    return 0;
}
