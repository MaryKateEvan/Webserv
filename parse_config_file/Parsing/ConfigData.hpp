
#pragma once
#include <iostream>
#include <sstream>
#include <vector>
#include "../Stringing/StringArr.hpp"
#include "../Stringing/StringDataTracker.hpp"

/*
	these are for gather all the configuration data
	and setting default values
*/
class ConfigData
{
	public:
		class	MemberData
		{
			public:
				const char *		name;
				int					setAtLine;
				bool				isSet;
				StringArr			*data;

				MemberData(const char * name);
				~MemberData();

				void	set(int argc, std::string args[], int line, std::string funcName, std::string className);
				template<typename T>
				T		get(T def, size_t idx = 0) const
				{
					if (!isSet)
						return (def);

					if (idx >= data -> num)
						return (def);

					try
					{
						T temp;
						std::stringstream ss;
						ss << (data -> arr[idx]);
						ss >> temp;
						return (temp);
					}
					catch(const std::exception& e)
					{
						return (def);
					}

					return (def);
				};
				void	print(std::string tab);
		};

		static void	printFuncArgs(std::string func, int argc, std::string args[]);

		class ServerLocationData
		{
				static const std::string	className;
			public:
				const std::string	path;
				MemberData			allowed_methods;
				MemberData			redirection;

				ServerLocationData(std::string path);
				~ServerLocationData();

				static void	set_allowed_methods(void * ptr, int line, int argc, std::string args[]);
				static void	set_redirection(void * ptr, int line, int argc, std::string args[]);

				void	print();
		};
		class ServerData
		{
				static const std::string	className;
			public:
				MemberData	server_name;
				MemberData	listen;
				MemberData	root;
				MemberData	index;
				MemberData	keepalive_timeout;
				MemberData	send_timeout;
				MemberData	max_body_size;
				MemberData	directory_listing;
				std::vector<ServerLocationData *> location;

				ServerData();
				~ServerData();

				static void	set_server_name(void * ptr, int line, int argc, std::string args[]);
				static void	set_listen(void * ptr, int line, int argc, std::string args[]);
				static void	set_root(void * ptr, int line, int argc, std::string args[]);
				static void	set_index(void * ptr, int line, int argc, std::string args[]);
				static void	set_keepalive_timeout(void * ptr, int line, int argc, std::string args[]);
				static void	set_send_timeout(void * ptr, int line, int argc, std::string args[]);
				static void	set_max_body_size(void * ptr, int line, int argc, std::string args[]);
				static void	set_directory_listing(void * ptr, int line, int argc, std::string args[]);
				static void	*newLocation(void * ptr, int line, int argc, std::string args[]);

				void	print();
		};
		class HttpData
		{
				static const std::string	className;
			public:
				std::vector<ServerData *> server;
				MemberData	server_timeout_time;

				HttpData();
				~HttpData();

				static void	*newServer(void * ptr, int line, int argc, std::string args[]);
				static void	set_server_timeout_time(void * ptr, int line, int argc, std::string args[]);

				void	print();
		};
		class MainData
		{
				static const std::string	className;
			public:
				std::vector<HttpData *> http;

				MainData();
				~MainData();

				static void	*newHttp(void * ptr, int line, int argc, std::string args[]);

				void	print();
		};
};

