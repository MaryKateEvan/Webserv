
#pragma once
#include <iostream>
#include "StringArr.hpp"

class StringHelp
{
	public:

		static size_t		find_unquoted(std::string str, size_t pos, char c);
		static std::string	cut(std::string str, size_t from, size_t to);


		class	Pair
		{
			public:
				size_t	p1;
				size_t	p2;

				Pair();

				bool	all_good() const;
				bool	any_good() const;

				std::string	cut_in(std::string str) const;
				std::string	cut_ex(std::string str) const;

				static Pair	find(std::string str, size_t pos, char c1, char c2);
		};


		static bool			is_whitespace(char c);
		static std::string	trim_whitespace(std::string str);
		static std::string	remove_comments(std::string str);


	public:
		class	NewLineTracker
		{
			public:
				std::string	file;
				size_t		pos;
				size_t		num;

				NewLineTracker();
				NewLineTracker(std::string file);
				~NewLineTracker();
				void	count(size_t from, size_t to);
				void	count(std::string landmark);
		};
};
