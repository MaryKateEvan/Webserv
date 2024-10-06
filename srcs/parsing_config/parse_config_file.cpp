
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "../../includes/parsing_config/parse_config_file.hpp"

bool	file_to_string(std::string name, std::string & text)
{
	std::ifstream file;
	file.open(name);

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
	ConfigParse how2parse = ConfigParse("file", 0, 0, NULL, NULL, 1, (ConfigParse[])
	{
		ConfigParse("http", 0, 0, ConfigData::MainData::newHttp, NULL, 1, (ConfigParse[])
		{
			ConfigParse("server", 0, 0, ConfigData::HttpData::newServer, NULL, 5, (ConfigParse[])
			{
				ConfigParse("server_names", 1, 1, NULL, ConfigData::ServerData::setName, 0, NULL),
				ConfigParse("listen", 1, 1, NULL, ConfigData::ServerData::setListen, 0, NULL),
				ConfigParse("root", 1, 1, NULL, ConfigData::ServerData::setRoot, 0, NULL),
				ConfigParse("index", 1, 1, NULL, ConfigData::ServerData::setIndex, 0, NULL),
				ConfigParse("location", 1, 1, ConfigData::ServerData::newLocation, NULL, 2, (ConfigParse[])
				{
					ConfigParse("root", 1, 1, NULL, ConfigData::ServerLocationData::setRoot, 0, NULL),
					ConfigParse("allowed_methods", 1, 3, NULL, ConfigData::ServerLocationData::setAllowedMethods, 0, NULL),
				}),
			}),
		}),
	});
	std::cout << "\n";

	how2parse.parse(data, config);	//	<--- config data is read here
	std::cout << "\nfile read (past tense)\n";
	//std::cout << "O:" data -> http[0] -> server[0] -> listen.data[0] << ":\n";
}



StringDataTracker	tracker;

bool	read_config_file(std::string file)
{
	std::cout << "\n";

	std::string conf;
	if (!file_to_string(file, conf))
	{
		tracker.report_generic(REPORT_ERROR | REPORT_ERRNO, "File could not be read");
		return (false);
	}

	tracker = StringDataTracker(conf);
	if (StringHelp::has_unicode(conf))
	{
		tracker.report_generic(REPORT_ERROR, "This file appears to contain Unicode . . . which is dumb. Why would you put Unicode in a server configuration file? Idiot");
		return (false);
	}

	conf = StringHelp::remove_comments(conf);
	conf = StringHelp::trim_whitespace(conf);

	std::cout << "\n";
	Pair pair = Pair::find(conf, 0, '{', '}');
	if (!pair.all_good())
	{
		tracker.report_generic(REPORT_ERROR, "There are unpaired {} in the file, not reading that");
		return (false);
	}
	else
	{
		ConfigData::MainData data;
		parseConfig(conf, &data);

		/*
			this is where you would turn the data into the read servers and stuff
		*/
		std::cout << "\n";
		for (int i = 0; i < data.http.size(); i++)
		{
			ConfigData::HttpData * http = data.http[i];
			std::cout << "http [" << i << "]\n";
			for (int j = 0; j < http -> server.size(); j++)
			{
				ConfigData::ServerData * server = http -> server[j];
				std::cout << "  server [" << j << "]\n";
				std::cout << "    name:" << server -> name.data[0] << ";\n";
				std::cout << "    listen:" << server -> listen.data[0] << ";\n";
				std::cout << "    root:" << server -> root.data[0] << ";\n";
				std::cout << "    index:" << server -> index.data[0] << ";\n";
				for (int k = 0; k < server -> location.size(); k++)
				{
					ConfigData::ServerLocationData * location = server -> location[k];
					std::cout << "    location [" << k << "]\n";
					std::cout << "      path:" << location -> path << ";\n";
					std::cout << "      root:" << location -> root.data[0] << ";\n";
					std::cout << "      allowed_methods:";
					for (int l = 0; l < location -> allowed_methods.data_num; l++)
						std::cout << " " << location -> allowed_methods.data[l];
					std::cout << ";\n";
				}
			}
		}
	}

	std::cout << "\n";
	return (true);
}


