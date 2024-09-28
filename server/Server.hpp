#ifndef SERVER_HPP
# define SERVER_HPP

# include "../webserv.h"

class Request;

class Server
{
	private:
		int					_fd_server;
		struct sockaddr_in	_address;
		const std::string	_name;
		std::string			_response;
		const std::string	_index_file;
		const std::string	_data_dir;
		const std::string	_www_dir;
		std::unordered_map<std::string, std::string> _mime_types;

		void				load_mime_types(const std::string& file_path);
		int					process_get(const Request& req);
		int					process_delete(const Request& req);
		std::string			extract_get_request(const std::string& request);
		std::string			map_to_directory(const std::string& file_path);
		bool				file_exists(const std::string& file_path);
		std::string			get_mime_type(const std::string& file_path);
		std::string			read_file(const std::string& file_path);
	public:
		Server(const std::string server_name, int port, const std::string ip_address, const std::string index_file, const std::string data_dir, const std::string www_dir);
		//use inet_pton() to convert IP address
		~Server();
		Server(const Server& copy);
		Server& operator=(const Server& copy);

		int					getServerFD(void) const;
		struct sockaddr_in	getAddress(void) const;
		const std::string	getName(void) const;
		int					acceptConnection(void);
		std::string			getResponse(void) const;
		void				setResponse(std::string response);
		int					process_request(const Request& req);
		int					send_error_message(int error_code, const Request& req);
};

#endif /*SERVER_HPP*/