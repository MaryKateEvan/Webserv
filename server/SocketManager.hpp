#ifndef SOCKETMANAGER_HPP
# define SOCKETMANAGER_HPP

# include "../webserv.h"
# include "Server.hpp"

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 4096
# endif /*BUFFER_SIZE*/

class Server;
// class Request;

class SocketManager
{
	private:
		std::unordered_map<int, Request>					_request_map; // Tracking requests by client_fd
		std::unordered_map<int, std::string>				_response_map; // Tracking responses by client_fd
		std::unordered_map<int, std::unique_ptr<Server>>	_server_map; // Tracking servers by port
		std::vector<pollfd>									_fds;

		void	handle_read(int client_fd);
		void	handle_write(int client_fd);
		void	set_pollevent(int client_fd, short events);
	public:
		SocketManager();
		~SocketManager();
		//temp, replace once config parser is merged
		void	add_server(int port, std::unique_ptr<Server> server);
		void	add_client(int client_fd);
		void	remove_client(int client_fd);
		void	handle_requests();
};

#endif /*SOCKETMANAGER_HPP*/