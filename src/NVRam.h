/*
 * NVRam.h
 *
 *  Created on: Sep 3, 2009
 *      Author: keary
 */

#ifndef NVRAM_H_
#define NVRAM_H_

#include "MemoryInterface.h"
//#include "NVRamManager.h"

class _NVRam {
public:

	friend class NVRamManager;
	void chk_init();

public:
	_NVRam(int siz, unsigned int modSig, int memSig, int memVer);
	void  get(long pos, int siz, void *obj);
	void  put(long pos, int siz, void *obj);


	MemoryInterface *mem;
	long memStart;
	int siz;
	unsigned int modSig;
	int memSig;
	int memVer;

};

template <class T>
class NVRam  {
public:
	NVRam(int cnt, unsigned int modSig, int memSig, int memVer) : _nvram(cnt*sizeof(T), modSig, memSig, memVer) { objCnt = cnt; }

	void get(int pos, T *obj) { _nvram.get(pos*sizeof(T), sizeof(T), (void *)obj); }
	void put(int pos, T *obj) { _nvram.put(pos*sizeof(T), sizeof(T), (void *)obj); }

protected:
	_NVRam _nvram;
	int objCnt;
};

#endif /* NVRAM_H_ */
