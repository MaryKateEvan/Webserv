#include "SocketManager.hpp"

/* -------------------------------------------------------------------------- */
/*                           Orthodox Canonical Form                          */
/* -------------------------------------------------------------------------- */

SocketManager::SocketManager(size_t global_timeout)
{
	if (global_timeout > 1000)
	{
		_global_timeout = global_timeout;
	}
	else
	{
		_global_timeout = 20000;
	}
	Logger::getInstance().log("", "Socket Manager Constructor called", 2);
}

SocketManager::~SocketManager()
{
	int	num_servers = _server_map.size();
	for (size_t i = num_servers; i < _fds.size(); i++)
	{
		remove_client(_fds[i].fd);
	}
	Logger::getInstance().log("", "Socket Manager Destructor called", 2);
}

/* -------------------------------------------------------------------------- */
/*                                  Functions                                 */
/* -------------------------------------------------------------------------- */

void SocketManager::add_server(int port, std::unique_ptr<Server> server)
{
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
	auto	it = _cgi_map.find(client_fd);
	if (it != _cgi_map.end())
	{
		ConnectionData&	con_data = it->second;
		Logger::getInstance().log("", std::to_string(client_fd) + " disconnected with running CGI script", 3);
		close(con_data.out_pipe[0]);
		kill(con_data.child_pid, SIGTERM);
		_cgi_map.erase(it);
	}
	_fds.erase(std::remove_if(_fds.begin(), _fds.end(),
	[client_fd](pollfd& pfd) { return pfd.fd == client_fd; }), _fds.end());
}

