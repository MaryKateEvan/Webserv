
#include "ConfigParse.hpp"

extern StringDataTracker	tracker;

ConfigParse::ConfigParse(
	std::string name,
	int arg_min,
	int arg_max,
	void * (*new_func)(void *, int, std::string[]),
	void (*set_func)(void *, int, std::string[]),
	int sub_num,
	ConfigParse *sub
) :
	name(name)
{
	this -> arg_min = arg_min;
	this -> arg_max = arg_max;
	this -> new_func = new_func;
	this -> set_func = set_func;
	this -> sub_num = sub_num;
	this -> sub = sub;

	if (new_func == NULL && set_func == NULL)
	{
		std::cout << "NOTE: Configuration Parsing Element '" << name << "' was given neither a new() nor a set() function.\n";
	}

	//std::cout << "++++ ConfigParse '" << this -> name << "'\n";
	//for (int j = 0; j < sub_num; j++)
	//	std::cout << "  " << sub[j].name << "\n";
	//std::cout << ";\n";
}
ConfigParse::ConfigParse(ConfigParse const & othr) :
	name(othr.name)
{
	this -> arg_min = othr.arg_min;
	this -> arg_max = othr.arg_max;
	this -> new_func = othr.new_func;
	this -> set_func = othr.set_func;
	this -> sub_num = othr.sub_num;
	this -> sub = othr.sub;

	//std::cout << "==== ConfigParse '" << this -> name << "'\n";
	//for (int j = 0; j < sub_num; j++)
	//	std::cout << "  " << sub[j].name << "\n";
	//std::cout << ";\n";
}
ConfigParse::~ConfigParse()
{

}



/*
	the content of the last layer is split into elements
		this means tooking for the next ';' or pair of '{}' and then splitting until after that
	then each element is split into segments
		this means they are split by ' ' except inside of "" or {}
	then the segments are sorted into 'name' 'args' and 'content'
		the allways the first segment
		the content is the last segment if it starts with '{' and ends with '}'
		everything else is args

	then a search is made for name in the list of accepted subtypes
	then a check is made for if content is required or not
	then a check is made for if the correct number of args were given
	then it checks which function to call
		if new_func is given
			args is given to the function and the pointer to the object is stored
			then parse() is performen recursivly on the new object
		if set_func is given
			args is given to the function
*/
void	ConfigParse::parse(void * ptr, std::string str) const
{
//	std::cout << "'" << name << "' has sub\n";
//	for (int j = 0; j < sub_num; j++)
//		std::cout << "  " << sub[j].name << "\n";
//	std::cout << ";\n";

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

		//tracker.update(name);
		//for (size_t j = 0; j < args.num; j++)
		//	tracker.update(args[j]);

		void *	tmp;
		ConfigParse	* found = NULL;
		for (size_t j = 0; j < sub_num; j++)
		{
			if (name == sub[j].name)
			{
				found = &(sub[j]);
				if (content == NULL && found -> sub_num != 0)
				{
					tracker.report_not_content(REPORT_WARNING | REPORT_LINE, found -> name);
					break;
				}
				if (content != NULL && found -> sub_num == 0)
				{
					tracker.report_got_content(REPORT_WARNING | REPORT_LINE, found -> name);
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
					tracker.report_args_number(REPORT_WARNING | REPORT_LINE, found -> name, found -> arg_min, found -> arg_max, args.num);
				}
				break;
			}
		}

		if (found == NULL)
		{
			tracker.report_unknown_subtype(REPORT_WARNING | REPORT_LINE, this -> name, name);
			std::cout << "none of:\n";
			for (int j = 0; j < sub_num; j++)
				std::cout << "  " << sub[j].name << "\n";
			std::cout << ";\n";
		}
		delete content;
	}
}
void	ConfigParse::print(std::string tab) const
{
	std::cout << tab << "Name: '" << name << "'\n";
	//std::cout << tab << "[min:max] [" << arg_min << ":" << arg_max << "]\n";
	//std::cout << tab << "[new:set] [" << new_func << ":" << set_func << "]\n";
	//std::cout << tab << "Sub: [" << sub_num << "]\n";
	for (int i = 0; i < sub_num; i++)
	{
		//std::cout << tab << "[" << i << "]\n";
		sub[i].print(tab + "  ");
	}
}
