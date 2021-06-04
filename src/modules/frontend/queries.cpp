#include "queries.h"

std::string extract_query(std::string b)
{
	std::string::size_type pos = b.find("query="); // Position at which query begins
	if (pos != std::string::npos)
		return b.substr(pos, b.find("}}", pos) - pos + 2);
	return "";
}

int get_id(const std::string &query)
{
	// Fragment of text that always precedes id number
	std::string pre_id = "(id:%20";
	// Amount of characters that the id consists of
	const int id_len = 4; // Example: 0001

	// Beginning of id number
	std::string::size_type b = query.find(pre_id) + pre_id.size();
	std::cout << query.substr(b, id_len) << " " << std::stoi(query.substr(b, id_len)) << "\n";

	return 0;
}

int resolve_query(char *buff)
{
	// First, convert char array into string
	std::string b(buff);
	std::string q = extract_query(b);

	// Query is not present or has wrong formatting
	if (q == "")
		return -1;

	std::cout << q << "\n";

	get_id(q);
	return 0;
}
