#pragma once
#ifndef QUERIES_H

#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstring>
#include <cstdio>
#include <sys/poll.h>
#include "../../defines.h"
#include "../sockets.h"

/* This file contains all of the logic
 * needed to parse HTTP requests and queries
 * from the frontend. */

// Isolate API request type from the rest of the buffer.
std::string isolate(const std::string &b);

/* Struct to be sent to the main module
 * and filed with data. */
struct post {
    unsigned int id;
    std::string title;
    std::string date;
    std::string description;
    std::string hero;
    std::string content;
};

struct project {
    unsigned int id;
    std::string name;
    std::string description;
    unsigned int progress;
    // Issues will be fetched by frontend based on id.
};

struct issue {
    unsigned int id;
    std::string title;
    std::string contents;
    unsigned int state;
    unsigned int project_id;
};

struct query {
    int fd;
    std::string request;
    std::string response;
};

/* Class responsible for keeping track of all queries
 * waiting to be handled. It adds new queries and responds to them. */
class Queries {
public:
    Queries(int main); // Assigns socket descriptor of main module;
    void add(std::string query, struct pollfd *cl_pfd);
    int remove(int i); // Removes a query from queue.
    int addResponse(std::string request, std::string response); // Adds response from main based on request contents.
    std::string getResponse(int fd); // Looks up response from main in queue.
    /* Compares query to list of supported queries and sends appropriate request
     * to the main module. Then it adds it to queue and sets appropriate socket to POLLOUT. */
    int resolve(char *buff, struct pollfd &cl_pfd);
private:
    std::vector<query> queue;
    int main_sock;
};

// Sending requests to main module and populating struct array with data.
int getClientSocket(); // Returns a connected UNIX socket.
int sendToMain(const char *message, const int &sock); // Send a message to main through UNIX sockets.
int sendRequest(struct post *po, const int &sock);
int sendRequest(struct project *pr, const int &sock);
int sendRequest(struct issue *is, const int &sock); // Struct will already have "project_id" field filled.

#endif /* QUERIES_H */
