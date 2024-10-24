#include "SocketManager.hpp"

/* -------------------------------------------------------------------------- */
/*                           Orthodox Canonical Form                          */
/* -------------------------------------------------------------------------- */

SocketManager::SocketManager()
{
	std::cout << "SocketManager Default Constructor called" << std::endl;
}

SocketManager::~SocketManager()
{
	std::cout << "SocketManager Default Destructor called" << std::endl;
}

/* -------------------------------------------------------------------------- */
/*                                  Functions                                 */
/* -------------------------------------------------------------------------- */

void SocketManager::add_server(int port, std::unique_ptr<Server> server)
{
	_server_map[port] = std::move(server);

	pollfd pfd = { _server_map[port]->getServerFD(), POLLIN, 0 };
	_fds.push_back(pfd);
}


void	SocketManager::add_client(int client_fd)
{
	pollfd	pfd = {client_fd, POLLIN, 0};
	_fds.push_back(pfd);
	_request_map[client_fd] = Request(client_fd);
}

void	SocketManager::remove_client(int client_fd)
{
	close(client_fd);
	_request_map.erase(client_fd);
	_response_map.erase(client_fd);
	_fds.erase(std::remove_if(_fds.begin(), _fds.end(),
	[client_fd](pollfd& pfd) { return pfd.fd == client_fd; }), _fds.end());
}

void	SocketManager::handle_requests()
{
	while (true)
	{
		int poll_count = poll(_fds.data(), _fds.size(), -1);

		if (poll_count == -1)
		{
			std::cerr << "Poll failed" << std::endl;
			break ;
		}
		accept_connections();
		for (size_t i = 0; i < _fds.size(); i++)
		{
			if (_fds[i].revents & POLLIN)
			{
				handle_read(_fds[i].fd);
			}
			if (_fds[i].revents & POLLOUT)
			{
				handle_write(_fds[i].fd);
			}
			if (_fds[i].revents & (POLLERR | POLLHUP | POLLNVAL))
			{
				remove_client(_fds[i].fd);
			}
		}
	}
}

void	SocketManager::handle_read(int client_fd)
{
	std::vector<char>	buffer(BUFFER_SIZE);
	int	bytes_read = read(client_fd, buffer.data(), buffer.size());

	if (bytes_read <= 0)
	{
		remove_client(client_fd);
	}
	else
	{
		int	status = _request_map[client_fd].read_chunk(buffer, bytes_read);
		if (_request_map[client_fd].get_port() != -1)
		{
			// std::cout << "Max body size: " << _server_map[_request_map[client_fd].get_port()]->getMaxBodySize() << std::endl;
			if (_request_map[client_fd].correct_body_size(_server_map[_request_map[client_fd].get_port()]->getMaxBodySize()) == false)
				status = -10;
		}
		if (status == 0)
		{
			int	port = _request_map[client_fd].get_port();

			if (_server_map.find(port) != _server_map.end())
			{
				std::string	response = _server_map[port]->process_request(_request_map[client_fd]);
				_response_map[client_fd] = response;
				set_pollevent(client_fd, POLLOUT);
			}
			else
				remove_client(client_fd);
		}
		else if (status == -10)
		{
			int	port = _request_map[client_fd].get_port();

			if (_server_map.find(port) != _server_map.end())
			{
				std::string	response = _server_map[port]->send_error_message(413);
				_response_map[client_fd] = response;
				set_pollevent(client_fd, POLLOUT);
			}
			else
				remove_client(client_fd);
		}
		else if (status < 0)
		{
			int	port = _request_map[client_fd].get_port();

			if (_server_map.find(port) != _server_map.end())
			{
				std::string	response = _server_map[port]->send_error_message(400);
				_response_map[client_fd] = response;
				set_pollevent(client_fd, POLLOUT);
			}
			else
				remove_client(client_fd);
		}
	}
}

void	SocketManager::handle_write(int client_fd)
{
	auto&	response = _response_map[client_fd];
	// std::cout << "Response:\n" << response << std::endl;
	ssize_t	bytes_sent = send(client_fd, response.c_str(), response.size(), 0);

	if (bytes_sent == -1)
	{
		std::cerr << "Send failed!" << std::endl;
		remove_client(client_fd);
	}
	else
	{
		response.erase(0, bytes_sent);
		if (response.empty())
		{
			// set_pollevent(client_fd, POLLIN);
			remove_client(client_fd);
		}
	}
	// std::cout << "I FINISHED HELLO" << std::endl;
}


void	SocketManager::set_pollevent(int client_fd, short events)
{
	for (auto& pfd : _fds) {
		if (pfd.fd == client_fd)
		{
			pfd.events = events;
			break;
		}
	}
}

void SocketManager::accept_connections()
{
	for (const auto& [port, server_ptr] : _server_map)
	{
		int server_fd = server_ptr->getServerFD();
		if (std::find_if(_fds.begin(), _fds.end(), [server_fd](const pollfd& pfd) { return pfd.fd == server_fd; }) != _fds.end())
		{
		struct sockaddr_in client_addr;
		socklen_t client_len = sizeof(client_addr);
		int new_socket = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
		if (new_socket < 0)
		{
			std::cerr << "❗ Failed to accept connection in socket: " << server_fd << " - " << strerror(errno) << std::endl;
			return ;
		}
		if (fcntl(new_socket, F_SETFL, O_NONBLOCK) < 0)
		{
			close(new_socket);
			throw SetSocketOptionFailedException("client");
		}
		add_client(new_socket);
		}
	}
}
