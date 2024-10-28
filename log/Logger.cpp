#include "Logger.hpp"

/* -------------------------------------------------------------------------- */
/*                           Orthodox Canonical Form                          */
/* -------------------------------------------------------------------------- */

Logger::Logger()
{
	const std::filesystem::path&	folder1 = "log/server";
	const std::filesystem::path&	folder2 = "log/error";
	if (!std::filesystem::exists(folder1))
		std::filesystem::create_directories(folder1);
	if (!std::filesystem::exists(folder2))
		std::filesystem::create_directories(folder2);
	std::string	time = get_time();
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
	delete_oldest_file("log/error", 5);
	_server_log.open(("log/server/" + time + "server.log"));
	// _server_log.open("log/server/test");
	if (!_server_log.is_open())
	{
		throw std::ios_base::failure("Failed to open server log file");
	}
	_error_log.open("log/error/" + time + "server.error.log");
	if (!_error_log.is_open())
	{
		throw std::ios_base::failure("Failed to open error log file");
	}
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
				auto	ftime = std::filesystem::last_write_time(entry);
				auto	cftime = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
					ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());
				files.emplace_back(entry.path(), cftime);
			}
		}
	}
	if (files.size() >= max_files)
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
 * @param level 0 for DEBUG, 1 for CLF, 2 for INFO to error log, 3 for WARNING, 4 for ERROR, ERROR only for FATAL error meaning server failure
 */
void	Logger::log(const std::string& server_name, const std::string& message, int level)
{
	std::string	time = get_time();
	std::string	msg = message;
	if (!server_name.empty())
	{
		msg = server_name + ":\t" + message;
	}
	switch (level)
	{
		case 1:
			_server_log << msg << std::endl;
			break;
		case 0:
			_error_log << time << " [DEBUG]\t" << msg << std::endl;
			break;
		case 2:
			_error_log << time << " [INFO]\t" << msg << std::endl;
			break;
		case 3:
			_error_log << time << " [WARNING]\t" << msg << std::endl;
			break;
		case 4:
			_error_log << time << " [ERROR]\t" << msg << std::endl;
			_server_log << time << " [ERROR]\t" << msg << std::endl;
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
	std::string	format = "[%d-%m-%Y:%H:%M:%S]";
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
