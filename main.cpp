#include "webserv.h"

/*
* Use curl localhost:PORT (replace port with actual value) to test responses
* USE wrk -t12 -c400 -d30s http://localhost:8080 to stress test server
* -t12 stands for 12 threads
* -c400 stands for 400 connections
* -d30s stands for a 30s test lenght
*/

void signal_handler(int signum);

int fd_server;

int main()
{
	try
	{
		std::signal(SIGINT, signal_handler);
		Server	server1("A little webserver", PORT, "0.0.0.0");
		server1.setResponse("HTTP/1.1 200 OK\nContent-Type: text/html\n\nA surprise to be sure, but a welcome one!\n");

		struct pollfd	fds[MAX_CLIENTS];
		int				nfds = 1;

		fds[0].fd = server1.getServerFD();
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
				int			fd_new_socket = server1.acceptConnection();

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

						std::string	request(buffer);
						std::string	url = server1.extract_request(request);
						std::string	file_path = server1.map_to_directory(url);

						if (server1.file_exists(file_path))
						{
							std::string	file_content = server1.read_file(file_path);
							std::string	mime_type = server1.get_mime_type(file_path);
							std::string response = "HTTP/1.1 200 OK\nContent-Type: " + mime_type + "\n\n" + file_content;
							if (send(fds[i].fd, response.c_str(), response.size(), 0) == -1)
								std::cerr << "Send failed on fd " << fds[i].fd << std::endl;
						}
						else
						{
							std::string error_response = "HTTP/1.1 404 Not Found\nContent-Type: text/html\n\n404 File Not Found";
							if (send(fds[i].fd, error_response.c_str(), error_response.size(), 0) == -1)
								std::cerr << "Send failed on fd " << fds[i].fd << std::endl;
						}
						close(fds[i].fd);
						fds[i].fd = -1;
					}
				}
			}
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}
	return (0);
}

//Closes FD to make sure proper cleanup happens
void signal_handler(int signum)
{
	std::cerr << "Signal " << signum << " received, closing server..." << std::endl;
	exit(0);
}