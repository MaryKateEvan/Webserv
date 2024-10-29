#include "MimeTypes.hpp"

/* -------------------------------------------------------------------------- */
/*                           Orthodox Canonical Form                          */
/* -------------------------------------------------------------------------- */

MimeTypes::MimeTypes()
{
	std::ifstream file("mime_type.csv");
	if (!file.is_open())
		throw OpenFailedException("MimeTypes", "mime_type.csv");
	std::string	line;
	while(std::getline(file, line))
	{
		std::istringstream	ss(line);
		std::string			extension;
		std::string			mime_type;

		if (std::getline(ss, extension, '\t') && std::getline(ss, mime_type))
			_mime_types[extension] = mime_type;
	}
	file.close();
}

MimeTypes::~MimeTypes()
{
}

/* -------------------------------------------------------------------------- */
/*                                  Functions                                 */
/* -------------------------------------------------------------------------- */

std::string	MimeTypes::get_mime_type(const std::string& file_path)
{
	std::string	file_extension = file_path.substr(file_path.find_last_of('.'));
	try
	{
		return (_mime_types.at(file_extension));
	}
	catch (const std::out_of_range& e)
	{
		Logger::getInstance().log("", file_path + " has unknown MIME Type", 3);
		return ("unknown/unknown");
	}
}

std::string MimeTypes::get_file_extension(const std::string& mime_type)
{
	for (const auto& pair : _mime_types)
	{
		if (pair.second == mime_type)
			return (pair.first);
	}
	Logger::getInstance().log("", mime_type + " has unknown file extension", 3);
	return (".unknown");
}
