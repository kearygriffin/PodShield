/*
 * ArduinoPodSerial.cpp
 *
 *  Created on: Aug 14, 2009
 *      Author: keary
 */

#include "podshield.h"
#ifndef STANDALONE_PC
#include "ArduinoSerial.h"

ArduinoSerial::ArduinoSerial(HardwareSerial *s) {
	hwSerial = s;
}

void ArduinoSerial::setBaud(long baud) {
	hwSerial->begin(baud);
}
uint8_t ArduinoSerial::available() {
	return hwSerial->available();
}

int ArduinoSerial::read(void) {
	return hwSerial->read();
}
void ArduinoSerial::flush(void) {
	hwSerial->flush();
}
void ArduinoSerial::write(uint8_t c) {
	hwSerial->write(c);
}


#endif
