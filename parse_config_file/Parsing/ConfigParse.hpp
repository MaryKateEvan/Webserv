
#pragma once
#include <iostream>
#include "../Stringing/StringHelp.hpp"
#include "../Stringing/StringArr.hpp"
#include "../Stringing/StringDataTracker.hpp"

class ConfigParse
{
	private:
		const std::string	name;
		int					arg_min;
		int					arg_max;
		void				*(*new_func)(void *, int, std::string[]);
		void				(*set_func)(void *, int, std::string[]);
		int					sub_num;
		ConfigParse			*sub;

	public:
		ConfigParse(
			const std::string name,
			int arg_min,
			int arg_max,
			void * (*new_func)(void *, int, std::string[]),
			void (*set_func)(void *, int, std::string[]),
			int sub_num = 0,
			ConfigParse *sub = NULL
		);
		ConfigParse(ConfigParse const & othr);
		~ConfigParse();

		void	parse(void * ptr, std::string str) const;
		void	print(std::string tab = "") const;
};
