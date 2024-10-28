#ifndef LOGGER_HPP
# define LOGGER_HPP

# include "../webserv.h"

class Logger
{
	private:
		Logger();
		~Logger();
		std::ofstream _server_log;
		std::ofstream _error_log;
		
		void		delete_oldest_file(const std::filesystem::path& folder, size_t max_files);
		std::string	get_time();
	public:
		static Logger&	getInstance()
		{
			static Logger	instance;
			return (instance);
		}

		Logger(const Logger& copy) = delete;
		void	operator=(const Logger& copy) = delete;

		void	log(const std::string& server_name, const std::string& message, int level);
};

#endif /*LOGGER_HPP*/