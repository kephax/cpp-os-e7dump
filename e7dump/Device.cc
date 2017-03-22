/** @file
 * Implementation of the Device class.
 */

#include "asserts.h"		// for asserts checking
#include "unix_error.h"		// for throwing unix errors

#include <fcntl.h>			// for: O_RDONLY, open(), lseek(), etc
#include <unistd.h>			// for: ???
#include <sys/stat.h>       // for: struct stat, lstat(), S_ISREG()
#include <cstdio>			// for: printf

#include "Block.h"
#include "Device.h"
#include "e7const.h"		// for: DBLKSIZ


// Open a "device" (i.e. a diskimage file)
Device::Device(const char *file, size_t climit)
	: dev_limit(0)
	, cache_limit(climit)
	, requests(0)
{
	require(file	!= 0);		// not null pointer
	require(file[0] != 0);		// not empty string

	// Open the image file
#ifdef	O_BINARY		// Compiling on Win32?
	fd = open(file, O_RDONLY|O_BINARY);	// For Win32: hands of my binary data!
#else
	fd = open(file, O_RDONLY);
#endif
	if (fd < 0) {						// could not open?
		throw unix_error(__HERE__, file);	// report the problem
	}

	// Inspect the file attributes
	struct stat  buf;
	if (fstat(fd, &buf) < 0) {			// request the file description
		throw unix_error(__HERE__, "fstat");		// oops!
	}
	if (S_ISREG(buf.st_mode)) {			// is it a regular file?
		dev_limit = buf.st_size;		// then the size is known (in bytes)
	} else {
		dev_limit = 0;					// means "infinite"
	}

	// A more cumbersome way to do it
	//dev_limit = lseek(fd, 0L, SEEK_END);		// determine the device size then
	//(void) lseek(fd, 0L, SEEK_SET);			// rewind back to start of medium
}


// Set the new upper limit to the number of blocks on this device
void	Device::setLimit(daddr_x  dlimit)
{
	if (dev_limit != 0)					// not "infinite" ?
		require(dlimit <= dev_limit);	// sanity check
	dev_limit = dlimit * DBLKSIZ;		// convert to bytes
}


// Read a datablock from the device
Block	*Device::getBlock(daddr_x  blocknumber)
{
	++requests;							// update usage statistics

	// Do we already have that block in the cache?
	BlockMap::iterator  i = cache.find(blocknumber);
	if (i != cache.end()) {		// yes,
		++(i->second->usage_counter);	// increment it's usage count
		return i->second;				// then return the block
	}

	// No, we have to read the block from the disk
	daddr_x  blockpos = blocknumber * DBLKSIZ;		// convert blocknumber -> byte offset
	check(0 <= blockpos);				// sanity check: within the device ?
	if (dev_limit > 0)
		check(blockpos < dev_limit);	// sanity check: within the device ?

	// Go to the right place
	if (lseek(fd, blockpos, SEEK_SET) != blockpos) {
		throw unix_error(__HERE__, "seeking block");
	}

	// Get that block in core
	Block  *bp = new Block();						// create a new, empty, block
	size_t  n = read(fd, bp->u.data, DBLKSIZ);		// and fill it's data[] from the file
	if (n != DBLKSIZ) {
		throw unix_error(__HERE__, "block size");
	}

	// Can we still cache it?
	if ((cache_limit > 0) && (cache.size() >= cache_limit))
	{
		// TODO:
		// Verify that the cache still has enough room for this new Block.
		// Iff not then use the Block.usage_counter to discard unused blocks from the cache.
		// Consult the sheets about "Page Replacement Algorithms" for possible tactics.
	}

	// Add the new Block to the cache
	cache[blocknumber] = bp;

	// And increment it's usage count
	bp->usage_counter++;

	// Then return it to the caller
	return bp;
}


Device::~Device()
{
 	close(fd);

 	if (requests == 0)	// anything to report?
		return;			// no

	// Calculate a statistics summary
	ulong  hits = requests - cache.size();			// i.e. the cache hits
 	printf("Device: needed %d reads for %lu requests (%ld%% cache hitrate)\n",
					cache.size(), requests, ((100 * hits) / (requests)));

	// Do we have to cleanup any cached blocks?
	if (!cache.empty()) {
		size_t  in_use = 0;
		for (auto p : cache) {
			Block *bp = p.second;
			if (bp->usage_counter > 0)
				++in_use;
			delete bp;
		}
		cache.clear();
		if (in_use > 0)
			printf("NOTE: There were still %u blocks in use!\n", in_use);
	}
}
