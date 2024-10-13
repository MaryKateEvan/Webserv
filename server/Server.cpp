#include "Server.hpp"

/* -------------------------------------------------------------------------- */
/*                           Orthodox Canonical Form                          */
/* -------------------------------------------------------------------------- */

Server::Server(const std::string server_name, int port, const std::string ip_address, const std::string index_file, const std::string data_dir, const std::string www_dir)
	: _name(server_name), _index_file(index_file), _data_dir(data_dir), _www_dir(www_dir)
{
	std::cout << "Server Default Constructor called" << std::endl;
	load_mime_types("mime_type.csv"); //! only this i have to add more!
	// _fd_server = socket(AF_INET, SOCK_STREAM, 0);
	// if (_fd_server == -1)
	// 	throw SocketCreationFailedException(_name);
	// int opt = 1;
	// if (setsockopt(_fd_server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	// {
	// 	close(_fd_server);
	// 	throw SetSocketOptionFailedException(_name);
	// }
	// _address.sin_family = AF_INET;
	// if (port < 0 || port > 65535)
	// {
	// 	close(_fd_server);
	// 	throw InvalidPortException(_name, port);
	// }
	// _address.sin_port = htons(port);
	if (inet_pton(AF_INET, ip_address.c_str(), &_address.sin_addr) != 1)
	{
		close(_fd_server);
		throw InvalidIPAdressException(_name, ip_address);
	}
	// if (bind(_fd_server, (struct sockaddr *)&_address, sizeof(_address)) != 0)
	// {
	// 	close(_fd_server);
	// 	throw BindFailedException(_name, ip_address);
	// }
	// if (listen(_fd_server ,SOMAXCONN) != 0)
	// {
	// 	close(_fd_server);
	// 	throw ListenFailedException(_name);
	// }
	// std::cout << "Server is now listening on port " << port << std::endl;
}

Server::~Server()
{
	std::cout << "Server Default Destructor called" << std::endl;
	close(_fd_server);
}

Server::Server(const Server& copy)
{
	std::cout << "Server Copy Constructor called" << std::endl;
	if (this != &copy)
	{
		// this->_fd_server = copy._fd_server;
		// this->_address = copy._address;
		// this->_name = copy._name;
	}
}

Server&	Server::operator=(const Server &copy)
{
	std::cout << "Server Copy Assignment called" << std::endl;
	if (this != &copy)
	{
		// close(this->_fd_server);
		// this->_fd_server = copy._fd_server;
		// this->_address = copy._address;
		// this->_name = copy._name;
	}
	return (*this);
}

/* -------------------------------------------------------------------------- */
/*                                  Functions                                 */
/* -------------------------------------------------------------------------- */

int		Server::acceptConnection(void)
{
	socklen_t	addr_len = sizeof(_address);
	int			fd_new_socket = accept(_fd_server,  (struct sockaddr *)&_address, &addr_len);
	//potentially add exception on accept fail
	return (fd_new_socket);
}

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

std::string	Server::get_mime_type(const std::string& file_path)
{
	std::string	file_extension = file_path.substr(file_path.find_last_of('.'));
	try
	{
		return (_mime_types.at(file_extension));
	}
	catch (const std::out_of_range& e)
	{
		return ("unknown/unknown");
	}
}

std::string	Server::read_file(const std::string& file_path)
{
	std::ifstream		file(file_path);
	std::stringstream	buffer;

	buffer << file.rdbuf();
	return (buffer.str());
}

void	Server::load_mime_types(const std::string& file_path)
{
	std::ifstream file(file_path);
	if (!file.is_open())
		throw OpenFailedException(_name, file_path);
	std::string	line;
	while(std::getline(file, line))
	{
		std::istringstream	ss(line);
		std::string			extension;
		std::string			mime_type;

		if (std::getline(ss, extension, '\t') && std::getline(ss, mime_type))
			_mime_types[extension] = mime_type;
	}
	file.close();
}

int	Server::process_request(const Request& req)
{
	int	method = req.get_method();

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
	return (1);
}