void	SocketManager::handle_requests()
{
	int	num_servers = _server_map.size();
	_fds.clear();
	_fds.reserve(_server_map.size());

	for (const auto& [port, server_ptr] : _server_map)
	{
		int server_fd = server_ptr->getServerFD();
		_fds.push_back({server_fd, POLLIN, 0});
	}
	while (true)
	{
		int poll_count = poll(_fds.data(), _fds.size(), _global_timeout);

		if (poll_count == -1)
		{
			throw PollFailedException();
		}
		accept_connections();

		for (size_t i = num_servers; i < _fds.size(); i++)
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
			if (_request_map[client_fd].correct_body_size(_server_map[_request_map[client_fd].get_port()]->getMaxBodySize()) == false)
				status = -10;
		}
		if (status <= 0)
		{
			// std::cout << "Request: " << _request_map[client_fd].get_method_in_string() << std::endl;
			int	port = _request_map[client_fd].get_port();
			std::string	response;
			if (_server_map.find(port) != _server_map.end())
			{
				switch (status)
				{
					case 0:
						response = _server_map[port]->process_request(_request_map[client_fd]);
						_server_map[port]->log_CLF_line();
						if (response == "")
							response = handle_cgi(client_fd, port);
						else if (response.find("302 Found") != std::string::npos)
							_disconnect_after_send.push_back(client_fd);
						break;
					case -10:
						response = _server_map[port]->send_error_message(413);
						_disconnect_after_send.push_back(client_fd);
						Logger::getInstance().log("",  _request_map[client_fd].get_client_ip() + " on " + std::to_string(_request_map[client_fd].get_fd()) + " " + std::to_string(413) + " \"Body too big\"", 3);
						break;
					default:
						response = _server_map[port]->send_error_message(400);
						_disconnect_after_send.push_back(client_fd);
						Logger::getInstance().log("", _request_map[client_fd].get_client_ip() + " on " + std::to_string(_request_map[client_fd].get_fd()) + " " + std::to_string(400) + " \"Wrong Request Header\"", 3);
						break;
				}
				_response_map[client_fd] = response;
				_request_map[client_fd].reset();
				// _request_map.erase(client_fd);
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

	// if (_cgi_map.find(client_fd) != _cgi_map.end())
	// {
	// 	handle_write_cgi(client_fd);
	// }
	if (response == "") // If the response is empty, it has to be a CGI process, in case of bug try if above
	{
		return (handle_write_cgi(client_fd));
	}
	ssize_t	bytes_sent = send(client_fd, response.c_str(), response.size(), 0);

	if (bytes_sent == -1)
	{
		Logger::getInstance().log("", "Sending to " + std::to_string(client_fd) + " failed/timeout", 2);
		remove_client(client_fd);
		return ;
	}
	response.erase(0, bytes_sent);
	// if (response.empty() || (response.find("302") != std::string::npos))
	if (response.empty())
	{
		if (std::find(_disconnect_after_send.begin(), _disconnect_after_send.end(), client_fd) != _disconnect_after_send.end())
		{
			auto it = std::remove(_disconnect_after_send.begin(), _disconnect_after_send.end(), client_fd);
			if (it != _disconnect_after_send.end())
			_disconnect_after_send.erase(it, _disconnect_after_send.end());
			remove_client(client_fd);
		}
		else
		{
			set_pollevent(client_fd, POLLIN);
			// remove_client(client_fd);
		}
	}
}

void	SocketManager::handle_write_cgi(int client_fd)
{
	auto	it = _cgi_map.find(client_fd);
	if (it == _cgi_map.end())
	{
		remove_client(client_fd);
		Logger::getInstance().log("", _request_map[client_fd].get_client_ip() + " on " + std::to_string(_request_map[client_fd].get_fd()) + " " + std::to_string(500) + " \"Something went wrong, no client inside the cgi_map, disconnecting client\"", 3);
		return;
	}
	ConnectionData&	con_data = it->second;
	char	buffer[BUFFER_SIZE];
	ssize_t	bytes_read;

	bytes_read = read(con_data.out_pipe[0], buffer, BUFFER_SIZE);
	if (bytes_read > 0)
	{
		ssize_t	bytes_sent = send(client_fd, buffer, bytes_read, 0);
		if (bytes_sent == -1)
		{
			Logger::getInstance().log("", "Sending to " + std::to_string(client_fd) + " failed/timeout", 2);
			close(con_data.out_pipe[0]);
			kill(con_data.child_pid, SIGTERM);
			_cgi_map.erase(it);
			remove_client(client_fd);
		}
	}
	else if (bytes_read == 0) // Pipe finished
	{}
	int	status;
	if (waitpid(con_data.child_pid, &status, WNOHANG) > 0)
	{
		close(con_data.out_pipe[0]);
		if (WIFEXITED(status) && WEXITSTATUS(status) != 0) // Script exited with an error code
		{
			Logger::getInstance().log("", _request_map[client_fd].get_client_ip() + " on " + std::to_string(_request_map[client_fd].get_fd()) + " " + std::to_string(500) + " \"Child Process exited with an error\"", 3);
			kill(con_data.child_pid, SIGTERM);
			_response_map[client_fd] = _server_map[con_data.server_port]->send_error_message(500);
			_cgi_map.erase(it);
		}
		else // Script exited as expected
		{
			kill(con_data.child_pid, SIGTERM);
			if (std::find(_disconnect_after_send.begin(), _disconnect_after_send.end(), client_fd) != _disconnect_after_send.end())
			{
				auto it2 = std::remove(_disconnect_after_send.begin(), _disconnect_after_send.end(), client_fd);
				if (it2 != _disconnect_after_send.end())
				_disconnect_after_send.erase(it2, _disconnect_after_send.end());
				_cgi_map.erase(it);
				remove_client(client_fd);
			}
			else
			{
				set_pollevent(client_fd, POLLIN);
			}
		}
	}
	else // Terminate if timeout
	{
		auto current_time = std::chrono::steady_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(current_time - con_data.start_time);

		if (elapsed >= std::chrono::seconds(SCRIPT_TIMEOUT))
		{
			close(con_data.out_pipe[0]);
			kill(con_data.child_pid, SIGTERM);
			_response_map[client_fd] = _server_map[con_data.server_port]->send_error_message(508);
			_cgi_map.erase(it);
		}
	}
}

std::string	SocketManager::handle_cgi(int client_fd, int server_port)
{
	// Ensures the client isnt already requesting a CGI Script
	// auto	it = _cgi_map.find(client_fd);
	// if (it != _cgi_map.end())
	// {
	// 	Logger::getInstance().log("",  _request_map[client_fd].get_client_ip() + " on " + std::to_string(_request_map[client_fd].get_fd()) + " " + std::to_string(429) + " \"Client tried running multiple scripts on one socket\"", 3);
	// 	return (_server_map[server_port]->send_error_message(429));
	// }
	if (_cgi_map.size() >= MAX_SCRIPTS) // A condition ensuring the server doesnt get overloaded
	{
		Logger::getInstance().log("",  _request_map[client_fd].get_client_ip() + " on " + std::to_string(_request_map[client_fd].get_fd()) + " " + std::to_string(500) + " \"Max amount of scripts already running\"", 3);
		return (_server_map[server_port]->send_error_message(503));
	}

	int		in_pipe[2];
	int		out_pipe[2];

	std::string	temp = _server_map[server_port]->getCgiFilePath();
	if (pipe(in_pipe) == -1)
	{
		Logger::getInstance().log("",  _request_map[client_fd].get_client_ip() + " on " + std::to_string(_request_map[client_fd].get_fd()) + " " + std::to_string(500) + " \"Pipe failed\"", 3);
		return (_server_map[server_port]->send_error_message(500));
	}
	if (pipe(out_pipe) == -1)
	{
		close(in_pipe[0]);
		close(in_pipe[1]);
		Logger::getInstance().log("",  _request_map[client_fd].get_client_ip() + " on " + std::to_string(_request_map[client_fd].get_fd()) + " " + std::to_string(500) + " \"Pipe failed\"", 3);
		return (_server_map[server_port]->send_error_message(500));
	}
	// * Setting the output pipe to NON_BLOCKING so reading from it doesnt halt the whole program.
	if (fcntl(out_pipe[1], F_SETFL, O_NONBLOCK) == -1)
	{
		close(in_pipe[0]);
		close(in_pipe[1]);
		close(out_pipe[0]);
		close(out_pipe[1]);
		Logger::getInstance().log("",  _request_map[client_fd].get_client_ip() + " on " + std::to_string(_request_map[client_fd].get_fd()) + " " + std::to_string(500) + " \"Configuring Pipe failed\"", 3);
		return (_server_map[server_port]->send_error_message(500));
	}
	if (fcntl(out_pipe[0], F_SETFL, O_NONBLOCK) == -1)
	{
		close(in_pipe[0]);
		close(in_pipe[1]);
		close(out_pipe[0]);
		close(out_pipe[1]);
		Logger::getInstance().log("",  _request_map[client_fd].get_client_ip() + " on " + std::to_string(_request_map[client_fd].get_fd()) + " " + std::to_string(500) + " \"Configuring Pipe failed\"", 3);
		return (_server_map[server_port]->send_error_message(500));
	}

	char *args[] = {
		const_cast<char *>("/usr/bin/python3"),
		const_cast<char *>(temp.c_str()),
		NULL
	};
	std::vector<const char*>	envp;
	std::vector<std::string>	temp1 = _server_map[server_port]->getCgiEnvStrings();
	for (auto& str : temp1)
		envp.push_back(str.c_str());
	envp.push_back(NULL);

	pid_t	pid = fork();
	if (pid == 0) // Child
	{
		dup2(in_pipe[0], STDIN_FILENO);
		dup2(out_pipe[1], STDOUT_FILENO);
		close(in_pipe[1]);
		close(out_pipe[0]);
		execve(args[0], const_cast<char* const*>(args), const_cast<char* const*>(envp.data()));
		exit(1);
	}
	else if (pid > 0) // Parent
	{
		close(in_pipe[0]);
		close(out_pipe[1]);
		if (_server_map[server_port]->getCgiPost() != "") // POSTing a CGI Method wants the POSTED as STDIN for the program.
		{
			write(in_pipe[1], _server_map[server_port]->getCgiPost().c_str(), _server_map[server_port]->getCgiPost().size());
		}
		close(in_pipe[1]);
		ConnectionData	con_data(client_fd, server_port, out_pipe, pid, std::chrono::steady_clock::now());
		_cgi_map[client_fd] = con_data;
		return ("");
	}
	else // Fork fail
	{
		close(in_pipe[0]);
		close(in_pipe[1]);
		close(out_pipe[0]);
		close(out_pipe[1]);
		Logger::getInstance().log("",  _request_map[client_fd].get_client_ip() + " on " + std::to_string(_request_map[client_fd].get_fd()) + " " + std::to_string(500) + " \"Fork failed\"", 3);
		return (_server_map[server_port]->send_error_message(500));
	}
}

void	SocketManager::set_pollevent(int client_fd, short events)
{
	for (auto& pfd : _fds)
	{
		if (pfd.fd == client_fd)
		{
			pfd.events = events;
			break;
		}
	}
}

void SocketManager::accept_connections()
{
	int	num_servers = _server_map.size();

	for (int i = 0; i < num_servers; i++)
	{
		if (_fds[i].revents & POLLIN)
		{
			struct sockaddr_in client_addr;
			socklen_t client_len = sizeof(client_addr);
			int new_socket = accept(_fds[i].fd, (struct sockaddr*)&client_addr, &client_len);

			if (new_socket < 0)
			{
				Logger::getInstance().log("", "Failed to accept connection on server socket: " + std::string(strerror(errno)), 3);
				continue;
			}

			if (fcntl(new_socket, F_SETFL, O_NONBLOCK) < 0)
			{
				Logger::getInstance().log("", "Failed to set non-blocking on new socket: " + std::string(strerror(errno)), 3);
				close(new_socket);
				continue;
			}
			_fds.push_back({ new_socket, POLLIN, 0 });
			_request_map[new_socket] = Request(new_socket);
			_request_map[new_socket].set_client_ip(client_addr.sin_addr.s_addr);
		}
	}
}
