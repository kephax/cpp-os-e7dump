/** @file here.cc
 * The implementation of the makeWhere function.
 * @version 4.1	2015/01/24
 */

// c++ hooks
#include <string>		// string
#include <sstream>		// ostringstream

// Make a "file[line] in func" line.
// If 'func' is zero we ommit the "in func" part.
std::string makeWhere( const char *file, int line, const char *func )
{
	std::ostringstream  s;
	s << file << '[' << line << "]";
	if (func)
		s << " in: " << func;
	return s.str();
}

// vim:sw=4:ai:aw:ts=4:
