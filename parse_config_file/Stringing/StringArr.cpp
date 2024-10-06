
#include "StringArr.hpp"

StringArr::StringArr() : num(0)
{
	//std::cout << "++++ String Arr []\n";
	arr = new std::string[num];
}
StringArr::StringArr(size_t n) : num(n)
{
	//std::cout << "++++ String Arr [" << num << "]\n";
	arr = new std::string[num];
}
StringArr::StringArr(std::vector<std::string> vec) : num(vec.size())
{
	//std::cout << "++++ String Arr <" << num << ">\n";
	arr = new std::string[num];
	for (size_t i = 0; i < num; i++)
		arr[i] = vec[i];
}
StringArr::StringArr(StringArr const & othr) : num(othr.num)
{
	//std::cout << "++++ String Arr [[" << num << "]]\n";
	arr = new std::string[num];
	for (size_t i = 0; i < num; i++)
		arr[i] = othr.arr[i];
}
StringArr::~StringArr()
{
	//std::cout << "---- String Arr [" << num << "]\n";
	delete[] arr;
	arr = NULL;
}



std::string & StringArr::operator[](size_t idx)
{
	return arr[idx];
}



extern StringDataTracker	tracker;

//	checks if the string starts and ends with '' or ""
//	and if so, removes them
std::string	StringArr::remove_quotes(std::string str)
{
	size_t	l = str.length();
	size_t	q;
	Twin	twin;

	twin = Twin::find(str, 0, '\'');
	if (twin.p1 == 0)
	{
		if (twin.p2 == l - 1)
			return (twin.cut_ex(str));
		tracker.report_extra_quotes(REPORT_WARNING | REPORT_LINE);
	}

	twin = Twin::find(str, 0, '\"');
	if (twin.p1 == 0)
	{
		if (twin.p2 == l - 1)
			return (twin.cut_ex(str));
		tracker.report_extra_quotes(REPORT_WARNING | REPORT_LINE);
	}

	return (str);
}
//	the first element in StringArr will be considered the {name}
//	if the last element starts with '{' and ends with '}', is is considered {content}
//	everything else will be returned as {args}
StringArr	StringArr::cut_name_args_content(std::string & name, std::string ** content)
{
	int n = num;

	name = arr[0];
	tracker.update(name);

	*content = NULL;
	if (num > 1)
	{
		Pair pair = Pair::find(arr[num - 1], 0, '{', '}');
		if (pair.all_good())
		{
			*content = new std::string((pair.cut_ex(arr[num - 1])).c_str());
			n--;
		}
	}
	n--;

	std::vector<std::string> vec;
	std::string temp;
	for (size_t i = 0; i < n; i++)
	{
		temp = remove_quotes(arr[i + 1]);
		if (!StringHelp::only_whitespace(temp))
			vec.push_back(temp);
		tracker.update(temp);
	}
	return (vec);
}



//	splits string by character
StringArr	StringArr::split(std::string str, char c)
{
	size_t	p1, p2;
	std::vector<std::string> vec;

	p1 = 0;
	p2 = StringHelp::find_ignore(str, p1, c, FIND_IGNORE_QUOTE);
	while (p2 != std::string::npos)
	{
		if (p1 != p2)
			vec.push_back(StringHelp::cut(str, p1, p2));
		p1 = p2 + 1;
		p2 = StringHelp::find_ignore(str, p1, c, FIND_IGNORE_QUOTE);
	}
	if (p1 < str.length())
		vec.push_back(str.substr(p1));

	return (StringArr(vec));
}



//	splits string into segments seperated by ';' or the end of a '{}' pair
StringArr	StringArr::split_elements(std::string str)
{
	size_t	pos;
	size_t	semi;
	Pair	pair;
	std::vector<std::string> vec;

	pos = 0;
	semi = StringHelp::find_ignore(str, pos, ';', FIND_IGNORE_QUOTE);
	pair = Pair::find(str, pos, '{', '}');
	while (semi != std::string::npos || (pair.p1 != std::string::npos && pair.p2 != std::string::npos))
	{
		if (semi < pair.p1)
		{
			vec.push_back(StringHelp::cut(str, pos, semi + 1));
			pos = semi + 1;
			semi = StringHelp::find_ignore(str, pos, ';', FIND_IGNORE_QUOTE);
		}
		else if (pair.p1 < semi)
		{
			vec.push_back(StringHelp::cut(str, pos, pair.p2 + 1));
			pos = pair.p2 + 1;
			semi = StringHelp::find_ignore(str, pos, ';', FIND_IGNORE_QUOTE);
			pair = Pair::find(str, pos, '{', '}');
		}
		else
			break;
	}

	return (StringArr(vec));
}

//	splits an elements by ' ' or content of '{}'
StringArr	StringArr::split_segments(std::string str)
{
	size_t	pos;
	size_t	space;
	size_t	semi;
	Pair	pair;
	std::vector<std::string> vec;

	pos = 0;
	space = StringHelp::find_ignore(str, pos, ' ', FIND_IGNORE_QUOTE);
	semi = StringHelp::find_ignore(str, pos, ';', FIND_IGNORE_QUOTE);
	pair = Pair::find(str, pos, '{', '}');

	while (1)
	{
		if (space < pair.p1)
		{
			if (space > pos)
				vec.push_back(StringHelp::cut(str, pos, space));
			pos = space + 1;
			space = StringHelp::find_ignore(str, pos, ' ', FIND_IGNORE_QUOTE);
		}
		else if (space > pair.p1)
		{
			vec.push_back(pair.cut_in(str));
			break;
		}
		else if (space > semi)
		{
			vec.push_back(StringHelp::cut(str, pos, semi));
			break;
		}
	}

	return (StringArr(vec));

}
