
Name = test
File_C = \
	Stringing/StringHelp.cpp \
	Stringing/StringArr.cpp \
	Stringing/Pair.cpp \
	Stringing/Twin.cpp \
	Stringing/StringDataTracker.cpp \
		Parsing/ConfigParse.cpp \
		Parsing/parse_config_file.cpp
File_O = $(File_C:.cpp=.o)

#Flags = -Wall -Wextra -Werror
Flags += -std=c++98


$(Name) : $(File_O)
	c++ $(Flags) $(File_O) -o $(Name)

all: $(Name)

%.o: %.cpp
	c++ $(Flags) -c $^ -o $@

clean:
	rm -f $(File_O)

fclean:
	rm -f $(File_O)
	rm -f $(Name)

re: fclean all

.Phony: all clean fclean re
