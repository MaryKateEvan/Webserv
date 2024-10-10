
#include "ConfigData.hpp"

extern StringDataTracker	tracker;



ConfigData::MemberData::MemberData(const char * name) :
	name(name)
{
	isSet = false;
	setAtLine = -1;
	data = NULL;
}
ConfigData::MemberData::~MemberData()
{
	delete data;
}
void	ConfigData::MemberData::set(int argc, std::string args[], int line, std::string funcName, std::string className)
{
	if (isSet)
	{
		tracker.report_already_set(REPORT_WARNING | REPORT_LINE, setAtLine, className, name);
		return;
	}
	printFuncArgs(className + "." + funcName, argc, args);
	isSet = true;
	setAtLine = line;

	data = new StringArr(argc);
	for (int i = 0; i < argc; i++)
		data -> arr[i] = args[i];
}
void	ConfigData::MemberData::print(std::string tab)
{
	std::cout << tab << name << " { ";
	if (isSet)
	{
		for (size_t i = 0; i < data -> num; i++)
			std::cout << data -> arr[i] << " ";
	}
	else
	{
		std::cout << "\e[31mNOT SET\e[m ";
	}
	std::cout << "};\n";
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
	allowed_methods("allowed_methods"),
	redirection("redirection"),
	request_types("request_types")
{

}
ConfigData::ServerLocationData::~ServerLocationData()
{

}
void	ConfigData::ServerLocationData::set_root(void * ptr, int line, int argc, std::string args[])
{
	ServerLocationData * p = (ServerLocationData *)ptr;
	p -> root.set(argc, args, line, __FUNCTION__, className);
}
void	ConfigData::ServerLocationData::set_allowed_methods(void * ptr, int line, int argc, std::string args[])
{
	ServerLocationData * p = (ServerLocationData *)ptr;
	p -> allowed_methods.set(argc, args, line, __FUNCTION__, className);
}
void	ConfigData::ServerLocationData::set_redirection(void * ptr, int line, int argc, std::string args[])
{
	ServerLocationData * p = (ServerLocationData *)ptr;
	p -> redirection.set(argc, args, line, __FUNCTION__, className);
}
void	ConfigData::ServerLocationData::set_request_types(void * ptr, int line, int argc, std::string args[])
{
	ServerLocationData * p = (ServerLocationData *)ptr;
	p -> request_types.set(argc, args, line, __FUNCTION__, className);
}
void	ConfigData::ServerLocationData::print()
{
	std::string	tab = "    ";

	root.print(tab);
	allowed_methods.print(tab);
	redirection.print(tab);
	request_types.print(tab);
}





const std::string ConfigData::ServerData::className = "    ServerData";
ConfigData::ServerData::ServerData() :
	server_name("server_name"),
	listen("listen"),
	root("root"),
	index("index"),
	keepalive_timeout("keepalive_timeout"),
	send_timeout("send_timeout"),
	max_body_size("max_body_size"),
	directory_listing("directory_listing")
{

}
ConfigData::ServerData::~ServerData()
{
	for (size_t i = 0; i < location.size(); i++)
		delete location[i];
}
void	ConfigData::ServerData::set_server_name(void *ptr, int line, int argc, std::string args[])
{
	ServerData * p = (ServerData *)ptr;
	p -> server_name.set(argc, args, line, __FUNCTION__, className);
}
void	ConfigData::ServerData::set_listen(void *ptr, int line, int argc, std::string args[])
{
	ServerData * p = (ServerData *)ptr;
	p -> listen.set(argc, args, line, __FUNCTION__, className);
}
void	ConfigData::ServerData::set_root(void *ptr, int line, int argc, std::string args[])
{
	ServerData * p = (ServerData *)ptr;
	p -> root.set(argc, args, line, __FUNCTION__, className);
}
void	ConfigData::ServerData::set_index(void *ptr, int line, int argc, std::string args[])
{
	ServerData * p = (ServerData *)ptr;
	p -> index.set(argc, args, line, __FUNCTION__, className);
}
void	ConfigData::ServerData::set_keepalive_timeout(void *ptr, int line, int argc, std::string args[])
{
	ServerData * p = (ServerData *)ptr;
	p -> keepalive_timeout.set(argc, args, line, __FUNCTION__, className);
}
void	ConfigData::ServerData::set_send_timeout(void *ptr, int line, int argc, std::string args[])
{
	ServerData * p = (ServerData *)ptr;
	p -> send_timeout.set(argc, args, line, __FUNCTION__, className);
}
void	ConfigData::ServerData::set_max_body_size(void *ptr, int line, int argc, std::string args[])
{
	ServerData * p = (ServerData *)ptr;
	p -> max_body_size.set(argc, args, line, __FUNCTION__, className);
}
void	ConfigData::ServerData::set_directory_listing(void *ptr, int line, int argc, std::string args[])
{
	ServerData * p = (ServerData *)ptr;
	p -> directory_listing.set(argc, args, line, __FUNCTION__, className);
}
void	*ConfigData::ServerData::newLocation(void *ptr, int line, int argc, std::string args[])
{
	ServerData * p = (ServerData *)ptr;
	printFuncArgs(className + "." + __FUNCTION__, argc, args);
	(void)line;

	ServerLocationData * location = new ServerLocationData(args[0]);
	p -> location.push_back(location);
	return location;
}
void	ConfigData::ServerData::print()
{
	std::string	tab = "  ";

	server_name.print(tab);
	listen.print(tab);
	root.print(tab);
	index.print(tab);
	keepalive_timeout.print(tab);
	send_timeout.print(tab);
	max_body_size.print(tab);
	directory_listing.print(tab);

	for (size_t i = 0; i < location.size(); i++)
	{
		std::cout << "\n    location [" << i << "]\n";
		location[i] -> print();
	}
}





const std::string ConfigData::HttpData::className = "  HttpData";
ConfigData::HttpData::HttpData() :
	server_timeout_time("server_timeout_time")
{

}
ConfigData::HttpData::~HttpData()
{
	for (size_t i = 0; i < server.size(); i++)
		delete server[i];
}
void	*ConfigData::HttpData::newServer(void *ptr, int line, int argc, std::string args[])
{
	HttpData * p = (HttpData *)ptr;
	printFuncArgs(className + "." + __FUNCTION__, argc, args);
	(void)line;

	ServerData * server = new ServerData();
	p -> server.push_back(server);
	return server;
}
void	ConfigData::HttpData::set_server_timeout_time(void *ptr, int line, int argc, std::string args[])
{
	HttpData * p = (HttpData *)ptr;
	p -> server_timeout_time.set(argc, args, line, __FUNCTION__, className);
}
void	ConfigData::HttpData::print()
{
	std::string	tab = "";

	server_timeout_time.print(tab);

	for (size_t i = 0; i < server.size(); i++)
	{
		std::cout << "\n  server [" << i << "]\n";
		server[i] -> print();
	}
}





const std::string ConfigData::MainData::className = "MainData";
ConfigData::MainData::MainData()
{

}
ConfigData::MainData::~MainData()
{
	for (size_t i = 0; i < http.size(); i++)
		delete http[i];
}
void	*ConfigData::MainData::newHttp(void *ptr, int line, int argc, std::string args[])
{
	MainData * p = (MainData *)ptr;
	printFuncArgs(className + "." + __FUNCTION__, argc, args);
	(void)line;

	HttpData * http = new HttpData();
	p -> http.push_back(http);
	return http;
}
void	ConfigData::MainData::print()
{
	std::string	tab = "";

	for (size_t i = 0; i < http.size(); i++)
	{
		std::cout << "\nhttp [" << i << "]\n";
		http[i] -> print();
	}
}

