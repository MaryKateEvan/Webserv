#ifndef EXCEPTIONS_HPP
# define EXCEPTIONS_HPP

# include "../webserv.h"

# ifdef __APPLE__
#  define NOEXCEPT_SPEC _NOEXCEPT
# else
#  define NOEXCEPT_SPEC noexcept
# endif

class BaseException : public std::exception
{
	protected:
		std::string			message;
	public:
		explicit			BaseException(const std::string& msg);
		virtual const char*	what() const NOEXCEPT_SPEC override;
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

class SetSocketNonBLockingModeException : public BaseException
{
	public:
		explicit SetSocketNonBLockingModeException(const std::string& server_name);
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

class FailedToBindSocketException : public BaseException
{
	public:
		explicit FailedToBindSocketException(const std::string& server_name);
};

class ListenFailedException : public BaseException
{
	public:
		explicit ListenFailedException(const std::string& server_name);
};

class OpenFailedException : public BaseException
{
	public:
		explicit OpenFailedException(const std::string& server_name, const std::string& file_path);
};

class NoMethodFoundException : public BaseException
{
	public:
		explicit NoMethodFoundException(const std::string& request);
};

class SendFailedException : public BaseException
{
	public:
		explicit SendFailedException(const std::string& server_name, const int& fd);
};

class PollFailedException : public BaseException
{
	public:
		explicit PollFailedException(void)
		: BaseException("Poll failed") {}
};

class InterruptSignal : public BaseException
{
	public:
		explicit InterruptSignal(void)
		: BaseException("Kill Signal received, shutting down!") {}
};

#endif /*EXCEPTIONS_HPP*/