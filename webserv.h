#ifndef WEBSERV_H
# define WEBSERV_H

# include <iostream>
# include <string>
# include <vector>
# include <deque>
# include <algorithm>
# include <limits>
# include <sys/time.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <unistd.h>
# include <climits>
# include <sstream>
# include <cstring>
# include <poll.h>
# include <csignal>
# include <arpa/inet.h>
# include <exception>
# include <fstream>
# include <unordered_map>
# include <cstdio>
# include <filesystem>
# include <fcntl.h>

# include "exceptions/Exceptions.hpp"
# include "utils/utils.h"

# ifndef PORT
#  define PORT 8080
# endif 

# ifndef MAX_CLIENTS
#  define MAX_CLIENTS 8080
# endif 

#endif /*WEBSERV_H*/