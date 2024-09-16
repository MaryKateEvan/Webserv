#include "webserv.h"

void signal_handler(int signum);

int fd_server;

int main()
{
	std::signal(SIGINT, signal_handler);
	fd_server = socket(AF_INET, SOCK_STREAM, 0);
	// AF_INET for IPv4 (http), SOCK_STREAM as a standart byte based sequenced stream (for tcp), 0 as no specific protocol is used

	if (fd_server == -1)
	{
		std::cerr << "Socket creation failed" << std::endl;
		return (1);
	}

	//Makes sure the socket can still be used if the program exited unexpected
	int opt = 1;
	if (setsockopt(fd_server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		std::cerr << "setsockopt failed" << std::endl;
		close(fd_server);
		return (1);
	}

	struct sockaddr_in  address;

	address.sin_family = AF_INET;
	//always AF_INET (otherwise error returned)

	address.sin_port = htons(PORT);
	//port in network byte order (htonl to confert integer to network Byte order), PORTS under 1024 may only bind if privileged

	address.sin_addr.s_addr = INADDR_ANY;
	//IP host address (stored in network byte order), INADDR_ANY to assign to 0.0.0.0 = any address, use inet_aton() to convert IP Address into network byte order

	if (bind(fd_server, (struct sockaddr *)&address, sizeof(address)) != 0)
	{
	//bind to assign an address to the file descriptor
		std::cerr << "Binding to socket failed" << std::endl;
		close(fd_server);
		return (1);
	}

	if (listen(fd_server ,SOMAXCONN) != 0)
	{
	//marks socket as a passive socket that will be used to accept connections using accept
	//takes the fd and a int as a backlog (how many request can be queued)
		std::cerr << "Setting file descriptor to listen failed" << std::endl;
		close(fd_server);
		return (1);
	}
	std::cout << "Server is now listening on port " << PORT << std::endl;

	struct pollfd	fds[MAX_CLIENTS];
	int				nfds = 1;

	fds[0].fd = fd_server;
	fds[0].events = POLLIN;

	for (int i = 1; i < MAX_CLIENTS; i++)
	{
		fds[i].fd = -1;
	}

	//Server Loop
	while (true)
	{
		int	poll_count = poll(fds, nfds, -1);

		if (poll_count == -1)
		{
			std::cerr << "poll() failed" << std::endl;
			break ;
		}
		//checks for new client requests
		if (fds[0].revents & POLLIN)
		{
			socklen_t	addr_len = sizeof(address);
			int			fd_new_socket = accept(fd_server, (struct sockaddr *)&address, &addr_len);

			if (fd_new_socket == -1)
			{
				std::cerr << "Accept failed" << std::endl;
				continue ;
			}

			for (int i = 1; i < MAX_CLIENTS; i++)
			{
				if (fds[i].fd == -1)
				{
					fds[i].fd = fd_new_socket;
					fds[i].events = POLLIN;
					nfds = std::max(nfds, i + 1);
					std::cout << "New client connected on fd " << fd_new_socket << std::endl;
					break;
				}
			}
		}
		//sends data do available clients
		for (int i = 1; i < nfds; i++)
		{
			if (fds[i].fd != -1 && (fds[i].revents & POLLIN))
			{
				char	buffer[4096] = {0};
				int		bytes_read = read(fds[i].fd, buffer, 4096);

				if (bytes_read <= 0)
				{
					std::cout << "Client on fd " << fds[i].fd << " disconnected" << std::endl;
					close(fds[i].fd);
					fds[i].fd = -1;
				}
				else
				{
					std::cout << "Received request from client on fd " << fds[i].fd << std::endl;
					std::cout << "Received request: " << std::endl << buffer << std::endl;
					const char *response = "HTTP/1.1 200 OK\nContent-Type: text/html\n\nA surprise to be sure, but a welcome one!\n";
					if (send(fds[i].fd, response, strlen(response), 0) == -1)
					{
						std::cerr << "Send failed on fd " << fds[i].fd << std::endl;
					}
					close(fds[i].fd);
					fds[i].fd = -1;
				}
			}
		}
	}
	close(fd_server);

	return (0);
}

//Closes FD to make sure proper cleanup happens
void signal_handler(int signum)
{
	std::cerr << "Signal " << signum << " received, closing server..." << std::endl;
	if (fd_server != -1)
	{
		close(fd_server);
	}
	exit(0);
}