
#pragma once
#include <iostream>
#include <vector>
#include "StringHelp.hpp"
#include "Pair.hpp"

class StringArr
{
	private:
	public:
		std::string *	arr;
		const size_t	num;

		StringArr();
		StringArr(size_t n);
		StringArr(std::vector<std::string> vec);
		StringArr(StringArr const & othr);
		~StringArr();


	public:
		std::string & operator[](size_t idx);

		static std::string	remove_quotes(std::string str);
		StringArr			cut_name_args_content(std::string & name, std::string ** content, int & line);

		static StringArr	split(std::string str, char c);
		static StringArr	split_elements(std::string str);
		static StringArr	split_segments(std::string str);
};
