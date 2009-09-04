/*
 * PodSerial.h
 *
 *  Created on: Aug 14, 2009
 *      Author: keary
 */

#ifndef PODSERIAL_H_
#define PODSERIAL_H_

#include <Print.h>
#include <inttypes.h>

class PodSerial : public Print {
public:
	virtual void setBaud(long baud) = 0;
	virtual uint8_t available() = 0;
	virtual int read(void) = 0;
	virtual void flush(void) = 0;
	virtual void write(uint8_t c) = 0;
	virtual bool canSend() { return true; };
};

#endif /* PODSERIAL_H_ */
