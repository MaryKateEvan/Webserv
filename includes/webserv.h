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

# include "./Server.hpp"
# include "./Request.hpp"
# include "./Exceptions.hpp"
# include "./utils.h"

//Headers form Dustin:
# include "./parsing_config/ConfigData.hpp"
# include "./parsing_config/ConfigParse.hpp"
# include "./parsing_config/parse_config_file.hpp"
# include "./stringing_from_config/Pair.hpp"
# include "./stringing_from_config/StringArr.hpp"
# include "./stringing_from_config/StringDataTracker.hpp"
# include "./stringing_from_config/StringHelp.hpp"
# include "./stringing_from_config/Twin.hpp"

# ifndef PORT
#  define PORT 8080
# endif 

# ifndef MAX_CLIENTS
#  define MAX_CLIENTS 8080
# endif 

#endif /*WEBSERV_H*/