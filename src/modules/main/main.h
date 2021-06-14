#pragma once
#ifndef MAIN_H

#include <system_error>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>
#include <optional>
#include "../../defines.h"

// Client that connects to main module.
class Client {
public:
     /* Assigns client's socket to it's member variable. */
    explicit Client(int cl_fd);
    std::string get_data() const;
    void sendData(const std::string &data) const;
    /* Closes file descriptor associated with
     * itself */
    ~Client();
private:
    int fd;
};

// Class representing an UNIX socket server.
class Server {
public:
    /* Gets socket's file descriptor, binds it and
     * sets it up for listening. */
    Server();
    /* Finds socket file descriptor.
     * Returns -1 on error. */
    static int sock();
    /* Defines socket features, removes old socket
     * and binds new socket to it's address.
     * Returns -1 on error. */
    int bindSock() const;
    /* Set up listening for incoming
     * connections. As always, returns -1
     * on error. */
    int listen() const;
    /* Accepts a new connection and creates a Client object
     * with its file descriptor */
     Client *accept() const;

private:
    int fd; // Socket file descriptor.
};

#endif /* MAIN_H */
