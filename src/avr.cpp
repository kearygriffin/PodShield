#include "podshield.h"

#ifndef STANDALONE_PC

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "EEPROM/EEPROM.h"

#define HEADEND_BAUDRATE	57600
//#define HEADEND_BAUDRATE	19200
#define	IPOD_BAUDRATE		HEADEND_BAUDRATE
#define XM_RADIO_BAUDRATE	9600
#define DEBUG_BAUDRATE		57600

//#define	USE_VDIP1
#define	HEADEND_RESET_DELAY	2500

extern  int isHeadendConnected() {
	return (digitalRead(33) == 0);
}
extern  void xmlog(char *fmt, ...) {
		if (DebugSerial != NULL) {
			char buf[80];
			va_list args;
			va_start(args, fmt);
			vsprintf(buf, fmt, args);
			DebugSerial->println(buf);
			va_end(args);
		}
}
extern void xmlog_n(char *fmt, ...) {
		if (DebugSerial != NULL) {
			char buf[80];
			va_list args;
			va_start(args, fmt);
			vsprintf(buf, fmt, args);
			DebugSerial->print(buf);
			va_end(args);
		}
}

extern void xmlog_init() {
	DebugSerial->setBaud(DEBUG_BAUDRATE);
}

int ctsPin = 23;
int rtsPin = 25;
int dtr_dataAckPin = 53;
int dsr_dataReqPin = 51;

void out_xm(int c) {
	//xmlog("outXm: %02x", c);
	SerialAux1->write(c);
}

#ifdef	USE_VDIP1
void out_vdip(int c) {
  while(digitalRead(rtsPin) == HIGH)
    ;
  out_xm(c);
}

int vdip_result(unsigned char *buf, int cnt) {
  int pos = 0;
  unsigned long timeout;
   timeout = millis() + 1500;
   while(millis() < timeout && cnt > pos) {
     if (SerialAux1->available()) {
       buf[pos++] = SerialAux1->read();
     }
   }
   //Serial.print("vdipResCnt: ");
   //Serial.println(pos);
   return pos;
}

void vdip_connect() {
  int inited = 0;
  int i,cnt, dev;
  unsigned int vid, pid;
  unsigned char buf[34];

  while(!inited) {
    //Serial.print("Reinit\n\r");
    digitalWrite(dsr_dataReqPin, HIGH);
    while(digitalRead(dtr_dataAckPin) != HIGH)
      ;
    //Serial.print("command mode confirmed\n\r");

    out_vdip(0x10);
    out_vdip(0x0d);
    cnt = vdip_result(buf, 2);
    out_vdip(0x91);
    out_vdip(0x0d);
    cnt = vdip_result(buf, 2);
    delay(250);
    SerialAux1->flush();

    for (i=0;i<16;i++) {
      out_vdip(0x85);
      out_vdip(0x20);
      out_vdip(i);
       out_vdip(0x0d);
      cnt = vdip_result(buf, 34);
      if (cnt != 34)
        continue;
      pid = (unsigned int)buf[16]  + (unsigned int)buf[17] * 256U;
      vid = (unsigned int)buf[14] + (unsigned int)buf[15] * 256U;


      //Serial.print(i);
      //Serial.print(": ");
//      Serial.print(pid);
      //Serial.print(", ");
      //Serial.print(pid);
      //Serial.print("\r\n");

      if (vid == 0x0403 && pid == 0xca82) {
        //Serial.print("Found!\r\n");
        inited = 1;
        dev = i;
        break;
      }
    }

  }

  // We found the device.  Set it up.

  // Select device
  out_vdip(0x86);
  out_vdip(0x20);
  out_vdip(dev);
  out_vdip(0x0d);
  cnt = vdip_result(buf, 2);

  // Force ftdi
  out_vdip(0x87);
  out_vdip(0x20);
  out_vdip(dev);
  out_vdip(0x0d);
  cnt = vdip_result(buf, 2);

  // Set baud rate 9600
  out_vdip(0x18);
  out_vdip(0x20);
  switch(XM_RADIO_BAUDRATE) {
	case 9600:
	  out_vdip(0x38);
	  out_vdip(0x41);
	  out_vdip(0x00);
	  break;
	case 19200:
	  out_vdip(0x9c);
	  out_vdip(0x80);
	  out_vdip(0x00);
	  break;
	case 38400:
	  out_vdip(0x4e);
	  out_vdip(0xc0);
	  out_vdip(0x00);
	  break;
	case 57600:
	  out_vdip(0x34);
	  out_vdip(0xc0);
	  out_vdip(0x00);
	  break;
	 /*
	case 115200:
	  out_vdip(0x1a);
	  out_vdip(0x00);
	  out_vdip(0x00);
	  break;
	*/


  }
  out_vdip(0x0d);
  cnt = vdip_result(buf, 2);

  // Set flow control
  out_vdip(0x1b);
  out_vdip(0x20);
  out_vdip(0x00);
  out_vdip(0x0d);
  cnt = vdip_result(buf, 2);

  // Set  8n1
  out_vdip(0x1a);
  out_vdip(0x020);
  out_vdip(0x08);
  out_vdip(0x00);
  out_vdip(0x0d);
  cnt = vdip_result(buf, 2);

  //Serial.print("Switching to data mode...");
    // Ok, switch to data mode
    digitalWrite(dsr_dataReqPin, LOW);
    while(digitalRead(dtr_dataAckPin) != LOW)
      ;

}
void setupVdip1() {

  xmlog("Setting up vdip1...");
  pinMode(ctsPin, OUTPUT);
  pinMode(rtsPin, INPUT);
  pinMode(dtr_dataAckPin, INPUT);
  pinMode(dsr_dataReqPin, OUTPUT);
  digitalWrite(ctsPin, LOW);
  delay(250);
  SerialAux1->setBaud(XM_RADIO_BAUDRATE);
    digitalWrite(dsr_dataReqPin, HIGH);
    while(digitalRead(dtr_dataAckPin) != HIGH)
      ;
    //Serial.print("command mode confirmed\n\r");

  vdip_connect();


}
#endif


