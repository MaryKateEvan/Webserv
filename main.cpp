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
		// Server	server1("A little webserver", PORT, "0.0.0.0", "index.html", "usrimg", "www");
		Server	server1("Instalight", PORT, "0.0.0.0", "index.html", "usrimg", "www_image_webpage", true, 0, 0, 3000000);

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
					Request	req(fds[i].fd);
					std::vector<char>	buffer(4096);
					int	ret;
					while (true)
					{
						if (req.correct_body_size(server1.getMaxBodySize()) == false)
						{
							ret = -10;
							break ;
						}
						int bytes_read = read(fds[i].fd, buffer.data(), buffer.size());
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
						std::string response = server1.process_request(req);
						send(req.get_fd(), response.c_str(), response.size(), 0);
						// std::cout << "The requested host is: " << req.get_host() << std::endl;
						close(fds[i].fd);
						fds[i].fd = -1;
					}
					if (ret == -10)
					{
						std::string response = server1.send_error_message(413);
						send(req.get_fd(), response.c_str(), response.size(), 0);
					}
					else if (ret < 0)
					{
						std::string response = server1.send_error_message(400);
						send(req.get_fd(), response.c_str(), response.size(), 0);
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
