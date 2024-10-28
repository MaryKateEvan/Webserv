#include "parse_config_file.hpp"

bool	file_to_string(std::string name, std::string & text)
{
	std::ifstream file;
	file.open(name.c_str());

	if (!file.is_open())
		return (false);

	std::string line;
	while (!file.eof())
	{
		std::getline(file, line);
		text += line + "\n";
	}

	file.close();
	return (true);
}



void	parseConfig(std::string config, ConfigData::MainData * data)
{
	ConfigParse how2parse = ConfigParse("file", 0, 0, NULL, NULL, 1, new ConfigParse[1]
	{
		ConfigParse("http", 0, 0, ConfigData::MainData::newHttp, NULL, 2, new ConfigParse[2]
		{
			ConfigParse("server", 0, 0, ConfigData::HttpData::newServer, NULL, 9, new ConfigParse[9]
			{
				ConfigParse("server_name", 1, 1, NULL, ConfigData::ServerData::set_server_name),
				ConfigParse("listen", 1, 1, NULL, ConfigData::ServerData::set_listen),
				ConfigParse("root", 1, 1, NULL, ConfigData::ServerData::set_root),
				ConfigParse("index", 1, 1, NULL, ConfigData::ServerData::set_index),
				ConfigParse("keepalive_timeout", 1, 1, NULL, ConfigData::ServerData::set_keepalive_timeout),
				ConfigParse("send_timeout", 1, 1, NULL, ConfigData::ServerData::set_send_timeout),
				ConfigParse("max_body_size", 1, 1, NULL, ConfigData::ServerData::set_max_body_size),
				ConfigParse("directory_listing", 1, 1, NULL, ConfigData::ServerData::set_directory_listing),
				ConfigParse("location", 1, 1, ConfigData::ServerData::newLocation, NULL, 2, new ConfigParse[2]
				{
					ConfigParse("allowed_methods", 1, 3, NULL, ConfigData::ServerLocationData::set_allowed_methods),
					ConfigParse("redirection", 1, 1, NULL, ConfigData::ServerLocationData::set_redirection),
				}),
			}),
			ConfigParse("server_timeout_time", 1, 1, NULL, ConfigData::HttpData::set_server_timeout_time),
		}),
	});
	//std::cout << "\n";
	how2parse.parse(data, config, "");	//	<--- config data is read here
}



StringDataTracker	tracker;

std::vector<ServerData>	read_config_file(std::string file)
{
	std::cout << "\n";

	std::string conf;
	if (!file_to_string(file, conf))
	{
		tracker.report_generic(REPORT_ERROR | REPORT_ERRNO, "File could not be read");
		throw ConfigErrorException();
	}

	tracker = StringDataTracker(conf);
	if (StringHelp::has_unicode(conf))
	{
		tracker.report_generic(REPORT_ERROR, "This file appears to contain Unicode");
		throw ConfigErrorException();
	}

	conf = StringHelp::remove_comments(conf);
	conf = StringHelp::trim_whitespace(conf);

	//std::cout << "\n";
	Pair pair = Pair::find(conf, 0, '{', '}');
	if (!pair.all_good())
	{
		tracker.report_generic(REPORT_ERROR, "There are unpaired {} in the file");
		throw ConfigErrorException();
	}

	ConfigData::MainData data;
	parseConfig(conf, &data);
	// std::cout << "INFO: config file reading done\n";
	Logger::getInstance().log("", "Config file read", 2);

	if (data.http.size() == 0)
	{
		tracker.report_generic(REPORT_ERROR, "No http blocks given");
		throw ConfigErrorException();
	}
	else if (data.http.size() != 1)
	{
		tracker.report_generic(REPORT_WARNING, "Multiple http blocks given. Only the first one will be used");
	}
	//std::cout << "\n";

	std::vector<ServerData>	server_vec;
	{
		ConfigData::HttpData * http_ptr = data.http[0];
		for (size_t s = 0; s < http_ptr -> server.size(); s++)
		{
			ConfigData::ServerData * server_ptr = http_ptr -> server[s];
			ServerData server;

			server.server_name = server_ptr -> server_name.get<std::string>("default");
			server.port_to_listen = server_ptr -> listen.get<short>(80);
			server.root = server_ptr -> root.get<std::string>("/");
			server.index_file = server_ptr -> index.get<std::string>("test");

			server.keepalive_timeout = server_ptr -> keepalive_timeout.get<size_t>(0);
			server.max_request_size = server_ptr -> max_body_size.get<size_t>(0);
			server.send_timeout = server_ptr -> send_timeout.get<size_t>(0);

			server.directory_listing = server_ptr -> directory_listing.get(false);
			for (size_t l = 0; l < server_ptr -> location.size(); l++)
			{
				ConfigData::ServerLocationData * location_ptr = server_ptr -> location[l];
				LocationData location;

				location.path = location_ptr -> path;
				location.redirection = location_ptr -> redirection.isSet;
				location.path_to_redirect = location_ptr -> redirection.get<std::string>("", location.redirection);
				location_ptr -> allowed_methods.get<std::string>(location.allowed_methods);

				server.locations.push_back(location);
			}

			server_vec.push_back(server);
		}
	}

	// std::cout << "INFO: config data interpreting done\n";
	Logger::getInstance().log("", "Config file interpreted", 2);

	return (server_vec);
}

const char * ConfigErrorException::what() const throw()
{
	return ("An Error occured when parsing the Configuration File.");
}


// int main(int argc, char * argv[])
// {
// 	if (argc == 2)
// 	{
// 		std::vector<ServerData> server_vec = read_config_file(argv[1]);

// 		std::cout << "\n";
// 		for (size_t s = 0; s < server_vec.size(); s++)
// 		{
// 			ServerData const & server = server_vec[s];
// 			std::cout << "\nserver [" << s << "]\n";
// 			std::cout << "  server_name:" << server.server_name << "\n";
// 			std::cout << "  port_to_listen:" << server.port_to_listen << "\n";
// 			std::cout << "  root:" << server.root << "\n";
// 			std::cout << "  index_file:" << server.index_file << "\n";
// 			std::cout << "  keepalive_timeout:" << server.keepalive_timeout << "\n";
// 			std::cout << "  max_request_size:" << server.max_request_size << "\n";
// 			std::cout << "  send_timeout:" << server.send_timeout << "\n";
// 			std::cout << "  directory_listing:" << server.directory_listing << "\n";

// 			for (size_t l = 0; l < server.locations.size(); l++)
// 			{
// 				LocationData const & location = server.locations[l];
// 				std::cout << "  location [" << l << "]\n";
// 				std::cout << "    path:" << location.path << "\n";
// 				std::cout << "    redirection:" << location.redirection << "\n";
// 				std::cout << "    path_to_redirect:" << location.path_to_redirect << "\n";
// 				std::cout << "    allowed_methods(";
// 				for (size_t m = 0; m < location.allowed_methods.size(); m++)
// 					std::cout << " " << location.allowed_methods[m];
// 				std::cout << " )\n";
// 			}
// 		}
// 		std::cout << "\n";
// 	}
// 	return (0);
// }
