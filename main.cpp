#include "webserv.h"
#include "server/SocketManager.hpp"

/*
* Use curl localhost:PORT (replace port with actual value) to test responses
* USE wrk -t12 -c400 -d30s http://localhost:8080 to stress test server
* -t12 stands for 12 threads
* -c400 stands for 400 connections
* -d30s stands for a 30s test lenght
*/

void signal_handler(int signum);

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
		SocketManager	socket_manager;
		socket_manager.add_server(8080, std::make_unique<Server>("Instalight", 8080, "0.0.0.0", "index.html", "usrimg", "www_image_webpage", true, 0, 0, 3000000));
		socket_manager.add_server(8081, std::make_unique<Server>("A little webserver", 8081, "0.0.0.0", "index.html", "usrimg", "www", true, 0, 0, 3000000));
		socket_manager.handle_requests();

	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}
	return (0);
}


