#include "Server.hpp"

/* -------------------------------------------------------------------------- */
/*                           Orthodox Canonical Form                          */
/* -------------------------------------------------------------------------- */

/**
 * @brief Creates a webserver
 * @param server_name The name of the server
 * @param port The port the server should listen to
 * @param index_file the index file of the server (relative to the www_dir)
 * @param data_dir the directory to POST/DELETE from (might be replaced by a redirect map in the future)
 * @param www_dir the location of the webpage files 
 * @param directory_listing_enabled true or false if the directory listing should be enabled
 * @param keepalive_timeout Keepalive timeout in seconds, 0 to disable
 * @param send_timeout Send timeout in seconds, 0 to disable
 * @param max_body_size Max Body size the Server will read (the server may send bigger responses to the client), 0 to diable
 */
Server::Server(const std::string server_name, int port, const std::string index_file,
		const std::string data_dir, const std::string www_dir, bool directory_listing_enabled, size_t keepalive_timeout,
		size_t send_timeout, size_t max_body_size, std::vector<LocationData> locations)
	: _name(server_name), _index_file(index_file), _data_dir(data_dir), _www_dir(www_dir),
	_directory_listing_enabled(directory_listing_enabled), _keepalive_timeout(keepalive_timeout),
	_send_timeout(send_timeout), _max_body_size(max_body_size), _locations(locations)
{
	Logger::getInstance().log(server_name, "Constructor called", 2);
	_fd_server = socket(AF_INET, SOCK_STREAM, 0);
	if (_fd_server == -1)
		throw SocketCreationFailedException(_name);
	int opt = 1;
	if (setsockopt(_fd_server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		close(_fd_server);
		throw SetSocketOptionFailedException(_name);
	}
	// Remove this line on MacOS to prevent issues
	if (setsockopt(_fd_server, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt)) < 0)
	{
		close(_fd_server);
		throw SetSocketOptionFailedException(_name);
	}
	int	keep_idle = _keepalive_timeout;
	if (RETOPT() < 0)
	{
		close(_fd_server);
		throw SetSocketOptionFailedException(_name);
	}
	struct timeval	send_timeout_val;
	send_timeout_val.tv_sec = _send_timeout;
	send_timeout_val.tv_usec = 0;
	// Remove this line on MacOS to prevent issues
	if (setsockopt(_fd_server, SOL_SOCKET, SO_SNDTIMEO, &send_timeout_val, sizeof(send_timeout_val)) < 0)
	{
		close(_fd_server);
		throw SetSocketOptionFailedException(_name);
	}
	_address.sin_family = AF_INET;
	if (port < 0 || port > 65535)
	{
		close(_fd_server);
		throw InvalidPortException(_name, port);
	}
	if (fcntl(_fd_server, F_SETFL, O_NONBLOCK) < 0)
	{
		close(_fd_server);
		throw SetSocketOptionFailedException(_name);
	}
	_address.sin_addr.s_addr = INADDR_ANY;
	_address.sin_port = htons(port);
	if (bind(_fd_server, (struct sockaddr *)&_address, sizeof(_address)) != 0)
	{
		close(_fd_server);
		throw BindFailedException(_name, port);
	}
	if (listen(_fd_server ,SOMAXCONN) != 0)
	{
		close(_fd_server);
		throw ListenFailedException(_name);
	}
	Logger::getInstance().log(server_name, "Server is now listening on " + std::to_string(port), 2);
}

Server::~Server()
{
	Logger::getInstance().log(_name, "Destructor called", 2);
	close(_fd_server);
}

/* -------------------------------------------------------------------------- */
/*                                  Functions                                 */
/* -------------------------------------------------------------------------- */

/// @brief Takes the http request and extracts the requested resource
/// @param request Takes the request sent by the client
/// @return String of the requested resource
std::string	Server::extract_get_request(const std::string& request)
{
	std::string::size_type	get = request.find("GET ") + 4;
	std::string::size_type	http = request.find(" HTTP/");

	if (get == std::string::npos || http == std::string::npos)
		return ("");
	return (request.substr(get, http - get));
}

/// @brief Takes the string from extract request and maps it to the directoy (currently www)
/// @param file_path the return value from extract_request
/// @return The path to the requested resource in our directory
std::string	Server::map_to_directory(const std::string& file_path)
{
	if (file_path == "/")
		return (_www_dir + "/" + _index_file);
	else
		return (_www_dir + file_path);
}

std::string	Server::read_file(const std::string& file_path)
{
	std::ifstream		file(file_path);
	std::stringstream	buffer;

	buffer << file.rdbuf();
	return (buffer.str());
}

