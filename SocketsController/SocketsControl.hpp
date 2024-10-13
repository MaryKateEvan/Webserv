#ifndef SOCKETS_CONTROL_HPP
# define SOCKETS_CONTROL_HPP

# include "../webserv.h"

struct ServerData;

class SocketsControl
{
	private:
		std::vector<ServerData> 	_servers;
		std::vector<int>			_server_fds;
		std::vector<struct pollfd>	_poll_fds;
		std::vector<int>			_used_ports;

		SocketsControl(); //only instantiated with the vector of ServerData passed

	public:
		SocketsControl(const std::vector<ServerData> & serversData);
		~SocketsControl();

		void initServerSockets();
		void bind_socket_and_listen_to_port(std::vector<ServerData>::iterator server);


		void close_server_sockets();

	
};





#endif //SOCKETS_CONTROL_HPP