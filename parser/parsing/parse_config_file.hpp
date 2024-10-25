
#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "../stringing/StringHelp.hpp"
#include "../stringing/StringArr.hpp"
#include "../stringing/Pair.hpp"
#include "../stringing/Twin.hpp"
#include "../stringing/StringDataTracker.hpp"
#include "ConfigParse.hpp"
#include "ConfigData.hpp"

struct LocationData
{
	std::string					path;
	std::vector<std::string> 	allowed_methods;
	bool						redirection;
	std::string					path_to_redirect;
};

struct ServerData 
{
	std::string		index_file;				// the "default file" to serve the client
	std::string		server_name;			// the hostname of the server
	short			port_to_listen;			// the port on which the server listens for incoming connections
	std::string		root;					// the directory where the server looks for files to serve the client (in our case it's always the `www`)
	bool			directory_listing;		// flag to be set to true or false. (whether to allow directory listing when no index file is present)
	size_t 			max_request_size;		// limitation of the request that the client can send
	size_t			keepalive_timeout;		// the time to keep connections open between requests
	size_t			send_timeout;			// the time to wait a response to be sent to the client

	std::vector<LocationData> locations;	// contains the info of the different `location` blocks
};

std::vector<ServerData>	read_config_file(std::string file);

class	ConfigErrorException : public std::exception
{
	public:
		const char * what() const throw();
};
