
#pragma once
#include <iostream>
#include "StringDataTracker.hpp"
#include "Twin.hpp"

#define FIND_IGNORE_SINGLE_QUOTE 0b001
#define FIND_IGNORE_DOUBLE_QUOTE 0b010
#define FIND_IGNORE_QUOTE FIND_IGNORE_SINGLE_QUOTE | FIND_IGNORE_DOUBLE_QUOTE
#define FIND_IGNORE_COMMENT      0b100

class StringHelp
{
	public:
		static bool			is_comment(std::string str, size_t pos);
		static bool			is_single_quote(std::string str, size_t pos);
		static bool			is_double_quote(std::string str, size_t pos);

		static size_t		find_ignore(std::string str, size_t pos, char c, char ignore = 0);
		static size_t		find_ignore(std::string str, size_t pos, std::string s, char ignore = 0);

		static std::string	cut(std::string str, size_t from, size_t to);
		static std::string	cut(std::string str, size_t from);

		static bool			is_whitespace(char c);
		static bool			only_whitespace(std::string str);
		static std::string	trim_whitespace(std::string str);
		static std::string	remove_comments(std::string str);
};
