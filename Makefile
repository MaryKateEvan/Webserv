# --------------------------------- MAKEFILE --------------------------------- #

# ---------------------------------------------------------------------------- #
#                                    Config                                    #
# ---------------------------------------------------------------------------- #

NAME			:= webserv
CPP				:= c++
CPPFLAGS		:= -Wall -Wextra -Werror

# ---------------------------------------------------------------------------- #
#                                     Files                                    #
# ---------------------------------------------------------------------------- #

OBJ_DIR			:= ./objs

VPATH			:= ./utils/ ./server/ ./exceptions/

SRC				:= main.cpp

UTILS_SRC		:=

SERVER_SRC		:= Server.cpp Request.cpp

EXCEPTIONS_SRC	:= Exception_server.cpp

SRCS			:= $(SRC) $(UTILS_SRC) $(SERVER_SRC) $(EXCEPTIONS_SRC)

OBJS			:= $(addprefix $(OBJ_DIR)/, $(SRCS:%.cpp=%.o))

# ---------------------------------------------------------------------------- #
#                                     Rules                                    #
# ---------------------------------------------------------------------------- #

all: $(NAME)

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