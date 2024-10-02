
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "parse_config_file.hpp"

/*
this is done now

PROBLEM ????
new lines get removed due to trim_whitespace()
but they cant be left because that would mess up the splitting
also for the parsing they would need to be removed anyway

SOLUTION ----
++++ instead of regular strings, inherit from it to have a paralles string that remembers
++++ what line each character is on
---- if the config file has more then 255 lines this beaks
---- using something larger would be an excessive use of resources

SOLUTION ----
++++ parse each line individually
---- this would be stupid and really complicated considering what it dose

i think any attempt at storing where all the new lines are would be excessive
so you would have to figure out what line youre on on the fly

SOLUTION
++++ use "landmarks"
++++ the code knows what #pair of {} it's in
++++ search the original file for this #{} and cound the new lines before this point
++++ find and skip all previous elements, allways counting how many new lines were passed
++++ now find thr current element
---- what are the "landmarks"
---- i would have to use every segment as everything in between may have been trimmed
++++ why not keep track on the go



i need to keep track of
	string	file;
	int		pos;
	int		nl;

int	count_new_lined_from_to(string file, int p1, int p2)
{
	int nl = 0;
	int pos = file.find(p1, '\n');
	while (pos < p2)
	{
		nl++;
		pos = file.find(p1, '\n');
	}
	return (nl);
}

when at a segment
	search for segment in file
	nl += count_new_lined_from_to()
	search for next segment in file

*/

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



/*
	these are for gather all the configuration data
	and setting default values
*/
class server_location_config_data
{
	public:
		std::string	path;
		std::string	root;
		std::string	* allowed_methods;
		int			allowed_methods_num;
		server_location_config_data(std::string p)
		{
			path = p;
			root = "\e[31mNOT SET\e[m";
			allowed_methods = new std::string[1];
			allowed_methods[0] = "\e[31mNOT SET\e[m";
			allowed_methods_num = 1;
		}
		~server_location_config_data()
		{
			delete [] allowed_methods;
			allowed_methods_num = 0;
		}

		static void	set_root(void * ptr, int argc, std::string args[])
		{
			server_location_config_data * p = (server_location_config_data *)ptr;
			std::cout << "    server_location.set_root() '" << args[0] << "'\n";

			p -> root = args[0];
		}
		static void	set_allowed(void * ptr, int argc, std::string args[])
		{
			server_location_config_data * p = (server_location_config_data *)ptr;
			std::cout << "    server_location.set_allowed()";

			delete [] p -> allowed_methods;
			p -> allowed_methods = new std::string[argc];
			p -> allowed_methods_num = argc;
			for (int i = 0; i < argc; i++)
			{
				std::cout << " " << args[i];
				p -> allowed_methods[i] = args[i];
			}
			std::cout << "\n";
		}
};
class server_config_data
{
	public:
		std::string name;
		std::string listen;
		std::string root;
		std::string index;
		std::vector<server_location_config_data *> location;
		server_config_data()
		{
			listen = "\e[31mNOT SET\e[m";
			root = "\e[31mNOT SET\e[m";
		}
		~server_config_data()
		{
			for (int i = 0; i < location.size(); i++)
				delete location[i];
		}

		static void	set_name(void *ptr, int argc, std::string args[])
		{
			server_config_data * p = (server_config_data *)ptr;
			std::cout << "    server.set_name() '" << args[0] << "'\n";

			p -> name = args[0];
		}
		static void	set_listen(void *ptr, int argc, std::string args[])
		{
			server_config_data * p = (server_config_data *)ptr;
			std::cout << "    server.set_listen() '" << args[0] << "'\n";

			p -> listen = args[0];
		}
		static void	set_root(void *ptr, int argc, std::string args[])
		{
			server_config_data * p = (server_config_data *)ptr;
			std::cout << "    server.set_root() '" << args[0] << "'\n";

			p -> root = args[0];
		}
		static void	set_index(void *ptr, int argc, std::string args[])
		{
			server_config_data * p = (server_config_data *)ptr;
			std::cout << "    server.set_index() '" << args[0] << "'\n";

			p -> index = args[0];
		}

