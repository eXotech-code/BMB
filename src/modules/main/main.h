#pragma once
#ifndef MAIN_H

#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>
#include <map>
#include "../../defines.h"
#include "../sockets.h"

// Client that connects to main module.
class Client {
public:
     /* Assigns client's socket to it's member variable. */
    explicit Client(int cl_fd);
    int getFd() const;
    /* Handles connection with client.
     * Returns 0 when client disconnects from server
     * or -1 on error. */
    int handleCon();
    /* Gets the whole data packet even if it is bigger than
     * size of the buffer. Returns a string containing message from
     * client. On error it returns an empty string. */
    std::string getData(ssize_t bytes, std::vector<char> buff) const;
    void sendData(const std::string &data) const;
    /* Closes file descriptor associated with
     * itself */
    ~Client();
private:
    int fd;
    std::map<int, std::string> identity_map {
            {1, "frontend"},
            {2, "database"},
            {3, "markdown-compiler"},
            {4, "github"}
        };
    std::string identity;
};

// Class representing an UNIX socket server.
class Server {
public:
    Server();
    /* Gets socket's file descriptor, binds it and
     * sets it up for listening. */
    int start();
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
    /* Accepts a new connection and returns file
     * descriptor of client's socket */
     int accept() const;

private:
    int fd; // Socket file descriptor.
};

#endif /* MAIN_H */
