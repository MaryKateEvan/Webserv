#include "Request.hpp"

/* -------------------------------------------------------------------------- */
/*                           Orthodox Canonical Form                          */
/* -------------------------------------------------------------------------- */

Request::Request(const std::string& request, int fd)
{
	std::cout << "Request Default Constructor called" << std::endl;
	_fd = fd;
	std::string::size_type	method;
	if (request.compare(0, 3, "GET") == 0)
	{
		_method = GET;
		method = request.find("GET ") + 4;
	}
	else if (request.compare(0, 4, "POST") == 0)
	{
		_method = POST;
		//! Add an extraction for _content_type
		method = request.find("POST ") + 5;
	}
	else if (request.compare(0, 6, "DELETE") == 0)
	{
		_method = DELETE;
		method = request.find("DELETE ") + 7;
	}
	else
		throw NoMethodFoundException(request);
	std::string::size_type	http = request.find(" HTTP/");
	//replace exception
	if (method == std::string::npos || http == std::string::npos)
		throw NoMethodFoundException(request);
	_file_path = request.substr(method, http - method);
}

Request::~Request()
{
	std::cout << "Request Default Destructor called" << std::endl;
}

Request::Request(const Request& copy)
{
	std::cout << "Request Copy Constructor called" << std::endl;
	if (this != &copy)
	{
	}
}

Request&	Request::operator=(const Request &copy)
{
	std::cout << "Request Copy Assignment called" << std::endl;
	if (this != &copy)
	{
	}
	return (*this);
}

/* -------------------------------------------------------------------------- */
/*                                  Functions                                 */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/*                                Getter/Setter                               */
/* -------------------------------------------------------------------------- */

int	Request::get_method(void) const
{
	return (_method);
}

int	Request::get_fd(void) const
{
	return (_fd);
}

std::string	Request::get_file_path(void) const
{
	return (_file_path);
}

std::string	Request::get_content_type(void) const
{
	return (_content_type);
}
