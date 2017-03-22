#pragma once
#ifndef BLOCK_H
#define BLOCK_H 5.1
/** @file Block.h
 * Definition of class Block.
 */

#include <string>	// for: std::string

// Unix 7th Edition definitions:
// The include files for the unix 7-th Edition Filesystem
#include "e7types.h"	// the basic types
#include "e7const.h"	// the fundamental constants
#include "e7filsys.h"	// the description of the information in the superblock
#include "e7fblk.h"		// a "free-block" administration description
#include "e7ino.h"	    // the description of the on-disk version of an inode
#include "e7dir.h"		// the description of a directory entry


/** @class Block
 * Represents a single datablock from the filesystem device.
 * It contains DBLKSIZ (=512) bytes of raw binairy data
 * which, thru the union mechanisme, can be interpreted
 * in different ways.
 */
class	Block	final	/* forbid inheritance */
{
	friend class Device;
	// Grants unlimited access rights to class Device.

private:

	// You are not allowed to create ...
	Block() : sentinel(0), usage_counter(0) {}

	// ... or delete Blocks yourself!
	~Block() {}

	// ... but class Device can do this because it is a friend.

public:

	/** The contents of this block are available in different views. */
	union {
		/** As a block with plain bytes */
		byte	data[DBLKSIZ];
		/** As an array of disk inodes */
		dinode	dino[INOPB];
		/** As an indirection block containing block numbers */
		daddr_x	bno[NINDIR];
		/** As a directory block containing directory entries */
		direct	dir[NDIRENT];
		// NOTE: The next two views are shorter than DBLKSIZ!
		/** As a superblock */
		filsys	fs;
		/** As a free-list block */
		fblk	fb;
	} u;

private:

	// Just in case somebody does a: cout << data
	int	sentinel;	// always 0 bytes

public:

	/** Get a sequence of at most 'count' ascii characters as a string.
	 * @param[in]	offset	the byte offset within this block where data starts
	 * @param[in]	count	the number of bytes
	 * @pre the data requested must be within the block
	 */
	std::string	getstring(off_x offset, unsigned count)	const;

	/** Copy a sequence of at most 'count' ascii characters to the given destination.
	 * @param[in]	dst		the destination address
	 * @param[in]	offset	the byte offset within this block where data starts
	 * @param[in]	count	the number of bytes
	 * @pre the data requested must be within the block
	 */
	void		getchars(char *dst, off_x offset, unsigned count)	const;

	/**
	 * A static function to convert 13, 3-byte, disk addresses to an array of type daddr_x.
	 * @param[out]	dst		the, i<b>caller provided</b> destination, an array of 13 daddr
	 * @param[in]	src		the array of 39 bytes to be converted
	 */
	static
	void		l3tol(daddr_x dst[NADDR], byte src[3*NADDR]);

protected:

	/** Block usage counter.
	 * Keeps track of how often this block is being used at the same time.
	 * Incremented by Device.getBlock() when you ask for a Block.
	 * Decremented by calling Block.release().
	 * Used by Device.getBlock() for a block replacement algoritm
	 * when we limit the size of the block cache.
	 */
	ulong	usage_counter;

public:

	/** Decrements the usage_counter.
	 * @pre The usage_counter must be > 0
	 */
	void	release();
};

#endif // BLOCK_H
