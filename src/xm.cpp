#include "podshield.h"
//#include <WProgram.h>
//#include <avr/pgmspace.h>
#include "pgmspace.h"
#include <string.h>
#include <stdio.h>

extern void delay(unsigned long);
extern unsigned long millis();

#define null	0L
//#define true	1
//#define false	0
//#define min(a,b) (((a) < (b)) ? (a) : (b))


extern void xmcomm_init();
extern int xmcomm_in();
extern void xmcomm_out(unsigned char);

extern void headcomm_init();
extern int headcomm_in();
extern void headcomm_out(unsigned char);
extern void headconn_reset();
extern int debug_in();
extern void xmlog(char *, ...);
extern void xmlog_init();
extern void xm_init_protocol();
extern void eeprom_write(long pos, unsigned char c);
extern unsigned char eeprom_read(long pos);
extern void eeprom_write_block(long pos, unsigned char *buf, int len);
extern void eeprom_read_block(long pos, unsigned char *buf, int len);

#define SAVE_DEFAULT_TIME (1000L*15);
#define CHANGE_CHANNEL_DELAY	10
#define DISPLAY_STATION_TIME	(1000L*3)
#define FLUSH_BOOT_CNT	16

#define EEPROM_SIZE	4096

#define	SAVED_CHANNEL_FLAG	0
#define BOOT_CNT_FLAG	1
#define CHANNEL_INIT_FLAG 2
#define SAVED_PLAYLIST_FLAG 3


struct eeChannelInfo_store {
	unsigned char genreId;
	unsigned char station[14];
};

struct eeChannelInfo {
	unsigned char genreId;
	char station[16];
};
struct ramChannelInfo {
	struct eeChannelInfo eeInfo;
	unsigned char channel;
};

/*
struct ramArtistInfo {
	char artist[16];
	unsigned char channel;
};
*/
struct genreInfo {
	unsigned char genreId;
	char genre[17];
};

#define NUM_FAVORITES_PLAYLISTS	1

#define	ALL_STATIONS_PLAYLIST	1
#define	FAVORITES_PLAYLIST	2
#define GENRE_PLAYLIST	(FAVORITES_PLAYLIST+NUM_FAVORITES_PLAYLISTS)

#define SPORTS_ID	128
#define TRAFFIC_WEATHER_ID	129
#define	UNKNOWN_ID	130
#define	POP_ID		150
#define COUNTRY_ID	151
#define MORE_MUSIC_ID	152
#define	CHRISTIAN_ID	153
#define ROCK_ID			154
#define HIP_HOP_RB_ID	155
#define JAZZ_ID			156
#define DANCE_ID		157
#define LATIN_ID		158
#define ENTERTAINMENT_ID	159
#define BEST_OF_ID		160
#define TALK_NEWS_ID	161
#define COMEDY_ID		162
#define RELIGION_ID		163
#define CLASSICAL_ID	164
#define FAMILY_HEALTH_ID 	165


#define	EMPTY_ID		0xff
#define	DELETED_ID		0xfe

PROGMEM struct  genreInfo builtInGenres[]  = {
		{SPORTS_ID, "Sports"},
		{TRAFFIC_WEATHER_ID, "Traffic/Weather"},
		{POP_ID, "Pop"},
		{COUNTRY_ID, "Country"},
		{MORE_MUSIC_ID, "More Music"},
		{CHRISTIAN_ID, "Christian"},
		{ROCK_ID, "Rock"},
		{HIP_HOP_RB_ID, "Hip-Hop/R&B"},
		{JAZZ_ID, "Jazz/Standards"},
		{DANCE_ID, "Dance/Electronic"},
		{LATIN_ID, "Latin/World"},
		{ENTERTAINMENT_ID, "Entertainment"},
		{BEST_OF_ID, "Best of SIRIUS"},
		{TALK_NEWS_ID, "Talk/News"},
		{COMEDY_ID, "Comedy"},
		{RELIGION_ID, "Religion"},
		{CLASSICAL_ID, "Classical"},
		{FAMILY_HEALTH_ID, "Family & Health"},
};

#define MAX_RAM_CHANNELS	2
#define MAX_FLUSH_RAM_COUNT	2

#define MAX_RAM_ARTISTS	128
struct ramChannelInfo ramChannels[MAX_RAM_CHANNELS];
//struct ramArtistInfo ramArtistInfo[MAX_RAM_ARTISTS];

//unsigned char saveArtistGenres[] = { SPORTS_ID } ;

#define MAX_GENRES	16
#define NUM_FAVORITES	15

#define VOLATILE_AREA_SIZE	8
#define	VOLATILE_EEPROM_AREA	(EEPROM_SIZE-VOLATILE_AREA_SIZE)
#define	STATIONS_EEPROM_AREA 	(VOLATILE_EEPROM_AREA -(255*sizeof(struct eeChannelInfo_store)))
#define	GENRE_EEPROM_AREA 	(STATIONS_EEPROM_AREA -(14 * MAX_GENRES))
#define FAVORITES_EEPROM_AREA (GENRE_EEPROM_AREA-(NUM_FAVORITES*NUM_FAVORITES_PLAYLISTS))
#define EEPROM_MIN	FAVORITES_EEPROM_AREA


#define XM_TIMEOUT 5000
#define XM_MAX_CMD_SIZE	10
#define XM_CMD_QUEUE_SIZE	8

#define XM_CHANNEL_LABEL_SIZE	16
#define XM_CATEGORY_LABEL_SIZE	16
#define XM_ARTIST_TITLE_SIZE	16

#define	XM_DEFAULT_CHANNEL		1

const char *eeVer = "KG06";
int bootCnt = 0;
static int playlist = 0;
int playlistGenreId = 0;

void ram_init() {
	memset(ramChannels, 0xff, sizeof(ramChannels));
	//memset(ramArtistInfo, 0xff, sizeof(ramArtistInfo));
}
int eeprom_getVolatile(int flag) {
	return eeprom_read(VOLATILE_EEPROM_AREA+flag);
	/*
	int i;
	int f;
	for (i=VOLATILE_EEPROM_AREA;i<VOLATILE_EEPROM_AREA+VOLATILE_AREA_SIZE;i+=2) {
		f = eeprom_read(i);
		if (f == flag)
			return eeprom_read(i+1);
	}
	return -1;
	*/
}

void eeprom_setVolatile(int flag, int val) {
	eeprom_write(VOLATILE_EEPROM_AREA+flag, val);
	/*
	int i;
	int f;
	int startPos = VOLATILE_EEPROM_AREA;
	for (i=VOLATILE_EEPROM_AREA;i<VOLATILE_EEPROM_AREA+VOLATILE_AREA_SIZE;i+=2) {
		f = eeprom_read(i);
		if (f == flag) {
			startPos = i;// + 2;
			//eeprom_write(i, 0xff);
			break;
		}
	}

	i = startPos;
	do {
		if (i >= VOLATILE_EEPROM_AREA+VOLATILE_AREA_SIZE)
			i = 0;
		f = eeprom_read(i);
		if (f == 0xff || f == flag) {
			eeprom_write(i, flag);
			eeprom_write(i+1, val);
			break;
		}
		i += 2;
	} while (i != startPos);
	*/

}
int xm_getDefaultChannel() {
	int c;
	c = eeprom_getVolatile(SAVED_CHANNEL_FLAG);
	if (c > 0 && c != 0xff)
		return c;
	return XM_DEFAULT_CHANNEL;
}

int incBootCount() {
	int c = eeprom_getVolatile(BOOT_CNT_FLAG);
	if (c == 0xff)
		c = 0;
	else
		c++;
	eeprom_setVolatile(BOOT_CNT_FLAG, c);
	return c;
}
void xmcomm_write(unsigned char *buf, int siz) {
	int i;
	for (i=0;i<siz;i++) {
		xmcomm_out(*(buf+i));
	}
}

void xmcomm_cmd(unsigned char *cmd, int len) {
	xmcomm_out(0x5a);
	xmcomm_out(0xa5);
	xmcomm_out(len/256);
	xmcomm_out(len&0xff);
	xmcomm_write(cmd, len);
	xmcomm_out(0xed);
	xmcomm_out(0xed);
}
void xmwait(long dly) {
	int x;
	unsigned long tim = millis();
	unsigned long last = tim;
	while(tim - last < dly) {
		tim = millis();
		if ((x = xmcomm_in()) >= 0) {
			last = tim;
			//xmlog("in: %x", x);
		}
	}
	//xmlog("Flushed: %ld, %ld, %ld", tim - last, tim, last);
}
unsigned char xm_resetCmd[] = { 0x74, 0x00, 0x01 };
unsigned char xm_enableAudioCmd[] = { 0x74, 0x02, 0x01, 0x01 };
unsigned char xm_dacMuteOffCmd[] = { 0x74, 0x0b, 0x01 };

