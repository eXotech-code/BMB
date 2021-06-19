#include "main.h"

// Definitions of "Server" class members
Server::Server() {
    fd = 0;
}

int Server::start() {
    if ((fd = sock()) == -1) {
        perror("Could not obtain a socket file descriptor.");
        return -1;
    }

    if (bindSock() == -1) {
        perror("Could not bind address of UNIX socket to file descriptor");
        return -1;
    }

    if (listen() == -1) {
        perror("Could not start listening for new connections");
        return -1;
    }

    return 0;
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

int Server::accept() const {
    struct sockaddr_un client_info = { 0 };
    socklen_t s = sizeof(client_info);

    int new_client = ::accept(fd, (struct sockaddr *)&client_info, &s);
    if (new_client == -1) {
        perror("Not possible to accept new connection");
    }

    return new_client;
}

// Definitions of "Client" class members.
Client::Client(int cl_fd) {
    fd = cl_fd;
}

int Client::getFd() const {
    return fd;
}

int Client::handleCon() {
    ssize_t bytes; // Received bytes.
    std::vector<char> buff(BUFF_SIZE);
    std::string data;

    // If the client has not disconnected, read and handle what it had sent.
    while (recv(fd, &buff[0], buff.size(), MSG_PEEK)) {

        // Establish which module is connected.
        // TODO: After we switch to polling for connections do this only once.
        identity = identity_map[buff[0] - '0'];

        if ((data = getData(bytes, buff)).empty())
            return -1;

        std::cout << identity << " sent: " << data << "\n";
    }

    std::cout << identity << " module on socket " << fd << " has disconnected.\n";

    return 0;
}

std::string Client::getData(ssize_t bytes, std::vector<char> buff) const{
    std::string data;

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
    Server serv;
    if (serv.start() == -1) {
        std::cerr << "Could not instantiate a server.\n";
        return -1;
    }

    // Just to check if everything works correctly.
    while (true) {

        // Create client and assign newly accepted fd to it
        Client c(serv.accept());
        if (c.getFd() == -1)    // If accepting failed
            return -1;

        std::cout << "New connection on socket: " << c.getFd() << "\n";

        // Receive messages
        c.handleCon();
    }

    return 0;
}
