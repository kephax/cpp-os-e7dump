using namespace std;
#include <string>
#include <sstream>	// for: ostringstream
#include "assert_error.h"


/** @file assert_error.cc
 * This file implements the assert_error exception class.
 *
 * @author R.A.Akkersdijk@saxion.nl
 * @version 4.1	2015/01/24
 */

static
// The assertion was not met, inform the user what is wrong.
string makeMessage3(const std::string& where,
							const char *type, const char *expr)
{
	ostringstream  ss;
	ss << where << "\n\t" << type;
	if (expr)
		ss << "(" << expr << ")";
	ss <<" failed";
	return ss.str();
}


/** @class assert_error
 * If an assertion fails an instance of assert_error is thrown.
 */
assert_error::assert_error(const std::string& where,
							const char *type, const char *expr)
	: logic_error( makeMessage3(where,type,expr) ) {}
