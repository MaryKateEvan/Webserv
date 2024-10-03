
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "parse_config_file.hpp"

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
}

StringDataTracker	tracker;

bool	read_config_file(std::string file)
{
	std::cout << "\n";

	std::string conf;
	if (!file_to_string(file, conf))
	{
		tracker.report_generic(REPORT_ERROR, "File could not be read");
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
				std::cout << "    listen:" << server -> listen << ";\n";
				std::cout << "    root:" << server -> root << ";\n";
				for (int k = 0; k < server -> location.size(); k++)
				{
					ConfigData::ServerLocationData * location = server -> location[k];
					std::cout << "    location [" << k << "]\n";
					std::cout << "      path:" << location -> path << ";\n";
					std::cout << "      root:" << location -> root << ";\n";
					std::cout << "      allowed_methods:";
					for (int l = 0; l < location -> allowed_methods_num; l++)
						std::cout << " " << location -> allowed_methods[l];
					std::cout << ";\n";
				}
			}
		}
	}

	std::cout << "\n";
	return (true);
}

void	test_quote_tracking()
{
	std::string str = "    \' 123 456 \'  \"777 _ _ _ 8\" 9   ";

	std::cout << "\n";
	std::cout << "[" << str << "]\n";
	str = StringHelp::trim_whitespace(str);
	std::cout << "[" << str << "]\n";
	std::cout << "\n";

/*
	std::string str = "0000'1111'0000\"2222'2222\"'1111''1111\"1111\"1111'0000";
	tracker = new StringDataTracker(str);
	std::cout << str << "\n";

	Twin	s, d;
	size_t	pos = 0;

	while (pos != std::string::npos)
	{
		s = Twin::find(str, pos, '\'', '\"');
		d = Twin::find(str, pos, '\"', '\'');

		if (d.all_good() && s.all_good())
		{
			if (s.p1 < d.p1)
			{
				std::cout << "s[" << s.cut_in(str) << "]\n";
				pos = s.p2 + 1;
			}
			if (d.p1 < s.p1)
			{
				std::cout << "d[" << d.cut_in(str) << "]\n";
				pos = d.p2 + 1;
			}
		}
		else if (s.all_good())
		{
			std::cout << "s[" << s.cut_in(str) << "]\n";
			pos = s.p2 + 1;
		}
		else if (d.all_good())
		{
			std::cout << "d[" << d.cut_in(str) << "]\n";
			pos = d.p2 + 1;
		}
		else
		{
			pos = std::string::npos;
		}
	}
*/

//	char	quotes[str.length() + 1];
//	for (int i = 0; i < str.length(); i++)
//		quotes[i] = ' ';
//	quotes[str.length()] = ' ';
//
//	int	step_size = 3;
//	for (int i = 0; i < str.length(); i += step_size)
//	{
//		if (tracker.isSingleQuote)
//			quotes[i] = '\'';
//		else if (tracker.isDoubleQuote)
//			quotes[i] = '\"';
//		else
//			quotes[i] = '|';
//
//		tracker.update(step_size);
//	}
//
//	std::cout << str << "\n";
//	//std::cout << tracker.quotes << "\n";
//	std::cout << quotes << "\n";
}

int main(int argc, char * argv[])
{
	//test_quote_tracking();
	if (argc == 2)
		read_config_file(argv[1]);
	return (0);
}
