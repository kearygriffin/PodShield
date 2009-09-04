/*
 * DiskMemory.cpp
 *
 *  Created on: Sep 3, 2009
 *      Author: keary
 */

#include "podshield.h"

#ifdef STANDALONE_PC
#include "NVRamManager.h"
#include "DiskMemory.h"
namespace  C {
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <string.h>
}


DiskMemory::DiskMemory() {
	eefd = -1;
}

void DiskMemory::init_eeprom() {
    int i;
    if (eefd > 0)
            return;
    eefd = C::open("fake-eeprom.bin", O_RDWR);
    if (eefd < 0) {
            eefd = C::open("fake-eeprom.bin", O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);
            unsigned char c = 0xff;
            for (i=0;i<EEPROM_SIZE;i++)
                    C::write(eefd, &c, 1);
    }

}
void DiskMemory::write(long pos, unsigned char c) {
    init_eeprom();
    C::lseek(eefd, pos, SEEK_SET);
    C::write(eefd, &c, 1);
}
unsigned char DiskMemory::read(long pos) {
    unsigned char c;
    init_eeprom();
    C::lseek(eefd, pos, SEEK_SET);
    C::read(eefd, &c, 1);
    return c;

}

/*
void DiskMemory::read(long pos, unsigned char *buf, int len) {
	for (int i=0;i<0;i++) {
		*(buf+i) = read(pos+i);
	}
}

void DiskMemory::write(long pos, unsigned char *buf, int len) {
	for (int i=0;i<0;i++) {
		write(pos+i, *(buf+i));
	}
}
*/
long DiskMemory::getMemorySize() {
	return EEPROM_SIZE;
}
int DiskMemory::getMemoryPriority() {
	return MEM_PRIORITY_DEFAULT;
}

#endif
