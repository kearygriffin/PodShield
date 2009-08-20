/*
 * PCPodSerial.h
 *
 *  Created on: Aug 17, 2009
 *      Author: keary
 */

#ifndef PCPODSERIAL_H_
#define PCPODSERIAL_H_

#include "PodSerial.h"

class PCSerial: public PodSerial {
private:
	char *portName;
	int fd;
	int buffered;
public:
	PCSerial(char *portName);
	virtual void setBaud(long baud);
	virtual uint8_t available();
	virtual int read(void);
	virtual void flush(void);
	virtual void write(uint8_t c);
private:
	int open_port(void);
	int configure_port(int fd, long baud);
};

#endif /* PCPODSERIAL_H_ */
