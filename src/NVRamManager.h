/*
 * NVRamManager.h
 *
 *  Created on: Sep 3, 2009
 *      Author: keary
 */

#ifndef NVRAMMANAGER_H_
#define NVRAMMANAGER_H_

#include "MemoryInterface.h"
#include "NVRam.h"

#define NVRAM_SIGNATURE	"KG10";

#define MAX_NVRAM_BLOCKS	16

#define MAX_MEMORY_INTERFACES	3

#define	MEMBLK_HEADER_SIZE	6

#define FREE_BLOCK_ID 0xffff


class NVRamManager {
public:
	NVRamManager();
	void add(MemoryInterface* m);
	long allocate(int siz, MemoryInterface **mem, unsigned int modSig, int memSig, int ver);

	void addNVRamBlock(_NVRam *nv);
	void chk_init();

protected:
	void init_memory(MemoryInterface *m);
	void free(MemoryInterface *m, long pos);
	long _allocate(MemoryInterface *m, int siz, unsigned int modSig, int memSig, int ver);

	MemoryInterface *mem[MAX_MEMORY_INTERFACES];
	bool memInited;
	int intCnt;
	static char *nvramSig;
	int sigCnt;
	_NVRam *sigBlocks[MAX_NVRAM_BLOCKS];
};

extern NVRamManager& getNVRamManager();

#endif /* NVRAMMANAGER_H_ */