std::string	Server::process_request(const Request& req)
{
	_CLF_line = req.get_CLF_line();
	int	method = req.get_method();

	if (method == GET || method == POST)
	{
		if (req.get_file_path().find(".py") != std::string::npos)
			return (process_cgi(req));
	}
	switch (method)
	{
		case GET:
			return (process_get(req));
			break;
		case DELETE:
			return (process_delete(req));
			break;
		case POST:
			return (process_post(req));
			break;
	}
	return (send_error_message(400));
}

bool Server::uri_is_a_location(const std::vector<LocationData>& locations, const std::string& targetPath) 
{
	for (size_t i = 0; i < locations.size(); ++i)
	{
		if (locations[i].path == targetPath)
			return true;
	}
	return false;
}

// A typical HTTP response of redirection is:
/*
HTTP/1.1 302 Found
Location: https://www.google.com/
Content-Length: 0
Content-Type: text/html
Connection: close
*/
std::string	Server::redirect_to(const std::string& redir_path)
{
	std::string	response = "HTTP/1.1 302 Found\r\n";
	response += ("Location: " + redir_path + "\r\n");
	response += ("Content-Length: 0\r\n");
	response += ("Content-Type: text/html\r\n");
	response += ("Connection: close\r\n");
	_CLF_line += " " + std::to_string(302) + " 0";
	return (response);
}

std::string	Server::handle_locations(const Request& req)
{
	std::string uri = req.get_file_path();
	LocationData loc_data;

	for (size_t i = 0; i < _locations.size(); ++i)
	{
		if (_locations[i].path == uri)
			loc_data = _locations[i];
	}
	if (loc_data.redirection)
	{
		if (std::find(loc_data.allowed_methods.begin(), loc_data.allowed_methods.end(), req.get_method_in_string()) == loc_data.allowed_methods.end())
			return (send_error_message(405));
		else
			return (redirect_to(loc_data.path_to_redirect));
	}
	return (send_error_message(405));
}

std::string	Server::process_get(const Request& req)
{
	std::string	url = req.get_file_path();

	if (url != "/" && uri_is_a_location(this->_locations, url))
	{
		return (handle_locations(req));
	}

	std::string	file_path = map_to_directory(url);

	std::string	response = "HTTP/1.1 200 OK\r\n";

	if (_directory_listing_enabled == true && std::filesystem::is_directory(file_path))
	{
		if (file_path != map_to_directory("/" + _data_dir + "/") && file_path != map_to_directory("/" + _data_dir))
		{
			return (send_error_message(403));
		}
		response += "Content-Type: text/html\r\n";
		std::string	response_body = "<!DOCTYPE html>\n<html>\n<head><title>Index of " + req.get_file_path() + "</title></head>\n";
		response_body += "<body>\n<hr>\n<ul>\n";
		response_body += "    <li><a href=\"../\">Parent Directory</a></li>\n";
		for (const auto& entry : std::filesystem::directory_iterator(file_path))
		{
			response_body += "    <li><a href=\"" + _data_dir + "/" + entry.path().filename().string() + "\">" + entry.path().filename().string() + "</a></li>\n";
		}
		response_body += "</ul>\n<hr>\n</body>\n</html>\n";
		std::string	con_len_str = std::to_string(response_body.size());
		response += "Content-Length: " + con_len_str + "\r\n";
		response += "\r\n";
		response += response_body;
		_CLF_line += " " + std::to_string(200) + " " + con_len_str;
		return (response);
	}
	else if (_directory_listing_enabled == false && std::filesystem::is_directory(file_path))
	{
		return (send_error_message(403));
	}
	else if (std::filesystem::exists(file_path))
	{
		std::string	file_content = read_file(file_path);
		std::string	mime_type = MimeTypes::getInstance().get_mime_type(file_path);
		response += "Content-Type: " + mime_type + "\r\n";
		std::string	con_len_str = std::to_string(file_content.size());
		response += "Content-Length: " + con_len_str + "\r\n";
		response += "\r\n";
		response += file_content;
		_CLF_line += " " + std::to_string(200) + " " + con_len_str;
		return (response);
	}
	else
		return (send_error_message(404));
}

std::string	Server::process_delete(const Request& req)
{
	std::string	url = req.get_file_path();
	std::string	file_path = _www_dir + "/" + _data_dir + "/" + url;
	if (std::filesystem::exists(file_path))
	{
		if (std::remove(file_path.c_str()) == 0)
		{
			return (send_error_message(200));
		}
		else
			return (send_error_message(500));
	}
	else
		return (send_error_message(404));
}

