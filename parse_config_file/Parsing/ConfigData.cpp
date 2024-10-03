
#include "ConfigData.hpp"

extern StringDataTracker	tracker;

void	ConfigData::printFuncArgs(std::string func, int argc, std::string args[])
{
	std::cout << "\e[38;2;63;63;63m" << func << "(";
	for (int i = 0; i < argc; i++)
		std::cout << " " << args[i];
	std::cout << " );\e[m\n";
}





const std::string ConfigData::ServerLocationData::className = "      ServerLocationData";
ConfigData::ServerLocationData::ServerLocationData(std::string p) :
	path(p)
{
	root = "\e[31mNOT SET\e[m";
	allowed_methods = new std::string[1];
	allowed_methods[0] = "\e[31mNOT SET\e[m";
	allowed_methods_num = 1;

	root_is_set = false;
	allowed_methods_is_set = false;
}
ConfigData::ServerLocationData::~ServerLocationData()
{
	delete [] allowed_methods;
	allowed_methods_num = 0;
}
void	ConfigData::ServerLocationData::setRoot(void * ptr, int argc, std::string args[])
{
	ServerLocationData * p = (ServerLocationData *)ptr;
	if (p -> root_is_set)
	{
		tracker.report_generic(REPORT_WARNING | REPORT_LINE, className + "." + "Root" + " is already set");
		return;
	}
	printFuncArgs(className + "." + __FUNCTION__, argc, args);

	p -> root = args[0];
	p -> root_is_set = true;
}
void	ConfigData::ServerLocationData::setAllowedMethods(void * ptr, int argc, std::string args[])
{
	ServerLocationData * p = (ServerLocationData *)ptr;
	if (p -> allowed_methods_is_set)
	{
		tracker.report_generic(REPORT_WARNING | REPORT_LINE, className + "." + "AllowedMethods" + " is already set");
		return;
	}
	printFuncArgs(className + "." + __FUNCTION__, argc, args);

	delete [] p -> allowed_methods;
	p -> allowed_methods = new std::string[argc];
	p -> allowed_methods_num = argc;
	for (int i = 0; i < argc; i++)
	{
		p -> allowed_methods[i] = args[i];
	}
	p -> allowed_methods_is_set = true;
}





const std::string ConfigData::ServerData::className = "    ServerData";
ConfigData::ServerData::ServerData()
{
	name = "\e[31mNOT SET\e[m";
	listen = "\e[31mNOT SET\e[m";
	root = "\e[31mNOT SET\e[m";
	index = "\e[31mNOT SET\e[m";

	name_is_set = false;
	listen_is_set = false;
	root_is_set = false;
	index_is_set = false;
}
ConfigData::ServerData::~ServerData()
{
	for (int i = 0; i < location.size(); i++)
		delete location[i];
}
void	ConfigData::ServerData::setName(void *ptr, int argc, std::string args[])
{
	ServerData * p = (ServerData *)ptr;
	if (p -> name_is_set)
	{
		tracker.report_generic(REPORT_WARNING | REPORT_LINE, className + "." + "Name" + " is already set");
		return;
	}
	printFuncArgs(className + "." + __FUNCTION__, argc, args);

	p -> name = args[0];
	p -> name_is_set = true;
}
void	ConfigData::ServerData::setListen(void *ptr, int argc, std::string args[])
{
	ServerData * p = (ServerData *)ptr;
	if (p -> listen_is_set)
	{
		tracker.report_generic(REPORT_WARNING | REPORT_LINE, className + "." + "Listen" + " is already set");
		return;
	}
	printFuncArgs(className + "." + __FUNCTION__, argc, args);

	p -> listen = args[0];
	p -> listen_is_set = true;
}
void	ConfigData::ServerData::setRoot(void *ptr, int argc, std::string args[])
{
	ServerData * p = (ServerData *)ptr;
	if (p -> root_is_set)
	{
		tracker.report_generic(REPORT_WARNING | REPORT_LINE, className + "." + "Root" + " is already set");
		return;
	}
	printFuncArgs(className + "." + __FUNCTION__, argc, args);

	p -> root = args[0];
	p -> root_is_set = true;
}
void	ConfigData::ServerData::setIndex(void *ptr, int argc, std::string args[])
{
	ServerData * p = (ServerData *)ptr;
	if (p -> index_is_set)
	{
		tracker.report_generic(REPORT_WARNING | REPORT_LINE, className + "." + "Index" + " is already set");
		return;
	}
	printFuncArgs(className + "." + __FUNCTION__, argc, args);

	p -> index = args[0];
	p -> index_is_set = true;
}
void	*ConfigData::ServerData::newLocation(void *ptr, int argc, std::string args[])
{
	ServerData * p = (ServerData *)ptr;
	printFuncArgs(className + "." + __FUNCTION__, argc, args);

	ServerLocationData * location = new ServerLocationData(args[0]);
	p -> location.push_back(location);
	return location;
}





const std::string ConfigData::HttpData::className = "  HttpData";
ConfigData::HttpData::HttpData()
{

}
ConfigData::HttpData::~HttpData()
{
	for (int i = 0; i < server.size(); i++)
		delete server[i];
}
void	*ConfigData::HttpData::newServer(void *ptr, int argc, std::string args[])
{
	HttpData * p = (HttpData *)ptr;
	printFuncArgs(className + "." + __FUNCTION__, argc, args);

	ServerData * server = new ServerData();
	p -> server.push_back(server);
	return server;
}





const std::string ConfigData::MainData::className = "MainData";
ConfigData::MainData::MainData()
{

}
ConfigData::MainData::~MainData()
{
	for (int i = 0; i < http.size(); i++)
		delete http[i];
}
void	*ConfigData::MainData::newHttp(void *ptr, int argc, std::string args[])
{
	MainData * p = (MainData *)ptr;
	printFuncArgs(className + "." + __FUNCTION__, argc, args);

	HttpData * http = new HttpData();
	p -> http.push_back(http);
	return http;
}