unsigned char xm_getChannelInfoCmd[] = { 0x25, 0x08, 0x00, 0x00 };
unsigned char xm_getExtChannelInfoCmd[] = { 0x22, 0x00 };
unsigned char xm_powerOnCmd[] = { 0x00, XM_CHANNEL_LABEL_SIZE, XM_CATEGORY_LABEL_SIZE, XM_ARTIST_TITLE_SIZE, 0x01 };
unsigned char xm_powerOffCmd[] = { 0x01, 0x00 };
unsigned char xm_initCmd[] = { 0x31 };
unsigned char xm_muteOffCmd[] = { 0x13, 0x00 };
unsigned char xm_muteOnCmd[] = { 0x13, 0x01};

unsigned char xm_changeChannelCmd[] = { 0x10, 0x02, 0x00, 0x00, 0x00, 0x01 };
unsigned char xm_monitorCmdOn[] = { 0x50, 0x00, 0x01, 0x01, 0x01, 0x01, };
unsigned char xm_monitorCmdOff[] = { 0x50, 0x00, 0x00, 0x00, 0x00, 0x00};
unsigned char xm_getSignalData[] = { 0x43 };

struct channelInfo {
	unsigned char serviceId;
	char channelName[33];
	char channelGenre[33];
	char artistName[33];
	char songTitle[33];
	int hasExtendedArtist;
	int hasExtendedSong;
};

int currentChannel = 1;
struct channelInfo curChannelInfo;

void eeprom_erase() {
	int i;
	unsigned char buf[16];

	xmlog("Erasing eeprom...");
	memset(buf, 0xff, sizeof(buf));

	for (i=0;i<EEPROM_SIZE;i += sizeof(buf)) {
		eeprom_write_block(i, buf, sizeof(buf));
	}
	eeprom_write_block(0, (unsigned char*)eeVer, strlen(eeVer));
	//xmlog("EEprom erase complete");
}
void eeprom_init() {
	unsigned char buf[sizeof(eeVer)];

	eeprom_read_block(0, buf, strlen(eeVer));
	if (memcmp(eeVer, buf, strlen(eeVer))) {
		eeprom_erase();
	}
}

struct xmCmdQueueEntry {
	unsigned char cmd[XM_MAX_CMD_SIZE];
	unsigned char cmdLen;
	int expectedResponse;
	int timeout;
	unsigned char resetOnFail;
	unsigned char userData;
	void (*handler)(struct xmCmdQueueEntry *cmdEntry, unsigned char *resp, int respLen);
};


struct xmCmdQueueEntry xmCmdQueue[XM_CMD_QUEUE_SIZE];
int xmCmdQueueTail = 0;
int xmCmdQueueHead = 0;

int xmRespPos = 0;
unsigned char xmresp[256];
int xmRespLen = 0;
int xmIsInited = 0;
int xmAwaitingResponse = 0;
unsigned long xmTimeoutTime = 0;
int needToSaveChannel = 0;
unsigned long saveDefaultChannelTime;

struct xmCmdQueueEntry xmCurCmd;

void xm_flush_cmd_queue() {
	xmCmdQueueTail = 0;
	xmCmdQueueHead = 0;
}

extern void xmChangeChannel(int newChannel, int delay);


extern void xm_send_cmd(unsigned char *data, int dataLen, void (*handler)(struct xmCmdQueueEntry *cmdEntry, unsigned char *resp, int respLen));

void xm_powerOff() {
	//xmlog("Power Off");
	xm_send_cmd(xm_powerOffCmd, sizeof(xm_powerOffCmd),  null);

}

int mute = 0;
long muteTime = 0;
void xm_init_complete(struct xmCmdQueueEntry *cmd, unsigned char *resp, int respLen) {
	xmIsInited = 1;
	xmChangeChannel(currentChannel, false);
	xmlog("Init complete.");
	if (mute) {
		muteTime = millis() + 250;
	}
}

char *getAntStatus(int stat) {
	if (stat == 0)
		return "No signal";
	else if (stat == 1)
		return "Fair";
	else if (stat == 2)
		return "Good";
	else if (stat == 3)
		return "Excellent";
	else return
		"Unknown";
}
void xm_signal_data(struct xmCmdQueueEntry *cmd, unsigned char *resp, int respLen) {
	int satStatus;
	int terStatus;
	int connected;

	satStatus = *(resp+3);
	connected = *(resp+4);
	terStatus = *(resp+5);
	//xmlog("Antenna: %s", connected ? "Connected" : "Disconnected");
	//xmlog("SatStatus: %s", getAntStatus(satStatus));
	//xmlog("TerStatus: %s", getAntStatus(terStatus));
}

void xm_power_on_status(struct xmCmdQueueEntry *cmd, unsigned char *resp, int respLen) {
	char radioId[9];
	//if (*(resp+1) == 0x03)
		//xmlog("WARN!: XM Radio not activated.");
	xmlog("RX version: %d", *(resp+4));
	//xmlog("RX date: %02x/%02x/%02x%02x", *(resp+5), *(resp+6), *(resp+7), *(resp+8));
	//xmlog("CMBver: %d", *(resp + 13));
	//xmlog("CMB date: %02x/%02x/%02x%02x", *(resp+14), *(resp+15), *(resp+16), *(resp+17));
	memcpy(radioId, resp+19, 8);
	radioId[8] = 0;
	xmlog("RadioID: %s", radioId);
}

void xm_push_cmd(struct xmCmdQueueEntry *cmd) {
	int newHead = xmCmdQueueHead + 1;
	if (newHead >= XM_CMD_QUEUE_SIZE)
		newHead = 0;
	if (newHead == xmCmdQueueTail) {
		//xmlog("Xm cmd queue full!");
		xm_init_protocol();
	}
	memcpy(&xmCmdQueue[xmCmdQueueHead], cmd, sizeof(*cmd));
	xmCmdQueueHead = newHead;
}

struct xmCmdQueueEntry *xm_pop_cmd() {
	struct xmCmdQueueEntry *cmd;
	if (xmCmdQueueHead == xmCmdQueueTail)
		return null;

	cmd = &xmCmdQueue[xmCmdQueueTail];
	xmCmdQueueTail++;
	if (xmCmdQueueTail >= XM_CMD_QUEUE_SIZE)
		xmCmdQueueTail = 0;
	return cmd;
}

void _xm_send_cmd(unsigned char *data, int dataLen, int expectedResponse, int timeout, int resetOnFail, int userData, void (*handler)(struct xmCmdQueueEntry *cmdEntry, unsigned char *resp, int respLen)) {
	struct xmCmdQueueEntry entry;
	memcpy(entry.cmd, data, dataLen);
	entry.cmdLen = dataLen;
	entry.expectedResponse = expectedResponse;
	entry.timeout = timeout;
	entry.resetOnFail = resetOnFail;
	entry.userData = userData;
	entry.handler = handler;
	xm_push_cmd(&entry);
}

void xm_mute() {
	mute = 1;
	xm_send_cmd(xm_muteOnCmd, sizeof(xm_muteOnCmd), null);
}

void xm_mute_off() {
	mute = 0;
	xm_send_cmd(xm_muteOffCmd, sizeof(xm_muteOffCmd), null);
}


void xm_send_cmd(unsigned char *data, int dataLen, void (*handler)(struct xmCmdQueueEntry *cmdEntry, unsigned char *resp, int respLen))
{
	_xm_send_cmd(data, dataLen, *(data)+0x80, XM_TIMEOUT, true, 0, handler);
}

void getString(char *str, unsigned char *data, int off, int len) {
	char *xstr, c;
	memset(str, 0, len+1);
	memcpy(str, data+off, len);
	while(strlen((char*)str) > 0 && *(str+(strlen((char*)str)-1)) == 32) {
		*(str+strlen((char*)str)-1) = 0;
	}
	xstr = str;
	while(*xstr) {
		c = *(xstr++);
		if (c < 0 || c > 127) {
			*str = 0;
			return;
		}
	}
}