std::string	Server::process_post(const Request& req)
{
	long unsigned int	failed = 0;
	for (const auto& entry : req._post_files)
	{
		const std::string&	file_name = entry.first;
		const std::string&	content = entry.second;
		std::string			full_path = _www_dir + "/" + _data_dir + "/" +file_name;

		std::ofstream out_file(full_path, std::ios::binary);
		if (!out_file)
		{
			failed++;
			continue;
		}
		out_file << content;
		out_file.close();
	}
	if (failed == req._post_files.size())
		return (send_error_message(500));
	else if (failed > 0)
		return (send_error_message(207));
	else
		return (send_error_message(200));
}


std::string	Server::send_error_message(int error_code)
{
	std::string	url = std::to_string(error_code);
	std::string	file_path = "error_pages/" + url + ".jpg";
	if (std::filesystem::exists(file_path))
	{
		std::string	file_content = read_file(file_path);
		std::string	mime_type = MimeTypes::getInstance().get_mime_type(file_path);
		std::string	response = "HTTP/1.1 " + std::to_string(error_code) + " Error\r\n";
		response += "Content-Type: " + mime_type + "\r\n";
		std::string	con_len_str = std::to_string(file_content.size());
		response += "Content-Length: " + con_len_str + "\r\n";
		response += "\r\n";
		response += file_content;
		_CLF_line += " " + std::to_string(error_code) + " " + con_len_str;
		return (response);
	}
	else
	{
		std::string response = "HTTP/1.1 404 Not Found\r\n";
		response += "Content-Type: text/html\r\n";
		std::string	con_len_str = std::to_string(strlen("404 File Not Found"));
		response += "Content-Length: " + con_len_str + "\r\n";
		response += "\r\n"; // End of headers
		response += "404 File Not Found"; // Body
		_CLF_line += " " + std::to_string(error_code) + " " + con_len_str;
		return (response);
	}
}

std::string	Server::process_cgi(const Request& req)
{
	std::string	url = req.get_file_path();
	std::string	full_path = map_to_directory(url);
	std::string main_part;
	size_t qmark_pos = full_path.find("?");
	if (qmark_pos != std::string::npos)
	{
		main_part = full_path.substr(qmark_pos + 1);
		_cgi_file_path = full_path.substr(0, qmark_pos);
	}
	else
	{
		_cgi_file_path = full_path;
	}
	if (!std::filesystem::exists(_cgi_file_path))
	{
		return (send_error_message(404));
	}
	if (req.get_method() == POST)
	{
		if (!req._post_files.empty())
		{
			auto it = req._post_files.begin();
			_cgi_post = it->second;
		}
		else
		{
			_cgi_post = "";
		}
	}
	else
	{
		_cgi_post = "";
	}
	// if (req.get_method_in_string() == "POST" && main_part.empty())
	// 	main_part = ... it should be the main body that has the input of the user
	std::vector<std::string> env_strings = 
	{
		"QUERY_STRING=" + main_part,
		"REQUEST_METHOD=" + req.get_method_in_string(),
		"CONTENT_LENGTH=" + std::to_string(req.get_content_len()),
		"CONTENT_TYPE=text/html",
		"GATEWAY_INTERFACE=CGI/1.1",
		"SCRIPT_NAME=" + _cgi_file_path,
		"SERVER_NAME=" + this->_name,
		"SERVER_PORT=" + std::to_string(req.get_port()),
		"SERVER_PROTOCOL=HTTP/1.1"
	};

	// std::vector<char*> envp;
	_cgi_env_strings = env_strings;
	// std::vector<const char*> envp;
	// for (auto& str : env_strings)
	// 	envp.push_back(str.c_str());
	// envp.push_back(NULL);
	// this->_cgi_env_strings = envp;
	return ("");
}

/* -------------------------------------------------------------------------- */
/*                                Getter/Setter                               */
/* -------------------------------------------------------------------------- */

int		Server::getServerFD(void) const
{
	return (_fd_server);
}

struct sockaddr_in	Server::getAddress(void) const
{
	return (_address);
}

const std::string	Server::getName(void) const
{
	return (_name);
}

std::string			Server::getCgiFilePath(void) const
{
	return (_cgi_file_path);
}

std::vector<std::string>	Server::getCgiEnvStrings(void) const
{
	return (_cgi_env_strings);
}

std::string			Server::getCgiPost(void) const
{
	return (_cgi_post);
}

size_t	Server::getMaxBodySize(void) const
{
	return (_max_body_size);
}

void	Server::log_CLF_line(void)
{
	Logger::getInstance().log(_name, _CLF_line, 1);
}
