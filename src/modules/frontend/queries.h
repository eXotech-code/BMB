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

// Removes unnecessary stuff from the beginning and end of query.
void unpack_query(std::string *query);

/* Parses the query and makes calls to resolver functions.
 * It returns a pointer to struct with results */
void *resolve_query(std::string query);

// Query resolvers

// Post class with resolver functions corresponding to different fields in the query.
class Post
{
    public:
        // We fetch the post's data based on an individual ID.
        Post(int post_id);
        // Resolver functions
        std::string rTitle();
        std::string rDate();
        std::string rDescription();
        std::string rContent();
    private:
        int id;
        std::string title;
        std::string date;
        std::string description;
        std::string content;
};

class Project
{
    public:
        Project(int project_id);
        // Resolver functions
    private:
        std::string name;
        std::string description;
        int progress;
        // struct issue *kanban; <--- this will be there after GitHub module will be done.
};

#endif /* QUERIES_H */
