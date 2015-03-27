/*
 * ArduinoPodSerial.h
 *
 *  Created on: Aug 14, 2009
 *      Author: keary
 */

#ifndef ARDUINOPODSERIAL_H_
#define ARDUINOPODSERIAL_H_


#include <HardwareSerial.h>
#include "PodSerial.h"

class ArduinoSerial: public PodSerial {
private:
	HardwareSerial *hwSerial;
public:
	ArduinoSerial(HardwareSerial *s);
	//virtual ~ArduinoSerial();
	virtual void setBaud(long baud);
	virtual uint8_t available();
	virtual int read(void);
	virtual void flush(void);
	virtual void write(uint8_t c);
	virtual bool canSend();

};

#endif /* ARDUINOPODSERIAL_H_ */
