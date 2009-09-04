/*
 * NVRamManager.cpp
 *
 *  Created on: Sep 3, 2009
 *      Author: keary
 */

#include "podshield.h"
#include "cppfix.h"
#include "NVRamManager.h"
#include "DiskMemory.h"
#include "EEMemory.h"

char *NVRamManager::nvramSig = NVRAM_SIGNATURE;

NVRamManager::NVRamManager() {
	intCnt = 0;
	sigCnt = 0;
	memInited = false;
}

void NVRamManager::init_memory(MemoryInterface *m) {
	long pos = 0;
	m->write(pos, (unsigned char *)nvramSig, strlen(nvramSig));
	pos += 4;

	while(pos < m->getMemorySize()) {
		m->write(pos++, 0xff);
	}
}
void NVRamManager::chk_init() {
	if (!memInited) {
		memInited = true;
		for (int i=0;i<intCnt;i++) {
			char sig[4];
			mem[i]->read(0, (unsigned char *)sig, sizeof(sig));
			if (strncmp(sig, nvramSig, strlen(nvramSig))) {
				init_memory(mem[i]);
			}
		}

		// Ok, now free all blocks we don't have sig's for
		for (int i=0;i<intCnt;i++) {
			long pos = strlen(nvramSig);
			while(pos < (mem[i]->getMemorySize()-MEMBLK_HEADER_SIZE)) {
				unsigned int s = mem[i]->read(pos) + (mem[i]->read(pos+1) << 8);
				if (s == FREE_BLOCK_ID)
					break;
				int mid = mem[i]->read(pos+2);
				int mver = mem[i]->read(pos+3);
				int blkSize = mem[i]->read(pos+4) + (mem[i]->read(pos+5) << 8);

				bool found = false;
				for (int j=0;j<sigCnt;j++) {
					if (sigBlocks[j]->modSig == s && sigBlocks[j]->memSig == mid && sigBlocks[j]->memVer == mver) {
						found = true;
						break;
					}
				}
				if (!found)
					free(mem[i], pos);
				else
					pos += (blkSize + MEMBLK_HEADER_SIZE);
			}
		}
		for (int i=0;i<sigCnt;i++)
			sigBlocks[i]->chk_init();

	}

}

long NVRamManager::_allocate(MemoryInterface *m, int siz, unsigned int modSig, int memSig, int ver) {
	long pos = strlen(nvramSig);
	while(pos < (m->getMemorySize()-MEMBLK_HEADER_SIZE)) {
		unsigned int s = m->read(pos) + (m->read(pos+1) << 8);
		if (s == FREE_BLOCK_ID) {
			long rem = m->getMemorySize() - (pos+MEMBLK_HEADER_SIZE);
			// We have a big enough chunk, allocate it
			if (rem >= siz) {
				m->write(pos, modSig&0xff);
				m->write(pos+1, modSig>>8);
				m->write(pos+2, memSig);
				m->write(pos+3, ver);
				int blkSize = siz;
				if (rem - blkSize < MEMBLK_HEADER_SIZE) {
					blkSize = rem;
				}
				m->write(pos+4, blkSize&0xff);
				m->write(pos+5, blkSize>>8);
				return pos + MEMBLK_HEADER_SIZE;
			}
			// We found the free stuff, but it's not big enough...
			break;
		}
	}
	return -1;
}

long NVRamManager::allocate(int siz, MemoryInterface **pmem, unsigned int modSig, int memSig, int ver) {
	chk_init();

	// Firs try to find it...
	for (int i=0;i<intCnt;i++) {
		long pos = strlen(nvramSig);
		while(pos < (mem[i]->getMemorySize()-MEMBLK_HEADER_SIZE)) {
			unsigned int s = mem[i]->read(pos) + (mem[i]->read(pos+1) << 8);
			if (s == FREE_BLOCK_ID)
				break;
			int mid = mem[i]->read(pos+2);
			int mver = mem[i]->read(pos+3);
			int blkSize = mem[i]->read(pos+4) + (mem[i]->read(pos+5) << 8);

			if (modSig == s && mid == memSig && mver == ver && blkSize >= siz) {
				*pmem = mem[i];
				return pos+MEMBLK_HEADER_SIZE;
			}
			pos += (blkSize + MEMBLK_HEADER_SIZE);
		}
	}



	int lastTriedPri = 999;
	int pri;
	do {
		pri = -1;
		for (int i=0;i<intCnt;i++) {
			MemoryInterface *m  = mem[i];
			if (m->getMemoryPriority() > pri && m->getMemoryPriority() < lastTriedPri)
				pri = m->getMemoryPriority();
		}

		if (pri >= 0) {
			lastTriedPri = pri;
			for (int i=0;i<intCnt;i++) {
				MemoryInterface *m  = mem[i];
				if (m->getMemoryPriority() == pri) {
					// attempt to allocate
					int pos = _allocate(m, siz, modSig, memSig, ver);
					if (pos >= 0) {
						// got it
						*pmem = m;
						return pos;
					}
				}
			}
		}
	} while(pri != -1);
	// Failed!
	return -1;
}
void NVRamManager::free(MemoryInterface *m, long pos) {
	// Collapse everyting
	int blkSize = m->read(pos+4) + (m->read(pos+5) << 8);
	long oldpos = pos + blkSize + MEMBLK_HEADER_SIZE;

	// Move everything down
	while(oldpos < m->getMemorySize()) {
		m->write(pos++, m->read(oldpos++));
	}

	// clear the new unused portion
	pos = m->getMemorySize() - (blkSize+MEMBLK_HEADER_SIZE);
	while(pos < m->getMemorySize()) {
		m->write(pos++, 0xff);
	}

}


void NVRamManager::add(MemoryInterface * m) {
	if (intCnt == MAX_MEMORY_INTERFACES)
		return;
	mem[intCnt++]  = m;
}


void NVRamManager::addNVRamBlock(_NVRam *nv) {
	sigBlocks[sigCnt++] = nv;
}

static int inited = 0;
NVRamManager& getNVRamManager() {
	static NVRamManager nvram;

	if (!inited) {
		inited = 1;
#ifdef	STANDALONE_PC
	static DiskMemory dm;
	nvram.add(&dm);
#else
#if (EEPROM_SIZE > 0)
	static EEMemory ee;
	nvram.add(&ee);
#endif
#if (NVRAM_SIZE > 0)
	static NVMemory nve;
	nvram.add(&nve);
#endif
#endif

	}

	return nvram;
}
