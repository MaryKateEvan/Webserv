#include "../webserv.h"
#include "../utils.h"

/*
* Use curl localhost:PORT (replace port with actual value) to test responses
* USE wrk -t12 -c400 -d30s http://localhost:8080 to stress test server
* -t12 stands for 12 threads
* -c400 stands for 400 connections
* -d30s stands for a 30s test lenght
*/

void signal_handler(int signum);

int fd_server;

int main(int argc, char **argv)
{
	std::cout << GREEN("🚀 Launching Webserv ... 🚀") << std::endl;
	if (argc > 2)
	{
		std::cout << ERROR("Expected usage: ") << UNDERLINE("./webserv") << " or " << UNDERLINE("./webserv <path_to_config_file>") << std::endl;
		return 0;
	}

	//receiving values from the `example1.config`:
	std::vector<ServerData> servers;
	fill_server_data(servers);
	print_server_data(servers);

	//so that then the SocketsControl receives the vector servers:
	SocketsControl controller(servers);

	return 0;

	try
	{
		// if (argc == 2)
		// 	read_config_file(argv[1]); //calls the parsing part
		std::signal(SIGINT, signal_handler);
		// Server	server1("A little webserver", PORT, "0.0.0.0", "index.html", "usrimg", "www");
		Server	server1("An Instagram Knockoff", PORT, "0.0.0.0", "index.html", "usrimg", "www_image_webpage");

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
					std::vector<char>	accumulated_request;
					std::vector<char>	buffer(4096);
					size_t				content_len = 0;
					bool				header_parsed = false;
					size_t				body_bytes_read = 0;
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
						if (!header_parsed)
						{
							size_t	header_end = temp_request.find("\r\n\r\n");
							if (header_end != std::string::npos)
							{
								header_parsed = true;
								std::string	headers = temp_request.substr(0, header_end + 4);
								size_t content_len_pos = headers.find("Content-Length: ");
								// std::cout << "Headers:\n" << headers << std::endl;
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
						Request req(std::string(accumulated_request.begin(), accumulated_request.end()), fds[i].fd);
 						server1.process_request(req);
					}
					accumulated_request.clear();
					close(fds[i].fd);
					fds[i].fd = -1;
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
