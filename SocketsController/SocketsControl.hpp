#ifndef SOCKETS_CONTROL_HPP
# define SOCKETS_CONTROL_HPP

# include "./webserv.h"

class SocketsControl
{
	private:
		std::vector<ServerData> _servers;

	public:
		SocketsControl();
		SocketsControl(const std::vector<ServerData> & serversData);
		~SocketsControl();

		void init_server_sockets();

	
};





#endif //SOCKETS_CONTROL_HPP