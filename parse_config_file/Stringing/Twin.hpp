
#pragma once
#include <iostream>
#include "StringHelp.hpp"

class	Twin
{
	public:
		size_t	p1;
		size_t	p2;

		Twin();

		bool	all_good() const;
		bool	any_good() const;

		std::string	cut_in(std::string str) const;
		std::string	cut_ex(std::string str) const;

		static Twin	find(std::string str, size_t pos, char c, char i);
};
