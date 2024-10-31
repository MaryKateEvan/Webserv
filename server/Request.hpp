#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "../webserv.h"

# ifndef GET
#  define GET 0
# endif /*GET*/

# ifndef POST
#  define POST 1
# endif /*POST*/

# ifndef DELETE
#  define DELETE 2
# endif /*DELETE*/

/// @brief Takes the request from a client and parses it.
class Request
{
	private:
		size_t				_fd;

		int					_method; // Assign one of the three defines above
		std::string			_file_path;
		size_t				_content_len;
		std::string			_host;
		int					_port;

		std::string			_content_type;

		std::string			_header;
		std::vector<char>	_accumulated_request;
		size_t				_body_bytes_read;
		bool				_header_parsed;
		bool				_finished_reading;

		std::string			_client_ip;
		std::string			_request_received_time;
		std::string			_request_line;
		std::string			_CLF_line;

		int			fill_in_request(void);

		int			process_post(void);
		void		fill_string_to_map(std::string input);
	public:
		std::unordered_map<std::string, std::string> _post_files;
		Request(size_t fd);
		Request();
		~Request();
		Request(const Request& copy);
		Request& operator=(const Request& copy);

		int			read_chunk(std::vector<char> buffer, int bytes_read);
		bool		correct_body_size(size_t max_size);

		int			get_method(void) const;
		std::string	get_method_in_string(void) const;
		int			get_fd(void) const;
		std::string	get_file_path(void) const;
		std::string	get_content_type(void) const;
		size_t		get_content_len(void) const;
		bool		get_finished_reading(void) const;
		int			get_port(void) const;
		std::string	get_host(void) const;
		std::string	get_client_ip(void) const;
		std::string	get_request_received_time(void) const;
		std::string	get_request_line(void) const;
		std::string	get_CLF_line(void) const;

		void		set_fd(size_t fd);
		void		set_client_ip(uint32_t ip_binary);

		void		reset();
};

#endif /*REQUEST_HPP*/