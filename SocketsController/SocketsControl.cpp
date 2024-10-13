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
	if (_poll_fds.size() > 0) //there has to be at least on server socket active to continue with polling
		loop_for_connections();

}

// private default constructor
SocketsControl::SocketsControl() {}

SocketsControl::~SocketsControl() {}

void SocketsControl::initServerSockets()
{
	std::vector<ServerData>::iterator it;
	for (it = _servers.begin(); it != _servers.end(); ++it)
	{
		// check for valid port:
		if (it->port_to_listen < 0 || it->port_to_listen > 65535)
			throw InvalidPortException(it->server_name, it->port_to_listen);

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
		bind_socket_and_listen_to_port(it);
		server_poll_data(it);
	}
}

void SocketsControl::bind_socket_and_listen_to_port(std::vector<ServerData>::iterator server)
{
	struct sockaddr_in address; // this struct is necessary when binding the socket to an IP address and port
	address.sin_family = AF_INET; // for IPv4 addresses
	address.sin_addr.s_addr = INADDR_ANY; // to listen in all available interfaces: localhost, ethernet etc...
	address.sin_port = htons(server->port_to_listen); //specifies the port number to listen to

	//and then i bind the socket to the specified address and port:
	if (bind(server->server_socket, (struct sockaddr*)&address, sizeof(address)) < 0)
	{
		close(server->server_socket);
		throw FailedToBindSocketException(server->server_name);
	}

	//listen to the specified, above, port:
	if (listen(server->server_socket, SOMAXCONN) < 0)
	{
		close(server->server_socket);
		throw ListenFailedException(server->server_name);
	}

	//save the port to the "occupied ports" vector:
	_used_ports.push_back(server->port_to_listen);

	std::cout << "Server socket " << UNDERLINE(server->server_socket) 
			<< " is now listening on port: " << BOLD(server->port_to_listen) << std::endl;
}

void SocketsControl::server_poll_data(std::vector<ServerData>::iterator server)
{
	//we need one `struct pollfd` for every server socket
	struct pollfd tmp;

	tmp.fd = server->server_socket; //tells the poll to monitor this server's socket
	tmp.events = POLLIN; //only pollin because the server socket is waiting to "receive", "read", new client connections

	//we add this struct to the vector of the poll_fds:
	_poll_fds.push_back(tmp);

	//keep the server's fd on the "Active server sockets" vector:
	_server_fds.push_back(server->server_socket); // ->the size() of this becomes the `nfds`

}

void SocketsControl::loop_for_connections()
{
	signal(SIGINT, signal_handler);

	while (true)
	{
		if (poll(&_poll_fds[0], _poll_fds.size(), 0) < 0) //third argument as 0, cause we want non-blocking behavior of poll, meaning to return immediately after checking the file descriptors and not to wait
		{
			std::cerr << RED("❗ poll() failed: ") << std::string(strerror(errno)) << std::endl;
			continue ;
		}

	}

}


void SocketsControl::close_server_sockets()
{

}

// the function given as second argument to the `signal` in order to exit the program
void signal_handler(int signum)
{
	std::cerr << RED("🛑 Signal " << signum << " received, closing server... 🔚") << std::endl;
	//! proper cleanup later
	exit(0);
}