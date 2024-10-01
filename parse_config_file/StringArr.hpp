
#pragma once
#include <iostream>
#include "StringHelp.hpp"
#include "ConfigParse.hpp"

class StringArr
{
	private:
	public:
		std::string *	arr;
		const size_t	num;

		StringArr();
		StringArr(size_t n);
		StringArr(StringArr const & othr);
		~StringArr();

	public:
		std::string & operator[](size_t idx);

		StringArr	remove_name_and_content(std::string & name, std::string & content);

		static StringArr	split(std::string str, char c);
		static StringArr	split(std::string str, char c1, char c2);
		static StringArr	split_elements(std::string str);
		static StringArr	split_segments(std::string str);
};
