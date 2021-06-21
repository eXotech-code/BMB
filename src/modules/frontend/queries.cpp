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

int getClientSocket() {
    int sock; // This module socket's file descriptor.
    struct sockaddr_un client = { 0 }; // Info about this socket.

    // Fill address and family info.
    client.sun_family = AF_UNIX;
    // "sun_path" is not directly assignable, so I have to use strncpy().
    std::size_t sock_path_len = std::strlen(SOCK_PATH);
    std::strncpy(client.sun_path, SOCK_PATH, sock_path_len);

    // Get file descriptor.
    if ((sock = socket(client.sun_family, SOCK_STREAM, 0)) == -1) {
        perror("Could not get a file descriptor for UNIX socket");
        return -1;
    }

    // Connect to main module.
    if ((connect(sock, (struct sockaddr *)&client, sock_path_len + sizeof(client.sun_family))) == -1) {
        perror("Could not connect to main module's socket");
        return -1;
    }
    std::cout << "Established a connection to main module.\n";

    return sock;
}

int sendToMain(const char *message, const int &sock) {
    /* Check if main closed socket on us just in case.
     * That's why we are using "MSG_NOSIGNAL" flag. */
    if (send(sock, message, strlen(message), MSG_NOSIGNAL) == -1) {
        perror("Could not send request to main");
        return -1;
    }

    return 0;
}

int sendRequest(struct post *po, const int &sock) {
    std::cout << "Fetching posts...\n";
    if (sendToMain("1 posts", sock) == -1) {
        return -1;
    }
    return 0;
}

int sendRequest(struct project *pr, const int &sock) {
    std::cout << "Fetching projects...\n";
    if (sendToMain("1 projects", sock) == -1) {
        return -1;
    }
    return 0;
}

int sendRequest(struct issue *is, const int &sock) {
    std::cout << "Fetching issues that belong to project with id = " << is[0].project_id << "\n";
    if (sendToMain(("1 issues/" + std::to_string(is[0].project_id)).c_str(), sock) == -1) {
        return -1;
    }
    return 0;
}

std::string resolveQuery(char *buff, const int &sock) {

	// First, convert char array into string
	std::string b(buff);

	// Isolate api path from the request
	std::string api_call = isolate(b);
	if (api_call.empty())
	    return "";

	/* Create appropriate struct according to which API is
	 * frontend trying to access. */
	const int buff_size = BUFF_SIZE;

	if (api_call == "posts") {
        auto *data = new struct post[buff_size];
        if (sendRequest(data, sock) == -1)
            return "";
    }

	if (api_call == "projects") {
        auto *data = new struct project[buff_size];
        if (sendRequest(data, sock) == -1)
            return "";
    }

	// This one's different, because project id will be in the url.
	if (api_call.find("issues/") != std::string::npos) {
	    auto *data = new struct issue[buff_size];

	    // Fill all structs in array with project id_numbers.
        initializeIssues(buff_size, api_call, data);
        if (sendRequest(data, sock) == -1)
            return "";
	} else {
        return ""; // API call text has wrong format or that API endpoint is not supported.
    }

	return api_call;
}