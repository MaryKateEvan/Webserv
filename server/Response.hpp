#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "../webserv.h"

class Request;

class Response
{
	private:
		const std::string	_name;
		const std::string	_index_file;
		const std::string	_data_dir;
		const std::string	_www_dir;
		std::unordered_map<std::string, std::string> _mime_types;

		void				load_mime_types(const std::string& file_path);
		int					process_get(const Request& req);
		int					process_delete(const Request& req);
		int					process_post(const Request& req);
		std::string			extract_get_request(const std::string& request);
		std::string			map_to_directory(const std::string& file_path);
		std::string			get_mime_type(const std::string& file_path);
		std::string			read_file(const std::string& file_path);

	public:

		Response(const std::string server_name, const std::string index_file, const std::string data_dir, const std::string www_dir);
		~Response();

		int					process_request(const Request& req);
		int					send_error_message(int error_code, const Request& req);
};

#endif /*RESPONSE_HPP*/