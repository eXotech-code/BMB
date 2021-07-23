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

/* Returns a fragment of text starting with
 * some string and ending with a space */
std::string textFragment(const std::string &txt, const std::string &search_str, char end_c = ' ');

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
    explicit Queries(int main); // Assigns socket descriptor of main module;
    static std::string isolate(const std::string &b); // Isolate API request type from the rest of the buffer.
    void add(std::string query, struct pollfd &cl_pfd);
    int find(const std::string& request); // Find which item in queue contains that request.
    void remove(int fd); // Removes a query from queue.
    static std::string *separate(char *message); // Separates request from response.
    static int addResponse(char *buff, struct pollfd &main_pfd, Queries &instance); // Adds response from main based on request contents.
    std::string getResponse(int fd); // Looks up response from main in queue.
    /* Compares query to list of supported queries and sends appropriate request
     * to the main module. Then it adds it to queue and sets appropriate socket to POLLOUT. */
    static int resolve(char *buff, struct pollfd &cl_pfd, Queries &instance);
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
