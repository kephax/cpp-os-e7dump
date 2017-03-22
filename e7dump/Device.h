#pragma once
#ifndef DEVICE_H
#define DEVICE_H 2.1
/** @file
 * Definition of the Device class which acts as a device driver.
 */

#include <map>		// for: std::map<K,V>
#include "e7types.h"

/** @class Device
 * This class handles the interaction with the floppy
 * and maintains the Block cache.
 * It is the equivalent of a "disk-driver".
 */
class Device	final	/* forbid inheritance */
{
	typedef								// create a shorthand
		std::map<daddr_x, class Block*>	// for map<...>
			BlockMap;					// called BlockMap

public:

	/** Open a "filesystemdevice".
	 * @param[in]	file	The name of the image file.
	 * @param[in]	climit	The upper limit to the cache size (default=infinite)
	 * */
	Device(const char *file, size_t climit=0 );

	/** Set the device limit on blocknumbers.
	 *  By default this "device driver" figures this out itself,
	 *  but this method allows you to set it by hand.
	 *  Setting it to 0 effectively sets it to infinite.
	 *  @param[in]	dlimit	The total number of blocks on this device.
	 *  @pre	The new limit must be equal or less to what the driver
	 * 			already knows.
	 */
	void	setLimit(daddr_x dlimit);

	/** Fetch a disk block from the device.
	 *  Blocks retrieved are kept in an internal blockcache
	 *  for faster retrieval when read again.
	 *  @param[in]	blocknumber	The number of the block to read.
	 *  @pre	The blocknumber requested must be within the device limit.
	 */
	Block	*getBlock(daddr_x blocknumber);

	/** Reports usage statistics then deletes all blocks in the cache
	 */
	~Device();

private:

	int			fd;				// the handle to the unix filedescriptor
	daddr_x		dev_limit;		// the upperlimit to the device size (or 0)

	size_t		cache_limit;	// the upper limit to the cache size (or 0)
	BlockMap	cache;			// the cache of recently read blocks

	// To gather statistics data (add more as desired)
	ulong		requests;		// getBlock request counter
};

#endif // DEVICE_H

