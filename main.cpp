#include "webserv.h"
#include "server/SocketManager.hpp"

/*
* Use curl localhost:PORT (replace port with actual value) to test responses
* USE wrk -t12 -c400 -d30s http://localhost:8080 to stress test server
* -t12 stands for 12 threads
* -c400 stands for 400 connections
* -d30s stands for a 30s test lenght
*/

void signalHandler(int signum)
{
	std::cout << "Interrupt signal (" << signum << ") received.\n";
	throw InterruptSignal();
}

int main(int argc, char **argv)
{
	std::cout << GREEN("🚀 Launching Webserv ... 🚀") << std::endl;
	if (argc > 2)
	{
		std::cout << ERROR("Expected usage: ") << UNDERLINE("./webserv") << " or " << UNDERLINE("./webserv <path_to_config_file>") << std::endl;
		return 0;
	}

	std::vector<ServerData> server_vec;
	try
	{
		if (argc == 2)
			server_vec = read_config_file(argv[1]);
		else
			server_vec = read_config_file("config_files/standart.conf");
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return (0);
	}
	std::signal(SIGINT, signalHandler);
	try
	{
		SocketManager	socket_manager;
		MimeTypes::getInstance();

		for (size_t s = 0; s < server_vec.size(); s++)
		{
			ServerData const & server = server_vec[s];
			socket_manager.add_server(server.port_to_listen, std::make_unique<Server>(server.server_name, server.port_to_listen, "0.0.0.0", server.index_file,
			"usrimg", server.root, server.directory_listing, server.keepalive_timeout, server.send_timeout, server.max_request_size, server.locations));
		}
		// socket_manager.add_server(8080, std::make_unique<Server>("Instalight", 8080, "0.0.0.0", "index.html", "usrimg", "www_image_webpage", true, 0, 0, 3000000));
		// socket_manager.add_server(8081, std::make_unique<Server>("A little webserver", 8081, "0.0.0.0", "index.html", "usrimg", "www", true, 0, 0, 3000000));
		socket_manager.handle_requests();
	}
	catch (const std::exception& e)
	{
			Logger::getInstance().log("", e.what(), 4);
	}
	return (0);
}
