/** @file
 * Implementation of class Block.
 */
#include "asserts.h"	// asserts checking

// Determine the byte-order of this platform.
//
// Normaly this would be specified in an <endian.h> system
// include file, but when your platform does not provide this
// file you can find a replacement endian.h file in the
// 'diversen' directory (copy-paste as needed).
// If you have to use that extra 'endian.h' file,
// don't forget to tell CodeBlocks it is used
// by your program.
#ifndef	__BYTE_ORDER__
#  include "endian.h"
#endif
#ifndef	__BYTE_ORDER__
# error	Oops, the byte order of your machine is undefined
#endif


#include "Block.h"


// get a sequence of 'count' ascii characters as a string starting at the given offset
std::string		Block::getstring(off_x offset, unsigned count) const
{
	require(0<=offset);
	require((offset+count)<=DBLKSIZ);
	std::string	 s;
	for (unsigned  i = 0 ; (i < count) && (u.data[offset] != 0) ; ++i, ++offset) {
		s += ((char)(u.data[offset]));
	}
	return s;
}


// copy a sequence of 'count' ascii characters starting at the given offset
void		Block::getchars(char *dst, off_x offset, unsigned count) const
{
	require(0<=offset);
	require((offset+count)<=DBLKSIZ);
	for (unsigned  i = 0 ; (i < count) ; ++i, ++dst, ++offset) {
		*dst = ((char)(u.data[offset]));
	}
}


/*
 * Converts 3-byte disk addresses to array of daddr.
 *	dst = an array of 13 daddr, the destination
 *	src = an array of 3*13 bytes, the source
 */
void	Block::l3tol(daddr_x dst[NADDR], byte src[3*NADDR])
{
	byte  *a = (byte *) dst;
	byte  *b = (byte *) src;
	for (int  i = 0 ; i < NADDR ; ++i) {
#if	__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
		/* little-endian => little-endian (PC) */
		a[3] = 0;
		a[2] = b[2];	// least
		a[1] = b[1];
		a[0] = b[0];	// most
		a += 4;
		b += 3;
#else
		/* big-endian => big-endian */		// TODO
		*a++ = 0;
		*a++ = *b++;	// least
		*a++ = *b++;
		*a++ = *b++;	// most
#endif
	}
}


// Decrement the usage counter
void	Block::release()
{
	require(usage_counter > 0);
	--usage_counter;
}

