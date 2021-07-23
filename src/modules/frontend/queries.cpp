#include "queries.h"

std::string textFragment(const std::string &txt, const std::string &search_str, char end_c) {
    std::string::size_type beg, end;

    if ((beg = txt.find(search_str)) == std::string::npos) // If search pattern not found...
        return "";

    beg += search_str.length();
    end = txt.find(end_c, beg);

    return txt.substr(beg, end - beg);
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

Queries::Queries(int main) {
    main_sock = main;
}

std::string Queries::isolate(const std::string &b) {
    // Find beginning and end of API request.
    std::string search_str = "/api/";

    return textFragment(b, search_str);
}

void Queries::add(std::string query, struct pollfd &cl_pfd) {
    /* Add this query to queue linked with socket file descriptor
     * that belongs to the client that sent it. */
    queue.push_back({cl_pfd.fd, std::move(query), ""});

    /* Set socket that sent this query to POLLOUT.
	 * So that we know when we can send our response to it. */
    cl_pfd.events = POLLOUT;
}

int Queries::find(const std::string& request) {
    for (int i = 0; i < queue.size(); i++) {
        // DEBUG
        std::cout << request << " : " << queue[i].request << "\n";

        if (queue[i].request == request)
            return i;
    }

    return -1;
}

void Queries::remove(int fd) {
    for (auto i = queue.begin(); i < queue.end(); i++) {
        if (i->fd == fd)
            queue.erase(i);
    }
}

std::string *Queries::separate(char *message) {
    // If this request didn't come from main, we have a problem.
    if (message[0] != '0') {
        std::cerr << "Could not invalidate response origin. Queries::separate()\n";
        return nullptr;
    }

    // Convert message to string.
    std::string m(message);

    auto *res = new std::string[2];

    res[0] = textFragment(m, "Q=");
    res[1] = textFragment(m, "R=", '}');

    return res;
}

int Queries::addResponse(char *buff, struct pollfd &main_pfd, Queries &instance) {
    std::string *message = Queries::separate(buff);
    // If separate() returned a nullptr or we got a invalid response.
    if (!message || message[1].empty()) {
        std::cerr << "Failed to separate request string from response string.\n";
        return -1;
    }

    // Find where to put the response by looking it up with request string.
    int i = instance.find(message[0]);
    if (i == -1) {
        std::cerr << "Not possible to lookup request in queue.\n";
        return -1;
    }

    // Add response to that request.
    instance.queue[i].response = message[1];

    return 0;
}

std::string Queries::getResponse(int fd) {
    for (const auto &i : queue) {
        if (i.fd == fd)
            return i.response;
    }

    return "";
}

int Queries::resolve(char *buff, struct pollfd &cl_pfd, Queries &instance) {

	// First, convert char array into string
	std::string b(buff);

	// Isolate api path from the request
	std::string api_call = isolate(b);
	if (api_call.empty())
	    return -1;

	/* Create appropriate struct according to which API is
	 * frontend trying to access. */

	if (api_call == "posts") {
        auto *data = new struct post[BUFF_SIZE];
        if (sendRequest(data, instance.main_sock) == -1)
            return -1;
    }

	if (api_call == "projects") {
        auto *data = new struct project[BUFF_SIZE];
        if (sendRequest(data, instance.main_sock) == -1)
            return -1;
    }

	// This one's different, because project id will be in the url.
	if (api_call.find("issues/") != std::string::npos) {
	    auto *data = new struct issue[BUFF_SIZE];

	    // Fill all structs in array with project id_numbers.
        initializeIssues(BUFF_SIZE, api_call, data);
        if (sendRequest(data, instance.main_sock) == -1)
            return -1;
	} else {
        return -1; // API call text has wrong format or that API endpoint is not supported.
    }

	// Add this query to queue.
    instance.add(api_call, cl_pfd);

	return 0;
}