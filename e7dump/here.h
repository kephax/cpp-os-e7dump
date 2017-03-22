#pragma once
#ifndef HERE_H
#define HERE_H 4.1
/** @file here.h
 * Provides the \__HERE__ macro which produces a
 * "file[line] in function" std::string
 * that you can prepend to a message when reporting
 * some problem.
 *
 * @author R.A.Akkersdijk@saxion.nl
 * @version 4.1	2015/01/24
 */

#include <string>


/**
 * The \__HERE__ macro produces a "file[line] in function" string
 * that you can use in error messages.
 *
 * @note It relies on the \__FILE__, \__LINE__ and \__PRETTY_FUNCTION__
 *		compiler magic names to actually identify lines of source code.
 * <br>If your compiler does not provide the \__PRETTY_FUNCTION__
 *		magic name, you can \#define it as \__FUNCTION__ (gcc) or
 *		\__func__ (any compiler).
 * 		You can also \#define it as 0 in which case HERE will omit
 *		the name of the function.
 */
#ifndef __HERE__
#define	__HERE__ makeWhere(__FILE__,__LINE__,__PRETTY_FUNCTION__)
#endif


/** The actual function doing the dirty work for \__HERE__.
 * If func is a 0 pointer, that part of the message is omitted.
 */
std::string makeWhere( const char *file, int line, const char *func=0 );


// vim:sw=4:ts=4:ai:aw:
#endif // HERE_H
