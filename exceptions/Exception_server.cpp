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

SetSocketOptionFailedException::SetSocketOptionFailedException(const std::string& server_name)
	: BaseException("Set Socker option for " + server_name + " failed") {}

InvalidPortException::InvalidPortException(const std::string& server_name, int port)
	: BaseException("The port " + std::to_string(port) + " for server " + server_name + " is invalid") {}

InvalidIPAdressException::InvalidIPAdressException(const std::string& server_name, const std::string& ip_address)
	: BaseException("The IP-Address " + ip_address + " for server " + server_name + " is invalid") {}

BindFailedException::BindFailedException(const std::string& server_name, const std::string& ip_address)
	: BaseException("Bindin IP " + ip_address + " to socket for server " + server_name + " failed") {}

ListenFailedException::ListenFailedException(const std::string& server_name)
	: BaseException("Setting server " + server_name " to listen failed") {}