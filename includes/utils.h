#ifndef UTILS_H
# define UTILS_H

# include "../includes/webserv.h"
// # include <vector>
// # include <string>

struct ServerData;

std::vector<std::string>	tokenize(const std::string& str, const std::string& delim);

// to be DELETED later:
void fill_server_data(std::vector<ServerData>& servers);
void print_server_data(const std::vector<ServerData>& servers);

#endif