#include "Server.hpp"

/* -------------------------------------------------------------------------- */
/*                           Orthodox Canonical Form                          */
/* -------------------------------------------------------------------------- */

Server::Server()
{
	std::cout << "Server Default Constructor called" << std::endl;
}

Server::~Server()
{
	std::cout << "Server Default Destructor called" << std::endl;
}

Server::Server(const Server& copy)
{
	std::cout << "Server Copy Constructor called" << std::endl;
	if (this != &copy)
	{
	}
}

Server&	Server::operator=(const Server &copy)
{
	std::cout << "Server Copy Assignment called" << std::endl;
	if (this != &copy)
	{
	}
	return (*this);
}

/* -------------------------------------------------------------------------- */
/*                                  Functions                                 */
/* -------------------------------------------------------------------------- */
