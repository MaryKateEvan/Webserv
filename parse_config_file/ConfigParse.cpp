
#include "ConfigParse.hpp"

ConfigParse::ConfigParse(
	std::string name,
	int arg_min,
	int arg_max,
	void * (*new_func)(void *, int, std::string[]),
	void (*set_func)(void *, int, std::string[]),
	int sub_num,
	ConfigParse *sub
)
{
	this -> name = name;
	this -> arg_min = arg_min;
	this -> arg_max = arg_max;
	this -> new_func = new_func;
	this -> set_func = set_func;
	this -> sub_num = sub_num;
	this -> sub = sub;
}
ConfigParse::~ConfigParse()
{

}

extern StringHelp::StringDataTracker	*tracker;

void	ConfigParse::parse(void * ptr, std::string str)
{
	StringArr elem = StringArr::split_elements(str);
	for (size_t i = 0; i < elem.num; i++)
	{
		StringArr seg = StringArr::split_segments(elem[i]);
		std::string name;
		std::string content;
		StringArr args = seg.remove_name_and_content(name, content);

		std::cout << "\e[38;2;255;0;0m[" << i << "]" << elem[i] << "\e[m\n";
		for (size_t j = 0; j < seg.num; j++)
			std::cout << "\e[38;2;0;0;255m  [" << j << "]" << seg[j] << "\e[m\n";
		std::cout << "\e[38;2;0;255;0mname '" << name << "'\e[m\n";
		for (size_t j = 0; j < args.num; j++)
			std::cout << "\e[38;2;0;255;0marg[" << j << "] '" << args[j] << "'\e[m\n";
		std::cout << "\e[38;2;0;255;0mcontent '" << content << "'\e[m\n";

		tracker -> update(name);
		for (size_t j = 0; j < args.num; j++)
		{
			tracker -> update(args[j]);
		}

		void *	tmp;
		bool	found = false;
		for (size_t j = 0; j < sub_num; j++)
		{
			if (name == sub[j].name)
			{
				if (content.length() == 0 && sub[j].sub_num != 0)
					std::cout << "warning: did not get {Content} when expected.\n";
				if (content.length() != 0 && sub[j].sub_num == 0)
					std::cout << "warning: got {Content} when not expected.\n";

				if (sub[j].new_func != NULL)
				{
					tmp = sub[j].new_func(ptr, args.num, args.arr);
					if (content.length() != 0)
						sub[j].parse(tmp, content);
				}
				if (sub[j].set_func != NULL)
				{
					sub[j].set_func(ptr, args.num, args.arr);
				}
				found = true;
				break;
			}
		}
		if (!found)
		{
			std::cout << "warning: "
				<< "line:" << tracker -> newLines << ": "
				<< "unknown " << this -> name << " argument '" << seg[0] << "'.\n";
		}
	}
}
