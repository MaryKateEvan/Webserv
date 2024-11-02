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
	if (argc > 2)
	{
		std::cout << ERROR("use either ") << UNDERLINE("./webserv") << " or " 
				<< UNDERLINE("./webserv <path_to_config_file>") << std::endl;
		return 0;
	}
	std::cout << GREEN("\n🚀 Webserv is up and running ... 🚀\n") << std::endl;
	std::cout << YELLOW(BOLD("💡 TIP: ")) << "For server's " << BOLD("output") 
			<< " check the " << UNDERLINE("Log directory") << ". 📁 🔍" << std::endl;

	std::vector<ServerData> server_vec;
	try
	{
		if (argc == 2)
			server_vec = read_config_file(argv[1]);
		else
			server_vec = read_config_file("config_files/default.conf");
	}
	catch(const std::exception& e)
	{
		std::cerr << RED("❗ " << UNDERLINE(e.what())) << std::endl;
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
			socket_manager.add_server(server.port_to_listen, std::make_unique<Server>(server.server_name, server.port_to_listen, server.index_file,
			"usrimg", server.root, server.directory_listing, server.keepalive_timeout, server.send_timeout, server.max_request_size, server.locations));
		}
		socket_manager.handle_requests();
	}
	catch (const std::exception& e)
	{
		Logger::getInstance().log("", e.what(), 4);
	}
	return (0);
}