int fillExtChannelInfo(struct channelInfo *channelInfo, unsigned char *resp, int len) {
	char str[32];
	int changed = 0;
	if (len < 75)
		return 0;
	//xmlog("ExtChan: %d", *(resp+3));
	if (*(resp+4) == 1) {
		getString(str, resp, 5, 32);
		//xmlog("ExtArtist: %s", str);
		if (strlen(str) > 0) {
			if (strcmp(channelInfo->artistName, str)) {
				changed = 1;
				strcpy(channelInfo->artistName, str);
				//channelInfo->hasExtendedArtist= 1;
			}
		}
	}
	if (*(resp+41) == 1) {
		getString(str, resp, 42, 32);
		//xmlog("ExtTitle: %s", str);
		if (strlen(str) > 0) {
			if (strcmp(channelInfo->songTitle, str)) {
				changed = 1;
				strcpy(channelInfo->songTitle, str);
				//channelInfo->hasExtendedSong = 1;
			}
		}
	}

	return changed;

}
extern void iap_track_changed();

int delayedNotification = 0;
unsigned long validDataTime = 0;

void notifyChange() {
	if (validDataTime == 0)
		iap_track_changed();
	delayedNotification = 0;
}
void logCurChannelInfo() {
	xmlog("%d - %s (%s): %s - %s", currentChannel, curChannelInfo.channelName, curChannelInfo.channelGenre, curChannelInfo.artistName, curChannelInfo.songTitle);
}
int infoChanged = 0;
struct channelInfo origInfo;
struct channelInfo newInfo;

int gettingInfo = 0;
int needNewInfo = 0;

void extChannelInfoHandler(struct xmCmdQueueEntry *cmd, unsigned char *resp, int len) {
	int changed = 0;
	gettingInfo = 0;
	if (*(resp+3) == currentChannel)  {
		infoChanged |= fillExtChannelInfo(&newInfo, resp, len);
		//changed = infoChanged;
		if (strcmp(origInfo.songTitle, newInfo.songTitle))
			changed = 1;
		if (strcmp(origInfo.artistName, newInfo.artistName))
			changed = 1;
		if (changed) {
			//xmlog("%s:%s %s:%s", origInfo.songTitle, origInfo.artistName, curChannelInfo.songTitle, curChannelInfo.artistName);
			//xmlog("extChannelInfoInfoHandler: iap_track_changed");
			memcpy(&curChannelInfo, &newInfo, sizeof(newInfo));
			logCurChannelInfo();
			if (needNewInfo)
				delayedNotification = 1;
			else
				notifyChange();
		}
	}

	else {
		//xmlog("Info for another channel");
		//gettingInfo = 0;
	}

}

int fillChannelInfo(struct channelInfo *channelInfo, unsigned char *resp, int len) {
	char str[17];
	int changed = 0;
	if (len < 74)
		return 0;
	//xmlog("Chan: %d, serviceId: %d", *(resp+3), *(resp+4));
	channelInfo->serviceId = *(resp+4);
	if (*(resp+5) == 1) {
		getString(str, resp, 6, 16);
		//xmlog("ChanName: %s", str);
		if (strcmp(channelInfo->channelName, str)) {
			changed = 1;
			strcpy(channelInfo->channelName, str);
		}
	}
	if (*(resp+22) == 1) {
		getString(str, resp, 24, 16);
		//xmlog("Genre: %s", str);
		if (strlen(str) > 0) {
			if (strcmp(channelInfo->channelGenre, str)) {
				changed = 1;
				strcpy(channelInfo->channelGenre, str);
			}
		}
	}
	if (*(resp+40) == 1) {
		getString(str, resp, 41, 16);
		//xmlog("Artist: %s", str);
		if (strlen(str) > 0 && channelInfo->hasExtendedArtist == 0) {
			if (strcmp(channelInfo->artistName, str)) {
				changed = 1;
				strcpy(channelInfo->artistName, str);
			}
		}
		getString(str, resp, 57, 16);
		if (strlen(str) > 0 && channelInfo->hasExtendedSong == 0) {
			if (strcmp(channelInfo->songTitle, str)) {
				changed = 1;
				strcpy(channelInfo->songTitle, str);
			}
		}
		//xmlog("Title: %s", str);
	}

	return changed;



}
void channelInfoHandler(struct xmCmdQueueEntry *cmd, unsigned char *resp, int len) {
	unsigned char extInfoCmd[sizeof(xm_getExtChannelInfoCmd)];

	if (*(resp+3) == currentChannel) {
		memcpy(&origInfo, &curChannelInfo, sizeof(origInfo));
		memcpy(&newInfo, &curChannelInfo, sizeof(origInfo));
		infoChanged = fillChannelInfo(&newInfo, resp, len);
		if (infoChanged) {
			//xmlog("channelInfoInfoHandler: iap_track_changed");
			//iap_track_changed();
			//logCurChannelInfo();
		}
		memcpy(extInfoCmd, xm_getExtChannelInfoCmd, sizeof(extInfoCmd));
		extInfoCmd[1] = currentChannel;
		xm_send_cmd(extInfoCmd, sizeof(extInfoCmd), extChannelInfoHandler);

	}
	else {
		//xmlog("Info for another channel");
		gettingInfo = 0;
	}


}

extern int getChannelInfo(int channel, struct eeChannelInfo *ee);
extern int getGenreString(unsigned char genreId, char *g);
//extern int getArtist(int channel, char *a);

unsigned long changeChannelTime = 0;

void reallyChangeChannel() {
	unsigned char channelCmd[sizeof(xm_changeChannelCmd)];
	//unsigned char infoCmd[sizeof(xm_getChannelInfoCmd)];
	unsigned char monitorCmd[sizeof(xm_monitorCmdOff)];

	changeChannelTime = 0;
	/*
	memcpy(infoCmd, xm_getChannelInfoCmd, sizeof(infoCmd));
	infoCmd[1] = 0x08;
	infoCmd[2] = currentChannel;
	xm_send_cmd(infoCmd, sizeof(infoCmd), channelInfoHandler);
	*/
	needNewInfo = 1;
	memcpy(channelCmd, xm_changeChannelCmd, sizeof(channelCmd));
	channelCmd[2] = currentChannel;
	xm_send_cmd(channelCmd, sizeof(channelCmd), null);

	//memcpy(extInfoCmd, xm_getExtChannelInfoCmd, sizeof(extInfoCmd));
	//extInfoCmd[1] = currentChannel;
	//xm_send_cmd(extInfoCmd, sizeof(extInfoCmd), extChannelInfoHandler);

	memcpy(monitorCmd, xm_monitorCmdOn, sizeof(xm_monitorCmdOn));
	monitorCmd[1] = currentChannel;
	xm_send_cmd(monitorCmd, sizeof(monitorCmd), null);
}

void xmChangeChannel(int newChannel, int delay) {
	unsigned char monitorCmd[sizeof(xm_monitorCmdOff)];
	char tmp[16];
	struct eeChannelInfo ee;

	memset(&curChannelInfo, 0, sizeof(curChannelInfo));
	if (getChannelInfo(newChannel, &ee)) {
		strncpy(curChannelInfo.channelName, ee.station, 16);
		if (getGenreString(ee.genreId, tmp)) {
			strncpy(curChannelInfo.channelGenre, tmp, 16);
		}
		/*
		if (getArtist(newChannel, tmp)) {
			strncpy(curChannelInfo.artistName, tmp, 16);
		}
		*/



	}
	if (changeChannelTime == 0) {
		memcpy(monitorCmd, xm_monitorCmdOff, sizeof(xm_monitorCmdOff));
		monitorCmd[1] = currentChannel;
		xm_send_cmd(monitorCmd, sizeof(monitorCmd), null);
	}

	currentChannel = newChannel;
	changeChannelTime = millis() + (delay ? CHANGE_CHANNEL_DELAY : 0);

	saveDefaultChannelTime = millis() + SAVE_DEFAULT_TIME;
	needToSaveChannel = 1;
	//xmlog("xmChangeChannel: iap_track_changed");
	// this forces the notification to go through
	validDataTime = 0;
	notifyChange();
	validDataTime = millis() + DISPLAY_STATION_TIME;
}
#define XM_INIT_TIMEOUT 	1000
void xm_init_protocol() {

	xmIsInited = 0;
	gettingInfo = 0;
	needNewInfo = 0;
	delayedNotification = 0;

	xmRespPos = 0;
	xmRespLen = 0;
	xmAwaitingResponse = 0;
	xm_flush_cmd_queue();
	_xm_send_cmd(xm_initCmd, sizeof(xm_initCmd), -1, XM_INIT_TIMEOUT, false, 0, null);
	_xm_send_cmd(xm_resetCmd, sizeof(xm_resetCmd), 0xe4, XM_INIT_TIMEOUT, true, 0, null);
	_xm_send_cmd(xm_enableAudioCmd, sizeof(xm_enableAudioCmd), 0xe4, XM_TIMEOUT, true, 0, null);
	_xm_send_cmd(xm_dacMuteOffCmd, sizeof(xm_dacMuteOffCmd), 0xe4, XM_TIMEOUT, true, 0, null);
	xm_send_cmd(xm_powerOnCmd, sizeof(xm_powerOnCmd), xm_power_on_status);
	xm_send_cmd(xm_getSignalData, sizeof(xm_getSignalData), xm_signal_data);

	if (mute) {
		xmlog("Sending mute for xm_init_protocol");
		xm_send_cmd(xm_muteOnCmd, sizeof(xm_muteOnCmd), xm_init_complete);
	}
	else {
		xm_send_cmd(xm_muteOffCmd, sizeof(xm_muteOffCmd), xm_init_complete);
	}

	//_xm_send_cmd(xm_dacMuteOffCmd, sizeof(xm_dacMuteOffCmd), 0, 0, true, 0, null);

}


