
#pragma once
#include <iostream>
#include "../stringing/StringHelp.hpp"
#include "../stringing/StringArr.hpp"
#include "../stringing/StringDataTracker.hpp"

class ConfigParse
{
	//private:
	public:
		const std::string	name;
		size_t				arg_min;
		size_t				arg_max;
		void				*(*new_func)(void *, int, int, std::string[]);
		void				(*set_func)(void *, int, int, std::string[]);
		const size_t		sub_num;
		const ConfigParse	*sub;

	public:
		ConfigParse(
			const std::string name,
			size_t arg_min,
			size_t arg_max,
			void * (*new_func)(void *, int, int, std::string[]),
			void (*set_func)(void *, int, int, std::string[]),
			const size_t sub_num = 0,
			const ConfigParse * sub = NULL
		);
		ConfigParse(ConfigParse const & othr);
		~ConfigParse();

		void	parse(void * ptr, std::string str, std::string tab) const;
		void	print(std::string tab = "") const;
};
