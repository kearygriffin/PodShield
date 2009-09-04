/*
 * IPodEmulator.h
 *
 *  Created on: Aug 24, 2009
 *      Author: keary
 */

#ifndef IPODEMULATOR_H_
#define IPODEMULATOR_H_

#include "Module.h"
#include "IPodInterface.h"
#include "PodSerial.h"

#define IPOD_SHUFFLE_OFF	0
#define IPOD_SHUFFLE_SONGS	1
#define IPOD_SHUFFLE_ALBUMS	2

#define IPOD_REPEAT_OFF	0
#define IPOD_REPEAT_SONGS	1
#define IPOD_REPEAT_ALBUMS	2

#define	DEFAULT_IPODEM_BAUDRATE	57600

#define RX_BUFLEN 64
#define TX_BUFLEN 128


class IPodSender : public ProtoThread {
public:
	IPodSender();
	virtual bool Run();
	bool isAvailable() { return !locked; }
	unsigned char *getBuffer(PROGMEM unsigned char *init = NULL, int len = 0);
	void releaseBuffer() { sending = false; }
	void send(int len = -1);
	void send_p(PROGMEM unsigned char *data, int len);
    PodSerial *Serial;
protected:
	int init_len;
	int response_i;
	bool sending;
	bool locked;
	bool startSending;
	unsigned char response[TX_BUFLEN];
	int responselen;

};

class IPodEmulator: public Module, public IPodInterface {
public:
	IPodEmulator(PodSerial *pserial, long baud = DEFAULT_IPODEM_BAUDRATE, bool autoRegister = true, bool startSuspended = false);
	virtual void initModule();
	virtual bool Run();
	void iap_reset();
	void iap_notify();
protected:
	int getVirtualCurrentPlaylistPosition(bool change);


	bool isFakePosition;
	int nochangeTicks;
	bool changedChannel;

	unsigned char serbuf[RX_BUFLEN];
	int serbuf_i;
	bool iap_getc(unsigned char c);
	IPodSender sender;
	bool iap_pollenabled;
    bool iap_updateflag;
    int iap_changedctr;
    unsigned long nextPoll;
    int lastPlaylistPos;
    int playlist;
    int filterPlaylist;


    unsigned char last_x;
    bool newpkt;
    unsigned char chksum;
    PodSerial *Serial;
    long baudRate;

};



#endif /* IPODEMULATOR_H_ */
