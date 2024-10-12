#include "../includes/webserv.h"

// this file fills all the struct data according to the example1.config

void fill_server_data(std::vector<ServerData>& servers)
{
	// Server 1
	{
		ServerData server1;
		server1.index_file = "index.html";
		server1.server_name = "localhost";
		server1.port_to_listen = 8080;
		server1.root = "www";
		server1.directory_listing = true;
		server1.max_request_size = 100000000;
		server1.keepalive_timeout = 31;
		server1.send_timeout = 11;

		// Locations for Server 1
		LocationData loc1;
		loc1.path = "/";
		loc1.allowed_methods = {"GET", "POST"};
		loc1.redirection = false;
		loc1.path_to_redirect = "";
		server1.locations.push_back(loc1);

		LocationData loc2;
		loc2.path = "/42heilbronn";
		loc2.allowed_methods = {"GET"};
		loc2.redirection = true;
		loc2.path_to_redirect = "https://www.42heilbronn.de/";
		server1.locations.push_back(loc2);

		LocationData loc3;
		loc3.path = "/google";
		loc3.allowed_methods = {};
		loc3.redirection = true;
		loc3.path_to_redirect = "www.google.com";
		server1.locations.push_back(loc3);

		LocationData loc4;
		loc4.path = "/uploads";
		loc4.allowed_methods = {"GET", "POST", "DELETE"};
		loc4.redirection = false;
		loc4.path_to_redirect = "";
		server1.locations.push_back(loc4);

		servers.push_back(server1);
	}

	// Server 2
	{
		ServerData server2;
		server2.index_file = "index.html";
		server2.server_name = "127.0.0.1";
		server2.port_to_listen = 8081;
		server2.root = "www";
		server2.directory_listing = false;
		server2.max_request_size = 200;
		server2.keepalive_timeout = 61;
		server2.send_timeout = 31;

		// Locations for Server 2
		LocationData loc1;
		loc1.path = "/";
		loc1.allowed_methods = {"GET", "POST"};
		loc1.redirection = false;
		loc1.path_to_redirect = "";
		server2.locations.push_back(loc1);

		LocationData loc2;
		loc2.path = "/google";
		loc2.allowed_methods = {"GET"};
		loc2.redirection = true;
		loc2.path_to_redirect = "www.google.com";
		server2.locations.push_back(loc2);

		LocationData loc3;
		loc3.path = "/uploads";
		loc3.allowed_methods = {"GET", "POST"};
		loc3.redirection = false;
		loc3.path_to_redirect = "";
		server2.locations.push_back(loc3);

		LocationData loc4;
		loc4.path = "/surprise";
		loc4.allowed_methods = {};
		loc4.redirection = true;
		loc4.path_to_redirect = "https://www.youtube.com/watch?v=aAkMkVFwAoo/";
		server2.locations.push_back(loc4);

		servers.push_back(server2);
	}
}

// And for verification of all the above:

void print_server_data(const std::vector<ServerData>& servers)
{
	for (size_t i = 0; i < servers.size(); ++i)
	{
		const ServerData& server = servers[i];
		std::cout << "Server " << i + 1 << ":\n";
		std::cout << "  Index file: " << server.index_file << "\n";
		std::cout << "  Server name: " << server.server_name << "\n";
		std::cout << "  Port: " << server.port_to_listen << "\n";
		std::cout << "  Root: " << server.root << "\n";
		std::cout << "  Directory listing: " << (server.directory_listing ? "true" : "false") << "\n";
		std::cout << "  Max request size: " << server.max_request_size << "\n";
		std::cout << "  Keepalive timeout: " << server.keepalive_timeout << "\n";
		std::cout << "  Send timeout: " << server.send_timeout << "\n";

		for (size_t j = 0; j < server.locations.size(); ++j)
		{
			const LocationData& location = server.locations[j];
			std::cout << "  Location " << j + 1 << ":\n";
			std::cout << "    Path: " << location.path << "\n";
			std::cout << "    Allowed methods: ";
			for (size_t k = 0; k < location.allowed_methods.size(); ++k)
			{
				std::cout << location.allowed_methods[k] << (k < location.allowed_methods.size() - 1 ? ", " : "");
			}
			std::cout << "\n    Redirection: " << (location.redirection ? "true" : "false") << "\n";
			if (location.redirection)
			{
				std::cout << "    Path to redirect: " << location.path_to_redirect << "\n";
			}
		}
		std::cout << std::endl;
	}
}