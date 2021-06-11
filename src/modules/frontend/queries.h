#pragma once
#ifndef QUERIES_H

#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>

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

// Sending requests to main module and populating struct array with data.
int sendRequest(struct post *po);
int sendRequest(struct project *pr);
int sendRequest(struct issue *is); // Struct will already have "project_id" field filled.

/* Compares query to list of supported queries and sends appropriate request
 * to the main module. Then, it returns a JSON string with results. */
std::string resolveQuery(char *query); // Returns empty string on error.

#endif /* QUERIES_H */
