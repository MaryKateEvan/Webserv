#ifndef POLLING_HPP
# define POLLING_HPP

# include "../webserv.h"
# include <map>

class Server;

struct ClientData
{
	int fd_client;
	int port_of_server;
	time_t last_seen;
};

class Polling
{
	private:
		std::vector<Server> 		_servers;
		std::vector<int>			_server_fds; //to keep track of the active server sockets
		std::vector<int>			_client_fds; //to keep track of the active client sockets
		std::vector<struct pollfd>	_poll_fds;
		std::vector<int>			_used_ports; //? maybe parser handles that?
		std::map<int, Server> 		_servers_map; //it indexes according to the client's fd
		std::map<int, ClientData> 	_clients_map; //it indexes according to the client's fd

		Polling(); //only instantiated with the vector of ServerData passed
		// void server_poll_data();
		void boot_servers();
		void loop_for_connections();
		void accept_new_client_connection(int server_fd);
		void read_client_request(pollfd & client_poll_fd);

	public:
		Polling(std::vector<Server> & servers);
		~Polling();

		// void initServerSockets();
		// void bind_socket_and_listen_to_port(std::vector<ServerData>::iterator server);

		// void close_server_sockets();//not used yet
	
};

void signal_handler(int signum);



#endif //POLLING_HPP