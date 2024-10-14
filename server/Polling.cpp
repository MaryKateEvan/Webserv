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

Polling::Polling(const std::vector<Server> & servers)
	: _servers(servers)
{
	server_poll_data();
	if (_poll_fds.size() > 0) //there has to be at least on server socket active to continue with polling
		loop_for_connections();
}

// private default constructor
Polling::Polling() {}

Polling::~Polling() {}


void Polling::server_poll_data()
{
	for (Server& server : _servers)
	// while(std::vector<Server>::iterator it = _servers.begin(); it != _servers.end(); ++it)
	{
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
	signal(SIGINT, signal_handler);

	while (true)
	{
		if (poll(&_poll_fds[0], _poll_fds.size(), 0) < 0) //third argument as 0, cause we want non-blocking behavior of poll, meaning to return immediately after checking the file descriptors and not to wait
		{
			std::cerr << RED("❗ poll() failed: ") << std::string(strerror(errno)) << std::endl;
			// continue ;
			break ;
		}
		// loop through all active file descriptors:
		for (size_t i = 0; i < _poll_fds.size(); i++)
		{
			// bool is_server_fd = std::find(_server_fds.begin(), _server_fds.end(), _poll_fds[i].fd) != _server_fds.end();
			bool is_server_fd = (i < _servers.size());
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
			// 2. check for POLLIN events in both server and client sockets:
			if (_poll_fds[i].revents & POLLIN)
			{
				if (is_server_fd)
					accept_new_client_connection(_poll_fds[i].fd);
				else
					read_client_request(_poll_fds[i]);
			}
			// 3. check for POLLOUT events which are spicific for client sockets:
			if (is_server_fd && (_poll_fds[i].revents & POLLOUT))
			{

			}
		}
	}
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
	//! maybe initialize here any other data of client structure, if i decide to make one

	std::cout << GREEN(" ✅ 👨‍💻 New client connected on socket: " << BOLD(new_socket)) << std::endl; 
}


void Polling::read_client_request(pollfd & client)
{
	std::vector<char>	accumulated_request;
	std::vector<char>	buffer(4096 * 4);
	size_t				content_len = 0;
	bool				header_parsed = false;
	size_t				body_bytes_read = 0;

	// Response response("client socket", "index.html", "usrimg", "www_image_webpage");


	while (true)
	{
		int bytes_read = read(client.fd, buffer.data(), buffer.size());

		if (bytes_read < 0)
		{
			std::cerr << RED("Error reading from fd " << client.fd) << std::endl;
			close(client.fd);
			_client_fds.erase(std::find(_client_fds.begin(), _client_fds.end(), client.fd));
			break;
		}
		if (bytes_read == 0) //when the client disconnects
		{
			std::cout << YELLOW("🔓 🙅 Client on fd " << client.fd << " disconnected") << std::endl;
			close(client.fd);
			_client_fds.erase(std::find(_client_fds.begin(), _client_fds.end(), client.fd));
			break;
		}
		accumulated_request.insert(accumulated_request.end(), buffer.begin(), buffer.begin() + bytes_read);
		std::string temp_request(accumulated_request.begin(), accumulated_request.end());
		if (!header_parsed)
		{
			size_t	header_end = temp_request.find("\r\n\r\n");
			if (header_end != std::string::npos)
			{
				header_parsed = true;
				std::string	headers = temp_request.substr(0, header_end + 4);
				size_t content_len_pos = headers.find("Content-Length: ");
				std::cout << "Headers:\n" << headers << std::endl;
				if (content_len_pos != std::string::npos)
				{
					size_t content_length_start = content_len_pos + 16;
					size_t content_length_end = headers.find("\r\n", content_length_start);
					std::string content_length_str = headers.substr(content_length_start, content_length_end - content_length_start);
					content_len = std::stoul(content_length_str);
					// std::cout << "|TEMPREQUESTBEGIN:\n\n\n|" << temp_request << "|TEMPREQUESTEND\n\n\n|" << std::endl;
				}
				body_bytes_read = temp_request.size() - (header_end + 4);
			}
		}
		// std::cout << "Body Bytes Read: " << body_bytes_read << std::endl;
		if (header_parsed && content_len > 0)
		{
			if (body_bytes_read >= content_len)
				break ;
		}
		if (header_parsed && content_len == 0)
				break ;
		if (header_parsed)
			body_bytes_read += bytes_read;
	}
	if (header_parsed && (content_len == 0 || body_bytes_read >= content_len))
	{
		Request req(std::string(accumulated_request.begin(), accumulated_request.end()), client.fd);
		// Response response("client socket", "index.html", "usrimg", "www_image_webpage");
		// req.getIP();

		_servers[0].process_request(req);
	}
	accumulated_request.clear();

	client.events |= POLLOUT;
	// close(client.fd);
	// client.fd = -1;

	// //in order to delete this client form the _poll_fds array:
	// std::vector<struct pollfd>::iterator search_client = std::find_if(_poll_fds.begin(), _poll_fds.end(),
	// [&client](const struct pollfd& pfd) { return pfd.fd == client.fd; });
	// if (search_client != _poll_fds.end()) {
	// 	_poll_fds.erase(search_client);
	// }
}

// the function given as second argument to the `signal` in order to exit the program
void signal_handler(int signum)
{
	std::cerr << RED(" 🛑 Signal " << signum << " received, closing server... 🔚") << std::endl;
	//! proper cleanup later
	exit(0);
}