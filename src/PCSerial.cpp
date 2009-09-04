/*
 * PCPodSerial.cpp
 *
 *  Created on: Aug 17, 2009
 *      Author: keary
 */

#include "podshield.h"
#ifdef STANDALONE_PC
#include "PCSerial.h"
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
#include <time.h>
}

PCSerial::PCSerial(char *portName) {
	fd = -1;
	buffered = -1;
	this->portName = portName;
}

int PCSerial::open_port(void){

	int fd; // file description for the serial port

	if (portName == NULL)
		return 1;

	fd = C::open(portName, O_RDWR | O_NOCTTY | O_NONBLOCK);

	if(fd == -1) // if open is unsucessful

	{
		return -1;

	}

	else

	{

	 	C::fcntl(fd, F_SETFL, O_NONBLOCK|O_NDELAY);

	}

	return(fd);

}



/*Port configuration*/

int PCSerial::configure_port(int fd , long baud)      {

	struct C::termios port_settings;      // structure to store the port settings in

	if (fd == 1)
		return fd;

	C::speed_t st;
	switch(baud) {
	case	300:
		st = B300;
		break;
	case	1200:
		st = B1200;
		break;
	case	2400:
		st = B2400;
		break;
	case	9600:
		st = B9600;
		break;
	case	19200:
		st = B19200;
		break;
	case	38400:
		st = B38400;
		break;
	case	57600:
		st = B57600;
		break;
	case	115200:
		st = B115200;
		break;
	}

	C::tcgetattr(fd, &port_settings);

	C::cfsetispeed(&port_settings, st);    // set baud rates

	C::cfsetospeed(&port_settings, st);

	port_settings.c_cflag &= ~PARENB;    // set no parity, stop bits, data bits

	port_settings.c_cflag &= ~CSTOPB;

	port_settings.c_cflag &= ~CSIZE;

	port_settings.c_cflag |= CS8;

	port_settings.c_cflag |= (CLOCAL | CREAD);

	port_settings.c_cflag &= ~(CRTSCTS);

	port_settings.c_lflag &= ~(ICANON| ECHO | ECHOE | ISIG| INLCR);

	port_settings.c_iflag =0 ; //&= ~(IXON | IXOFF | IXANY);

	port_settings.c_oflag = 0;

	C::tcsetattr(fd, TCSANOW, &port_settings);    // apply the settings to the port

	return(fd);

}


void PCSerial::setBaud(long baud) {

	if (fd < 0) {
		fd = open_port();
		if (fd < 0) {
			printf("Err opening: %s", portName);
			//exit(1);
		}
	}
	configure_port(fd, baud);

}
uint8_t PCSerial::available() {
	if(fd <= 2)
		return 0;
	if (buffered >= 0)
		return 1;
	int cnt;
	unsigned char c;
	cnt = C::read(fd, &c, 1);
	if (cnt > 0) {
		buffered = c;
		return 1;
	}
	return 0;
}
int PCSerial::read(void) {
	int c;
	if (fd <= 2)
		return -1;
	if (buffered >= 0) {
		c = buffered;
		buffered = -1;
		return c;
	}
	int n = C::read(fd, &c, 1);
	if (n > 0) {
		//debug("PCIn: %02x");
		return c;
	}
	else
		return -1;
}
void PCSerial::flush(void) {
	buffered = -1;
	while (available())
		read();
}
void PCSerial::write(uint8_t c) {
	unsigned char byt = (unsigned char)c;
	C::write(fd, &c, 1);
}

void Print::write(uint8_t) {

}


#endif
