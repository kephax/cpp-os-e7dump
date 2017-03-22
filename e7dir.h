#pragma once
#ifndef E7DIR_H
#define E7DIR_H 1.1

/* @(#)dir.h	1.1 */

/** @file
 * Definitions about directory entries.
 */

#include "e7types.h"

#define	DIRSIZ	(14)	/*!< max characters per filename entry */

/** @struct direct
 * The layout of a single directory entry as it appears on disk.
 * A diskblock contains an array of NDIRENT such entries.
 * @warning
 * When a filename is exactly DIRSIZ long, it will \b NOT be 0-terminated!
 */
#pragma pack(1) // for MINGW
struct	direct
{
	ino_x	d_ino;		    /*!< an inode number or 0 when the entry is not used */
	char	d_name[DIRSIZ];	/*!< the, '\0' padded, filename */
} __attribute__((__packed__));	// Prevents padding-bytes between fields
#pragma pack() // for MINGW

/** The number of directory entries per block */
#define	NDIRENT (DBLKSIZ/sizeof(direct))


#endif /*E7DIR_H*/