void xm_handle_queue_response() {
	xmAwaitingResponse = 0;
	if (xmCurCmd.handler != null)
		(*xmCurCmd.handler)(&xmCurCmd, xmresp, xmRespLen);
}

void handleSongTimeInfo(unsigned char *resp, int len) {

}



void _handleChannelInfoChange() {
	unsigned char infoCmd[sizeof(xm_getChannelInfoCmd)];

	gettingInfo = 1;
	needNewInfo = 0;

	memcpy(infoCmd, xm_getChannelInfoCmd, sizeof(infoCmd));
	infoCmd[1] = 0x08;
	infoCmd[2] = currentChannel;
	xm_send_cmd(infoCmd, sizeof(infoCmd), channelInfoHandler);
}

void handleChannelInfoChange(unsigned char *resp, int len) {
	//unsigned char infoCmd[sizeof(xm_getChannelInfoCmd)];

	if (*(resp+1) != currentChannel)
		return;
	needNewInfo = 1;
	//_handleChannelInfoChange();
}


void handleExtChannelInfoChange(unsigned char *resp, int len) {
	unsigned char infoCmd[sizeof(xm_getExtChannelInfoCmd)];

	if (*(resp+1) != currentChannel)
		return;
	memcpy(infoCmd, xm_getExtChannelInfoCmd, sizeof(infoCmd));
	infoCmd[1] = currentChannel;
	xm_send_cmd(infoCmd, sizeof(infoCmd), extChannelInfoHandler);



}

void handleExtTitleChange(unsigned char *resp, int len) {
	/*
	char str[33];
	if (len < 35)
		return;
	if (*(resp+2) == 0)
		return;
	if (*(resp+1) != currentChannel)
		return;
	getString(str, resp, 3, 32);
	strcpy(curChannelInfo.songTitle, str);
	xmlog("MonExtTitle: %s", str);
	*/
	//handleExtChannelInfoChange(resp, len);
	handleChannelInfoChange(resp, len);
}

void handleExtArtistChange(unsigned char *resp, int len) {
	/*
	char str[33];
	if (len < 35)
		return;
	if (*(resp+2) == 0)
		return;
	if (*(resp+1) != currentChannel)
		return;
	getString(str, resp, 3, 32);
	strcpy(curChannelInfo.artistName, str);
	xmlog("MonExtArtist: %s", str);
	*/
	//handleExtChannelInfoChange(resp, len);
	handleChannelInfoChange(resp, len);
}

void handleChannelNameChange(unsigned char *resp, int len) {
	/*
	char str[17];
	if (len < 19)
		return;
	if (*(resp+2) == 0)
		return;
	if (*(resp+1) != currentChannel)
		return;
	getString(str, resp, 3, 16);
	xmlog("MonChannelChange: %s", str);
	*/
	handleChannelInfoChange(resp, len);
}
void handleChannelGenreChange(unsigned char *resp, int len) {
	/*
	char str[17];
	if (len < 20)
		return;
	if (*(resp+2) == 0)
		return;
	if (*(resp+1) != currentChannel)
		return;
	getString(str, resp, 4, 16);
	xmlog("MonGenreChange: %s", str);
	*/
	handleChannelInfoChange(resp, len);
}

void handleChannelArtistTitleChange(unsigned char *resp, int len) {
	/*
	char str[17];
	if (len < 19)
		return;
	if (*(resp+2) == 0)
		return;
	if (*(resp+1) != currentChannel)
		return;
	getString(str, resp, 3, 16);
	xmlog("MonArtTitleChange: %s", str);
	*/
	handleChannelInfoChange(resp, len);
}

void handleMonitorMessage(unsigned char *data, int len) {
	if (len > 0) {
		switch(*(data)) {
		case 0xd1:
			handleChannelNameChange(data, len);
			break;
		case 0xd2:
			handleChannelGenreChange(data, len);
			break;
		case 0xd3:
			handleChannelArtistTitleChange(data, len);
			break;
		case 0xd4:
			handleExtArtistChange(data, len);
			break;
		case 0xd5:
			handleExtTitleChange(data, len);
			break;
		case 0xd6:
			handleSongTimeInfo(data, len);
			break;
		}
	}
}
void xm_buildresponse() {
	int c;
	while((c = xmcomm_in()) >= 0) {
		if (xmRespPos == 0 && c == 0x5a)
			xmRespPos++;
		else if (xmRespPos == 1 && c == 0xa5)
			xmRespPos++;
		else if (xmRespPos == 2 && c == 0x00)
			xmRespPos++;
		else if (xmRespPos == 3) {
			xmRespPos++;
			xmRespLen = c;
		} else if (xmRespPos >= 4 && xmRespPos < (4+xmRespLen)) {
			xmresp[xmRespPos - 4] = c;
			xmRespPos++;
		} else if (xmRespPos == 4+xmRespLen)
			xmRespPos++;
		else if (xmRespPos == 5+xmRespLen) {
			xmRespPos = 0;
			if (xmAwaitingResponse && (xmresp[0] == xmCurCmd.expectedResponse || xmCurCmd.expectedResponse == -1)) {
				xm_handle_queue_response();
			} else {
				handleMonitorMessage(xmresp, xmRespLen);
			}
		}
		else
			xmRespPos = 0;
	}
	if (xmAwaitingResponse && millis() > xmTimeoutTime) {
		if (xmCurCmd.resetOnFail)
			xm_init_protocol();
		else {
			xmAwaitingResponse = 0;
			if (xmCurCmd.handler != null) {
				(*xmCurCmd.handler)(&xmCurCmd, null, 0);
			}
		}
	}
}

int hasChannels = 0;
int lastChannel = 0;
int tripsThroughChannels = 0;

int getRamStationCount() {
	int i;
	int cnt = 0;
	for (i=0;i<MAX_RAM_CHANNELS;i++) {
		if (ramChannels[i].eeInfo.genreId == EMPTY_ID)
			continue;
		cnt++;
	}
	return cnt;
}


int read_bits(unsigned char *data, int bitPos, int len) {
	int byte, bit, bits, b;
	unsigned char mask;
	int c = 0;
	b = 0;
	while(len > 0) {
		byte = bitPos / 8;
		bit = bitPos % 8;
		bits = min(len, 8-bit);
		mask = (1 << bits) - 1;
		mask = ~mask;
		mask = mask << bit | (0xff >> (8-bit));
		c |= ((*(data+byte) & ~mask) >> bit) << b;
		len -= bits;
		b += bits;
		bitPos += bits;
	}
	return c;
}

void write_bits(unsigned char *data, int bitPos, int c, int len) {
	int byte, bit, bits;
	unsigned char mask;
	while(len > 0) {
		byte = bitPos / 8;
		bit = bitPos % 8;
		bits = min(len, 8-bit);
		mask = (1 << bits) - 1;
		mask = ~mask;
		mask = mask << bit | (0xff >> (8-bit));
		*(data+byte) = (*(data+byte) & mask) | ((c << bit) & 0xff);
		c = c >> bits;
		len -= bits;
		bitPos += bits;
	}
}

