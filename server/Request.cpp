#include "Request.hpp"

/* -------------------------------------------------------------------------- */
/*                           Orthodox Canonical Form                          */
/* -------------------------------------------------------------------------- */

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

Request::Request()
{
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
		this->_fd = copy._fd;
		this->_client_ip = copy._client_ip;
		// this->_method = copy._method;
		// this->_content_len = copy._content_len;
		// this->_port = copy._port;
		// this->_body_bytes_read = copy._body_bytes_read;
		// this->_header_parsed = copy._header_parsed;
		// this->_finished_reading = copy._finished_reading;
		// this->_file_path = copy._file_path;
		// this->_host = copy._host;
	}
}

Request&	Request::operator=(const Request &copy)
{
	// std::cout << "Request Copy Assignment called" << std::endl;
	if (this != &copy)
	{
		this->_fd = copy._fd;
		this->_client_ip = copy._client_ip;
}
	return (*this);
}

/* -------------------------------------------------------------------------- */
/*                                  Functions                                 */
/* -------------------------------------------------------------------------- */

int		Request::read_chunk(std::vector<char> buffer, int bytes_read)
{
	if (_finished_reading == true)
		return (0);
	if (bytes_read < 0)
	{
		return (-1);
		}
	if (bytes_read == 0)
	{
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
			// std::cout << "HEADER:\n" << _header << std::endl;
			if (fill_in_request() == 1)
				return (-1);
			return (0);
		}
	}
	if (_header_parsed && _content_len == 0)
	{
		// std::cout << "HEADER:\n" << _header << std::endl;
		if (fill_in_request() == 1)
			return (-1);
		_finished_reading = true;
		return (0);
	}
	return (1);
}

int	Request::fill_in_request(void)
{
	std::time_t	now = std::time(NULL);
	std::tm local_tm = *std::localtime(&now);
	char	buffer[100];
	std::string	format = "[%d/%b/%Y:%H:%M:%S %z]";
	if (std::strftime(buffer, sizeof(buffer), format.c_str(), &local_tm))
	{
		_request_received_time = std::string(buffer);
	}
	else
	{
		_request_received_time = "Error-Formating-Time";
	}
	std::string::size_type pos = _header.find('\n');
	if (pos != std::string::npos) 
	{
		_request_line = _header.substr(0, pos -1);
	}
	else
	{
		_request_line = "Error-Finding-Request_Line";
	}
	_CLF_line = _client_ip + "\t" + std::to_string(_fd) + "\t- " + _request_received_time + "\t\"" + _request_line + "\"";

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
		return (1);
	std::string::size_type	http = _header.find(" HTTP/");
	if (method == std::string::npos || http == std::string::npos)
		return (1);
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
		std::string	file_extention = MimeTypes::getInstance().get_file_extension(con_type);
		std::string body_str(_accumulated_request.begin(), _accumulated_request.end());
		_post_files["unknown" + file_extention] = body_str;
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

std::string	Request::get_client_ip(void) const
{
	return (_client_ip);
}

std::string	Request::get_request_received_time(void) const
{
	return (_request_received_time);
}

std::string	Request::get_request_line(void) const
{
	return (_request_line);
}

void	Request::set_fd(size_t fd)
{
	_fd = fd;
}

void	Request::set_client_ip(uint32_t ip_binary)
{
	std::string	ip_str;

	for (int i = 0; i < 4; i++)
	{
		if (i != 0)
			ip_str += ".";
		ip_str += std::to_string((ip_binary >> (8 * i)) & 0xFF);
	}
	_client_ip = ip_str;
}

void	Request::reset()
{
	_method = -1;
	_file_path.clear();
	_content_len = 0;
	_host.clear();
	_port = -1;
	_content_type.clear();
	_header.clear();
	_accumulated_request.clear();
	_body_bytes_read = 0;
	_header_parsed = false;
	_finished_reading = false;
	_request_received_time.clear();
	_request_line.clear();
	_post_files.clear();
	_CLF_line.clear();
}

std::string	Request::get_CLF_line(void) const
{
	return (_CLF_line);
}
