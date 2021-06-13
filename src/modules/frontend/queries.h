#pragma once
#ifndef QUERIES_H

#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <sys/socket.h>
#include <sys/un.h>
#include <cstring>
#include <cstdio>

/* This file contains all of the logic
 * needed to parse HTTP requests and queries
 * from the frontend. */

#define SOCK_PATH "~/bmb_socket"

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

// Sending requests to main module and populating struct array with data.
int getClientSocket(); // Returns a connected UNIX socket.
int sendToMain(const char *message, const int &sock); // Send a message to main through UNIX sockets.
int sendRequest(struct post *po, const int &sock);
int sendRequest(struct project *pr, const int &sock);
int sendRequest(struct issue *is, const int &sock); // Struct will already have "project_id" field filled.

/* Compares query to list of supported queries and sends appropriate request
 * to the main module. Then, it returns a JSON string with results. */
std::string resolveQuery(char *query, const int &sock); // Returns empty string on error.

#endif /* QUERIES_H */
