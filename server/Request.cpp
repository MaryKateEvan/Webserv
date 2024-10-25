#include "Request.hpp"

/* -------------------------------------------------------------------------- */
/*                           Orthodox Canonical Form                          */
/* -------------------------------------------------------------------------- */

Request::Request(size_t fd)
{
	std::cout << "Request Constructed" << std::endl;
	_fd = fd;
	_method = -1;
	_content_len = 0;
	_port = -1;
	_body_bytes_read = 0;
	_header_parsed = false;
	_finished_reading = false;
}

Request::Request()
{
	std::cout << "Request Constructed" << std::endl;
	_method = -1;
	_content_len = 0;
	_port = -1;
	_body_bytes_read = 0;
	_header_parsed = false;
	_finished_reading = false;
}

Request::~Request()
{
	// std::cout << "Request Default Destructor called" << std::endl;
}

Request::Request(const Request& copy)
{
	// std::cout << "Request Copy Constructor called" << std::endl;
	if (this != &copy)
	{
	}
}

Request&	Request::operator=(const Request &copy)
{
	// std::cout << "Request Copy Assignment called" << std::endl;
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
			std::cout << "HEADER:\n" << _header << std::endl;
			if (fill_in_request() == 1)
				return (1);
			return (0);
		}
	}
	if (_header_parsed && _content_len == 0)
	{
		std::cout << "HEADER:\n" << _header << std::endl;
		if (fill_in_request() == 1)
			return (1);
		_finished_reading = true;
		return (0);
	}
	return (1);
}

int	Request::fill_in_request(void)
{
	std::string::size_type	method;
	if (_header.compare(0, 3, "GET") == 0)
	{
		_method = GET;
		method = _header.find("GET ") + 4;
	}
	else if (_header.compare(0, 4, "POST") == 0)
	{
		_method = POST;
		if (process_post() == 1)
			return (1);
		method = _header.find("POST ") + 5;
	}
	else if (_header.compare(0, 6, "DELETE") == 0)
	{
		_method = DELETE;
		method = _header.find("DELETE ") + 7;
	}
	else
		throw NoMethodFoundException(_header);
	std::string::size_type	http = _header.find(" HTTP/");
	//replace exception
	if (method == std::string::npos || http == std::string::npos)
		throw NoMethodFoundException(_header);
	_file_path = _header.substr(method, http - method);
	return (0);
}


int	Request::process_post(void)
{
	size_t	con_type_begin = _header.find("Content-Type: ");
	if (con_type_begin == std::string::npos)
		return (1);
	con_type_begin += 14;
	std::string	temp = _header.substr(con_type_begin);
	size_t	semicolon_pos = temp.find(';');
	if (semicolon_pos == std::string::npos)
		return (1);
	std::string con_type= temp.substr(0, semicolon_pos);
	_content_type = con_type;
	if (con_type == "multipart/form-data")
	{
		size_t	boundary_pos = temp.find("boundary=");
		if (boundary_pos == std::string::npos)
			return (1);
		std::string	boundary = "--" + temp.substr(boundary_pos + 9);
		std::string body_str(_accumulated_request.begin(), _accumulated_request.end());
		std::vector<std::string>	temp = tokenize(body_str, boundary);
		for (size_t i = 0; i < temp.size(); ++i)
		{
			fill_string_to_map(temp[i]);
		}
	}
	else
	{
		//! Replace the unknown filename with something like unknow.fileending or random.fileending, getting the ending from the mimetype map that is currently in the server class
		std::string body_str(_accumulated_request.begin(), _accumulated_request.end());
		_post_files["unknown"] = body_str;
	}
	return (0);
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

bool	Request::correct_body_size(size_t max_size)
{
	if (_header_parsed == false || max_size == 0)
		return (true);
	if (max_size < _content_len)
		return (false);
	return (true);
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