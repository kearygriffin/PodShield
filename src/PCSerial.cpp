/*
 * PCPodSerial.cpp
 *
 *  Created on: Aug 17, 2009
 *      Author: keary
 */

#ifdef STANDALONE_PC
#include "PCSerial.h"

PCSerial::PCSerial(char *portName) {
	fd = -1;
	buffered = -1;
	this->portName = portName;
}

int PCSerial::open_port(void){

	int fd; // file description for the serial port

	if (portName == NULL)
		return 1;

	fd = open(portName, O_RDWR | O_NOCTTY | O_NONBLOCK);

	if(fd == -1) // if open is unsucessful

	{
		return -1;

	}

	else

	{

	 	fcntl(fd, F_SETFL, O_NONBLOCK|O_NDELAY);

	}

	return(fd);

}



/*Port configuration*/

int PCSerial::configure_port(int fd , long baud)      {

	struct termios port_settings;      // structure to store the port settings in

	if (fd == 1)
		return fd;

	speed_t st;
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

	tcgetattr(fd, &port_settings);

	cfsetispeed(&port_settings, st);    // set baud rates

	cfsetospeed(&port_settings, st);

	port_settings.c_cflag &= ~PARENB;    // set no parity, stop bits, data bits

	port_settings.c_cflag &= ~CSTOPB;

	port_settings.c_cflag &= ~CSIZE;

	port_settings.c_cflag |= CS8;

	port_settings.c_cflag |= (CLOCAL | CREAD);

	port_settings.c_cflag &= ~(CRTSCTS);

	port_settings.c_lflag &= ~(ICANON| ECHO | ECHOE | ISIG| INLCR);

	port_settings.c_iflag =0 ; //&= ~(IXON | IXOFF | IXANY);

	port_settings.c_oflag = 0;

	tcsetattr(fd, TCSANOW, &port_settings);    // apply the settings to the port

	return(fd);

}


void PCSerial::setBaud(long baud) {

	if (fd < 0) {
		fd = open_port();
		if (fd < 0) {
			printf("Err opening: %s", portName);
			exit(1);
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
	cnt = read(fd, &c, 1);
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
	int n = read(fd, &c, 1);
	if (n > 0)
		return c;
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
	write(fd, &c, 1);
}


#endif
