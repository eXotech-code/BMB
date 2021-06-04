#pragma once
#ifndef QUERIES_H

#include <string>
#include <iostream>

/* This file contains all of the logic
 * needed to parse HTTP requests and queries
 * from the frontend. */

// Returns query extracted from raw buffer
std::string extract_query(std::string b);

// Retrieve id from argument field in query
int get_id(const std::string &query);

/* Parses the query and makes calls to resolver functions.
 * It returns a pointer to struct with results */
int resolve_query(char *query); // For now it just return 0 on succes and -1 on error

#endif /* QUERIES_H */
