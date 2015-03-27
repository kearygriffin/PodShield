/*
 * NVRam.cpp
 *
 *  Created on: Sep 3, 2009
 *      Author: keary
 */

#include "podshield.h"

#include "NVRam.h"
#include "NVRamManager.h"


_NVRam::_NVRam(int siz, unsigned int modSig, int memSig, int memVer) {


	this->siz = siz;
	this->modSig = modSig;
	this->memSig = memSig;
	this->memVer = memVer;

	this->mem = NULL;

	getNVRamManager().addNVRamBlock(this);

}

void _NVRam::chk_init() {
	if (mem != NULL)
		return;


	memStart = getNVRamManager().allocate(siz, &mem, modSig, memSig, memVer);
	if (memStart < 0) {
		fatal_error("Can't allocate enough NVRAM");
	}

}

void  _NVRam::get(long pos, int siz, void *obj)
{
	chk_init();
	for (int i=0;i<siz;i++) {
		*(((unsigned char *)obj)+i) = mem->read(memStart+pos+i);
	}
}

void  _NVRam::put(long pos, int siz, void *obj)
{
	chk_init();
	for (int i=0;i<siz;i++) {
		 mem->write(memStart+pos+i, *(((unsigned char *)obj)+i));
	}
}

