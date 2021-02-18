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

#endif /* QUERIES_H */
