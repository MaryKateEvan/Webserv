#ifndef UTILS_H
# define UTILS_H

# include "../webserv.h"

std::vector<std::string>	tokenize(const std::string& str, const std::string& delim);

// to be DELETED later:
struct ServerData;

void fill_server_data(std::vector<ServerData>& servers);
void print_server_data(const std::vector<ServerData>& servers);

#endif