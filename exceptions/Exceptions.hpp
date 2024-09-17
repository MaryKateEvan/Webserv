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

class InvalidIPAddressException : public BaseException
{
	public:
		explicit InvalidIPAddressException(const std::string& ip_address);
};

class SocketCreationFailedException : public BaseException
{
	public:
		explicit SocketCreationFailedException(const std::string& server_name);
};

class SetSocketOptionFailedException : public BaseException
{
	public:
		explicit SetSocketOptionFailedException(const std::string& server_name);
};

class InvalidPortException : public BaseException
{
	public:
		explicit InvalidPortException(const std::string& server_name, int port);
};

class InvalidIPAdressException : public BaseException
{
	public:
		explicit InvalidIPAdressException(const std::string& server_name, const std::string& ip_address);
};

class BindFailedException : public BaseException
{
	public:
		explicit BindFailedException(const std::string& server_name, const std::string& ip_address);
};

class ListenFailedException : public BaseException
{
	public:
		explicit ListenFailedException(const std::string& server_name);
};

#endif /*EXCEPTIONS_HPP*/