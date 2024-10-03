
#pragma once
#include <iostream>
#include "../Stringing/StringHelp.hpp"
#include "../Stringing/StringArr.hpp"
#include "../Stringing/StringDataTracker.hpp"

class ConfigParse
{
	private:
		std::string			name;
		int					arg_min;
		int					arg_max;
		void				*(*new_func)(void *, int, std::string[]);
		void				(*set_func)(void *, int, std::string[]);
		int					sub_num;
		ConfigParse			*sub;

	public:
		ConfigParse(
			std::string name,
			int arg_min,
			int arg_max,
			void * (*new_func)(void *, int, std::string[]),
			void (*set_func)(void *, int, std::string[]),
			int sub_num,
			ConfigParse *sub
		);
		ConfigParse(ConfigParse const & othr);
		~ConfigParse();

		void	parse(void * ptr, std::string str) const;
};
