
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

	if (new_func == NULL && set_func == NULL)
	{
		std::cout << "NOTE: Configuration Parsing Element '" << name << "' was given neighter a new() nor a set() function.\n";
	}
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
		<< "expected between " << min << " and " << max 
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

		//std::cout << "\e[38;2;255;0;0melement\e[m\n";
		//std::cout << "\e[38;2;255;0;0m[" << i << "]" << elem[i] << "\e[m\n";
		//std::cout << "\e[38;2;0;0;255msegments\e[m\n";
		//for (size_t j = 0; j < seg.num; j++)
		//	std::cout << "\e[38;2;0;0;255m  [" << j << "]" << seg[j] << "\e[m\n";
		//std::cout << "\e[38;2;0;255;0mname:" << name << ";\e[m\n";
		//for (size_t j = 0; j < args.num; j++)
		//	std::cout << "\e[38;2;0;255;0marg[" << j << "]:" << args[j] << ";\e[m\n";
		//std::cout << "\e[38;2;0;255;0mcontent:" << content << ";\e[m\n";
		//if (content != NULL)
		//	std::cout << "\e[38;2;0;255;0mcontent:" << *content << ";\e[m\n";

		tracker -> update(name);
		for (size_t j = 0; j < args.num; j++)
			tracker -> update(args[j]);

		void *	tmp;
		ConfigParse	* found = NULL;
		for (size_t j = 0; j < sub_num; j++)
		{
			if (name == sub[j].name)
			{
				found = &(sub[j]);
				if (content == NULL && found -> sub_num != 0)
				{
					warning_not_content(found -> name);
					break;
				}
				if (content != NULL && found -> sub_num == 0)
				{
					warning_got_content(found -> name);
					break;
				}

				if (found -> arg_min <= args.num && args.num <= found -> arg_max)
				{
					if (found -> new_func != NULL)
					{
						tmp = found -> new_func(ptr, args.num, args.arr);
						found -> parse(tmp, *content);
					}
					else if (found -> set_func != NULL)
					{
						found -> set_func(ptr, args.num, args.arr);
					}
				}
				else
				{
					warning_args_number(found -> name, found -> arg_min, found -> arg_max, args.num);
				}
				break;
			}
		}

		if (found == NULL)
		{
			warning_unknown_subtype(this -> name, name);
		}
		delete content;
	}
}