void write_pack_char(unsigned char *data, int charPos, int c) {
	write_bits(data, charPos * 7, c, 7);
}

unsigned char read_pack_char(unsigned char *data, int charPos) {
	return read_bits(data, charPos * 7, 7);
}
void readEEStoreChannel(int channel, struct eeChannelInfo *edest) {
	struct eeChannelInfo_store ee;
	int i;
	unsigned char c;
	eeprom_read_block(STATIONS_EEPROM_AREA + ((channel-1) * sizeof(struct eeChannelInfo_store)), (unsigned char *)&ee, sizeof(ee));
	// convert...
	edest->genreId = ee.genreId;
	for (i =0;i<16;i++) {
		c = read_pack_char(ee.station, i);
		edest->station[i] = c;
	}
}

void writeEEStoreChannel(int channel, struct eeChannelInfo *esrc) {
	struct eeChannelInfo_store ee;
	int i;
	unsigned char c;
	ee.genreId = esrc->genreId;
	for (i=0;i<16;i++) {
		c = esrc->station[i];
		write_pack_char(ee.station, i, c);
	}
	eeprom_write_block(STATIONS_EEPROM_AREA + ((channel-1) * sizeof(struct eeChannelInfo_store)), (unsigned char *)&ee, sizeof(ee));
}
int flushedToEeprom = 0;

void flushStationsToEeprom() {
	int i, channel;
	flushedToEeprom = 1;
	struct eeChannelInfo ee;
	for (i=0;i<MAX_RAM_CHANNELS;i++) {
		if (ramChannels[i].eeInfo.genreId == EMPTY_ID)
			continue;
		channel = ramChannels[i].channel;
		if (ramChannels[i].eeInfo.genreId == DELETED_ID) {
			if (eeprom_read(STATIONS_EEPROM_AREA + ((channel-1) * sizeof(struct eeChannelInfo_store)) != EMPTY_ID)) {
				//xmlog("Deleting station from eeprom: %d", channel);
				eeprom_write(STATIONS_EEPROM_AREA + ((channel-1) * sizeof(struct eeChannelInfo_store)), EMPTY_ID);
			}
		} else {
			readEEStoreChannel(channel, &ee);
			if (ramChannels[i].eeInfo.genreId != ee.genreId || strncmp(ramChannels[i].eeInfo.station, ee.station, 16) != 0) {
				//xmlog("Writing channel to eeprom %d", channel);
				writeEEStoreChannel(channel, &ramChannels[i].eeInfo);

			}
		}
		ramChannels[i].eeInfo.genreId = EMPTY_ID;
	}
}
void possiblyFlushStations() {
	int channelInit = eeprom_getVolatile(CHANNEL_INIT_FLAG);
	//xmlog("PosibbyFlush: bootCnt=%d, trips: %d, channelInit: %d", bootCnt, tripsThroughChannels, channelInit);

	if ((((bootCnt % FLUSH_BOOT_CNT) == 0) && tripsThroughChannels == 2) ||
			(((flushedToEeprom && getRamStationCount() > 0) || getRamStationCount() > MAX_FLUSH_RAM_COUNT) && tripsThroughChannels >= 2) ||
			(tripsThroughChannels == 1 && channelInit < 0)) {
				flushStationsToEeprom();
				if (channelInit < 0 || channelInit == 0xff)
					eeprom_setVolatile(CHANNEL_INIT_FLAG, 1);
			}
}

int getChannelInfo(int channel, struct eeChannelInfo *info) {
	int i;
	for (i=0;i<MAX_RAM_CHANNELS;i++) {
		if (ramChannels[i].eeInfo.genreId == EMPTY_ID)
			break;
		if (ramChannels[i].channel == channel) {
			if (ramChannels[i].eeInfo.genreId == DELETED_ID)
				return 0;
			memcpy(info, &ramChannels[i].eeInfo, sizeof(struct eeChannelInfo));
			return 1;
		}
	}

	// try out eeprom
	readEEStoreChannel(channel, info);

	if (info->genreId == EMPTY_ID)
		return 0;
	return 1;
}

int isGenreActive(int genreId) {
	struct eeChannelInfo info;
	int i;
	for (i=0;i<256;i++) {
		if (getChannelInfo(i, &info)) {
			if (info.genreId == genreId)
				return 1;
		}
	}
	return 0;
}

unsigned char builtInGenresUsed[sizeof(builtInGenres)/sizeof(*builtInGenres)];
int isBuiltInGenreUsed(int p) {
	int byt = p/8;
	int bit = p % 8;
	int v;

	v = builtInGenresUsed[byt] & (1 << bit);
	if (v)
		return true;
	else
		return false;

}
void setBuiltInGenreUsed(int p, int v) {
	int byt = p/8;
	int bit = p % 8;
	if (v)
		builtInGenresUsed[byt] |= 1 << bit;
	else
		builtInGenresUsed[byt] &= ~(1 << bit);
}

int compactGenres()
{
	char gtmp[17];
	struct genreInfo iTemp;
	gtmp[16] = 0;
	int i;
	int emptyPos = -1;
	for (i=0;i<MAX_GENRES;i++) {
		eeprom_read_block(GENRE_EEPROM_AREA + (i*14), (unsigned char*)gtmp, 16);
		if ((unsigned char)gtmp[0] == 0xff) {
			if (emptyPos == -1)
				emptyPos = i;
			continue;
		}
		if (!isGenreActive(i)) {
			eeprom_write(GENRE_EEPROM_AREA + (i*14), 0xff);
			if (emptyPos == -1)
				emptyPos = i;
		}
	}

	for (i=0;i<sizeof(builtInGenres)/ sizeof(struct genreInfo);i++) {
		memcpy_P(&iTemp, &builtInGenres[i], sizeof(iTemp));
		if (isGenreActive(iTemp.genreId)) {
			setBuiltInGenreUsed(i, 1);
		} else
			setBuiltInGenreUsed(i, 0);
	}

	return emptyPos;
}

int getGenreString(unsigned char id, char *genre) {
	int i;
	int pos;
	unsigned char tmp[14];
	struct genreInfo iTemp;

	if (id < 128 && id >= MAX_GENRES)
		return 0;
	if (id >= 128) {
		for (i=0;i<sizeof(builtInGenres)/ sizeof(struct genreInfo);i++) {
			memcpy_P(&iTemp, &builtInGenres[i], sizeof(iTemp));
			if (iTemp.genreId == id) {
				if (genre != null)
					strncpy(genre, iTemp.genre, 16);
				return 1;
			}
		}
		return 0;
	}

	pos = GENRE_EEPROM_AREA + (id*14);
	if (eeprom_read(pos) == 0xff)
		return 0;
	for (i=0;i<16;i++) {
		eeprom_read_block(pos, tmp, 14);
		*(genre+i) = read_pack_char(tmp, i);
	}
	return 1;
}

int getGenreId(char *genre, int save) {
	int i, j;
	int emptyPos = -1;
	char gtmp[17];
	unsigned char tmp[14];
	struct genreInfo iTemp;

	gtmp[16] = 0;

	for (i=0;i<sizeof(builtInGenres)/ sizeof(struct genreInfo);i++) {
		memcpy_P(&iTemp, &builtInGenres[i], sizeof(iTemp));
		if (strncmp(iTemp.genre, genre, 16) == 0) {
			return iTemp.genreId;
		}
	}

	for (i=0;i<MAX_GENRES;i++) {
		if (eeprom_read(GENRE_EEPROM_AREA + (i*14)) == 0xff) {
			if (emptyPos == -1)
				emptyPos = i;
			continue;
		}

		eeprom_read_block(GENRE_EEPROM_AREA + (i*14), tmp, 14);
		for (j=0;j<16;j++) {
			gtmp[j] = read_pack_char(tmp, j);
		}
		if (strncmp(gtmp, genre, 16) == 0) {
			return i;
		}
	}

	// Not found.

	if (save) {
		// Are there any empty positions?
		if (emptyPos == -1)
			emptyPos = compactGenres();
		if (emptyPos != -1) {
			strncpy(gtmp, genre, 16);
			//xmlog("Adding genre: %s", gtmp);
			for (i=0;i<16;i++) {
				write_pack_char(tmp, i, genre[i]);
			}
			eeprom_write_block(GENRE_EEPROM_AREA+ (emptyPos*14), tmp, 14);
			return emptyPos;
		} else {
			//xmlog("Warning: genre full!");
		}
	}

	return UNKNOWN_ID;
}


