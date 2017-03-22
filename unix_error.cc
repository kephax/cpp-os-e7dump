/** @file unix_error.cc
 * The implementation of the unix_error exception class.
 * @version 4.1	2015/01/24
 */

// linux & c++ hooks
#include <cstring>		// extern char *strerror(int errnum);
#include <cerrno>		// extern int errno;
#include <sstream>		// ostringstream

// our own stuff
#include "unix_error.h"	// class unix_error


static // (i.e. only visible in this source file)
// A function to combine the given argument with the unix error message
// that belongs with the given errornumber into a nice c++ string.
std::string	makeErrorString(const std::string& what, int errornumber)
{
	std::ostringstream  s;
	if (!what.empty()) {
		s << what << ": ";
	} else {
		s << "errno=" << errornumber << ": ";
	}
	s << strerror(errornumber);
	return  s.str();
}


// Construct a unix system error
unix_error::unix_error(const std::string& what)
	: std::runtime_error( makeErrorString(what, errno) ), uxerror(errno)
{
}

// Construct a unix system error
unix_error::unix_error(const char *what)
	: std::runtime_error( makeErrorString( std::string(what), errno) ), uxerror(errno)
{
}

// Construct a unix system error
unix_error::unix_error(const std::string& where, const char *what)
	: std::runtime_error( where + "\n\t" + makeErrorString( std::string(what), errno) ), uxerror(errno)
{
}

// Construct a unix system error
unix_error::unix_error(const std::string& where, const std::string& what)
	: std::runtime_error( where + "\n\t" + makeErrorString( what, errno) ), uxerror(errno)
{
}

// vim:sw=4:ai:aw:ts=4:
