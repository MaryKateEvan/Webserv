#include "Server.hpp"

/* -------------------------------------------------------------------------- */
/*                           Orthodox Canonical Form                          */
/* -------------------------------------------------------------------------- */

Server::Server(const std::string server_name, int port, const std::string ip_address) : _name(server_name)
{
	std::cout << "Server Default Constructor called" << std::endl;
	load_mime_types("mime_type.csv");
	_fd_server = socket(AF_INET, SOCK_STREAM, 0);
	if (_fd_server == -1)
		throw SocketCreationFailedException(_name);
	int opt = 1;
	if (setsockopt(_fd_server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
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
	_address.sin_port = htons(port);
	if (inet_pton(AF_INET, ip_address.c_str(), &_address.sin_addr) != 1)
	{
		close(_fd_server);
		throw InvalidIPAdressException(_name, ip_address);
	}
	if (bind(_fd_server, (struct sockaddr *)&_address, sizeof(_address)) != 0)
	{
		close(_fd_server);
		throw BindFailedException(_name, ip_address);
	}
	if (listen(_fd_server ,SOMAXCONN) != 0)
	{
		close(_fd_server);
		throw ListenFailedException(_name);
	}
	std::cout << "Server is now listening on port " << port << std::endl;
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
		return ("www/index.html");
	else
		return ("www" + file_path);
}

bool	Server::file_exists(const std::string& file_path)
{
	std::ifstream file(file_path.c_str());

	return (file.is_open());
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
	}
	return (1);
}

int	Server::process_get(const Request& req)
{
	std::string	url = req.get_file_path();
	std::string	file_path = map_to_directory(url);
	if (file_exists(file_path))
	{
		std::string	file_content = read_file(file_path);
		std::string	mime_type = get_mime_type(file_path);
		std::string	response = "HTTP/1.1 200 OK\nContent-Type: " + mime_type + "\n\n" + file_content;
		if (send(req.get_fd(), response.c_str(), response.size(), 0) == -1)
				throw SendFailedException(_name, req.get_fd());
	}
	else
	{
		std::string error_response = "HTTP/1.1 404 Not Found\nContent-Type: text/html\n\n404 File Not Found";
		if (send(req.get_fd(), error_response.c_str(), error_response.size(), 0) == -1)
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

std::string	Server::getResponse(void) const
{
	return (_response);
}

void		Server::setResponse(std::string response)
{
	_response = response;
}