void saveChannelToRam(int channel, int genreId, struct channelInfo *info) {
	int i;

	//xmlog("Saving channel: %d, %s", channel, info ? info->channelName : "deleted");
	for (i=0;i<MAX_RAM_CHANNELS;i++) {
		if (ramChannels[i].eeInfo.genreId == EMPTY_ID)
			break;
	}
	if (i >= MAX_RAM_CHANNELS) {
		flushStationsToEeprom();
		i = 0;
	}

	ramChannels[i].channel = channel;
	ramChannels[i].eeInfo.genreId = genreId;

	if (info != null) {
		strncpy(ramChannels[i].eeInfo.station, info->channelName, 16);
	}

}

/*
int getArtist(int channel, char *artist) {
	int i;

	for (i=0;i<MAX_RAM_ARTISTS;i++) {
		if ((unsigned char)ramArtistInfo[i].artist[0] != 0xff && ramArtistInfo[i].channel == channel) {
			strncpy(artist, ramArtistInfo[i].artist, 16);
			return 1;
		}
	}
	*artist = 0;
	return 0;
}

void updateArtist(int channel, char *artist) {
	int i;
	int empty = -1;


	if (artist != null) {
		for (i=0;i<16 && artist[i];i++) {
			if (artist[i] < 0 || artist[i] > 127) {
				artist = null;
				break;
			}
		}
	}
	for (i=0;i<MAX_RAM_ARTISTS;i++) {
		if ((unsigned char)ramArtistInfo[i].artist[0] != 0xff) {
			//xmlog("Inspecting artist: %02x", ramArtistInfo[i].artist[0]);
			if (ramArtistInfo[i].channel == channel) {
				if (artist == null) {
					ramArtistInfo[i].artist[0] = -1;
					return;
				} else {
					strncpy(ramArtistInfo[i].artist, artist, 16);
					return;
				}
			}
		} else {
			if (empty == -1)
				empty = i;
		}
	}
	//xmlog("Saving artist in empty: %d", empty);

	if (empty == -1 || artist == null)
		return;
	ramArtistInfo[empty].channel = channel;
	strncpy(ramArtistInfo[empty].artist, artist, 16);

}
*/
void eraseChannel(int channel) {
	struct eeChannelInfo eeInfo;

	if (getChannelInfo(channel, &eeInfo)) {
		saveChannelToRam(channel, DELETED_ID, null);
		//updateArtist(channel, null);
	}
	//xmlog("Erase channel: %d", channel);
}

void updateChannel(int channel, struct channelInfo *info) {

	unsigned char genreId = UNKNOWN_ID;
	struct eeChannelInfo eeInfo;
	int i;
	//int saveArtist = 0;
	char chanName[17];
	chanName[16] = 0;

	if (info->channelName[0] == 0)
		return;

	for (i=0;i<16 && info->channelName[i];i++) {
		if (info->channelName[i] < 0 || info->channelName[i] > 127)
			return;
	}
	if (info->channelGenre[0] != 0) {
		genreId = getGenreId(info->channelGenre, true);
	}


	if (!getChannelInfo(channel, &eeInfo)) {
		// channel not saved yet, save it
		saveChannelToRam(channel, genreId, info);
	} else {
		if (strncmp(info->channelName, eeInfo.station, 16) != 0 || genreId != eeInfo.genreId) {
			saveChannelToRam(channel, genreId, info);
		}
	}


	/*
	for (i=0;i<sizeof(saveArtistGenres);i++) {
		if (genreId == saveArtistGenres[i]) {
			updateArtist(channel, info->artistName);
			saveArtist = 1;
			break;
		}
	}
	if (saveArtist == 0) {
		updateArtist(channel, null);
	}
	*/

	//xmlog("Updating chan: %d - %s (%s)", channel, info->channelName, info->channelGenre);
	//if (strlen(info->artistName) > 0 || strlen(info->songTitle) > 0)
		//xmlog("  %s - %s", info->artistName, info->songTitle);
}

void logStations() {
	int i;
	char chan[17];
	char art[17];
	char genre[17];
	struct eeChannelInfo info;

	chan[16] = 0;
	art[16] = 0;
	genre[16] = 0;

	//compactGenres();
	for (i=0;i<128;i++) {
		if (getGenreString(i, genre)) {
			//xmlog("Genre Id %d: %s", i, genre);
		}
	}
	for (i=0;i<256;i++) {
		if (getChannelInfo(i, &info)) {
			strncpy(chan, info.station, 16);
			//getArtist(i, art);
			art[0] = 0;
			//xmlog("Station %d: %s - %s", i, chan, art);
		}
	}
}

int getChannelCount() {
	int i;
	int cnt = 0;
	struct eeChannelInfo info;

	for (i=0;i<256;i++) {
		if (getChannelInfo(i, &info)) {
			cnt++;
		}
	}
	return cnt;
}

int ipLastSn = 9999;
int ipLastChannel = 0;
int lp = -1;

int songNumberToChannelAll(int sn) {
	int i, start;
	int cnt = 0;
	struct eeChannelInfo info;

	if (ipLastSn <= sn) {
		start = ipLastChannel;
		cnt = ipLastSn;
	} else
		start = 0;
	for (i=start;i<256;i++) {
		if (getChannelInfo(i, &info)) {
			if (sn == cnt) {
				ipLastSn = sn;
				ipLastChannel = i;
				return i;
			}
			cnt++;
		}
	}
	return 1;
}

int songNumberToChannelFavorites(int sn) {
	int i, c;

	if (sn >= NUM_FAVORITES)
		return 1;

	for (i=0;i<NUM_FAVORITES;i++) {
		c = eeprom_read(FAVORITES_EEPROM_AREA+i);
		if (c != 0xff)
			sn--;
		if (sn < 0) {
			return c;
		}
	}
	return 1;


}

int  getGenreIdFromPlaylist(int p);
int lastGenrePlaylist = -1;
int lastGenreId = -1;

int songNumberToChannelGenre(int p, int sn) {
	int i, start, cnt = 0;
	struct eeChannelInfo info;
	int genreId;

	if (lastGenrePlaylist == p) {
		genreId = lastGenreId;
	} else {
		genreId = getGenreIdFromPlaylist(p);
		lastGenreId = genreId;
		lastGenrePlaylist = p;
	}
		if (genreId < 0)
			return 1;

	if (ipLastSn <= sn) {
		start = ipLastChannel;
		cnt = ipLastSn;
	} else
		start = 0;

	for (i=start;i<256;i++) {
		if (getChannelInfo(i, &info)) {
			if (info.genreId == genreId) {
				if (sn == cnt) {
					ipLastSn = sn;
					ipLastChannel = i;
					return i;
				}
				cnt++;
			}
		}
	}
	return 1;

}

int songNumberToChannel(int pl, int sn) {

	if (pl != lp) {
		ipLastSn = 9999;
		ipLastChannel = 0;
		lp = pl;
	}
	if (pl <= ALL_STATIONS_PLAYLIST)
		return songNumberToChannelAll(sn);
	else if (pl == FAVORITES_PLAYLIST)
		return songNumberToChannelFavorites(sn);
	else
		return songNumberToChannelGenre(pl, sn);
}

int channelToSn(int channel) {
	int i;
	int cnt = 0;
	struct eeChannelInfo info;

	for (i=0;i<256;i++) {
		if (getChannelInfo(i, &info)) {
			if (i == channel)
				return cnt;
			cnt++;
		}
	}
	return 0;
}

