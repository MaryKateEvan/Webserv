#include "utils.h"


std::vector<std::string>	tokenize(const std::string& str, const std::string& delim)
{
	std::vector<std::string>	tokens;
	size_t	start = 0;
	size_t	end = str.find(delim);

	while (end != std::string::npos)
	{
		tokens.push_back(str.substr(start, end - start));
		start = end + 1;
		end = str.find(delim, start);
	}

	tokens.push_back(str.substr(start));

	return (tokens);
}