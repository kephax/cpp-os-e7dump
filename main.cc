// -----------------------------------------------------
/** @file main.cc
 * This is the program that you have to finish.
 */
// -----------------------------------------------------

// Our own includes
#include "ansi.h"		// for: ansi color code strings
#include "asserts.h"	// for: notreached() e.a.
#include "unix_error.h"	// for: the unix_error() exception class
#include "cstr.h"		// for: the cstr() wrapper and it's output operator

// C/C++/STL specific
#include <iostream>	    // for: std::cout, std::endl
#include <ctime>		// for: ctime()
#include <cstdlib>		// for: EXIT_SUCCESS, EXIT_FAILURE
#include <unistd.h>		// for: close() etc
#include <fcntl.h>		// for: O_WRONLY etc

// Our own classes
#include "Device.h"		// the "device driver"
#include "Block.h"		// the "data blocks"

#include <stddef.h>     // MYCODE - offsetof
#include <cmath>        // MYCODE
#include <string.h>     // MYCODE
#include <list>         // MYCODE
#include "swap_endian.h"// MYCODE
#include <sstream>      // MYCODE - addSpaces

// ================================================================

// TODO: write all the functions etc you need

/* //////////////////////////////////////////////////////////////////////////////////////////////////// */
enum BlkUseage
{
    B_UNKNOWN,                              // default status
    B_BOOT, B_SUPER, B_INODE,               // covers the first part of the medium
    B_DATA, B_INDIR1, B_INDIR2, B_INDIR3,   // known used blocks in the data area
    B_FREE                                  // known unused blocks in the data area
};
std::map<daddr_x,BlkUseage>  currentBlock;
std::list<int>  freeInodes;
std::list<int>::iterator freeInodesIterator = freeInodes.begin();



/* //////////////////////////////////////////////////////////////////////////////////////////////////// */
void    useBlock(daddr_x bno, BlkUseage use)
{
    if (currentBlock[bno] == B_UNKNOWN)
    {
        currentBlock[bno] = use;
    }
    else if (currentBlock[bno] != use)
    {
        // std::cerr <<( "\nUsage: %ld was %d, now %d?\n" , AA_RESET, bno, currentBlock[bno], use);
    }
}


/* //////////////////////////////////////////////////////////////////////////////////////////////////// */
/* /// Converts the mode to a readable string for the user /// */
std::string parseMode(imode_x mode)
{
    std::string parsedMode = "----------";
    parsedMode[0] = (mode & X_IFMT) == X_IFDIR ? 'd' : '-';

    parsedMode[1] = (mode & X_IUREAD)  ? 'r' : '-';
    parsedMode[2] = (mode & X_IUWRITE) ? 'w' : '-';
    parsedMode[3] = (mode & X_IUEXEC)  ? 'x' : '-';

    parsedMode[4] = (mode & X_IGREAD)  ? 'r' : '-';
    parsedMode[5] = (mode & X_IGWRITE) ? 'w' : '-';
    parsedMode[6] = (mode & X_IGEXEC)  ? 'x' : '-';

    parsedMode[7] = (mode & X_IOREAD)  ? 'r' : '-';
    parsedMode[8] = (mode & X_IOWRITE) ? 'w' : '-';
    parsedMode[9] = (mode & X_IOEXEC)  ? 'x' : '-';

    return parsedMode;
}

/* //////////////////////////////////////////////////////////////////////////////////////////////////// */
/* /// Adds spaces for nicer layout /// */
std::string addSpaces( int n )
{
    std::string str = "";
    std::stringstream str2;
    str2 << n;                                                                                  // Converts an int to a stringstream
    str = str2.str();                                                                           // Converts a stringstream to a string

    if( str.length() == 3 ) { str = " "   + str; }
    if( str.length() == 2 ) { str = "  "  + str; }
    if( str.length() == 1 ) { str = "   " + str; }

    return str;
}


/* //////////////////////////////////////////////////////////////////////////////////////////////////// */
/* /// Print the Inode informaiton for the Folder /// */
void dumpFolder( Device& device, dinode di, int x, direct dir, Block *b )
{
    int dipos = x*512;                                                //

    for( int i = 0; (i*16 < 512) && (i*16+dipos < di.di_size); i++ )
    {
        dir.d_ino = b->u.data[i * 16];

        char *dest = dir.d_name;
        off_x offset = i * 16 + 2;

        for (int  j = 0 ; (j < 14) ; ++j, ++dest, ++offset)
        {
            require( (0 <= offset) && (offset < DBLKSIZ) );                                     // Assert
            *dest = ((char)(b->u.data[offset]));                                                // Get the first chars from the offset and cast it to a char
        }

        // Display the list of files ("." = current directory, ".." = up directory)
        if( dir.d_ino > 0)
        {
            std::cout << "  " << addSpaces( dir.d_ino ) << " '" << dir.d_name << "'"         << std::endl;
        }
      }
      b->release();
}