void channelBuilderHandler(struct xmCmdQueueEntry *cmd, unsigned char *resp, int respLen) {
	int i;
	struct channelInfo info;
	memset(&info, 0, sizeof(info));

	int newChannel = *(resp+3);

	fillChannelInfo(&info, resp, respLen);
	if (newChannel != 0)
		updateChannel(newChannel, &info);
	for (i=lastChannel+1;i<((newChannel == 0) ? 256 : newChannel);i++)
		eraseChannel(i);
	//xmlog("Adding chan: %d(%d) - %s (%s)", newChannel, *(resp+4), info.channelName, info.channelGenre);
	//if (strlen(info.artistName) > 0 || strlen(info.songTitle) > 0)
		//xmlog("  %s - %s", info.artistName, info.songTitle);
	if (newChannel == 0) {
		//hasChannels = 1;
		if (tripsThroughChannels < 30000)
			tripsThroughChannels++;
		possiblyFlushStations();
		flushedToEeprom = 0;
		compactGenres();
		if (tripsThroughChannels >= 3) {
			hasChannels = 1;
			xmlog("Done scanning channels.");
		}
		//logStations();
		//xmlog("Finished building channels: %d (%d)", channelCount, size);
	}
	lastChannel = newChannel;

}
void buildChannelList() {
	unsigned char infoCmd[sizeof(xm_getChannelInfoCmd)];

	memcpy(infoCmd, xm_getChannelInfoCmd, sizeof(infoCmd));
	infoCmd[1] = 0x09;
	infoCmd[2] = lastChannel;
	xm_send_cmd(infoCmd, sizeof(infoCmd), channelBuilderHandler);

}
void xm_process_queue() {
	struct xmCmdQueueEntry *entry;

	if (!xmAwaitingResponse) {
		entry = xm_pop_cmd();
		if (entry != null) {
			memcpy(&xmCurCmd, entry, sizeof(*entry));
			if (entry->expectedResponse != 0) {
				xmAwaitingResponse = 1;
				xmTimeoutTime = millis() + entry->timeout;
			} else
				xmTimeoutTime = 0xffffffffl;
			xmcomm_cmd(entry->cmd, entry->cmdLen);
		}
	}
}

extern void iap_setup();

void head_setup() {
	headcomm_init();
	iap_setup();
}


int channelToGenreSn() {
	int i;
	int cnt = 0;
	struct eeChannelInfo info;
	int genreId;

	genreId = playlistGenreId;
	for (i=0;i<256;i++) {
		if (i == currentChannel)
			return cnt;
		if (getChannelInfo(i, &info)) {
			if (info.genreId == genreId)
				cnt++;
		}
	}
	return 0;

}
int channelToFavoritesSn() {
	int i, c;
	int cnt = 0;

	for (i=0;i<NUM_FAVORITES;i++) {
		c = eeprom_read(FAVORITES_EEPROM_AREA+i);
		if (c != 0xff) {
			if (c == currentChannel)
				return cnt;
			cnt++;
		}
	}

	return 0;

}
int _getCurrentPlaylistPosition() {
	//int sn;

	if (playlist <= ALL_STATIONS_PLAYLIST)
		return channelToSn(currentChannel);
	else if (playlist == FAVORITES_PLAYLIST)
		return channelToFavoritesSn();
	else
		return channelToGenreSn();

	//return sn;
}

int cachedPosition = -1;
int cachedPlaylist = -1;
int cachedChannel = -1;

int getCurrentPlaylistPosition() {
	if (cachedChannel == currentChannel && cachedPosition >= 0 && cachedPlaylist == playlist)
		return cachedPosition;
	cachedPosition = _getCurrentPlaylistPosition();
	cachedPlaylist = playlist;
	cachedChannel = currentChannel;
	return cachedPosition;
}

void ipodNextFavorite(int dir) {
	int i, c;
	int favNum = 0;

	for (i=0;i<NUM_FAVORITES;i++) {
		c = eeprom_read(FAVORITES_EEPROM_AREA+i);
		if (c == currentChannel) {
			favNum = i;
			break;
		}
	}

	i = favNum + dir;
	if (i < 0)
		i = NUM_FAVORITES-1;
	else if (i >= NUM_FAVORITES)
		i = 0;

	while(i != favNum) {
		c = eeprom_read(FAVORITES_EEPROM_AREA+i);
		if (c != 0xff) {
			xmChangeChannel(c, true);
			return;
		}
		i += dir;
		if (i < 0)
			i = NUM_FAVORITES-1;
		else if (i >= NUM_FAVORITES)
			i = 0;
	}
	return;

}

void ipodNextGenre(int dir) {
	int chan;
	struct eeChannelInfo ee;

	chan = currentChannel + dir;
	//xmlog("Ipod NextGenre, dir: %d, genreId: %d", dir, playlistGenreId);
	while (chan != currentChannel) {
		if (getChannelInfo(chan, &ee)) {
			if (ee.genreId == playlistGenreId)
				break;
		}
		chan += dir;
		if (chan < 0)
			chan = 255;
		else if (chan > 255)
			chan = 0;
		if (chan == currentChannel) {
			//ipodAck();
			return;
		}
	}
	xmChangeChannel(chan, true);
	//ipodAck();
}

void ipodNextAll(int dir) {
	int chan;
	struct eeChannelInfo ee;

	chan = currentChannel += dir;
	//xmlog("Ipod Next, dir: %d", dir);
	while (getChannelInfo(chan, &ee) == 0) {
		chan += dir;
		if (chan < 0)
			chan = 255;
		else if (chan > 255)
			chan = 0;
		if (chan == currentChannel) {
			//ipodAck();
			return;
		}
	}
	xmChangeChannel(chan, true);
	//ipodAck();
}

int  getGenreIdFromPlaylist(int p) {
	int c, i;
	struct genreInfo iTemp;
	//int p = playlist;
	if (p >= GENRE_PLAYLIST) {
		p -= GENRE_PLAYLIST;
		for (i=0;i<sizeof(builtInGenres)/ sizeof(struct genreInfo);i++) {
			if (isBuiltInGenreUsed(i))
				p--;
			if (p  < 0) {
				memcpy_P(&iTemp, &builtInGenres[i], sizeof(iTemp));
				return iTemp.genreId;
			}

		}
		for (i=0;i<MAX_GENRES;i++) {
			//eeprom_read_block(GENRE_EEPROM_AREA + (i*14), (unsigned char*)gtmp, 16);
			c = eeprom_read(GENRE_EEPROM_AREA + (i*14));
			if (c != 0xff)
				p--;
			if (p < 0) {
				return c;
			}
		}
	}

	return -1;

}


void setGenreIdFromPlaylist() {
	playlistGenreId = getGenreIdFromPlaylist(playlist);
}

int playlistCnt = -1;


int getPlaylistName(int t, char *name) {
	int c, i;
	struct genreInfo iTemp;

	if (t <= ALL_STATIONS_PLAYLIST) {
		strcpy(name, "XM Radio");
		return 1;
	}
	else if (t == FAVORITES_PLAYLIST) {
		strcpy(name, "XM Favorites");
		return 1;
	}
	else if (t >= GENRE_PLAYLIST) {
		t -= GENRE_PLAYLIST;
		for (i=0;i<sizeof(builtInGenres)/ sizeof(struct genreInfo);i++) {
			if (isBuiltInGenreUsed(i))
			//if (isGenreActive(builtInGenres[i].genreId))
				t--;
			if (t  < 0) {
				memcpy_P(&iTemp, &builtInGenres[i], sizeof(iTemp));
				strcpy(name, "XM Genre ");
				*(name+strlen(name)+16) = 0;
				strncpy(name+strlen(name), iTemp.genre, 16);
				return 1;
			}

		}
		for (i=0;i<MAX_GENRES;i++) {
			//eeprom_read_block(GENRE_EEPROM_AREA + (i*14), (unsigned char*)gtmp, 16);
			c = eeprom_read(GENRE_EEPROM_AREA + (i*14));
			if (c != 0xff)
				t--;
			if (t < 0) {
				strcpy(name, "XM Genre ");
				*(name+strlen(name)+16) = 0;
				getGenreString(c, name+strlen(name));
				return 1;
			}
		}

	}
	return 0;
}

int ipodGetPlaylistCount() {
	int i, c;
	int cnt = 0;

	//compactGenres();
	for (i=0;i<MAX_GENRES;i++) {
		//eeprom_read_block(GENRE_EEPROM_AREA + (i*14), (unsigned char*)gtmp, 16);
		c = eeprom_read(GENRE_EEPROM_AREA + (i*14));
		if (c != 0xff)
			cnt++;
	}

	for (i=0;i<sizeof(builtInGenres)/ sizeof(struct genreInfo);i++) {
		if (isBuiltInGenreUsed(i))
		//if (isGenreActive(builtInGenres[i].genreId))
			cnt++;
	}


	return 2 + cnt;
}

int getFavoritesSongCount() {
	int i, c, cnt = 0;
	for (i=0;i<NUM_FAVORITES;i++) {
		c = eeprom_read(FAVORITES_EEPROM_AREA+i);
		if (c != 0xff) {
			cnt++;
		}
	}

	return cnt;
}

