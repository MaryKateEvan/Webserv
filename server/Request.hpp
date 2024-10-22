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



		int			process_post(const std::string& request);
		void		fill_string_to_map(std::string input);
	public:
		std::unordered_map<std::string, std::string> _post_files;
		Request(size_t fd);
		~Request();
		Request(const Request& copy);
		Request& operator=(const Request& copy);

		int			read_chunk(std::vector<char> buffer, int bytes_read);

		int			get_method(void) const;
		int			get_fd(void) const;
		std::string	get_file_path(void) const;
		std::string	get_content_type(void) const;
		bool		get_finished_reading(void) const;
		int			get_port(void) const;
		std::string	get_host(void) const;
};

#endif /*REQUEST_HPP*/