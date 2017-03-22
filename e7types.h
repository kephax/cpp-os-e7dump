#pragma once
#ifndef E7TYPES_H
#define E7TYPES_H 4.1

/* @(#)e7types.h	4.1 2015/01/24 */

/** @file
 * Supplies the platform dependent mappings from logical to physical types.
 * You may have to alter these if you have a different computer or compiler.
 * The comments tell the size expected.
 */

typedef unsigned char	byte;	/*!< 1 byte : general type */
typedef unsigned short	ushort;	/*!< 2 byte : general type */
typedef unsigned int	uint;	/*!< 4 byte : general type */
typedef unsigned long	ulong;	/*!< 4 byte : general type (oke if 8 bytes) */


// Normally (by convention) the names of logical types (e.g. size_t)
// end with _t. However in this assignment such names would clash with
// the logical types already defined for your native platform.
// For this reason we use our own _x names.
// Unfortunately for timestamps (time_t/time_x) this may cause problems
// when trying to using ctime(). In that case you may have to use a
// two-step approach:
// 1: assign the time_x value to some time_t variable
// 2: use ctime() to convert that time_t value to text.
typedef ushort	size_x;    	/*!< 2 byte : a plain size */
typedef ushort	ino_x;     	/*!< 2 byte : an i-node number */
typedef ushort	imode_x;	/*!< 2 byte : the type of the i_mode field */
typedef ushort	id_x;		/*!< 2 byte : an identification type (e.g. uid,gid) */
typedef ushort	dev_x;    	/*!< 2 byte : a major:minor tuple */
typedef int		off_x;    	/*!< 4 byte : an offset in a file */
typedef long	time_x;   	/*!< 4 byte : a timestamp in seconds since 1970 */
typedef int		daddr_x;  	/*!< 4 byte : a disk address ake blocknumber */
				// NOTE: In an inode_on_disk these daddr_x number
				//   are encoded as 24 bit quantities.
				// 	 Anywhere else they are 32 bit daddr_x entities.

#endif /*E7TYPES_H*/