/* //////////////////////////////////////////////////////////////////////////////////////////////////// */
/* /// Print the Inode informaiton /// */
void dumpInode(int inodenr, Device& device, std::string filename, std::string path, dinode di)
{
    try
    {
        Block*  ip = device.getBlock( itod(inodenr) );

        std::cout << std::endl;
        std::cout << "Reading inode " << inodenr << std::endl;
        std::cout << "mode="  << di.di_mode  << " (" << parseMode(di.di_mode) << ")" << std::endl;  // This uses the method defined above
        std::cout << "nlink=" << di.di_nlink <<
                     " uid="  << di.di_uid   <<
                     " gid="  << di.di_gid   << std::endl;
        std::cout << "atime=" << ctime(&di.di_ctime);
        std::cout << "mtime=" << ctime(&di.di_mtime);
        std::cout << "ctime=" << ctime(&di.di_atime);
        std::cout << "size="  << di.di_size  << " (max blocks=" << ceil(di.di_size / 512) + 1 << ")" << std::endl;

        std::cout << "addr: ";
        daddr_x  diskaddrs[NADDR];		                                                        // 13 blocknumbers
        ip->l3tol(diskaddrs, di.di_addr);
        for(int i=0; i < NADDR; i++)
        {
            daddr_x daddrx = diskaddrs[i];
            std::cout << " " << daddrx;
        }
        std::cout << std::endl;


        if((di.di_mode & X_IFMT) == X_IFDIR)
        {
            //ip->release();                                                                      // one block at a time in use

            std::cout << "Direct Block: ";
            for(int i=0; i < NADDR; i++)
            {
                daddr_x daddrx = diskaddrs[i];
                if(daddrx != 0)
                {
                    Block *bp =  device.getBlock(daddrx);
                    std::cout << daddrx << " ";
                    bp->release();
                }
            }
            std::cerr << std::endl;


            /* ////////////////////////////////////////////////////////////////////////////  */
            std::cerr << "Contents of directory:" << std::endl;

            /* DUMP FOLDER */
            for(int i = 0; i <= (di.di_size - di.di_size % 512)/512; i++)
            {
                Block *b = device.getBlock(diskaddrs[i]);
                direct dir;
                dumpFolder( device, di, i, dir, b);                                             // Toon de inhoud
            }
            /* DUMP FOLDER */
            /* ////////////////////////////////////////////////////////////////////////////  */
        }
        else
        {
            // Not a directroy
            ip->release();                                                                      // one block at a time in use
        }
    }
    catch(...)
    {
        std::cerr << "ERROR: dumpInode - Catch" << std::endl;
    }
}


/* //////////////////////////////////////////////////////////////////////////////////////////////////// */
/* /// Show the Super Block Parameters (see documentation) /// */
void showSuperBlockParams(Device& device)
{
    Block  *sp = device.getBlock(SUPERB);
    useBlock(SUPERB, B_SUPER);

    std::cout << "---------------------------------------" << std::endl;
    std::cout << "Dump of superblock on " << sp->u.fs.s_fname << "." << sp->u.fs.s_fpack  << std::endl;
    std::cout << "---------------------------------------" << std::endl;
    std::cout << "userdata area starts in block: "         << sp->u.fs.s_isize << std::endl;
    std::cout << "number of blocks on volume is: "         << sp->u.fs.s_fsize << std::endl;
    std::cout << "number of freeblocks is: "               << sp->u.fs.s_nfree << std::endl;

    for (int  i = 0; i < sp->u.fs.s_nfree; ++i)
    {
        std::cout << "" << sp->u.fs.s_free[i] << " ";
    }

    std::cout << std::endl;
    std::cout << "number of free inodes         : "        << sp->u.fs.s_ninode << std::endl;

    for (int  i = 0; i < sp->u.fs.s_ninode; ++i)
    {
        std::cout << "" << sp->u.fs.s_inode[i] << ", ";
        freeInodes.insert(freeInodesIterator,sp->u.fs.s_inode[i]);
    }

    std::cout << std::endl;
    std::cout << "freelist lock flag            : "        << sp->u.fs.s_flock  << std::endl;
    std::cout << "read only flag                : "        << sp->u.fs.s_ronly  << std::endl;

    std::cout << "last update time = "                     << ctime(&sp->u.fs.s_time) << std::endl;

    std::cout << "total number of free blocks is "         << sp->u.fs.s_tfree  << std::endl;
    std::cout << "total number of free inodes is "         << sp->u.fs.s_tinode << std::endl;

    std::cout << "Interleave factors are: m="              << sp->u.fs.s_m      <<
                                        " n="              << sp->u.fs.s_n      << std::endl;
    std::cout << "File system name="                       << sp->u.fs.s_fname  << std::endl;
    std::cout << "File system pack= "                      << sp->u.fs.s_fpack  << std::endl;

    // print all the free blocks
    std::cout << "---------------------------------------" << std::endl;
    std::cout << "Rest of free list continues in "         << sp->u.fs.s_free[0] << std::endl;
    std::cout << "---------------------------------------" << std::endl;

    daddr_x addr = sp->u.fs.s_free[0];
    std::cout << "Freeblock "                              << addr               << std::endl;

    Block  *bp = device.getBlock(addr);
    off_x  offset = 0;
    daddr_x lastdaddr = bp->u.fs.s_free[0];
    while (lastdaddr != NULL)
    {
        for (int  i = 0; i < NICFREE; ++i)
        {
            daddr_x addrsub = bp->u.fs.s_free[i -1];
            std::cout  << " " << addrsub;
            offset += sizeof(daddr_x);
            useBlock(addrsub, B_FREE);
        }
        std::cout << std::endl << "Freeblock " << bp->u.fs.s_free[-1] << std::endl;
        daddr_x firstdaddrofblock = bp->u.fs.s_free[-1];
        bp = device.getBlock(firstdaddrofblock);

        if(bp != NULL)
        {
            lastdaddr = firstdaddrofblock;
        }
    }
    std::cout << std::endl;

    bp->release();
    std::cout << "Holds " << NICFREE << " entries: " << std::endl;
    std::cout << "---------------------------------------" << std::endl;

    sp->release();
}


