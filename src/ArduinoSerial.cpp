/*
 * ArduinoPodSerial.cpp
 *
 *  Created on: Aug 14, 2009
 *      Author: keary
 */

#define HARDWARE_SERIAL_HACK

#ifdef	HARDWARE_SERIAL_HACK
#define _SKIP_WPROGRAM_H
#endif

#include "podshield.h"

#ifndef	STANDALONE_PC

#define protected public
#define private   public
#include <HardwareSerial.h>
#undef protected
#undef private

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

#ifdef	HARDWARE_SERIAL_HACK
bool ArduinoSerial::canSend() {
	if(!((*(hwSerial->_ucsra)) & (1 << hwSerial->_udre)))
		return false;
	return true;

}
#endif


#endif
