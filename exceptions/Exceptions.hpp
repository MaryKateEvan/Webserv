#ifndef EXCEPTIONS_HPP
# define EXCEPTIONS_HPP

# include "../webserv.h"

class BaseException : public std::exception
{
	protected:
		std::string			message;
	public:
		explicit			BaseException(const std::string& msg);
		virtual const char*	what() const noexcept override;
};

class: InvalidIPAddressException : public BaseException
{
	public:
		explicit IvalidIPAddressException(const std::string& ip_address);
};

#endif /*EXCEPTIONS_HPP*/