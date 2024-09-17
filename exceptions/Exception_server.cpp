#include "Exceptions.hpp"

BaseException::BaseException(const std::string& msg) : message(msg)
{
}

const char* BaseException::what() const noexcept
{
	return message.c_str();
}

InvalidIPAddressException::InvalidIPAddressException(const std::string& ip_address)
	: BaseException("IP-Address: " + ip_address + " is invalid") {}

SocketCreationFailedException::SocketCreationFailedException(const std::string& server_name)
	: BaseException("Socket Creation for " + server_name + " failed") {}