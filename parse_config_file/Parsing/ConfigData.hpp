
#pragma once
#include <iostream>
#include <vector>

/*
	these are for gather all the configuration data
	and setting default values
*/
class ConfigData
{
	public:
		static void	printFuncArgs(std::string func, int argc, std::string args[]);

		class ServerLocationData
		{
				static const std::string	className;
			public:
				std::string	path;
				std::string	root;
				std::string	* allowed_methods;
				int			allowed_methods_num;

				ServerLocationData(std::string path);
				~ServerLocationData();

				static void	setRoot(void * ptr, int argc, std::string args[]);
				static void	setAllowed(void * ptr, int argc, std::string args[]);
		};
		class ServerData
		{
				static const std::string	className;
			public:
				std::string name;
				std::string listen;
				std::string root;
				std::string index;
				std::vector<ServerLocationData *> location;

				ServerData();
				~ServerData();

				static void	setName(void * ptr, int argc, std::string args[]);
				static void	setListen(void * ptr, int argc, std::string args[]);
				static void	setRoot(void * ptr, int argc, std::string args[]);
				static void	setIndex(void * ptr, int argc, std::string args[]);
				static void	*newLocation(void * ptr, int argc, std::string args[]);
		};
		class HttpData
		{
				static const std::string	className;
			public:
				std::vector<ServerData *> server;

				HttpData();
				~HttpData();

				static void	*newServer(void * ptr, int argc, std::string args[]);
		};
		class MainData
		{
				static const std::string	className;
			public:
				std::vector<HttpData *> http;

				MainData();
				~MainData();

				static void	*newHttp(void * ptr, int argc, std::string args[]);
		};
};

