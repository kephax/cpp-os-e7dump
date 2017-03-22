/** @file cstr.cc
 * The implementation of the cstr output operator.
 * @version 1.1	2015/02/08
 */
 #include "cstr.h"

std::ostream& operator<<(std::ostream& os, const cstr& c)
{
	register const char*	cp = c.cp;
	register unsigned		len = c.len;
	// while not maxlen reached and non null character
	for( ; (len>0) && (*cp) ; --len, ++cp) {
		os << *cp;
	}
	return os;
}
