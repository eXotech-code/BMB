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
        std::cerr << "ERROR: query does not begin with proper keyword.\n";

        return -1;
    }

    return 0;
}

struct idarg get_id(std::string *query, size_t type_location) {
    // Cut arguments inside '()'
    size_t arg_begin = query->find("(", type_location);
    size_t arg_end = query->find(")", arg_begin);
    struct idarg id_and_last_bracket;
    id_and_last_bracket.arg_end = arg_end;
    std::string arg = query->substr(arg_begin + 1, arg_end - arg_begin -1);

    // Location in arg after HTML encoded space.
    size_t end_of_space = arg.find("%20") + 3;
    std::string id_str = arg.substr(end_of_space, arg.length() - end_of_space);
    int id = std::stoi(id_str);

    return id_and_last_bracket;
}

Post::Post(int post_id) {
    id = post_id;
}

// Those resolver function are only temporary.
// For now they add fake data to object.
// Later they will make calls to main module for data.
void Post::getTitle() {
    title = std::to_string(id);
}

void Post::getDate() {
    date = "January 1, 1970 at 0:00";
}

void Post::getDescription() {
    description = "This is description for post under id: " + std::to_string(id);
}

void Post::getContent() {
    content = "Lorem ipsum dolor sit amet, consectetur adipiscing elit."
        "Sed nulla libero, iaculis a dignissim quis, elementum sed quam."
        "Aenean pharetra congue turpis, quis interdum arcu volutpat congue."
        "Duis placerat urna vitae euismod volutpat.";
}

Project::Project(int project_id) {
    id = project_id;
}

void *resolve_query(std::string query) {
    std::string post_type = "post(";
    std::string project_type = "project(";
    size_t type_location;

    // If the query is in the wrong format, raise an error.
    if (unpack_query(&query) == -1) {
        return nullptr;
    }

    // Structure containing results of query.
    void *data = nullptr;

    while (query != "") {
        // Try to find either post or project type.
        if ((type_location = query.find(post_type)) != -1) {
            struct idarg id_and_last_bracket = get_id(&query, type_location);
            data = new Post(id_and_last_bracket.id);
            query = query.substr(0, query.length() - id_and_last_bracket.arg_end);
            std::cout << query << "\n";
        }

        if ((type_location = query.find(project_type)) != -1) {
            data = new Project(get_id(&query, type_location).id);
        }
    }

    return data;
}
