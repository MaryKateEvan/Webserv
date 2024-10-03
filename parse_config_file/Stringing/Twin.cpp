
#include "Twin.hpp"

Twin::Twin()
{
	p1 = std::string::npos;
	p2 = std::string::npos;
}

bool	Twin::all_good() const
{
	return (p1 != std::string::npos && p2 != std::string::npos);
}
bool	Twin::any_good() const
{
	return (p1 != std::string::npos || p2 != std::string::npos);
}

//	cuts the string inside the Twin including the twin characters
std::string	Twin::cut_in(std::string str) const
{
	return str.substr(p1, (p2 - p1) + 1);
}
//	cuts the string inside the Twin excluding the twin characters
std::string	Twin::cut_ex(std::string str) const
{
	return str.substr(p1 + 1, (p2 - p1) - 1);
}

/*
	search for c and i
	while i is close, look for next i
	if c is close, look for next c, done
*/
Twin	Twin::find(std::string str, size_t pos, char c, char i)
{
	Twin	twin;

	size_t	c1, i1;
	c1 = StringHelp::find_ignore(str, pos, c);
	i1 = StringHelp::find_ignore(str, pos, i);

	if (c1 == std::string::npos)
		return (twin);

	while (i1 < c1)
	{
		pos = StringHelp::find_ignore(str, i1 + 1, i);
		if (pos == std::string::npos)
			return (twin);
		pos++;
		c1 = StringHelp::find_ignore(str, pos, c);
		i1 = StringHelp::find_ignore(str, pos, i);
	}
	twin.p1 = c1;
	twin.p2 = StringHelp::find_ignore(str, c1 + 1, c);
	return (twin);
}
