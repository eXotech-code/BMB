#include "main.h"
#include "../sockets.h"

// Definitions of "Server" class members
Server::Server() {
    if ((fd = sock()) == -1)
        throw std::system_error(errno, std::system_category());

    if (bindSock() == -1)
        throw std::system_error(errno, std::system_category());

    if (listen() == -1)
        throw std::system_error(errno, std::system_category());
}

int Server::sock() {
    int sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    return sock_fd;
}

int Server::bindSock() const {
    struct sockaddr_un server_info = { AF_UNIX, SOCK_PATH };
    unsigned long len = strlen(server_info.sun_path) + sizeof(server_info.sun_family);

    // Remove old socket if it exists and bind address to new socket.
    unlink(server_info.sun_path);
    return bind(fd, (struct sockaddr *)&server_info, len);
}

int Server::listen() const {
    return ::listen(fd, BACKLOG);
}

Client *Server::accept() const{
    struct sockaddr_un client_info = { 0 };
    socklen_t s = sizeof(client_info);
    return new Client(::accept(fd, (struct sockaddr *)&client_info, &s));
}

// Definitions of "Client" class members.
Client::Client(int cl_fd) {
    fd = cl_fd;
}

std::string Client::get_data() const{
    ssize_t bytes; // Received bytes.
    std::string data;
    std::vector<char> buff(BUFF_SIZE);

    do {
        if ((bytes = recv(fd, &buff[0], buff.size(), 0)) == -1) {
            perror("Error while trying to receive data from other module");
            return "";
        }

        data.append(buff.cbegin(), buff.cend());

    } while (bytes == BUFF_SIZE);

    return data;
}

void Client::sendData(const std::string &data) const{
    if (send(fd, data.c_str(), data.size(), 0) == -1) {
        throw std::system_error(errno, std::system_category());
    }
}

Client::~Client() {
    close(fd);
}

int main() {
    // Create server
    std::optional<Server> serv;
    try {
        serv.emplace();
    } catch (std::system_error &e) {
        std::cerr << "Could not instantiate a server: " << e.what() << "\n";
        exit(1);
    }

    // Just to check if everything works correctly.
    while (true) {
        Client *c = serv->accept();

        // Send message from client to stdout
        std::string data = c->get_data();
        std::cout << data << "\n";
        /*try {
            c->sendData(data);
        } catch (std::system_error &e) {
            std::cerr << "Not possible to send data at this time: " << e.what() << "\n";
            exit(1);
        }*/
    }

    return 0;
}