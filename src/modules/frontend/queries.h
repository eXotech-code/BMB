#include <map>
#include <string>
#include <sstream>
#include <iostream>

/* This file contains all of the logic
 * needed to parse HTTP requests and queries
 * from the frontend. */

// Checks if this is a valid query and extracts it from the HTTP request.
std::string extract_query(std::string request) {
    // Check if the query starts with "OPTIONS"
    std::stringstream ss(request);
    std::string word;

    // Iterate over words in the request.
    while (ss >> word) {
        /* If the query starts with "/graphql"
         * that means we found what we were looking for */
        if (word.find("/graphql") == 0) {
           return word;
        }
    }

    return {};
}
