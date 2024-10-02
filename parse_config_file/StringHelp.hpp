
#pragma once
#include <iostream>

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

		static size_t		find_regular(std::string str, size_t pos, char c, char ignore = 0);
		static size_t		find_regular(std::string str, size_t pos, std::string s, char ignore = 0);

		static std::string	cut(std::string str, size_t from, size_t to);
		static std::string	cut(std::string str, size_t from);


		/*
			Pair refers to a section of text that is in between a opening and closing character
			find() will ignore any other Pairs inside this Pair
		*/
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
		class	Twin
		{
			public:
				size_t	p1;
				size_t	p2;

				Twin();

				bool	all_good();
				bool	any_good();

				std::string	cut_in(std::string str) const;
				std::string	cut_ex(std::string str) const;

				static Twin	find(std::string str, size_t pos, char c, char i);
		};


		static bool			is_whitespace(char c);
		static std::string	trim_whitespace(std::string str);
		static std::string	remove_comments(std::string str);


	public:
		class	StringDataTracker
		{
			private:
				std::string	file;
				size_t		pos;
			public:
				size_t		newLines;
				bool		isSingleQuote;
				bool		isDoubleQuote;

			public:
				StringDataTracker();
				StringDataTracker(std::string file);
				~StringDataTracker();

			public:
				static int	countNewLines(std::string str, size_t from, size_t to);
				static void	trackQuotes(std::string str, size_t from, size_t to, bool & isSQ, bool & isDQ);
				void	update(std::string landmark);
				void	update(size_t move);
		};
};
