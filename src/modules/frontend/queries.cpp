#include "queries.h"

std::string isolate(const std::string &b) {
    // Find beginning and end of API request.
    std::string search_str = "/api/";
    std::string::size_type beg, end;

    if ((beg = b.find(search_str)) == std::string::npos) // If search pattern not found...
        return "";

    beg += search_str.length(); // Beginning of API request is after "/api/".
    end = b.find(' ', beg);

    return b.substr(beg, end - beg);
}

void initializeIssues(const int &buff_size, const std::string &api_call, struct issue *data) {
    // Id of project that those issues belong to.
    int project_id = std::stoi(api_call.substr(api_call.find('/') + 1));

    // Initialize project_id fields for all issues.
    /* Those will be used later to send individual requests
     * for issues that belong to a particular project */
    for (int i = 0; i < buff_size; i++) {
        data[i].project_id = project_id;
    }
}

int sendRequest(struct post *po) {
    std::cout << "Fetching posts...\n";
    return 0;
}

int sendRequest(struct project *pr) {
    std::cout << "Fetching projects...\n";
    return 0;
}

int sendRequest(struct issue *is) {
    std::cout << "Fetching issues that belong to project with id = " << is[0].project_id << "\n";
    return 0;
}

std::string resolveQuery(char *buff) {

	// First, convert char array into string
	std::string b(buff);

	// Isolate api path from the request
	std::string api_call = isolate(b);
	if (api_call.empty())
	    return "";

	/* Create appropriate struct according to which API is
	 * frontend trying to access. */
	const int buff_size = 64;

	if (api_call == "posts") {
        auto *data = new struct post[buff_size];
        sendRequest(data);
    }

	if (api_call == "projects") {
        auto *data = new struct project[buff_size];
        sendRequest(data);
    } else {
	    return ""; // API call text has wrong format or that API endpoint is not supported.
	}

	// This one's different, because project id will be in the url.
	if (api_call.find("issues/") != std::string::npos) {
	    auto *data = new struct issue[buff_size];

	    // Fill all structs in array with project id_numbers.
        initializeIssues(buff_size, api_call, data);
        sendRequest(data);
	}

	return api_call;
}
