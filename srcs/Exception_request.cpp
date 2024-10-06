#include "../includes/Exceptions.hpp"

NoMethodFoundException::NoMethodFoundException(const std::string& request)
	: BaseException("No Method (GET/POST/DELETE) could be found in the request: " + request) {}