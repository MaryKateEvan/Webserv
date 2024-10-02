
#include "StringDataTracker.hpp"

StringDataTracker::StringDataTracker()
{

}
StringDataTracker::StringDataTracker(std::string file)
{
	this -> file = file;
	pos = 0;
	newLines  = 1;
	isSingleQuote = false;
	isDoubleQuote = false;
}
StringDataTracker::~StringDataTracker()
{

}



//	counts the number of '\n' between {from} and {to}
int	StringDataTracker::countNewLines(std::string str, size_t from, size_t to)
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
void	StringDataTracker::trackQuotes(std::string str, size_t from, size_t to, bool & isSQ, bool & isDQ)
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

void	warning_landmark_not_found(int line)
{
	std::cout
		<< "warning: a problem occured with the StringDataTracker, "
		<< "Line Numbers and Quote parsing may be inaccurate after this point"
		<< "(line:" << line << ")."
		<< " Someone should probably fix that\n";
}

void	StringDataTracker::update(std::string landmark)
{
	size_t	p = StringHelp::find_ignore(file, pos, landmark, FIND_IGNORE_COMMENT);
	if (p != std::string::npos)
	{
		newLines += countNewLines(file, pos, p);
		trackQuotes(file, pos, p, isSingleQuote, isDoubleQuote);
		pos = p;
	}
	else
	{
		warning_landmark_not_found(newLines);
	}
	//std::cout << "  landmark '" << landmark << "' found on line [" << newLines << "]\n";
}
void	StringDataTracker::update(size_t move)
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
		warning_landmark_not_found(newLines);
	}
	//std::cout << "  landmark '" << landmark << "' found on line [" << newLines << "]\n";
}
