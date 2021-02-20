#include "queries.h"

std::string extract_query(std::string request) {
    std::stringstream ss(request);
    std::string word;

    // Iterate over words in the request.
    while (ss >> word) {
        /* If the query starts with "/graphql"
         * that means we found what we were looking for */
        if (word.find("/graphql?") == 0) {
           return word.erase(0, 9);
        }
    }

    return {};
}

int unpack_query(std::string *query) {
    // Unpack the request.
    const std::string query_beginning = "query={";
    if (query->find(query_beginning) == 0) {
        query->erase(0, query_beginning.length());
        // Remove last bracket.
        query->erase(query->length() - 1, query->length());
    } else {
        // TODO: Examine why this code runs anyway. xD
        std::cerr << "This is not a valid query!\n";

        return 1;
    }

    return 0;
}

void *parse_query(std::string query) {
    // If the query is in the wrong format, raise an error.
    if (unpack_query(&query)) {
        return nullptr;
    }

    std::stringstream ss(query);
    std::string word;

    // Structure containing results of query.
    void *data = nullptr;

    while (ss >> word) {

    }
}

Post::Post(int post_id) {
    id = post_id;
}
