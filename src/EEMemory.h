/*
 * EEMemory.h
 *
 *  Created on: Sep 3, 2009
 *      Author: keary
 */

#ifndef EEMEMORY_H_
#define EEMEMORY_H_

#include "MemoryInterface.h"

class EEMemory : public MemoryInterface {
public:
	EEMemory();
	virtual void write(long pos, unsigned char c);
	virtual unsigned char read(long pos);
	//virtual void write(long pos, unsigned char *buf, int len);
	//virtual void read(long pos, unsigned char *buf, int len);

	virtual long getMemorySize();
	virtual int getMemoryPriority();
};

#endif /* EEMEMORY_H_ */
