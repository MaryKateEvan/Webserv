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