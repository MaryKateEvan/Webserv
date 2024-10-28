# --------------------------------- MAKEFILE --------------------------------- #

# ---------------------------------------------------------------------------- #
#                                    Config                                    #
# ---------------------------------------------------------------------------- #

NAME			:= webserv
CPP				:= c++
CPPFLAGS		:= -Wall -Wextra -Werror -std=c++17

# ---------------------------------------------------------------------------- #
#                                     Files                                    #
# ---------------------------------------------------------------------------- #

OBJ_DIR			:= ./objs

VPATH			:= ./utils/ ./server/ ./exceptions/ ./parser/

SRC				:= main.cpp

UTILS_SRC		:= utils1.cpp

SERVER_SRC		:= Server.cpp Request.cpp SocketManager.cpp

EXCEPTIONS_SRC	:= Exception_server.cpp Exception_request.cpp

PARSER_SRC		:= stringing/StringHelp.cpp stringing/StringArr.cpp stringing/Pair.cpp \
					stringing/Twin.cpp stringing/StringDataTracker.cpp parsing/ConfigParse.cpp \
					parsing/ConfigData.cpp parsing/parse_config_file.cpp

LOG_SRC			:= log/Logger.cpp

SRCS			:= $(SRC) $(UTILS_SRC) $(SERVER_SRC) $(EXCEPTIONS_SRC) $(PARSER_SRC) $(LOG_SRC)

OBJS			:= $(addprefix $(OBJ_DIR)/, $(SRCS:%.cpp=%.o))

# ---------------------------------------------------------------------------- #
#                                     Rules                                    #
# ---------------------------------------------------------------------------- #

all: submodule $(NAME)

submodule:
	@git submodule update --init --recursive

$(NAME): $(OBJS)
	$(CPP) $(CPPFLAGS) $(OBJS) -o $(NAME)
	@echo $(GREEN)"Linking $(NAME)!"$(DEFAULT);

$(OBJ_DIR)/%.o: %.cpp
	mkdir -p $(dir $@)
	$(CPP) $(CPPFLAGS) -c $< -o $@

debug: CPPFLAGS += -g
debug: re

nflag: CPPFLAGS = 
nflag: all

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re

DEFAULT	= "\033[39m"
GREEN	= "\033[32m"