/* //////////////////////////////////////////////////////////////////////////////////////////////////// */
/* /// Show Attributes of Inodes /// */
void showInodesAttributes(Device& device)
{
    int nHighestInode = freeInodes.front();

    std::cout << "---------------------------------------" << std::endl;
    std::cout << "Reading " << nHighestInode << " inodes"  << std::endl;
    std::cout << "---------------------------------------" << std::endl;


    std::list<int>  usedInodes;
    std::list<int>::iterator usedInodesIterator = usedInodes.begin();

    while(nHighestInode > 0)
    {
        // Fill list variable with used inodes, get all the used nodes by going from the highest to 0.
        bool isused = true;
        for (freeInodesIterator=freeInodes.begin(); freeInodesIterator!=freeInodes.end(); ++freeInodesIterator)
        {
            int nFreeInode = *freeInodesIterator;
            if(nHighestInode == nFreeInode)
            {
                isused = false;
            }
        }
        if (isused)
        {
            usedInodes.insert(usedInodesIterator, nHighestInode);
        }

        --nHighestInode;                                                // Verminder steeds zodat we dicht bij 0 komen
    }

    // Print the information for each iNode
    for (usedInodesIterator=usedInodes.begin(); usedInodesIterator!=usedInodes.end(); ++usedInodesIterator)
    {
        int nFreeInode = *usedInodesIterator;

        Block*  ip = device.getBlock( itod(nFreeInode) );
        dinode di = ip->u.dino[ itoo(nFreeInode) ];
        dumpInode(nFreeInode, device, "name", "", di );                 // Haal gedetaileerde informatie voor de inode
        ip->release();
    }
    std::cout << "---------------------------------------" << std::endl;
}



/* //////////////////////////////////////////////////////////////////////////////////////////////////// */
void	dump( const char * floppie )
{
    std::cerr << "Opening device '" << floppie <<"'\n";
    Device  device(floppie);

    showSuperBlockParams(device);
    showInodesAttributes(device);
}


// ================================================================

// Main is just the TUI
int  main(int argc, const char* argv[])
{
	try {
		// To get the output into file 'log.txt', change the 0 below into a 1
#if	0
		std::cerr << "Sending output to log.txt" << std::endl;
		close(1);
		if( open("log.txt", O_WRONLY|O_TRUNC|O_CREAT, 0666) < 0)
			throw unix_error("log.txt");
#endif
		// Magic to add a 0 or 0x prefix when printing in non-decimal notation
		std::cout << std::showbase;
		// Pass a given parameter or use the default ?
		dump((argc > 1) ? argv[1] : "floppie.img");
		return EXIT_SUCCESS;
	} catch(const unix_error& e) {
		std::cerr << AC_RED "SYSTEM: " << e.what() << AA_RESET << std::endl;
		return EXIT_FAILURE;
	} catch(const std::exception& e) {
		std::cerr << AC_RED "OOPS: " << e.what() << AA_RESET << std::endl;
		return EXIT_FAILURE;
	} catch(...) {
		std::cerr << AC_RED "OOPS: something went wrong" AA_RESET << std::endl;
		return EXIT_FAILURE;
	}
}


// vim:aw:ai:ts=4:

