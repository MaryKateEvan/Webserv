#include "Polling.hpp"

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

volatile sig_atomic_t g_run = true;

Polling::Polling(std::vector<Server> & servers)	: _servers(servers)
{
	// to verify the servers' data passed here:
	for (Server& server : _servers) {
		std::cout << "Hi from server " << server.getName() << std::endl;
	}
	
	// server_poll_data();
	boot_servers();
	if (_poll_fds.size() > 0) //there has to be at least one server socket active to continue with polling
		loop_for_connections();
}

// private default constructor (not called)
Polling::Polling() {}

Polling::~Polling() {}

void Polling::boot_servers()
{
	for (Server& server : _servers)
	{
		server.initServerSocket(this->_used_ports);
		server.bind_socket_and_listen(this->_used_ports);

		//we need one `struct pollfd` for every server socket
		struct pollfd tmp;

		tmp.fd = server.getServerFD(); //tells the poll to monitor this server's socket
		tmp.events = POLLIN; //only pollin because the server socket is waiting to "receive", "read", new client connections

		//we add this struct to the vector of the poll_fds:
		_poll_fds.push_back(tmp);

		//keep the server's fd on the "Active server sockets" vector:
		_server_fds.push_back(tmp.fd); // ->the size() of this becomes the `nfds`
	}
}

void Polling::loop_for_connections()
{
	std::cout << "... Starting the poll loop ..." << std::endl;

	signal(SIGINT, signal_handler);
	while (g_run)
	{
		if (poll(&_poll_fds[0], _poll_fds.size(), 0) < 0) //third argument as 0, cause we want non-blocking behavior of poll, meaning to return immediately after checking the file descriptors and not to wait
		//? maybe 3rd argument should be the server's time_out if the parser saves such value from the config?
		{
			std::cerr << RED("❗ poll() failed: ") << std::string(strerror(errno)) << std::endl;
			// throw std::runtime_error("Poll failed, shutting down server.");
			break ;
		}
		// loop through all active file descriptors:
		for (size_t i = 0; i < _poll_fds.size(); i++)
		{
			bool is_server_fd = std::find(_server_fds.begin(), _server_fds.end(), _poll_fds[i].fd) != _server_fds.end();
			// bool is_server_fd = (i < _servers.size());
			// 2. check for POLLIN events in both server and client sockets:
			if (_poll_fds[i].revents & POLLIN)
			{
				std::cout << GREEN("Received Request on socket " << _poll_fds[i].fd ) << std::endl;
				if (is_server_fd)
					accept_new_client_connection(_poll_fds[i].fd);
				else
				{
					read_client_request(_poll_fds[i]);

				}
			}
			// 1. check for errors first:
			if (_poll_fds[i].revents & (POLLERR | POLLHUP | POLLNVAL))
			{
				std::cerr << RED("❗ Error on file descriptor: ") << _poll_fds[i].fd 
						<< " - " << strerror(errno) << std::endl;
				// we close the socket and remove it from the tracking structures:
				close(_poll_fds[i].fd);
				_poll_fds.erase(_poll_fds.begin() + i); // removes it from the _poll_fds vector of active fds
				--i; // we decrement the index to account for the erased element

				// we also delete it from server_fds vector, if it is a server socket:
				if (is_server_fd)
					_server_fds.erase(std::find(_server_fds.begin(), _server_fds.end(), _poll_fds[i].fd));
				// std::vector<int>::iterator search = std::find(_server_fds.begin(), _server_fds.end(), _poll_fds[i].fd);
				// if (search != _server_fds.end())
				// 	_server_fds.erase(search);

				//! i will also need to delete it after from the clients container
				continue ; //skips further steps for this fd that had the problem
			}
			// 3. check for POLLOUT events which are spicific for client sockets:
			if (is_server_fd && (_poll_fds[i].revents & POLLOUT))
			{

			}
		}
	}
	//TODO sockets_cleanup here, after the poll loop breaks from the signal
}


