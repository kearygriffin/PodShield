/*
 * MemoryInterface.h
 *
 *  Created on: Sep 3, 2009
 *      Author: keary
 */

#ifndef MEMORYINTERFACE_H_
#define MEMORYINTERFACE_H_

#define MEM_PRIORITY_MIN	0
#define MEM_PRIORITY_LOW	10
#define MEM_PRIORITY_DEFAULT	20
#define MEM_PRIORITY_HIGH	30
#define	MEM_PRIORITY_MAX	40

class MemoryInterface {
public:
	virtual void write(long pos, unsigned char c) = 0;
	virtual unsigned char read(long pos) = 0;
	virtual void write(long pos, unsigned char *buf, int len) { while(len--) write(pos++, *(buf++)); }
	virtual void read(long pos, unsigned char *buf, int len) { while(len--) *(buf++) = read(pos++); }
	virtual long getMemorySize() = 0;
	virtual int getMemoryPriority() = 0;
};

#endif /* MEMORYINTERFACE_H_ */