int	Server::process_get(const Request& req)
{
	std::string	url = req.get_file_path();
	std::string	file_path = map_to_directory(url);
	std::string	response = "HTTP/1.1 200 OK\r\n";

	if (std::filesystem::is_directory(file_path))
	{
		if (file_path != map_to_directory("/" + _data_dir + "/") && file_path != map_to_directory("/" + _data_dir))
		{
			// std::cout << "File Path: " << file_path << std::endl;
			// std::cout << "Data Path: " <<  map_to_directory("/" + _data_dir) << std::endl;
			send_error_message(403, req);
			return (0);
		}
		response += "Content-Type: text/html\r\n";
		std::string	response_body = "<!DOCTYPE html>\n<html>\n<head><title>Index of " + req.get_file_path() + "</title></head>\n";
		response_body += "<body>\n<hr>\n<ul>\n";
		response_body += "    <li><a href=\"../\">Parent Directory</a></li>\n";
		for (const auto& entry : std::filesystem::directory_iterator(file_path))
		{
			// std::cout << entry.path().filename().string() << std::endl;
			response_body += "    <li><a href=\"" + _data_dir + "/" + entry.path().filename().string() + "\">" + entry.path().filename().string() + "</a></li>\n";
		}
		response_body += "</ul>\n<hr>\n</body>\n</html>\n";
		response += "Content-Length: " + std::to_string(response_body.size()) + "\r\n";
		response += "\r\n";
		response += response_body;
		if (send(req.get_fd(), response.c_str(), response.size(), 0) == -1)
				throw SendFailedException(_name, req.get_fd());
	}
	else if (std::filesystem::exists(file_path))
	{
		std::string	file_content = read_file(file_path);
		std::string	mime_type = get_mime_type(file_path);
		response += "Content-Type: " + mime_type + "\r\n";
		response += "Content-Length: " + std::to_string(file_content.size()) + "\r\n";
		response += "\r\n";
		response += file_content;
		if (send(req.get_fd(), response.c_str(), response.size(), 0) == -1)
				throw SendFailedException(_name, req.get_fd());
	}
	else
		send_error_message(404, req);
	return (0);
}

int	Server::process_delete(const Request& req)
{
	std::string	url = req.get_file_path();
	std::string	file_path = _www_dir + "/" + _data_dir + "/" + url;
	if (std::filesystem::exists(file_path))
	{
		if (std::remove(file_path.c_str()) == 0)
		{
			std::string	response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
			if (send(req.get_fd(), response.c_str(), response.size(), 0) == -1)
				throw SendFailedException(_name, req.get_fd());
		}
		else
			send_error_message(500, req);
	}
	else
		send_error_message(404, req);
	return (0);
}

int	Server::process_post(const Request& req)
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
		send_error_message(500, req);
	if (failed > 0)
		send_error_message(207, req);
	send_error_message(200, req);
	return (0);
}


int	Server::send_error_message(int error_code, const Request& req)
{
	std::string	url = std::to_string(error_code);
	std::string	file_path = "error_pages/" + url + ".jpg";
	if (std::filesystem::exists(file_path))
	{
		std::string	file_content = read_file(file_path);
		std::string	mime_type = get_mime_type(file_path);
		std::string	response = "HTTP/1.1 " + std::to_string(error_code) + " Error\r\n";
		response += "Content-Type: " + mime_type + "\r\n";
		response += "Content-Length: " + std::to_string(file_content.size()) + "\r\n";
		response += "\r\n";
		response += file_content;
		if (send(req.get_fd(), response.c_str(), response.size(), 0) == -1)
				throw SendFailedException(_name, req.get_fd());
	}
	else
	{
		std::string response = "HTTP/1.1 404 Not Found\r\n";
		response += "Content-Type: text/html\r\n";
		response += "Content-Length: " + std::to_string(strlen("404 File Not Found")) + "\r\n";
		response += "\r\n"; // End of headers
		response += "404 File Not Found"; // Body
		if (send(req.get_fd(), response.c_str(), response.size(), 0) == -1)
			throw SendFailedException(_name, req.get_fd());
	}
	return (0);
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
