
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

void	warning_args_number(std::string type, int min, int max, int num)
{
	std::cout
		<< "warning: "
		<< "line:" << tracker -> newLines << ": "
		<< "type '" << type << "' got invalid number of arguments: "
		<< "expected " << min << " to " << max 
		<< ", got " << num << ".\n";
}

void	warning_unknown_subtype(std::string type, std::string subtype)
{
	std::cout
		<< "warning: "
		<< "line:" << tracker -> newLines << ": "
		<< "type '" << type << "' got unknown subtype '" << subtype << "'.\n";
}

void	warning_not_content(std::string type)
{
	std::cout
		<< "warning: "
		<< "line:" << tracker -> newLines << ": "
		<< "type '" << type << "' did not get {Content} when expected.\n";
}
void	warning_got_content(std::string type)
{
	std::cout
		<< "warning: "
		<< "line:" << tracker -> newLines << ": "
		<< "type '" << type << "' got {Content} when not expected.\n";
}

void	ConfigParse::parse(void * ptr, std::string str)
{
	StringArr elem = StringArr::split_elements(str);
	for (size_t i = 0; i < elem.num; i++)
	{
		StringArr seg = StringArr::split_segments(elem[i]);
		std::string name;
		std::string * content;
		StringArr args = seg.cut_name_args_content(name, &content);

		std::cout << "\e[38;2;255;0;0melement\e[m\n";
		std::cout << "\e[38;2;255;0;0m[" << i << "]" << elem[i] << "\e[m\n";
		std::cout << "\e[38;2;0;0;255msegments\e[m\n";
		for (size_t j = 0; j < seg.num; j++)
			std::cout << "\e[38;2;0;0;255m  [" << j << "]" << seg[j] << "\e[m\n";
		std::cout << "\e[38;2;0;255;0mname:" << name << ";\e[m\n";
		for (size_t j = 0; j < args.num; j++)
			std::cout << "\e[38;2;0;255;0marg[" << j << "]:" << args[j] << ";\e[m\n";
		std::cout << "\e[38;2;0;255;0mcontent:" << content << ";\e[m\n";
		if (content != NULL)
			std::cout << "\e[38;2;0;255;0mcontent:" << *content << ";\e[m\n";

		tracker -> update(name);
		for (size_t j = 0; j < args.num; j++)
			tracker -> update(args[j]);

		void *	tmp;
		bool	found = false;
		for (size_t j = 0; j < sub_num; j++)
		{
			if (name == sub[j].name)
			{
				found = true;
				if (content == NULL && sub[j].sub_num != 0)
				{
					warning_not_content(sub[j].name);
					break;
				}
				if (content != NULL && sub[j].sub_num == 0)
				{
					warning_got_content(sub[j].name);
					break;
				}

				if (sub[j].new_func != NULL)
				{
					if (sub[j].arg_min <= args.num && args.num <= sub[j].arg_max)
					{
						tmp = sub[j].new_func(ptr, args.num, args.arr);
						sub[j].parse(tmp, *content);
					}
					else
					{
						warning_args_number(sub[j].name, sub[j].arg_min, sub[j].arg_max, args.num);
					}
				}
				else if (sub[j].set_func != NULL)
				{
					if (sub[j].arg_min <= args.num && args.num <= sub[j].arg_max)
					{
						sub[j].set_func(ptr, args.num, args.arr);
					}
					else
					{
						warning_args_number(sub[j].name, sub[j].arg_min, sub[j].arg_max, args.num);
					}
				}
				break;
			}
		}

		if (!found)
		{
			warning_unknown_subtype(this -> name, name);
		}
		delete content;
	}
}