int getGenreSongCount(int p) {
	int i;
	int cnt = 0;
	struct eeChannelInfo info;
	int genreId;

	genreId = getGenreIdFromPlaylist(p);
	for (i=0;i<256;i++) {
		if (getChannelInfo(i, &info)) {
			if (info.genreId == genreId)
			cnt++;
		}
	}
	return cnt;
}
int ipodGetSongCount(int p) {
	if (p < 0) {
		return playlistCnt;
	}

	if (p <= ALL_STATIONS_PLAYLIST)
		return getChannelCount();
	else if (p == FAVORITES_PLAYLIST)
		return getFavoritesSongCount();
	else
		return getGenreSongCount(p);
}


void ipodSetPlaylist(int p) {
	//int i, c;
	playlist = p;
	setGenreIdFromPlaylist();
	playlistCnt = ipodGetSongCount(p);
}

void toggleFavorite() {
	int i, c, p = -1;
	for (i=0;i<NUM_FAVORITES;i++) {
		c = eeprom_read(FAVORITES_EEPROM_AREA+i);
		if (c != 0xff) {
			if (c == currentChannel) {
				//xmlog("Removing favorite %d from pos: %d", currentChannel, i);
				eeprom_write(FAVORITES_EEPROM_AREA+i, 0xff);
				return;
			}
		} else if (p == -1)
			p = i;
	}
	if (p < 0) {
		//xmlog("Favorites full!");
		return;
	}
	//xmlog("Saving favorite %d at pos: %d", currentChannel, p);
	eeprom_write(FAVORITES_EEPROM_AREA+p, currentChannel);

}

void ipodNext(int dir) {
	if (playlist <= ALL_STATIONS_PLAYLIST)
		ipodNextAll(dir);
	else if (playlist == FAVORITES_PLAYLIST)
		ipodNextFavorite(dir);
	else
		ipodNextGenre(dir);
}

void ipodChangeChannel(int tracknum) {
	int channel = songNumberToChannel(playlist, tracknum);
	if (channel != currentChannel)
		xmChangeChannel(channel, false);
}

int getIPodChannel(int pl, int tracknum, char *data, int useExt) {
	int channel;
	struct eeChannelInfo ee;
	char station[17];
	station[16] = 0;
	//int i;

	channel = songNumberToChannel(pl, tracknum);

	if (getChannelInfo(channel, &ee)) {
		strncpy(station, ee.station, 16);
		sprintf(data, "%d-%s", channel, station);
		/*
		for (i=0;i<sizeof(saveArtistGenres);i++) {
			if (ee.genreId == saveArtistGenres[i]) {
				return 2;
			}
		}
		*/
		return 1;
	} else {
		*data = 0;
		return -1;
	}
}

int getIPodGenre(int pl, int tracknum, char *data) {
	int channel;
	char genre[17];
	struct eeChannelInfo ee;

	genre[16] = 0;

	channel = songNumberToChannel(pl, tracknum);
	if (getChannelInfo(channel, &ee)) {
		return getGenreString(ee.genreId, data);
	} else
		return 0;

}
void getIPodSongTitle(int pl, int tracknum, char *data, int useExt) {
	int channel;
	char song[17];
	song[16] = 0;

	channel = songNumberToChannel(pl, tracknum);
	if (useExt && currentChannel == channel && (millis() > validDataTime || validDataTime == 0)) {
		strcpy(data, curChannelInfo.songTitle);
		/*
		if (strlen(data) && strlen(curChannelInfo.artistName)) {
			strcpy(data+strlen(data), ", ");
		}
		strcpy(data+strlen(data), curChannelInfo.artistName);
		*/
	} else {
		 getIPodChannel(pl, tracknum, data, useExt);
	}
}

void getIPodArtist(int pl, int tracknum, char *data, int useExt) {
	int channel;
	char artist[17];
	artist[16] = 0;

	channel = songNumberToChannel(pl, tracknum);
	if (useExt && currentChannel == channel) {
		strcpy(data, curChannelInfo.artistName);
	} else {
		if (0 /* getArtist(channel, artist) */) {
			strcpy(data, artist);
		} else
			*data = 0;
	}
}

extern int isIpodConnected();
extern int ipodcomm_in();
extern void ipodcomm_out(unsigned char c);
extern void ipodcomm_init();

int ipodConnected = 0;
void setup() {
	int n;

	xmlog_init();

	//xmlog("EEProm base: %d", EEPROM_MIN);
	if (EEPROM_MIN < strlen(eeVer)) {
		//xmlog("ERROR!!! EEPROM_MIN to small");
	}
	//n = analogRead(5);
	//xmlog("iPodDetect: %d", n);


	eeprom_init();
	ram_init();

	bootCnt = incBootCount();
	xmcomm_init();
	currentChannel = xm_getDefaultChannel();
	playlist = eeprom_getVolatile(SAVED_PLAYLIST_FLAG);
	if (playlist == 0xff)
		playlist = 0;
	ipodSetPlaylist(playlist);
	compactGenres();
	//setGenreIdFromPlaylist();
	ipodConnected = isIpodConnected();
	if (ipodConnected) {
		xmlog("IpodConnected... Muting?");
		mute = 1;
	}

	xm_init_protocol();
	head_setup();
	ipodcomm_init();

	validDataTime = millis() + DISPLAY_STATION_TIME;
}

extern void head_loop();


/*
int turnOff = 1;
unsigned long nxtTurn = 0;
*/
void xm_loop() {
	xm_buildresponse();
	xm_process_queue();

	/*
	if (millis() > nxtTurn) {
		turnOff = !turnOff;
		if (turnOff && xmIsInited)
			xm_powerOff();
		else if (!turnOff && !xmIsInited) {
			xm_init_protocol(false);
		}
		nxtTurn = millis() + 10000L;
	}
	*/
	if (mute && xmIsInited && !xmAwaitingResponse && muteTime > 0) {
		if (millis() >= muteTime) {
			xmlog("Resending mute...");
			xm_mute();
			muteTime = 0;
		}
	}
	if (xmIsInited == 1 && !xmAwaitingResponse && !hasChannels) {
		buildChannelList();
	}
	if (needToSaveChannel && millis() > saveDefaultChannelTime) {
		needToSaveChannel = 0;
		if (eeprom_getVolatile(SAVED_CHANNEL_FLAG) != currentChannel) {
			eeprom_setVolatile(SAVED_CHANNEL_FLAG, currentChannel);
			//xmlog("Saved new default channel: " + currentChannel);
		}
		if (eeprom_getVolatile(SAVED_PLAYLIST_FLAG) != playlist) {
			eeprom_setVolatile(SAVED_PLAYLIST_FLAG, playlist);
		}
	}
	if (changeChannelTime && millis() >= changeChannelTime) {
		reallyChangeChannel();
	}
	if (needNewInfo && !gettingInfo)
		_handleChannelInfoChange();
	else if (validDataTime > 0 && millis() >= validDataTime) {
		//xmlog("vaildDataTime: iap_track_changed");
		validDataTime = 0;
		notifyChange();
	}
	else if (!needNewInfo && !gettingInfo && delayedNotification)
		notifyChange();

}

void ipod_direct() {
	int c;
	while((c = ipodcomm_in()) >= 0) {
		headcomm_out(c);
	}
	while((c = headcomm_in()) >= 0) {
		ipodcomm_out(c);
	}
}
void yield() {
	xm_loop();
}
extern void iap_reset();

long ipodConnectedMillis = 0;
#define IPOD_CONNECT_MILLIS 100
void loop() {
	int c;


	int ic = isIpodConnected();
	if (ipodConnected != ic) {
		if (ipodConnectedMillis == 0)
			ipodConnectedMillis = millis() + IPOD_CONNECT_MILLIS;
		else if (millis() >= ipodConnectedMillis) {
			ipodConnected = ic;
			if (!ipodConnected) {
				iap_reset();
				xm_mute_off();
				headconn_reset();
			} else {
				xm_mute();
				headconn_reset();
			}
			ipodConnectedMillis = 0;
		}
	} else
		ipodConnectedMillis = 0;

	xm_loop();
	if (ipodConnected) {
		ipod_direct();
	}
	else
		head_loop();

	c = debug_in();
	if (c >= 0) {
		if (c >= '0' && c <= '9') {
			xmChangeChannel(c-'0', false);
		} else if (c == 'r') {
			headconn_reset();
		} else if (c == 'm') {
		}
	}
}
