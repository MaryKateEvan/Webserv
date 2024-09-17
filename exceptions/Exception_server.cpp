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
