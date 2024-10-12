
#include "../../includes/parsing_config/ConfigData.hpp"

extern StringDataTracker	tracker;



ConfigData::MemberData::MemberData(std::string name, int num) :
	name(name),
	data(num)
{
	this -> isSet = false;
	for (int i = 0; i < num; i++)
		this -> data[i] = "\e[31mNOT SET\e[m";
	data_num = 0;
}
void	ConfigData::MemberData::set(int argc, std::string args[], std::string funcName, std::string className)
{
	if (isSet)
	{
		tracker.report_generic(REPORT_WARNING | REPORT_LINE, className + "." + name + " is already set");
		return;
	}
	printFuncArgs(className + "." + funcName, argc, args);
	for (int i = 0; i < argc; i++)
		data.arr[i] = args[i];
	data_num = argc;
}



void	ConfigData::printFuncArgs(std::string func, int argc, std::string args[])
{
	std::cout << "\e[38;2;63;63;63m" << func << "(";
	for (int i = 0; i < argc; i++)
		std::cout << " " << args[i];
	std::cout << " );\e[m\n";
}





const std::string ConfigData::ServerLocationData::className = "      ServerLocationData";
ConfigData::ServerLocationData::ServerLocationData(std::string p) :
	path(p),
	root("root"),
	allowed_methods("allowed_methods", 3)
{

}
ConfigData::ServerLocationData::~ServerLocationData()
{

}
void	ConfigData::ServerLocationData::setRoot(void * ptr, int argc, std::string args[])
{
	ServerLocationData * p = (ServerLocationData *)ptr;
	p -> root.set(argc, args, __FUNCTION__, className);
}
void	ConfigData::ServerLocationData::setAllowedMethods(void * ptr, int argc, std::string args[])
{
	ServerLocationData * p = (ServerLocationData *)ptr;
	p -> allowed_methods.set(argc, args, __FUNCTION__, className);
}





const std::string ConfigData::ServerData::className = "    ServerData";
ConfigData::ServerData::ServerData() :
	name("name"),
	listen("listen"),
	root("root"),
	index("index")
{

}
ConfigData::ServerData::~ServerData()
{
	for (int i = 0; i < location.size(); i++)
		delete location[i];
}
void	ConfigData::ServerData::setName(void *ptr, int argc, std::string args[])
{
	ServerData * p = (ServerData *)ptr;
	p -> name.set(argc, args, __FUNCTION__, className);
}
void	ConfigData::ServerData::setListen(void *ptr, int argc, std::string args[])
{
	ServerData * p = (ServerData *)ptr;
	p -> listen.set(argc, args, __FUNCTION__, className);
}
void	ConfigData::ServerData::setRoot(void *ptr, int argc, std::string args[])
{
	ServerData * p = (ServerData *)ptr;
	p -> root.set(argc, args, __FUNCTION__, className);
}
void	ConfigData::ServerData::setIndex(void *ptr, int argc, std::string args[])
{
	ServerData * p = (ServerData *)ptr;
	p -> index.set(argc, args, __FUNCTION__, className);
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
