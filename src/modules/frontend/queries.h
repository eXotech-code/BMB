#pragma once
#ifndef QUERIES_H

#include <map>
#include <string>
#include <sstream>
#include <iostream>

/* This file contains all of the logic
 * needed to parse HTTP requests and queries
 * from the frontend. */

// Checks if this is a valid query and extracts it from the HTTP request.
std::string extract_query(std::string request);

// Query resolver functions and structs.

// Post struct as per my notes.
// TODO: Make a handler for image files and add a corresponding field in the stract for hero images.
struct post
{
    std::string title;
    std::string date;
    std::string description;
    std::string content;
};

/* Query class with individual
 * resolvers for different query fields. */
class Query
{
    public:
        // Return an array of posts within a range specified in amount.
        struct post *allPosts(int amount, struct post *hints);
};

#endif /* QUERIES_H */
