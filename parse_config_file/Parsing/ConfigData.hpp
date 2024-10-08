
#pragma once
#include <iostream>
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
				MemberData			redirection;
				MemberData			request_types;

				ServerLocationData(std::string path);
				~ServerLocationData();

				static void	setRoot(void * ptr, int argc, std::string args[]);
				static void	setAllowedMethods(void * ptr, int argc, std::string args[]);
				static void	setRedirection(void * ptr, int argc, std::string args[]);
				static void	setRequest_types(void * ptr, int argc, std::string args[]);
		};
		class ServerData
		{
				static const std::string	className;
			public:
				MemberData	name;
				MemberData	listen;
				MemberData	root;
				MemberData	index;
				MemberData	keepAlive;
				MemberData	sendTimeout;
				MemberData	maxBodySize;
				MemberData	directoryListing;
				std::vector<ServerLocationData *> location;

				ServerData();
				~ServerData();

				static void	setName(void * ptr, int argc, std::string args[]);
				static void	setListen(void * ptr, int argc, std::string args[]);
				static void	setRoot(void * ptr, int argc, std::string args[]);
				static void	setIndex(void * ptr, int argc, std::string args[]);
				static void	setKeepAlive(void * ptr, int argc, std::string args[]);
				static void	setSendTimeout(void * ptr, int argc, std::string args[]);
				static void	setMaxBodySize(void * ptr, int argc, std::string args[]);
				static void	setDirectoryListing(void * ptr, int argc, std::string args[]);
				static void	*newLocation(void * ptr, int argc, std::string args[]);
		};
		class HttpData
		{
				static const std::string	className;
			public:
				std::vector<ServerData *> server;
				MemberData	server_timeout_time;

				HttpData();
				~HttpData();

				static void	*newServer(void * ptr, int argc, std::string args[]);
				static void	setServer_timeout_time(void * ptr, int argc, std::string args[]);
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

