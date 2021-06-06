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

// Returns query extracted from raw buffer
std::string extract_query(const std::string &b);

// Retrieve id from argument field in query
int get_id(const std::string &query, std::string::size_type &end_id_pos);

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

// Replaces all occurences of a string with a space.
void removeOccurences(const std::string &seq, std::string &s);

std::vector<std::string> separate_args(std::string params);
// This will only make requests for posts for now
struct post *compile_request(std::vector<std::string> args);

/* Parses the query and makes calls to resolver functions.
 * It returns a pointer to struct with results */
int resolve_query(char *query); // For now it just return 0 on success and -1 on error

#endif /* QUERIES_H */
