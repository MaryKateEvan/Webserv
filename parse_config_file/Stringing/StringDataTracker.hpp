
#pragma once
#include <iostream>
#include "StringHelp.hpp"

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
