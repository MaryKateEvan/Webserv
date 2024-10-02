
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



//	checks if the string starts and ends with '' or ""
//	and if so, removes them
std::string	StringArr::remove_quotes(std::string str)
{
	size_t	l = str.length();

	if ((str[0] == '\"' && str[l - 1] == '\"') ||
		(str[0] == '\'' && str[l - 1] == '\''))
	{
		return (str.substr(1, l - 2));
	}
	return (str);
}
//	the first element in StringArr will be considered the {name}
//	if the last element starts with '{' and ends with '}', is is considered {content}
//	everything else will be returned as {args}
StringArr	StringArr::cut_name_args_content(std::string & name, std::string & content)
{
	int n = num;

	if (num > 1)
	{
		StringHelp::Pair pair = StringHelp::Pair::find(arr[num - 1], 0, '{', '}');
		if (pair.all_good())
		{
			content = pair.cut_ex(arr[num - 1]);
			n--;
		}
		else
			content = "";
	}
	name = arr[0];
	n--;

	StringArr args(n);
	for (size_t i = 0; i < n; i++)
		args[i] = remove_quotes(arr[i + 1]);
	return (args);
}



StringArr	StringArr::split(std::string str, char c)
{
/*
	size_t	num, p1, p2;
	num = 0;
	p1 = 0;
	p2 = StringHelp::find_ignore(str, p1, c, FIND_IGNORE_QUOTE);
	while (p2 != std::string::npos)
	{
		if (p1 != p2)
			num++;
		p1 = p2 + 1;
		p2 = StringHelp::find_ignore(str, p1, c, FIND_IGNORE_QUOTE);
	}
	if (p1 < str.length())
		num++;

	StringArr splt(num);
	num = 0;
	p1 = 0;
	p2 = StringHelp::find_ignore(str, p1, c, FIND_IGNORE_QUOTE);
	while (p2 != std::string::npos)
	{
		if (p1 != p2)
		{
			splt.arr[num] = StringHelp::cut(str, p1, p2);
			num++;
		}
		p1 = p2 + 1;
		p2 = StringHelp::find_ignore(str, p1, c, FIND_IGNORE_QUOTE);
	}
	if (p1 < str.length())
		splt.arr[num] = str.substr(p1);

	return (splt);
*/

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

StringArr	StringArr::split(std::string str, char c1, char c2)
{
/*
	size_t	num, p;
	StringHelp::Pair pair;

	num = 0;
	p = 0;
	pair = StringHelp::Pair::find(str, p, c1, c2);
	while (pair.p1 != -1)
	{
		if (p != pair.p1)
			num++;
		num++;
		p = pair.p2 + 1;
		pair = StringHelp::Pair::find(str, p, c1, c2);
	}
	num++;

	StringArr splt(num);
	num = 0;
	p = 0;
	pair = StringHelp::Pair::find(str, p, c1, c2);
	while (pair.p1 != -1)
	{
		if (p != pair.p1)
		{
			splt.arr[num] = StringHelp::cut(str, p, pair.p1);
			num++;
		}
		//splt.arr[num] = StringHelp::cut(str, pair.p1 + 1, pair.p2 - 1);	//	without {}
		splt.arr[num] = StringHelp::cut(str, pair.p1, pair.p2);			//	with {}
		num++;
		p = pair.p2 + 1;
		pair = StringHelp::Pair::find(str, p, c1, c2);
	}
	splt.arr[num] = str.substr(p);

	return (splt);
*/

	size_t	p;
	StringHelp::Pair pair;
	std::vector<std::string> vec;

	p = 0;
	pair = StringHelp::Pair::find(str, p, c1, c2);
	while (pair.p1 != -1)
	{
		if (p != pair.p1)
			vec.push_back(StringHelp::cut(str, p, pair.p1));
		vec.push_back(pair.cut_ex(str));
		p = pair.p2 + 1;
		pair = StringHelp::Pair::find(str, p, c1, c2);
	}
	vec.push_back(str.substr(p));

	return (StringArr(vec));
}



/*
	split string into segments seperated by ';' or the end of a '{}' pair
*/
StringArr	StringArr::split_elements(std::string str)
{
/*
	size_t	pos;
	size_t	semi;
	StringHelp::Pair	pair;
	size_t	num;

	num = 0;
	pos = 0;
	semi = StringHelp::find_ignore(str, pos, ';');
	pair = StringHelp::Pair::find(str, pos, '{', '}');
	while (semi != std::string::npos || (pair.p1 != std::string::npos && pair.p2 != std::string::npos))
	{
		if (semi < pair.p1)
		{
			num++;
			pos = semi + 1;
			semi = StringHelp::find_ignore(str, pos, ';');
		}
		else if (pair.p1 < semi)
		{
			num++;
			pos = pair.p2 + 1;
			semi = StringHelp::find_ignore(str, pos, ';');
			pair = StringHelp::Pair::find(str, pos, '{', '}');
		}
		else
			break;
	}

	StringArr splt(num);
	num = 0;
	pos = 0;
	semi = StringHelp::find_ignore(str, pos, ';');
	pair = StringHelp::Pair::find(str, pos, '{', '}');
	while (semi != std::string::npos || (pair.p1 != std::string::npos && pair.p2 != std::string::npos))
	{
		if (semi < pair.p1)
		{
			splt.arr[num] = StringHelp::cut(str, pos, semi + 1);
			num++;
			pos = semi + 1;
			semi = StringHelp::find_ignore(str, pos, ';');
		}
		else if (pair.p1 < semi)
		{
			splt.arr[num] = StringHelp::cut(str, pos, pair.p2 + 1);
			num++;
			pos = pair.p2 + 1;
			semi = StringHelp::find_ignore(str, pos, ';');
			pair = StringHelp::Pair::find(str, pos, '{', '}');
		}
		else
			break;
	}

	return (splt);
*/

	size_t	pos;
	size_t	semi;
	StringHelp::Pair	pair;
	std::vector<std::string> vec;

	pos = 0;
	semi = StringHelp::find_ignore(str, pos, ';', FIND_IGNORE_QUOTE);
	pair = StringHelp::Pair::find(str, pos, '{', '}');
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
			pair = StringHelp::Pair::find(str, pos, '{', '}');
		}
		else
			break;
	}

	return (StringArr(vec));
}

