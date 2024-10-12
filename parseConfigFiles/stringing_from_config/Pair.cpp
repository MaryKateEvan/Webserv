
#include "../../includes/stringing_from_config/Pair.hpp"

Pair::Pair()
{
	p1 = std::string::npos;
	p2 = std::string::npos;
}

//	checks if both pair characters were found
bool	Pair::all_good() const
{
	return (p1 != std::string::npos && p2 != std::string::npos);
}
//	checks if eighter pair characters were found
bool	Pair::any_good() const
{
	return (p1 != std::string::npos || p2 != std::string::npos);
}

//	cuts the string inside the Pair including the pair characters
std::string	Pair::cut_in(std::string str) const
{
	return (StringHelp::cut(str, p1, p2 + 1));
}
//	cuts the string inside the Pair excluding the pair characters
std::string	Pair::cut_ex(std::string str) const
{
	return (StringHelp::cut(str, p1 + 1, p2));
}

//	looks for the outer most pair of characters denoted by c1 and c2
Pair	Pair::find(std::string str, size_t pos, char c1, char c2)
{
	Pair	pair;
	pos = StringHelp::find_ignore(str, pos, c1, FIND_IGNORE_QUOTE);
	if (pos == std::string::npos)
		return (pair);
	pair.p1 = pos;

	size_t	p1, p2;
	size_t	unpaired = 0;
	do
	{
		p1 = StringHelp::find_ignore(str, pos, c1, FIND_IGNORE_QUOTE);
		p2 = StringHelp::find_ignore(str, pos, c2, FIND_IGNORE_QUOTE);
		if (p1 == std::string::npos && p2 == std::string::npos)
		{
			return (pair);
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
			return (pair);
		}
	} while (unpaired != 0);
	pair.p2 = pos - 1;
	return (pair);
}