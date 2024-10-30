#ifndef SOCKETMANAGER_HPP
# define SOCKETMANAGER_HPP

# include "../webserv.h"
# include "Server.hpp"

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 4096
# endif /*BUFFER_SIZE*/

# ifndef MAX_SCRIPTS
#  define MAX_SCRIPTS 20
# endif

class Server;
// class Request;

struct ConnectionData
{
	int					client_fd;
	int					server_port;
	int 				out_pipe[2];
	pid_t				child_pid;

	ConnectionData(int client, int port, int pipe[2], pid_t pid)
		: client_fd(client), server_port(port), child_pid(pid)
	{
		out_pipe[0] = pipe[0];
		out_pipe[1] = pipe[1];
	}
	ConnectionData(const ConnectionData& other)
		: client_fd(other.client_fd), server_port(other.server_port), child_pid(other.child_pid)
	{
		out_pipe[0] = other.out_pipe[0];
		out_pipe[1] = other.out_pipe[1];
	}
	ConnectionData& operator=(const ConnectionData& other)
	{
		if (this != &other)
		{
			client_fd = other.client_fd;
			server_port = other.server_port;
			out_pipe[0] = other.out_pipe[0];
			out_pipe[1] = other.out_pipe[1];
			child_pid = other.child_pid;
		}
		return (*this);
	}
	ConnectionData() : client_fd(0), server_port(0), out_pipe{0, 0}, child_pid(0) {}
};

class SocketManager
{
	private:
		std::unordered_map<int, Request>					_request_map; // Tracking requests by client_fd
		std::unordered_map<int, std::string>				_response_map; // Tracking responses by client_fd
		std::unordered_map<int, std::unique_ptr<Server>>	_server_map; // Tracking servers by port
		std::vector<pollfd>									_fds;
		std::vector<int>									_disconnect_after_send;

		/**
		 * @brief Stores the data necessary for CGI relative to the clients fd
		 */
		std::unordered_map<int, ConnectionData>				_cgi_map;

		void		handle_read(int client_fd);
		void		handle_write(int client_fd);
		void		handle_write_cgi(int client_fd);
		std::string	handle_cgi(int client_fd, int server_port);
		void		set_pollevent(int client_fd, short events);
		void		accept_connections();
	public:
		SocketManager();
		~SocketManager();

		void		add_server(int port, std::unique_ptr<Server> server);
		void		add_client(int client_fd);
		void		remove_client(int client_fd);
		void		handle_requests();
};

#endif /*SOCKETMANAGER_HPP*/