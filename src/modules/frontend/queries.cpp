#include "queries.h"

std::string extract_query(const std::string &b)
{
	std::string::size_type pos = b.find("query="); // Position at which query begins
	if (pos != std::string::npos)
		return b.substr(pos, b.find("}}", pos) - pos + 2);
	return "";
}

int get_id(const std::string &query, std::string::size_type &end_id_pos)
{
	// Fragment of text that always precedes id number
	std::string pre_id = "(id:%20";
	// Amount of characters that the id consists of
	const int id_len = 4; // Example: 0001

	// Beginning of id number
	std::string::size_type b = query.find(pre_id) + pre_id.size();
	if (b == std::string::npos)
	    return -1;

	end_id_pos = b + id_len;

	return std::stoi(query.substr(b, id_len));
}
void removeOccurences(const std::string &seq, std::string &s)
{
    size_t start_pos = 0;
    while ((start_pos = s.find(seq, start_pos)) != std::string::npos)
    {
        s.replace(start_pos, seq.length(), " ");
        start_pos += seq.length();
    }
}

std::vector<std::string> separate_args(std::string params)
{
    // Remove spaces
    std::string url_space = "%20";
    removeOccurences(url_space, params);

    std::vector<std::string> args;

    // Separate arguments into different places in the vector.
    std::stringstream ss(params);
    std::string word;
    while (ss >> word)
    {
        args.push_back(word);
        if (ss.peek() == ',')
            ss.ignore();
    }

    return args;
}

struct post *compile_request(std::vector<std::string> args)
{

}

int resolve_query(char *buff)
{
	// First, convert char array into string
	std::string b(buff);
	std::string q = extract_query(b);

	// Query is not present or has wrong formatting
	if (q.empty())
		return -1;

	std::cout << q << "\n";

	std::string::size_type end_id_pos = 0; // Location of the end of id field.
	int id = get_id(q, end_id_pos);

	// Id field not present
	if (id == -1)
	    return -1;

	// Query parameters
    std::string params = q.substr(end_id_pos+2);

    // DEBUG: Just checking if separating arguments works as expected
    std::vector<std::string> p_separated = separate_args(params);
    for (std::vector<std::string>::const_iterator i = p_separated.begin(); i < p_separated.end(); i++)
    {
        std::cout << "from vec: " << *i << "\n";
    }

	return 0;
}
