# ---------------------------------------------------------------------------- #
#                                    Config                                    #
# ---------------------------------------------------------------------------- #

NAME			:= webserv
CPP				:= c++
CPPFLAGS		:= -std=c++17 #-Werror -Wall -Wextra

# ---------------------------------------------------------------------------- #
#                                     Files                                    #
# ---------------------------------------------------------------------------- #

OBJ_DIR			:= ./objs/
# HEADERS			:= -I ./includes

SRCS 	:= $(wildcard srcs/*.cpp \
					srcs/parsing_config/*.cpp \
					srcs/stringing_from_config/*.cpp)

OBJS	:= $(patsubst src/%.c,$(OBJ_DIR)src/%.o,$(SRCS))

# ---------------------------------------------------------------------------- #
#                                     Rules                                    #
# ---------------------------------------------------------------------------- #

all: $(NAME)

$(NAME): $(OBJS)
	$(CPP) $(CPPFLAGS) $(OBJS) -o $(NAME)
	@echo $(GREEN)"Linking $(NAME)!"$(DEFAULT);

$(OBJ_DIR)src/%.o: src/%.c
	@mkdir -p $(dir $@)
	@$(CPP) $(CFLAGS) -o $@ -c $<

debug: CPPFLAGS += -g
debug: re

nflag: CPPFLAGS = 
nflag: all

clean:
	@rm -rf $(OBJ_DIR)

fclean: clean
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re

DEFAULT	= "\033[39m"
GREEN	= "\033[32m"