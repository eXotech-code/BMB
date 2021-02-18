#include "queries.h"

std::string extract_query(std::string request) {
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

// This is just a mockup of what would a query return.
struct post *Query::allPosts(int amount) {
    // TODO: Communicate to database module and get the data from there.
    struct post temp_post;

    struct post *temp_post_list;
    temp_post_list = new struct post[amount];

    temp_post.title = "Temp";
    temp_post.description = "Lorem ipsum dolor.";
    temp_post.date = "January 1, 1970 at 0:00";
    temp_post.content = "I love roses because they smell great. Whatever. This is just a test content.";

    for (int i = 0; i < amount; i++) {
        temp_post_list[i].title = temp_post.title;
        temp_post_list[i].description = temp_post.description;
        temp_post_list[i].date = temp_post.date;
        temp_post_list[i].content = temp_post.content;
    }

    return temp_post_list;
}