/*
	splits an elements by ' ' or content of '{}'
*/
StringArr	StringArr::split_segments(std::string str)
{
/*
	size_t	pos;
	size_t	space;
	size_t	semi;
	StringHelp::Pair	pair;
	size_t	num;

	num = 0;
	pos = 0;
	space = StringHelp::find_ignore(str, pos, ' ', FIND_IGNORE_QUOTE);
	semi = StringHelp::find_ignore(str, pos, ';', FIND_IGNORE_QUOTE);
	pair = StringHelp::Pair::find(str, pos, '{', '}');
	while (1)
	{
		if (space < pair.p1)
		{
			if (space > pos)
				num++;
			pos = space + 1;
			space = StringHelp::find_ignore(str, pos, ' ', FIND_IGNORE_QUOTE);
		}
		else if (space > pair.p1)
		{
			//num++;
			num++;
			break;
		}
		else if (space > semi)
		{
			num++;
			break;
		}
	}

	StringArr splt(num);
	num = 0;
	pos = 0;
	space = StringHelp::find_ignore(str, pos, ' ', FIND_IGNORE_QUOTE);
	semi = StringHelp::find_ignore(str, pos, ';', FIND_IGNORE_QUOTE);
	pair = StringHelp::Pair::find(str, pos, '{', '}');
	while (1)
	{
		if (space < pair.p1)
		{
			if (space > pos)
			{
				splt.arr[num] = StringHelp::cut(str, pos, space);
				num++;
			}
			pos = space + 1;
			space = StringHelp::find_ignore(str, pos, ' ', FIND_IGNORE_QUOTE);
		}
		else if (space > pair.p1)
		{
			//splt.arr[num] = StringHelp::cut(str, pos, space);
			//num++;
			splt.arr[num] = pair.cut_in(str);
			num++;
			break;
		}
		else if (space > semi)
		{
			splt.arr[num] = StringHelp::cut(str, pos, semi);
			num++;
			break;
		}
	}

	return (splt);
*/

	size_t	pos;
	size_t	space;
	size_t	semi;
	StringHelp::Pair	pair;
	std::vector<std::string> vec;

	pos = 0;
	space = StringHelp::find_ignore(str, pos, ' ', FIND_IGNORE_QUOTE);
	semi = StringHelp::find_ignore(str, pos, ';', FIND_IGNORE_QUOTE);
	pair = StringHelp::Pair::find(str, pos, '{', '}');

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