void Polling::accept_new_client_connection(int server_fd)
{
	struct sockaddr_in client_addr; //struct to hold client's IP address, port, and other info
	socklen_t client_len = sizeof(client_addr); //necessary for the accept, to know how much space it has to store the client's address
	
	//system call to accept a new connection from a client:
	int new_socket = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
	//created a file descriptor for the newly accepted connection
	if (new_socket < 0)
	{
		std::cerr << RED("❗ Failed to accept connection in socket: ") << server_fd 
				<< " - " << strerror(errno) << std::endl;
		return ;
	}
	if (fcntl(new_socket, F_SETFL, O_NONBLOCK) < 0) //for non-blocking mode of the client socket
	{
		close(new_socket);
		throw SetSocketNonBLockingModeException("client");
	}
	//add the poll_fd of the client in the _poll_fds vector:
	struct pollfd client_poll_fd;
	client_poll_fd.fd = new_socket;
	client_poll_fd.events = POLLIN;
	_poll_fds.push_back(client_poll_fd);

	//add the client fd to the _client_fds vector:
	_client_fds.push_back(new_socket);

	_clients[new_socket] = ClientData();
	_clients[new_socket].fd_client = new_socket;
	std::time(&_clients[new_socket].last_seen);

	std::cout << GREEN(" ✅ 👨‍💻 New client connected on socket: " << BOLD(new_socket)) << std::endl; 
}

//! for now i changed all the occurences of 'fd[i].fd` to `client.fd`
//! and the server1.getMaxBodySize to `this->_servers[0].getMaxBodySize()
//TODO but i need to check how to put this in a loop for all servers.
void Polling::read_client_request(pollfd & client) 
{
	Request				req(client.fd);
	std::vector<char>	buffer(4096);
	int	ret;
	while (true)
	{
		if (req.correct_body_size(this->_servers[0].getMaxBodySize()) == false)
		{
			ret = -10;
			break ;
		}
		int bytes_read = read(client.fd, buffer.data(), buffer.size());
		ret = req.read_chunk(buffer, bytes_read);
		if (ret < 0)
			break ;
		if (ret == 1)
			continue;
		if (ret == 0)
			break ;
	}
	if (ret == 0)
	{
		std::string response = this->_servers[0].process_request(req);
		send(req.get_fd(), response.c_str(), response.size(), 0);
		// std::cout << "The requested host is: " << req.get_host() << std::endl;
		// close(client.fd);
		// client.fd = -1;
	}
	if (ret == -10)
	{
		std::string response = this->_servers[0].send_error_message(413);
		send(req.get_fd(), response.c_str(), response.size(), 0);
	}
	else if (ret < 0)
	{
		std::string response = this->_servers[0].send_error_message(400);
		send(req.get_fd(), response.c_str(), response.size(), 0);
		// close(client.fd);
		// client.fd = -1;
	}
}


// void Polling::read_client_request(pollfd &client) 
// {
//     Request req(client.fd);
//     std::vector<char> buffer(4096);
//     int ret;

//     // Process the client request
//     while (true)
//     {
//         // Check for body size
//         if (req.correct_body_size(this->_servers[0].getMaxBodySize()) == false)
//         {
//             ret = -10;
//             break;
//         }

//         // Read from client socket
//         int bytes_read = read(client.fd, buffer.data(), buffer.size());
//         ret = req.read_chunk(buffer, bytes_read);

//         // Check the result of reading
//         if (ret < 0)
//             break;
//         if (ret == 1)
//             continue;
//         if (ret == 0)
//             break;
//     }

//     // Request processed successfully, ready to send response
//     if (ret == 0)
//     {
//         std::string response = this->_servers[0].process_request(req);
        
//         // Store the response in the client object
//         req.set_write_string(response); // Assuming you have a similar method to store the response

//         // Set the POLLOUT event so we can send the response in the next poll loop
//         client.events = POLLOUT;
//     }

//     // Handle errors for large body size
//     if (ret == -10)
//     {
//         std::string response = this->_servers[0].send_error_message(413);
//         send(req.get_fd(), response.c_str(), response.size(), 0);
//     }
//     // Handle general errors in the request
//     else if (ret < 0)
//     {
//         std::string response = this->_servers[0].send_error_message(400);
//         send(req.get_fd(), response.c_str(), response.size(), 0);
//     }
// }

// the function given as second argument to the `signal` in order to exit the program
void signal_handler(int signum)
{
	std::cerr << RED(" 🛑 Signal " << signum << " received, closing server... 🔚") << std::endl;
	// exit(0);
	g_run = true;
	//the cleanup continues below the poll_loop, because of the `volatile sig_atomic_t `
}