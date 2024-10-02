
#include "StringHelp.hpp"

//	checks if position is inside a comment, comment being anything betweedd '#' and the next '\n'
//	in files ending with .conf, vscode also marks anything after ';' to the end of the line as a comment (makes it green)
//		but here it only considers '#'
bool		StringHelp::is_comment(std::string str, size_t pos)
{
	size_t hash = str.rfind('#', pos);
	if (hash != -1)
	{
		size_t newl = str.find('\n', hash);
		if (pos < newl)
			return true;
	}
	return false;
}


//	setting ignore means that anything found inside ignore will be ignored and the next result will be searched for
size_t		StringHelp::find_ignore(std::string str, size_t pos, char c, char ignore)
{
	size_t p = std::string::npos;
	bool isSQ = false, isDQ = false;
	do
	{
		p = str.find(c, pos);
		if ((ignore & FIND_IGNORE_COMMENT) && is_comment(str, p))
			pos = p + 1;
		else if (ignore & (FIND_IGNORE_SINGLE_QUOTE | FIND_IGNORE_DOUBLE_QUOTE))
		{
			StringDataTracker::trackQuotes(str, pos, p, isSQ, isDQ);
			if ((ignore & FIND_IGNORE_SINGLE_QUOTE) && isSQ)
				pos = p + 1;
			else if ((ignore & FIND_IGNORE_DOUBLE_QUOTE) && isDQ)
				pos = p + 1;
			else
				break;
		}
		else
			break;
	}
	while (p != std::string::npos);
	return (p);
}
size_t		StringHelp::find_ignore(std::string str, size_t pos, std::string s, char ignore)
{
	size_t p = std::string::npos;
	bool isSQ = false, isDQ = false;
	do
	{
		p = str.find(s, pos);
		if ((ignore & FIND_IGNORE_COMMENT) && is_comment(str, p))
			pos = p + 1;
		else if (ignore & FIND_IGNORE_QUOTE)
		{
			StringDataTracker::trackQuotes(str, pos, p, isSQ, isDQ);
			if ((ignore & FIND_IGNORE_SINGLE_QUOTE) && isSQ)
				pos = p + 1;
			else if ((ignore & FIND_IGNORE_DOUBLE_QUOTE) && isDQ)
				pos = p + 1;
			else
				break;
		}
		else
			break;

	}
	while (p != std::string::npos);
	return (p);
}



std::string	StringHelp::cut(std::string str, size_t from, size_t to)
{
	return str.substr(from, to - from);
}
std::string	StringHelp::cut(std::string str, size_t from)
{
	return str.substr(from);
}





StringHelp::Pair::Pair()
{
	p1 = std::string::npos;
	p2 = std::string::npos;
}
//	checks if both pair characters were found
bool StringHelp::Pair::all_good() const
{
	return (p1 != std::string::npos && p2 != std::string::npos);
}
//	checks if eighter pair characters were found
bool StringHelp::Pair::any_good() const
{
	return (p1 != std::string::npos || p2 != std::string::npos);
}

//	cuts the string inside the Pair including the pair characters
std::string StringHelp::Pair::cut_in(std::string str) const
{
	return str.substr(p1, (p2 - p1) + 1);
}
//	cuts the string inside the Pair excluding the pair characters
std::string StringHelp::Pair::cut_ex(std::string str) const
{
	return str.substr(p1 + 1, (p2 - p1) - 1);
}

//	looks for the outer most pair of characters denoted by c1 and c2
StringHelp::Pair	StringHelp::Pair::find(std::string str, size_t pos, char c1, char c2)
{
	Pair pair;
	pos = StringHelp::find_ignore(str, pos, c1, FIND_IGNORE_QUOTE);
	if (pos == std::string::npos)
		return pair;
	pair.p1 = pos;
	size_t p1, p2;
	size_t unpaired = 0;
	do
	{
		p1 = StringHelp::find_ignore(str, pos, c1, FIND_IGNORE_QUOTE);
		p2 = StringHelp::find_ignore(str, pos, c2, FIND_IGNORE_QUOTE);
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
	p2 = find_ignore(str, p1, '#');
	while (p2 != std::string::npos)
	{
		trim += cut(str, p1, p2);
		p1 = find_ignore(str, p2, '\n');
		p2 = find_ignore(str, p1, '#');
	}
	trim += cut(str, p1);

	return (trim);
}





StringHelp::StringDataTracker::StringDataTracker()
{

}
StringHelp::StringDataTracker::StringDataTracker(std::string file)
{
	this -> file = file;
	pos = 0;
	newLines  = 1;
	isSingleQuote = false;
	isDoubleQuote = false;
}
StringHelp::StringDataTracker::~StringDataTracker()
{

}



//	counts the number of '\n' between {from} and {to}
int	StringHelp::StringDataTracker::countNewLines(std::string str, size_t from, size_t to)
{
	size_t	p = str.find('\n', from);
	int	nl = 0;
	while (p < to)
	{
		p = str.find('\n', p + 1);
		nl++;
	}
	return (nl);
}
//	tracks what '"quote level"' you're in
//	isSQ and isDQ should be set to which quotes you're in at the start
//	and will denote which you're in at the end
void	StringHelp::StringDataTracker::trackQuotes(std::string str, size_t from, size_t to, bool & isSQ, bool & isDQ)
{
	size_t	p = from;
	size_t	s = str.find('\'', p);
	size_t	d = str.find('\"', p);

	while (s < to || d < to)
	{
		if (isSQ && s < to)
		{
			isSQ = false;
			p = s + 1;
		}
		else if (isDQ && d < to)
		{
			isDQ = false;
			p = d + 1;
		}
		else if (!isDQ && s < d)
		{
			isSQ = true;
			p = s + 1;
		}
		else if (!isSQ && d < s)
		{
			isDQ = true;
			p = d + 1;
		}
		else
		{
			break;
		}
		s = str.find('\'', p);
		d = str.find('\"', p);
	}
}

void	StringHelp::StringDataTracker::update(std::string landmark)
{
	size_t	p = find_ignore(file, pos, landmark, FIND_IGNORE_COMMENT);
	if (p != std::string::npos)
	{
		newLines += countNewLines(file, pos, p);
		trackQuotes(file, pos, p, isSingleQuote, isDoubleQuote);
		pos = p;
	}
	else
	{
		std::cout << "warning: a problem occured with the new-line-tracker\n  readings after point may be inaccurate.\n";
	}
	//std::cout << "  landmark '" << landmark << "' found on line [" << newLines << "]\n";
}
void	StringHelp::StringDataTracker::update(size_t move)
{
	size_t	p = pos + move;
	if (p != std::string::npos)
	{
		newLines += countNewLines(file, pos, p);
		trackQuotes(file, pos, p, isSingleQuote, isDoubleQuote);
		pos = p;
	}
	else
	{
		std::cout << "warning: a problem occured with the new-line-tracker\n  readings after point may be inaccurate.\n";
	}
	//std::cout << "  landmark '" << landmark << "' found on line [" << newLines << "]\n";
}
