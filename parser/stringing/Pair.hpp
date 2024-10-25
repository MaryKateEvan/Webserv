
#pragma once
#include "StringHelp.hpp"

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