#ifndef SERVER_HPP
# define SERVER_HPP

# include "../webserv.h"

class Server
{
	private:
		int					_fd_server;
		struct sockaddr_in	_address;
		const std::string	_name;
	public:
		Server(const std::string server_name, int port, const std::string ip_address);
		//use inet_pton() to convert IP address
		~Server();
		Server(const Server& copy);
		Server& operator=(const Server& copy);
};

#endif /*SERVER_HPP*/