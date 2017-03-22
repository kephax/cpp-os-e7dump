#pragma once
#ifndef E7FBLK_H
#define E7FBLK_H 1.2.2

/* @(#)e7fblk.h	1.2.2 */

/** @file
 * Definitions about the free block list.
 */

#include "e7types.h"
#include "e7filsys.h"

/** @struct fblk
 * The layout of (the first part of) a free list block.
 * It's contents coincide with the s_nfree/s_free part in the super block
 * so the OS can simply replace that part of the superblock
 * with the data from a free list block (and vice-versa).
 */
#pragma pack(1) // for MINGW
struct	fblk
{
	daddr_x		df_nfree;		    /*!< the number of entries used */
	daddr_x		df_free[NICFREE];	/*!< the free block "stack" */
} __attribute__((__packed__));	// Prevent padding-bytes between fields
#pragma pack() // for MINGW

#endif /*E7FBLK_H*/
