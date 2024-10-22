#include "Request.hpp"

/* -------------------------------------------------------------------------- */
/*                           Orthodox Canonical Form                          */
/* -------------------------------------------------------------------------- */

// Request::Request(const std::string& request, int fd)
// {
// 	std::cout << "Request Default Constructor called" << std::endl;
// 	_fd = fd;
// 	std::string::size_type	method;
// 	if (request.compare(0, 3, "GET") == 0)
// 	{
// 		_method = GET;
// 		method = request.find("GET ") + 4;
// 	}
// 	else if (request.compare(0, 4, "POST") == 0)
// 	{
// 		_method = POST;
// 		if (process_post(request) == 1)
// 			std::cout << "RETURNED 1\n";
// 		method = request.find("POST ") + 5;
// 	}
// 	else if (request.compare(0, 6, "DELETE") == 0)
// 	{
// 		_method = DELETE;
// 		method = request.find("DELETE ") + 7;
// 	}
// 	else
// 		throw NoMethodFoundException(request);
// 	std::string::size_type	http = request.find(" HTTP/");
// 	//replace exception
// 	if (method == std::string::npos || http == std::string::npos)
// 		throw NoMethodFoundException(request);
// 	_file_path = request.substr(method, http - method);
// }

Request::Request(size_t fd)
{
	_fd = fd;
	_method = -1;
	_content_len = 0;
	_port = -1;
	_body_bytes_read = 0;
	_header_parsed = false;
	_finished_reading = false;

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

int		Request::read_chunk(std::vector<char> buffer, int bytes_read)
{
	//! Make the main loop check if the content_len variable is bigger than the allowed one for that server, in that case we would send an error
	//! basically if this function returns something that isnt 0 or 1 send an error to the client
	if (_finished_reading == true)
		return (0);
	if (bytes_read < 0)
	{
		// std::cerr << "Error reading from fd " << fds[i].fd << std::endl;
		// close(fds[i].fd);
		// fds[i].fd = -1;
		return (-1);
		}
	if (bytes_read == 0)
	{
		// std::cout << "Client on fd " << fds[i].fd << " disconnected" << std::endl;
		// close(fds[i].fd);
		// fds[i].fd = -1;
		return (-2);
	}
	_accumulated_request.insert(_accumulated_request.end(), buffer.begin(), buffer.begin() + bytes_read);
	std::string temp_request(_accumulated_request.begin(), _accumulated_request.end());
	if (!_header_parsed)
	{
		size_t	header_end = temp_request.find("\r\n\r\n");
		if (header_end != std::string::npos)
		{
			_header_parsed = true;
			_header = temp_request.substr(0, header_end + 4);
			//! Accumulated request is only the body now
			_accumulated_request.erase(_accumulated_request.begin(), _accumulated_request.begin() + header_end + 4);
			size_t content_len_pos = _header.find("Content-Length: ");
			if (content_len_pos != std::string::npos)
			{
				size_t	content_length_start = content_len_pos + 16;
				size_t	content_length_end = _header.find("\r\n", content_length_start);
				std::string content_length_str = _header.substr(content_length_start, content_length_end - content_length_start);
				_content_len = std::stoul(content_length_str);
			}
			size_t	host_pos = _header.find("Host: ");
			if (host_pos != std::string::npos)
			{
				size_t	host_start = host_pos + 6;
				size_t	host_end = _header.find("\r\n", host_start);
				_host = _header.substr(host_start, host_end - host_start);
			}
			else
				return (-3);
			size_t colon_pos = _host.find(':');
			if (colon_pos != std::string::npos)
				_port = std::stoi(_host.substr(colon_pos + 1));
		}
	}
	if (_header_parsed)
		_body_bytes_read = _accumulated_request.size();
	if (_header_parsed && _content_len > 0)
	{
		if (_body_bytes_read >= _content_len)
		{
			_finished_reading = true;
			return (0);
		}
	}
	if (_header_parsed && _content_len == 0)
	{
			std::cout << "HEADER:\n" << _header << std::endl;
		_finished_reading = true;
		return (0);
	}
	return (1);
}

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

int	Request::get_port(void) const
{
	return (_port);
}

bool	Request::get_finished_reading(void) const
{
	return (_finished_reading);
}

std::string	Request::get_host(void) const
{
	return (_host);
}