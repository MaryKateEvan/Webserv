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
	std::cout << "CONTENT typE: " + con_type << std::endl;
	if (con_type == "multipart/form-data")
	{
		std::string	boundary = "--" + line.substr(end + 12);
		// std::cout << "Boundary = " + boundary << std::endl;
		std::istringstream	stream2(request);
		while (std::getline(stream2, line) && line != "\r") {}
		while (std::getline(stream2, line))
		{
			if (line.find(boundary) != std::string::npos)
			{
				if (last_line.find(boundary) != std::string::npos)
					break;
			}
				std::string file_name;
				std::getline(stream2, line);
				pos = line.find("filename=\"");
				if (pos == std::string::npos)
					file_name = "unknown";
				else
					file_name = line.substr(pos + 10, line.length() - (pos + 10) - 2);
				std::cout << "filename: " + file_name << "|" << std::endl;
				std::getline(stream2, line);
				std::getline(stream2, line);
				std::string content;
				while (std::getline(stream2, line))
				{
					if (line.find(boundary) != std::string::npos)
						break;
					content += line + "\n";
				}
			_post_files[file_name] = content; 
		}
		last_line = line;
	}
	return (0);
}

// void	Server::load_mime_types(const std::string& file_path)
// {
// 	std::ifstream file(file_path);
// 	if (!file.is_open())
// 		throw OpenFailedException(_name, file_path);
// 	std::string	line;
// 	while(std::getline(file, line))
// 	{
// 		std::istringstream	ss(line);
// 		std::string			extension;
// 		std::string			mime_type;

// 		if (std::getline(ss, extension, '\t') && std::getline(ss, mime_type))
// 			_mime_types[extension] = mime_type;
// 	}
// 	file.close();
// }


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
