#pragma once
#ifndef CSTR_H
#define CSTR_H 4.1
/** @file cstr.h
 * Provides the cstr utility class to print a non-null terminated C string in an ostream.
 *
 * @author R.A.Akkersdijk@saxion.nl
 * @version 4.1	2015/02/08
 */

#include <ostream>	// for: std::ostream

/** A utility class to print non-null terminated C strings on an ostream
*/
class cstr
{
	const char* cp;		// the actual C string
	unsigned	len;	// the maximum length to print
public:
	/** Construct a cstr object
	 * @param cp the actual C string
	 * @param len the maximum number of characters to print
	 * @code
	 * std::cout << cstr(somestring,len) << std::endl;
	 * @endcode
	 */
	cstr(const char cp[], unsigned len)
		: cp(cp), len(len) {}

	/*** The output operator for a cstr() */
	friend std::ostream& operator<<(std::ostream&, const cstr&);
};

#endif // CSTR_H
