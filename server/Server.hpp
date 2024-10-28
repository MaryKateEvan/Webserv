#ifndef SERVER_HPP
# define SERVER_HPP

# include "../webserv.h"
# include "Request.hpp"
// class Request;

class Server
{
	private:
		int					_fd_server;
		struct sockaddr_in	_address;
		const std::string	_name;
		const std::string	_index_file;
		const std::string	_data_dir;
		const std::string	_www_dir;
		bool				_directory_listing_enabled;
		size_t				_keepalive_timeout;
		size_t				_send_timeout;
		size_t				_max_body_size;
		std::string			_CLF_line;
		std::unordered_map<std::string, std::string> _mime_types;

		void				load_mime_types(const std::string& file_path);
		std::string			process_get(const Request& req);
		std::string			process_delete(const Request& req);
		std::string			process_post(const Request& req);
		std::string			extract_get_request(const std::string& request);
		std::string			map_to_directory(const std::string& file_path);
		std::string			get_mime_type(const std::string& file_path);
		std::string			read_file(const std::string& file_path);
	public:
		Server(const std::string server_name, int port, const std::string ip_address, const std::string index_file,
		const std::string data_dir, const std::string www_dir, bool directory_listing_enabled, size_t keepalive_timeout,
		size_t send_timeout, size_t max_body_size);
		~Server();

		int					getServerFD(void) const;
		struct sockaddr_in	getAddress(void) const;
		const std::string	getName(void) const;
		size_t				getMaxBodySize(void) const;
		std::string			process_request(const Request& req);
		std::string			send_error_message(int error_code);
		void				log_CLF_line(void);
};

#endif /*SERVER_HPP*/