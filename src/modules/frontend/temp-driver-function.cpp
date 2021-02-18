#include "queries.h"
#include <iostream>

// This is just a quick program to test if the query resolver functions work correctly.

int main() {
    Query sample_query;
    struct post *temp_post_list = sample_query.allPosts(8);

    for (int i = 0; i < 8; i++) {
        std::cout << temp_post_list[i].title << "\n";
    }

    return 0;
}
