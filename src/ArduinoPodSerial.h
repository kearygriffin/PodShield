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

class ArduinoPodSerial: public PodSerial {
public:
	ArduinoPodSerial();
	virtual ~ArduinoPodSerial();
};

#endif /* ARDUINOPODSERIAL_H_ */
