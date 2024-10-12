
#pragma once
#include <iostream>
#include <vector>
#include "../stringing_from_config/StringArr.hpp"
#include "../stringing_from_config/StringDataTracker.hpp"

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
				const std::string	name;
				bool				isSet;
				StringArr			data;
				int					data_num;

				MemberData(std::string name, int num = 1);

				void	set(int argc, std::string args[], std::string funcName, std::string className);
		};

		static void	printFuncArgs(std::string func, int argc, std::string args[]);

		class ServerLocationData
		{
				static const std::string	className;
			public:
				const std::string	path;
				MemberData			root;
				MemberData			allowed_methods;

				ServerLocationData(std::string path);
				~ServerLocationData();

				static void	setRoot(void * ptr, int argc, std::string args[]);
				static void	setAllowedMethods(void * ptr, int argc, std::string args[]);
		};
		class ServerData
		{
				static const std::string	className;
			public:
				MemberData	name;
				MemberData	listen;
				MemberData	root;
				MemberData	index;
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

