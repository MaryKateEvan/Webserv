#ifndef MIMETYPES_HPP
# define MIMETYPES_HPP

# include "../webserv.h"

class MimeTypes
{
	private:
		/*args*/
		MimeTypes();
		~MimeTypes();
		std::unordered_map<std::string, std::string> _mime_types;
	public:
		static MimeTypes&	getInstance()
		{
			static MimeTypes	instance;
			return (instance);
		}

		MimeTypes(const MimeTypes& copy) = delete;
		void operator=(const MimeTypes& copy) = delete;

		std::string			get_mime_type(const std::string& file_path);
};

#endif /*MIMETYPES_HPP*/