		static void	*new_location(void *ptr, int argc, std::string args[])
		{
			server_config_data * p = (server_config_data *)ptr;
			std::cout << "    server.new_location() '" << args[0] << "'\n";

			server_location_config_data * location = new server_location_config_data(args[0]);
			p -> location.push_back(location);
			return location;
		}
};
class http_config_data
{
	public:
		std::vector<server_config_data *> server;
		http_config_data()
		{

		}
		~http_config_data()
		{
			for (int i = 0; i < server.size(); i++)
				delete server[i];
		}

		static void	*new_server(void *ptr, int argc, std::string args[])
		{
			http_config_data * p = (http_config_data *)ptr;
			std::cout << "  http.new_server()\n";

			server_config_data * server = new server_config_data();
			p -> server.push_back(server);
			return server;
		}
};
class main_config_data
{
	public:
		std::vector<http_config_data *> http;
		main_config_data()
		{

		}
		~main_config_data()
		{
			for (int i = 0; i < http.size(); i++)
				delete http[i];
		}

		static void	*new_http(void *ptr, int argc, std::string args[])
		{
			main_config_data * p = (main_config_data *)ptr;
			std::cout << "main.new_http()\n";

			http_config_data * http = new http_config_data();
			p -> http.push_back(http);
			return http;
		}
};



//	this thing is for giving the line when a "warning unknown" is printed
StringDataTracker	*tracker;

bool	read_config_file(std::string file)
{
	std::cout << "\n";

	std::string conf;
	if (!file_to_string(file, conf))
	{
		std::cerr << "could not read config file because: '" << strerror(errno) << "'\n";
		return (false);
	}
	tracker = new StringDataTracker(conf);

	conf = StringHelp::remove_comments(conf);
	conf = StringHelp::trim_whitespace(conf);

	/*
		this is where the structure for all the data is specified
	*/
	ConfigParse how2parse("file", 0, 0, NULL, NULL, 1, (ConfigParse[])
	{
		ConfigParse("http", 0, 0, main_config_data::new_http, NULL, 1, (ConfigParse[])
		{
			ConfigParse("server", 0, 0, http_config_data::new_server, NULL, 5, (ConfigParse[])
			{
				ConfigParse("server_names", 1, 1, NULL, server_config_data::set_name, 0, NULL),
				ConfigParse("listen", 1, 1, NULL, server_config_data::set_listen, 0, NULL),
				ConfigParse("root", 1, 1, NULL, server_config_data::set_root, 0, NULL),
				ConfigParse("index", 1, 1, NULL, server_config_data::set_index, 0, NULL),
				ConfigParse("location", 1, 1, server_config_data::new_location, NULL, 2, (ConfigParse[])
				{
					ConfigParse("root", 1, 1, NULL, server_location_config_data::set_root, 0, NULL),
					ConfigParse("allowed_methods", 1, 3, NULL, server_location_config_data::set_allowed, 0, NULL),
				}),
			}),
		}),
	});

	std::cout << "\n";
	Pair pair = Pair::find(conf, 0, '{', '}');
	if (!pair.all_good())
	{
		std::cout << "unpaired {} in config file\n";
		return (false);
	}
	else
	{
		main_config_data data;
		how2parse.parse(&data, conf);	//	<--- config data is read here


		/*
			this is where you would turn the data into the read servers and stuff
		*/
		std::cout << "\n";
		for (int i = 0; i < data.http.size(); i++)
		{
			http_config_data * http = data.http[i];
			std::cout << "http [" << i << "]\n";
			for (int j = 0; j < http -> server.size(); j++)
			{
				server_config_data * server = http -> server[j];
				std::cout << "  server [" << j << "]\n";
				std::cout << "    listen:" << server -> listen << ";\n";
				std::cout << "    root:" << server -> root << ";\n";
				for (int k = 0; k < server -> location.size(); k++)
				{
					server_location_config_data * location = server -> location[k];
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
	delete tracker;
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
//		if (tracker -> isSingleQuote)
//			quotes[i] = '\'';
//		else if (tracker -> isDoubleQuote)
//			quotes[i] = '\"';
//		else
//			quotes[i] = '|';
//
//		tracker -> update(step_size);
//	}
//
//	std::cout << str << "\n";
//	//std::cout << tracker -> quotes << "\n";
//	std::cout << quotes << "\n";

	delete tracker;
}

int main(int argc, char * argv[])
{
	//test_quote_tracking();
	if (argc == 2)
		read_config_file(argv[1]);
	return (0);
}
