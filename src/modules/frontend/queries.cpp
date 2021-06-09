#include "queries.h"

std::string isolate(std::string b) {
    // Find beginning and end of API request.
    std::string search_str = "/api/";
    std::string::size_type beg, end;

    if ((beg = b.find(search_str)) == std::string::npos) // If search pattern not found...
        return "";

    beg += search_str.length(); // Beginning of API request is after "/api/".
    end = b.find(' ', beg);

    return b.substr(beg, end - beg - 1);
}

std::string resolveQuery(char *buff) {

	// First, convert char array into string
	std::string b(buff);

	// Isolate api type from the request
	std::string api_type = isolate(b);

	return api_type;
}
