
#include "StringHelp.hpp"



size_t		StringHelp::find_unquoted(std::string str, size_t pos, char c)
{
	//	not programmed
	return str.find(c, pos);
}
std::string	StringHelp::cut(std::string str, size_t from, size_t to)
{
	return str.substr(from, to - from);
}



StringHelp::Pair::Pair()
{
	p1 = std::string::npos;
	p2 = std::string::npos;
}
bool StringHelp::Pair::all_good() const
{
	return (p1 != std::string::npos && p2 != std::string::npos);
}
bool StringHelp::Pair::any_good() const
{
	return (p1 != std::string::npos || p2 != std::string::npos);
}
std::string StringHelp::Pair::cut_in(std::string str) const
{
	return str.substr(p1, (p2 - p1) + 1);
}
std::string StringHelp::Pair::cut_ex(std::string str) const
{
	return str.substr(p1 + 1, (p2 - p1) - 1);
}

StringHelp::Pair	StringHelp::Pair::find(std::string str, size_t pos, char c1, char c2)
{
	Pair pair;
	pos = find_unquoted(str, pos, c1);
	if (pos == std::string::npos)
		return pair;
	pair.p1 = pos;
	size_t p1, p2;
	size_t unpaired = 0;
	do
	{
		p1 = find_unquoted(str, pos, c1);
		p2 = find_unquoted(str, pos, c2);
		if (p1 == std::string::npos && p2 == std::string::npos)
		{
			return pair;
		}
		else if (p1 < p2)
		{
			unpaired++;
			pos = p1 + 1;
		}
		else if (p2 < p1)
		{
			unpaired--;
			pos = p2 + 1;
		}
		else
		{
			return pair;
		}
	} while (unpaired != 0);
	pair.p2 = pos - 1;
	return pair;
}



bool			StringHelp::is_whitespace(char c)
{
	return (c == ' ' || c == '\t' || c == '\n' || c == '\r');
}
std::string		StringHelp::trim_whitespace(std::string str)
{
	std::string trim = "";
	bool last_was_white = false;
	for (int i = 0; i < str.length(); i++)
	{
		if (is_whitespace(str[i]))
		{
			if (!last_was_white)
			{
				trim += ' ';
				last_was_white = true;
			}
		}
		else
		{
			last_was_white = false;
			trim += str[i];
		}
	}
	return (trim);
}



std::string		StringHelp::remove_comments(std::string str)
{
	std::string trim;
	size_t	p1, p2;

	p1 = 0;
	p2 = find_unquoted(str, p1, '#');
	while (p2 != std::string::npos)
	{
		trim += cut(str, p1, p2);
		p1 = find_unquoted(str, p2, '\n');
		p2 = find_unquoted(str, p1, '#');
	}
	trim += str.substr(p1);

	return (trim);
}



StringHelp::NewLineTracker::NewLineTracker()
{

}
StringHelp::NewLineTracker::NewLineTracker(std::string file)
{
	this -> file = file;
	pos = 0;
	num = 1;
}
StringHelp::NewLineTracker::~NewLineTracker()
{

}
void StringHelp::NewLineTracker::count(size_t from, size_t to)
{
	//std::cout << "from:" << from << " to:" << to << "\n";
	size_t	p = file.find('\n', from);
	while (p < to)
	{
		p = file.find('\n', p + 1);
		num++;
	}
}
void StringHelp::NewLineTracker::count(std::string landmark)
{
	size_t	p = file.find(landmark, pos);;
	//std::cout << "  landmark '" << landmark << "' found at [" << p << "]\n";
	if (p != std::string::npos)
	{
		count(pos, p);
		pos = p;
	}
	else
	{
		std::cout << "warning: a problem occured with the new-line-tracker\n  readings after point may be inaccurate.\n";
	}
}
