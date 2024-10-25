#pragma once
#include <iostream>
#include <cstring>
#include "StringHelp.hpp"

#define	REPORT_DEBUG   0b011
#define	REPORT_INFO    0b010
#define	REPORT_WARNING 0b001
#define	REPORT_ERROR   0b000
#define REPORT_LINE    0b0100
#define REPORT_ERRNO   0b1000

class	StringDataTracker
{
	private:
		std::string	file;
		size_t		pos;
	public:
		size_t		newLines;
		bool		isSingleQuote;
		bool		isDoubleQuote;

	public:
		StringDataTracker();
		StringDataTracker(std::string file);
		~StringDataTracker();

	public:
		static int	countNewLines(std::string str, size_t from, size_t to);
		static void	trackQuotes(std::string str, size_t from, size_t to, bool & isSQ, bool & isDQ);
		void	update(std::string landmark);
		void	update(size_t move);

	private:
		void	report_start(char report);
		void	report_end(char report);
	public:
		void	report_landmark_not_found(char report);
		void	report_generic(char report, std::string str);
		void	report_already_set(char report, int line, std::string className, std::string memberName);
		void	report_args_number(char report, std::string type, int min, int max, int num);
		void	report_unknown_subtype(char report, std::string type, std::string subtype);
		void	report_not_content(char report, std::string type);
		void	report_got_content(char report, std::string type);
		void	report_extra_quotes(char report);
};
