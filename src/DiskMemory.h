/*
 * DiskMemory.h
 *
 *  Created on: Sep 3, 2009
 *      Author: keary
 */

#ifndef DISKMEMORY_H_
#define DISKMEMORY_H_

#include "MemoryInterface.h"

class DiskMemory: public MemoryInterface {
public:
	DiskMemory();
	virtual void write(long pos, unsigned char c);
	virtual unsigned char read(long pos);
	//virtual void write(long pos, unsigned char *buf, int len);
	//virtual void read(long pos, unsigned char *buf, int len);
	virtual long getMemorySize();
	virtual int getMemoryPriority();
protected:
	int eefd;
	void init_eeprom();
};

#endif /* DISKMEMORY_H_ */
