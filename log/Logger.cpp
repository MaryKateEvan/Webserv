#include "Logger.hpp"

/* -------------------------------------------------------------------------- */
/*                           Orthodox Canonical Form                          */
/* -------------------------------------------------------------------------- */

Logger::Logger()
{
	std::string	time = get_time();
	const std::filesystem::path&	folder1 = "/log/server";
	if (std::filesystem::exists(folder1) && std::filesystem::is_directory(folder1))
	{
		for (const auto& entry : std::filesystem::directory_iterator(folder1))
		{
			if (std::filesystem::is_regular_file(entry.status()) && entry.path().extension() == ".log")
			{
				std::filesystem::remove(entry.path());
			}
		}
	}
	delete_oldest_file("/log/error", 5);
	_server_log.open("/log/server/" + time + "server.log");
	_error_log.open("/log/error/" + time + "server.error.log");
}

Logger::~Logger()
{
	if (_server_log.is_open())
		_server_log.close();
	if (_error_log.is_open())
		_error_log.close();
}

/* -------------------------------------------------------------------------- */
/*                                  Functions                                 */
/* -------------------------------------------------------------------------- */

void	Logger::delete_oldest_file(const std::filesystem::path& folder, size_t max_files)
{
	std::vector<std::pair<std::filesystem::path, std::chrono::system_clock::time_point>>	files;

	if (std::filesystem::exists(folder) && std::filesystem::is_directory(folder))
	{
		for (const auto& entry : std::filesystem::directory_iterator(folder))
		{
			if (std::filesystem::is_regular_file(entry.status()) && entry.path().extension() == ".log")
			{
				files.emplace_back(entry.path(), std::filesystem::last_write_time(entry));
			}
		}
	}
	if (files.size() > max_files)
	{
		std::sort(files.begin(), files.end(), [](const auto& a, const auto& b) {
			return (a.second < b.second);
		});
		std::filesystem::remove(files.front().first);
	}
}

// DEBUG, INFO, WARNING, ERROR

/**
 * @brief Adds a log to the logfile
 * @param server_name the name of the server, empty if the error is general
 * @param message the message, in the standart format, has to be in the Common Log Format for the server log
 * @param level 0 for DEBUG, 1 for INFO, 2 for WARNING, 3 for ERROR, ERROR only for FATAL error meaning server failure
 */
void	Logger::log(const std::string& server_name, const std::string& message, int level)
{
	std::string	time = get_time();
	switch (level)
	{
		case 1:
			_server_log << message << std::endl;
			break;
		case 0:
			_error_log << time << " [DEBUG]\t" << message << std::endl;
			break;
		case 2:
			_error_log << time << " [WARNING\t]" << message << std::endl;
			break;
		case 3:
			_error_log << time << " [ERROR\t]" << message << std::endl;
			_server_log << time << " [ERROR\t]" << message << std::endl;
			break;
		default:
			break;
	}
}

std::string	Logger::get_time()
{
	std::string	time;
	std::time_t	now = std::time(NULL);
	std::tm local_tm = *std::localtime(&now);
	char	buffer[100];
	std::string	format = "%d/%b/%Y:%H:%M:%S";
	if (std::strftime(buffer, sizeof(buffer), format.c_str(), &local_tm))
	{
		time = std::string(buffer);
	}
	else
	{
		time = "Error-Formating-Time";
	}
	return (time);
}
