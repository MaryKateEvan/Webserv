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

void	SocketManager::add_server(int port, std::unique_ptr<Server> server)
{
	//! if necessary add check here for duplicated ports
	_server_map[port] = std::move(server);
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
		for (auto& pfd : _fds)
		{
			if (pfd.revents & POLLIN)
			{
				handle_read(pfd.fd);
			}
			if (pfd.revents & POLLOUT)
			{
				handle_write(pfd.fd);
			}
			if (pfd.revents & (POLLERR | POLLHUP | POLLNVAL))
			{
				remove_client(pfd.fd);
			}
		}
	}
}

//  Server* get_server(int port) {
//         if (server_map.find(port) != server_map.end()) {
//             return server_map[port].get();
//         }
//         return nullptr;
//     }

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
			if (_request_map[client_fd].correct_body_size(_server_map[_request_map[client_fd].get_port()]->getMaxBodySize() == false))
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
			set_pollevent(client_fd, POLLIN);
			// remove_client(client_fd);
		}
	}
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