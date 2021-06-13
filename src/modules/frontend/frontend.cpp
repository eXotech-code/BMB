#include "frontend.h"

void raise_error(const std::string &type) {
   std::cerr << type << " error: " << errno << " - " << strerror(errno);
}

void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

bool key_pressed(char key)
{
    // Struct to listen for key presses
    struct pollfd fds[1];
    fds[0].fd = 0; // Standard input (stdin)
    fds[0].events = POLLIN; // Check if data is ready to read
    fds[0].revents = 0;

    int is_ready = poll(fds, 1, 0);

    if (is_ready && fds[0].revents & POLLIN) {
        char buff[1024]; // Buffer of chars from stdin.
        memset(buff, 0, sizeof(buff));
        ssize_t read_bytes = read(0, buff, sizeof(buff) - 1);

        /* Loop through the buffer to see if certain key has
         * been pressed. */
        for (int i = 0; i < read_bytes; i++) {
            if (buff[i] == key) {
                std::cout << "Exiting...\n";
                return true;
            }
        }
    }

    return false;
}

// Listener class member definitions
Listener::Listener() {
    int status; // Status of getaddrinfo.
    struct addrinfo hints = { 0 };
    struct addrinfo *p; // // *p is used to loop through linked list of addresses at *ai
    int yes = 1; // This is used to set option value in "setsockopt()".

    //memset(&hints, 0, sizeof hints); // This is the same as initializing struct with zeros.
    hints.ai_family = AF_UNSPEC; // Use either IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // localhost

    // Pack the *ai with getaddrinfo() and lookup the error code on failure
    if ((status = getaddrinfo(nullptr, PORT, &hints, &ai))) {
       std::cerr << "getaddrinfo error for listener: " << gai_strerror(status) << "\n";
       exit(1); // Close all file descriptors and terminate program.
    }

    // Use first result from getaddrinfo that works.
    for (p = ai; p != nullptr; p = p->ai_next) {
       fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
       if (fd == -1) {
           continue;
       }

       /* Set options for that socket.
        * "SOL_SOCKET" means that "setsockopt" sets options at socket level. */
       if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))) {
           raise_error("setsockopt");
       }

       if (bind(fd, p->ai_addr, p->ai_addrlen)) {
           raise_error("bind");
           continue;
       }

       // We found the address. That's it!
       break;
    }

    freeaddrinfo(ai);

    // If the socket couldn't bind. Set fd to -1. That will raise an error.
    if (p == nullptr) {
        fd = -1;
    }

    // Listen for incoming connections.
    if (listen(fd, BACKLOG)) {
        raise_error("listen");
        fd = -1;
    }
}

int Listener::get_fd() const {
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

void Connections::delete_con(int i) {
   // Replace provided file descriptor with current last.
   fds[i] = fds[fd_count - 1];

   fd_count--;
}

struct pollfd* Connections::get_fds() {
    return fds;
}

int Connections::get_fd_count() const {
    return fd_count;
}

int main() {
    int new_fd; // Newly accepted socket descriptor
    struct sockaddr_storage client_addr = { 0 }; // New client address
    socklen_t addrlen;
    char client_ip[INET6_ADDRSTRLEN];

    // TEMPORARY
    char buffer[256];

    // Socket for passing information between modules.
    int unix_sock;

    Listener obj_listener; // Instantiate listener object.
    std::cout << "Listening on socket: " << obj_listener.get_fd() << " port: " << PORT << "\n";
    std::cout << "Press 'q' to exit.\n";

    if (obj_listener.get_fd() == -1) {
        std::cerr << "Error while trying to get listener socket.\n";
        exit(1);
    }

    // Instantiate connections object.
    Connections obj_connections;

    // Add listener to connections.
    obj_connections.add_new(obj_listener.get_fd());

    /* Connect to main module to communicate requests and responses
     * between modules. */
    if ((unix_sock = getClientSocket()) == -1) {
        raise_error("unix socket");
        exit(1);
    }

    // This is where the fun begins.
    while (true) {
        // Poll() returns number of sockets ready to be read from.
        int poll_count = poll(obj_connections.get_fds(), obj_connections.get_fd_count(), 100);

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
                // If this is not a listener then it is client.
                } else {
                    int client_fd = obj_connections.get_fds()[i].fd; // Client's file descriptor. D.R.Y.

                    ssize_t recv_bytes = recv(client_fd, &buffer, sizeof buffer, 0);

                    if (recv_bytes <= 0) {
                        if (recv_bytes == 0) {
                            std::cout << "Connection closed by client.\n";
                        // If we got -1 from "recv()".
                        } else {
                            raise_error("recv");
                        }

                        close(client_fd);
                        obj_connections.delete_con(i);
                    // If we got some data.
                    } else {
                        std::string data;

                        if ((data = resolveQuery(buffer, unix_sock)).empty())
						{
							std::cout << "Couldn't find any query in this data packet.\n";
						} else {
                            std::cout << data << "\n";
                        }
                    }
                }
            }
        }

        if (key_pressed('q'))
            break;
    }

    return 0;
}
