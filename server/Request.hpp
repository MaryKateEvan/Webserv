#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "../webserv.h"

/// @brief Takes the request from a client and parses it.
class Request
{
	private:
		/*args*/
	public:
		Request(const std::string& request);
		~Request();
		Request(const Request& copy);
		Request& operator=(const Request& copy);
};

#endif /*REQUEST_HPP*/