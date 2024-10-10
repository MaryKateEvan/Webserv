
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
			ConfigParse("server", 0, 0, ConfigData::HttpData::newServer, NULL, 10, new ConfigParse[10]
			{
				ConfigParse("server_names", 1, 1, NULL, ConfigData::ServerData::set_server_name),
				ConfigParse("server_name", 1, 1, NULL, ConfigData::ServerData::set_server_name),
				ConfigParse("listen", 1, 1, NULL, ConfigData::ServerData::set_listen),
				ConfigParse("root", 1, 1, NULL, ConfigData::ServerData::set_root),
				ConfigParse("index", 1, 1, NULL, ConfigData::ServerData::set_index),
				ConfigParse("keepalive_timeout", 1, 1, NULL, ConfigData::ServerData::set_keepalive_timeout),
				ConfigParse("send_timeout", 1, 1, NULL, ConfigData::ServerData::set_send_timeout),
				ConfigParse("max_body_size", 1, 1, NULL, ConfigData::ServerData::set_max_body_size),
				ConfigParse("directory_listing", 1, 1, NULL, ConfigData::ServerData::set_directory_listing),
				ConfigParse("location", 1, 1, ConfigData::ServerData::newLocation, NULL, 4, new ConfigParse[4]
				{
					ConfigParse("root", 1, 1, NULL, ConfigData::ServerLocationData::set_root),
					ConfigParse("allowed_methods", 1, 3, NULL, ConfigData::ServerLocationData::set_allowed_methods),
					ConfigParse("redirection", 1, 1, NULL, ConfigData::ServerLocationData::set_redirection),
					ConfigParse("request_types", 1, 3, NULL, ConfigData::ServerLocationData::set_request_types),
				}),
			}),
			ConfigParse("server_timeout_time", 1, 1, NULL, ConfigData::HttpData::set_server_timeout_time),
		}),
	});
	std::cout << "\n";
	how2parse.parse(data, config, "");	//	<--- config data is read here
	std::cout << "\nfile read (past tense)\n";
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
	}

	std::cout << "\n";
	return (true);
}

int main(int argc, char * argv[])
{
	if (argc == 2)
		read_config_file(argv[1]);
	return (0);
}
