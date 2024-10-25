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
//	and will denote which one you're in at the end
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
		report_landmark_not_found(REPORT_WARNING);
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
		report_landmark_not_found(REPORT_WARNING);
	}
	//std::cout << "  landmark '" << landmark << "' found on line [" << newLines << "]\n";
}





void	StringDataTracker::report_start(char report)
{
	char	type = report & 0b11;
	if (type == REPORT_ERROR)
		std::cout << "error: ";
	else if (type == REPORT_WARNING)
		std::cout << "warning: ";
	else if (type == REPORT_INFO)
		std::cout << "info: ";
	else
		std::cout << "debug: ";

	if (report & REPORT_LINE)
		std::cout << "line:" << newLines << ": ";
}
void	StringDataTracker::report_end(char report)
{
	std::cout << ".";
	if (report & REPORT_ERRNO)
		std::cout << " errno: " << strerror(errno);
	std::cout << "\n";
}

void	StringDataTracker::report_generic(char report, std::string str)
{
	report_start(report);
	std::cout
		<< str;
	report_end(report);
}
void	StringDataTracker::report_already_set(char report, int line, std::string className, std::string memberName)
{
	report_start(report);
	std::cout
		<< className << "." << memberName
		<< " was already set at line:" << line;
	report_end(report);
}
void	StringDataTracker::report_landmark_not_found(char report)
{
	report_start(report);
	std::cout
		<< "a problem occured with the StringDataTracker, "
		<< "Line Numbers and Quote parsing may be inaccurate after this point"
		<< "(line:" << newLines << ")."
		<< " Someone should probably fix that";
	report_end(report);
}
void	StringDataTracker::report_args_number(char report, std::string type, int min, int max, int num)
{
	report_start(report);
	std::cout
		<< "type '" << type << "' got invalid number of arguments: "
		<< "expected between " << min << " and " << max 
		<< ", got " << num;
	report_end(report);
}
void	StringDataTracker::report_unknown_subtype(char report, std::string type, std::string subtype)
{
	report_start(report);
	std::cout
		<< "type '" << type << "' got unknown subtype '" << subtype << "'";
	report_end(report);
}
void	StringDataTracker::report_not_content(char report, std::string type)
{
	report_start(report);
	std::cout
		<< "type '" << type << "' did not get {Content} when expected";
	report_end(report);
}
void	StringDataTracker::report_got_content(char report, std::string type)
{
	report_start(report);
	std::cout
		<< "type '" << type << "' got {Content} when not expected";
	report_end(report);
}
void	StringDataTracker::report_extra_quotes(char report)
{
	report_start(report);
	std::cout
		//<< "an argument appears to be a concatenation of quoted regions, no quotes will be removed for this argument";
		<< "Some weird quote thing appear to be happening here, no quotes removed";
	report_end(report);
}
