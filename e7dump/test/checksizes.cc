// include system headers
#include <cstdio>		// for: printf
// include project headers
#include "../ansi.h"
#include "../e7const.h"
#include "../e7dir.h"
#include "../e7fblk.h"
#include "../e7filsys.h"
#include "../e7ino.h"
#include "../e7types.h"
#include "../Block.h"


// Using the pre-processor to generate some code
#define	tellOK(type,expect)\
	printf(AC_GREEN "sizeof %s, is %u as expected" AA_RESET "\n", #type, expect);
#define	tellBAD(type,expect)\
	printf(AC_RED "sizeof %s, expected %u, but got %u" AA_RESET "\n", #type, expect, sizeof(type));
#define  Verify(type,expect) \
	if (sizeof(type) == (expect)) { \
		tellOK(type,expect); \
	} else { \
		tellBAD(type,expect); \
		++bad; \
	}
// End pre-processor defines


// Verify our code is indeed binary compatible
int	main()
{
	int	bad = 0;

	printf("Checking datatypesizes:\n");

	// basics
	Verify(size_x, 2);		// a size field
	Verify(ino_x, 2);		// an inode number
	Verify(imode_x, 2);		// the inode mode field
	Verify(id_x, 2);		// a user/group identifier
	Verify(dev_x, 2);		// a device major:minor code
	Verify(off_x, 4);		// a byte offset
	Verify(time_x, 4);		// a timestamp
	Verify(daddr_x, 4);		// a diskaddress

	// compounds
	Verify(filsys, 440);		// the data in the superblock
	Verify(fblk, 204);			// the data in a "freeblocklist" block
	Verify(dinode, INSIZ);		// an on-disk inode
	Verify(direct, DIRSIZ+2);	// a directory entry
	// a Block has DBLKSIZ data bytes + some extra stuff
	Verify(Block, DBLKSIZ+sizeof(int)+sizeof(size_t));

	if(bad > 0)
		printf("WARNING: First adjust the specs in e7types.h before proceeding\n");
	else
		printf("Sizes look good. Go ahead with your assignment.\n");
}
