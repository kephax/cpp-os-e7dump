#pragma once
#ifndef	ASSERTS_H
#define	ASSERTS_H 4.1
//#ident	"@(#)asserts.h	4.1	AKK	2015/01/24"
/** @file asserts.h
 * This file defines various macro's that can be used as checks in your program.
 * <br>If NDEBUG has been defined the assertions will be totally disabled for that
 * translation unit.
 * @note It relies on the \__HERE__ macro to tell where the problem occurred.
 *
 * @author R.A.Akkersdijk@saxion.nl
 * @version 4.1	2015/01/24
 */


// First remove these macro's
#undef	require
#undef	check
#undef	ensure
#undef	notreached


#ifdef	NDEBUG	/* disable asserts? */

// Make sure that another #include "asserts.h" can be used
// to enable assertions again later.
# undef ASSERTS_H

// Define some dummy macro's that have no side-effects
// and will be eliminated by the compiler
# define require(expr)	((void)(0))
# define check(expr)	((void)(0))
# define ensure(expr)	((void)(0))
# define notreached()	((void)(0))


#else /* asserts enabled */


#include "here.h"			// get the __HERE__ macro
#include "assert_error.h"	// and the assert_error class


/**
 * Verify whether a precondition is met, for instance: require(x>0);
 */
# define	require(condition) \
	if(!(condition)) \
		throw assert_error(__HERE__,"require",#condition)

/**
 * Verify an internal consistenty check, for instance: check(x>0);
 */
# define	check(condition) \
	if(!(condition)) \
		throw assert_error(__HERE__,"check",#condition)

/**
 * Verify whether a postcondition is met, for instance: ensure(x>0);
 */
# define	ensure(condition) \
	if(!(condition)) \
		throw assert_error(__HERE__,"ensure",#condition)

/**
 * Verify that this line of code is not reached. For instance
 * when the 'default' case of a 'switch' should never be reached
 * you can say:
 * 		default: notreached();
 */
# define	notreached() \
	throw assert_error(__HERE__,"notreached")

#endif	/* not NDEBUG */

// vim:sw=4:ai:aw:ts=4:
#endif	/*ASSERTS_H*/