extern void xmcomm_init() {
#ifdef USE_VDIP_1
		setupVdip1();
#else
		SerialAux1->setBaud(XM_RADIO_BAUDRATE);
#endif

}
extern int xmcomm_in() {
	int c;
	if (SerialAux1->available()) {
		c = SerialAux1->read();
		//xmlog("xmIn: %02x", c);
		return c;
	}
	else
		return -1;
}

extern void xmcomm_out(unsigned char c) {
#ifdef USE_VDIP1
	out_vdip(c);
#else
	out_xm(c);
#endif
}

extern void headconn_on() {
	digitalWrite(37, HIGH);
	digitalWrite(31, HIGH);
}

extern void headconn_off() {
	digitalWrite(37, LOW);
	digitalWrite(31, LOW);
}

extern void headconn_reset() {
	headconn_off();
	delay(HEADEND_RESET_DELAY);
	headconn_on();
}
extern void headcomm_init() {
	pinMode(37, OUTPUT);
	pinMode(31, OUTPUT);
	pinMode(33, INPUT);
	headconn_on();
	HeadSerial->setBaud(HEADEND_BAUDRATE);

}


extern int headcomm_in() {
	int c;
	if (HeadSerial->available()) {
		c = HeadSerial->read();
		//xmlog("ipodin: %02x", c);
		return c;
	}
	else
		return -1;
}

extern void headcomm_out(unsigned char c) {
	HeadSerial->write(c);
}

extern  void ipodcomm_init() {
	pinMode(49, INPUT);
	IPodSerial->setBaud(IPOD_BAUDRATE);

}


extern int ipodcomm_in() {
	int c;
	if (IPodSerial->available()) {
		c = IPodSerial->read();
		//xmlog("ipodin: %02x", c);
		return c;
	}
	else
		return -1;
}

extern  void ipodcomm_out(unsigned char c) {
	IPodSerial->write(c);
}

extern int isIpodConnected() {
	return (digitalRead(49));
}
extern int debug_in() {
	if (DebugSerial->available())
		return DebugSerial->read();
	else
		return -1;

}

extern void eeprom_write(long pos, unsigned char c) {
	EEPROM.write(pos, c);
}
extern unsigned char eeprom_read(long pos) {
	return EEPROM.read(pos);
}

extern void eeprom_write_block(long pos, unsigned char *buf, int len) {
	for (int i=0;i<len;i++)
		eeprom_write(pos+i, *(buf+i));
}

extern void eeprom_read_block(long pos, unsigned char *buf, int len) {
	for (int i=0;i<len;i++)
		*(buf+i) = eeprom_read(pos+i);
}

#endif
