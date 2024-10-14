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

int main(int argc, char **argv)
{
	std::cout << GREEN("🚀 Launching Webserv ... 🚀") << std::endl;
	if (argc > 2)
	{
		std::cout << ERROR("Expected usage: ") << UNDERLINE("./webserv") << " or " << UNDERLINE("./webserv <path_to_config_file>") << std::endl;
		return 0;
	}

	//receiving values from the `example1.config`:
	// fill_server_data(servers); //what the parser from the config will fill out
	// print_server_data(servers); // for verification
	(void)argv;

	try
	{
		//so that then the SocketsControl receives the vector servers:
		Server	server1("Instalight", PORT, "0.0.0.0", "index.html", "usrimg", "www_image_webpage", true, 0, 0, 0);
		std::vector<Server> servers;
		servers.push_back(server1);
		Polling	manager(servers);

		// std::signal(SI
	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}
	return (0);
}


