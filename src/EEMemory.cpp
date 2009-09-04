/*
 * EEMemory.cpp
 *
 *  Created on: Sep 3, 2009
 *      Author: keary
 */

#include "podshield.h"

#ifndef STANDALONE_PC
#include "NVRamManager.h"
#include "EEMemory.h"
#include "EEPROM/EEPROM.h"


EEMemory::EEMemory() {

}

void EEMemory::write(long pos, unsigned char c) {
	EEPROM.write(pos, c);
}
unsigned char EEMemory::read(long pos) {
	return EEPROM.read(pos);
}

}


long EEMemory::getMemorySize() {
	return EEPROM_SIZE;
}
int EEMemory::getMemoryPriority() {
	return MEM_PRIORITY_LOW;
}

#endif
