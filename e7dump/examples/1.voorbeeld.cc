/** @file 1.voorbeeld.cc
 *	Dump some information from a 7-th edition filesystem.
 */
#include "ansi.h"		// for: ANSI color codes
#include "asserts.h"	// for: asserts checking
#include "unix_error.h"	// for: system error reporting
#include "cstr.h"		// for: the cstr() wrapper and its output operator

// C++ hooks to C functions
#include <ctime>		// for: ctime()
	// Converts a unix timestamp (seconds since 1970)
	// into human readable text e.g. "Wed Jun 30 21:49:08 1993\n".
	// The total length will always be 25+1 characters.
#include <cstdio>	    // for: printf()
#include <cstdlib>		// for: EXIT_SUCCESS, EXIT_FAILURE

// C++/STL specific
#include <iostream>	    // for: std::cout, std::endl

// And our own classes
#include "Device.h"		// the "device driver"
#include "Block.h"		// the "datablocks"
// NOTE:
// The include files for the unix 7-th Edition Filesystem
// headers, e.g. e7types.h, are already included by Block.h

// ========================================================

// a few "forward declarations"
void	printSuperblock(Device&);
void	printRootinode(Device&);


// Dump some information from the given "device"
void	dump(const char* floppie)
{
	std::cout << "Opening device '" << floppie << "'" << std::endl;

	Device  device(floppie);

	printSuperblock(device);
	printRootinode(device);
}

void	printSuperblock(Device& device)
{
	// - - - - - - - - - - -
	// read SUPERBLOCK data
	// Also see: e7filsys.h
	// - - - - - - - - - - -

	// Fetch the block containing the super-block
	Block*  sp = device.getBlock(SUPERB);
	// We are going to use the u.fs view on it's contents

	// You can print the data with the printf function from C
	// which gives you fine control over the layout ...
	printf("printf     fs.s_isize=%d fs.s_fsize=%d\n",
			sp->u.fs.s_isize, sp->u.fs.s_fsize);
	// see also: man 3 printf
	// ... or use the iostream operators from C++
	std::cout << "std::cout  fs.s_isize=" << sp->u.fs.s_isize
			  << " fs.s_fsize=" << sp->u.fs.s_fsize << std::endl;
	// see also: boek/diktaat c++ etc.

	// The superblock-last-update timestamp can be
	// converted to human readable format with ctime()
	// (which always adds a newline character!) ...
	std::cout << "fs.s_time=" << ctime(&sp->u.fs.s_time);   // see: man 3 ctime
	// ... or use printf and limit the output to 24 chars
	printf("fs.s_time=%.24s\n", ctime(&sp->u.fs.s_time));	// see: man 3 ctime
	// Note: The time shown may differ from the given dump.txt
	//		 example because daylight savings time and timezones.
	//		 Internally all times are expressed in GMT.

	// Because on the actual filesystem "strings" are not
	// guarantied to be null-terminated, simply printing
	// them you may risk printing the entire memory until
	// we somewhere run into a 0-byte.
	// To handle this problem a special C-string wrapper class
	// with a suitable output operator is provided ...
	std::cout << "fs.s_fname=" << cstr(sp->u.fs.s_fname,6)
			  << ", fs.s_fpack=" << cstr(sp->u.fs.s_fpack,6) << std::endl;
	// .. or use printf again
	printf("fs.s_fname=%.6s, fs.s_fpack=%.6s\n",
			sp->u.fs.s_fname, sp->u.fs.s_fpack);

	sp->release();	// We no longer need the super block
}


void	printRootinode(Device& device)
{
	// - - - - - - - - - - - - -
	// read INODE's from disk
	// Also see: e7ino.h
	// - - - - - - - - - - - - -

	// Fetch the block containing the root inode
	Block*  ip = device.getBlock( itod(ROOTINO) );

	// Use a pointer as a shortcut to the wanted dinode in this block
	dinode*	dp = &ip->u.dino[ itoo(ROOTINO) ];

	// The inode type + protection flags.
	// If (di_mode == 0) then this inode is not used
	printf("inode %d, mode=%#o (expect 040777)\n", ROOTINO, dp->di_mode);
	std::cout << "inode " << ROOTINO << ", mode="
			  // switching between octal/decimal notation
			  << std::oct << dp->di_mode << std::dec
			  << " (expect 040777)" << std::endl;

	// Verify this is a directory inode
	// using a bit-mask test
	if((dp->di_mode & X_IFMT) == X_IFDIR) {
		printf(AC_GREEN	"Good: it is a directory\n"		AA_RESET);
	} else {
		printf(AC_RED	"Oops: it is not a directory\n"	AA_RESET);
	}

	// Convert the 13, 24-bits, blocknumbers in the inode
	// to normal 32-bit daddr_x values (only valid for DIR or REG type!)
	daddr_x  diskaddrs[NADDR];				// room for 13 blocknumbers
	Block::l3tol(diskaddrs, dp->di_addr);	// the convertion routine

	std::cout << "diskaddr: ";
	for(auto addr : diskaddrs) {			// A C++ "range-base-for-loop"
		printf(" %d", addr);
	}
	std::cout << std::endl;

	ip->release();	// We no longer need this inode block
}


// ====================

// Main is just the TUI
int  main(int argc, const char* argv[])
{
	// This adds a 0 or 0x prefix when printing in non decimal notation
	std::cout << std::showbase;

	try {
		// Pass a given parameter or use the default ?
		dump((argc > 1) ? argv[1] : "floppie.img");
		return EXIT_SUCCESS;
	} catch(const std::exception& e) {
		std::cerr << AC_RED "OOPS: " << e.what() << AA_RESET << std::endl;
		return EXIT_FAILURE;
	} catch(...) {
		std::cerr << AC_RED "OOPS: something went wrong" AA_RESET << std::endl;
		return EXIT_FAILURE;
	}
}


// vim:aw:ai:ts=4:
