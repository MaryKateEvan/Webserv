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
		Server	server1("A little webserver", PORT, "0.0.0.0", "index.html", "usrimg", "www");
		// Server	server1("A Instagram Knockoff", PORT, "0.0.0.0", "index.html", "images", "image_website");
		// Set response Method is outdated since the server is using the file system in dir /www/
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
					std::vector<char> accumulated_request;
					std::vector<char> buffer(4096); 
					while (true)
					{
						int bytes_read = read(fds[i].fd, buffer.data(), buffer.size());

						if (bytes_read < 0)
						{
							std::cerr << "Error reading from fd " << fds[i].fd << std::endl;
							close(fds[i].fd);
							fds[i].fd = -1;
							break;
						}
						if (bytes_read == 0)
						{
							std::cout << "Client on fd " << fds[i].fd << " disconnected" << std::endl;
							close(fds[i].fd);
							fds[i].fd = -1;
							break;
						}
						accumulated_request.insert(accumulated_request.end(), buffer.begin(), buffer.begin() + bytes_read);
						std::string temp_request(accumulated_request.begin(), accumulated_request.end());
						if (temp_request.find("\r\n\r\n") != std::string::npos)
						{
							// std::cout << "Request:\n\n" << std::string(accumulated_request.begin(), accumulated_request.end()) << std::endl;
							Request req(std::string(accumulated_request.begin(), accumulated_request.end()), fds[i].fd);
							server1.process_request(req);
							accumulated_request.clear();
							close(fds[i].fd);
							fds[i].fd = -1;
							break;
						}
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
