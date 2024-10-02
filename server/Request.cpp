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
		if (process_post(request) == 1)
			std::cout << "RETURNED 1\n";
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

void	Request::fill_string_to_map(std::string input)
{
			std::istringstream	ss(input);
			std::string			line;
			size_t				line_count = 0;
			std::string			file_name;
			std::string			file_content;
			size_t				pos;

			while (std::getline(ss, line))
			{
				if (line_count == 0)
				{
					line_count++;
					continue;
				}
				if (line_count == 1)
				{
					pos = line.find("filename=\"");
					if (pos == std::string::npos)
						file_name = "unknown";
					else
						file_name = line.substr(pos + 10, line.length() - (pos + 10) - 2);
					line_count++;
					continue;
				}
				if (line_count == 2 || line_count == 3)
				{
					line_count++;
					continue;
				}
				file_content += line + "\n";
			}
			_post_files[file_name] = file_content;
}

int	Request::process_post(const std::string& request)
{
	std::istringstream	stream(request);
	std::string			line;
	std::string			last_line;
	size_t				pos;

	while (std::getline(stream, line))
	{
		// std::cout << "LINE: " + line << std::endl;
		pos = line.find("Content-Type: ");
		if (pos != std::string::npos)
			break ;
	}
	if (line.empty() || !std::getline(stream, last_line))
		return (1);
	size_t	end = line.find(';');
	if (end == std::string::npos)
		return (1);
	pos += 14;
	std::string	con_type = line.substr(pos, end - pos);
	_content_type = con_type;
	if (con_type == "multipart/form-data")
	{
		std::string	boundary = "--" + line.substr(end + 12);
		std::vector<std::string>	temp = tokenize(request, boundary);
		for (size_t i = 1; i < temp.size(); ++i)
		{
			fill_string_to_map(temp[i]);
		}
	}
	return (0);
}

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
