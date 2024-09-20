#include "Server.hpp"

/* -------------------------------------------------------------------------- */
/*                           Orthodox Canonical Form                          */
/* -------------------------------------------------------------------------- */

Server::Server(const std::string server_name, int port, const std::string ip_address) : _name(server_name)
{
	std::cout << "Server Default Constructor called" << std::endl;
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
std::string	Server::extract_request(const std::string& request)
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

bool	file_end(const std::string& file_path, const std::string& suffix)
{
	if (suffix.size() > file_path.size())
		return (false);
	return (std::equal(suffix.rbegin(), suffix.rend(), file_path.rbegin()));
}

std::string	Server::get_mime_type(const std::string& file_path)
{
	if (file_end(file_path, ".html"))
		return ("text/html");
	if (file_end(file_path, ".jpg") || file_end(file_path, ".jpeg"))
		return ("image/jpeg");
	if (file_end(file_path, ".png"))
		return ("image/png");
	return ("text/plain");
}

std::string	Server::read_file(const std::string& file_path)
{
	std::ifstream		file(file_path);
	std::stringstream	buffer;

	buffer << file.rdbuf();
	return (buffer.str());
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