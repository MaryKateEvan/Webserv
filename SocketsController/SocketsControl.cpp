#include "SocketsControl.hpp"

/*
	GENERAL ARCHITECTURE OF SOCKET MANAGEMENT:
	1. Socket Creation: A socket is created using socket().
	2. Binding: The socket is bound to the specified port.
	3. Listening: The server listens for incoming connections.
	4. poll() Loop: The server uses poll() to monitor multiple file descriptors (clients and the server socket). This is non-blocking, meaning the server can handle multiple connections.
	5. Client Connection: When a new client connects, it is accepted, and the client's file descriptor is added to the poll() array.
	6. Handling Requests: When the server receives a request, it sends a simple HTTP response with "Hello World" (in my case) as the body.
	7. Connection Close: After responding, the server closes the connection to the client.
*/

SocketsControl::SocketsControl(const std::vector<ServerData> & serversData)
	: _servers(serversData)
{
	initServerSockets();

}

// private default constructor
SocketsControl::SocketsControl() {}

SocketsControl::~SocketsControl() {}

void SocketsControl::initServerSockets()
{
	std::vector<ServerData>::iterator it;
	for (it = _servers.begin(); it != _servers.end(); ++it)
	{
		// a port can be bound to one socket at a time, so if this port is already bound, we skip this socket creation
		if (std::find(_used_ports.begin(), _used_ports.end(), it->port_to_listen) != _used_ports.end())
			continue ;
		
		it->server_socket = socket(AF_INET, SOCK_STREAM, 0);
		if (it->server_socket == -1)
			throw SocketCreationFailedException(it->server_name);
		int opt = 1;
		if (setsockopt(it->server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		{
			close(it->server_socket);
			throw SetSocketOptionFailedException(it->server_name);
		}
		if (fcntl(it->server_socket, F_SETFL, O_NONBLOCK) < 0) //for non-blocking mode of the server socket
		{
			close(it->server_socket);
			throw SetSocketNonBLockingModeException(it->server_name);
		}
		bound_socket_to_port(it);
	}
}

void SocketsControl::bound_socket_to_port(std::vector<ServerData>::iterator server)
{
	
}

void SocketsControl::close_server_sockets()
{

}