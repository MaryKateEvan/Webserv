#include "Exceptions.hpp"

BaseException::BaseException(const std::string& msg) : message(msg)
{
}

const char* BaseException::what() const NOEXCEPT_SPEC
{
	return message.c_str();
}

SocketCreationFailedException::SocketCreationFailedException(const std::string& server_name)
	: BaseException("Socket Creation for server \"" + server_name + "\" failed") {}

SetSocketOptionFailedException::SetSocketOptionFailedException(const std::string& server_name)
	: BaseException("Set Socket option for server \"" + server_name + "\" failed") {}

InvalidPortException::InvalidPortException(const std::string& server_name, int port)
	: BaseException("The port " + std::to_string(port) + " for server \"" + server_name + "\" is invalid") {}

BindFailedException::BindFailedException(const std::string& server_name, int port)
	: BaseException("Binding server \"" + server_name + "\" to port " + std::to_string(port) + " failed") {}

ListenFailedException::ListenFailedException(const std::string& server_name)
	: BaseException("Setting server server \"" + server_name + "\" to listen failed") {}

OpenFailedException::OpenFailedException(const std::string& server_name, const std::string& file_path)
	: BaseException("Server server \"" + server_name + "\" couldn't open file " + file_path) {}
