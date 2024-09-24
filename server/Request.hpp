#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "../webserv.h"

# ifndef GET
#  define GET 0
# endif /*GET*/

# ifndef POST
#  define POST 1
# endif /*POST*/

# ifndef DELETE
#  define DELETE 1
# endif /*DELETE*/

/// @brief Takes the request from a client and parses it.
class Request
{
	private:
		int			_method; // Assign one of the three defines above
		std::string	_file_path;
		std::string	_content_type;
	public:
		Request(const std::string& request);
		~Request();
		Request(const Request& copy);
		Request& operator=(const Request& copy);
};

#endif /*REQUEST_HPP*/