/*
 * Headend.h
 *
 *  Created on: Aug 28, 2009
 *      Author: keary
 */

#ifndef HEADEND_H_
#define HEADEND_H_

#include "IPodEmulator.h"
#include "NVRam.h"
#include "PodModule.h"
#include "pgmspace.h"

#define MAX_POD_MODS	5
#define MAX_MENU_ITEMS	(MAX_POD_MODS+10)


#define	IPOD_PASSTHRU_MODULE MODULE_ID('R','I')

#define	PODSHIELD_PLAYLIST_NAME PSTR("PodShield")

struct HeadendProperties {
	unsigned int savedModuleId;
};
#define HEADEND_PROPERTIES_ID	1
#define HEADEND_PROPERTIES_VERSION	1

#define HEADEND_MODULE_ID	MODULE_ID('H', 'E')

class Headend;

class HeadendInitThread : public ProtoThread {
public:
	HeadendInitThread(Headend *head) { this->head = head; }
	virtual bool Run();
protected:
	Headend *head;
};

class PodShieldMenuItem {
public:
	char *menuName;
};

class Headend: public IPodEmulator {
public:
	Headend();

	virtual void initModule();

	void disableHeadend();
	void enableHeadend();

	PodModule *getCurrentModule() { return currentModule; }
	PodModule *getPodModule(int x) { return podMods[x]; }
	int getPodModCount() { return podModCnt; }

	void setCurrentModule(PodModule *m);

	// ipod emulation functions
	virtual int getPlaylistSongCount(int playlist) ;
	virtual int getCurrentPlaylistPosition() ;
	virtual int getPlaylistCount() ;
	virtual bool getGenre(int playlist, int tracknum, char *genre) ;
	virtual void getSongTitle(int playlist, int tracknum, char *title) ;
	virtual void getSongTitleForPlaylist(int playlist, int tracknum, char *title) ;
	virtual void getArtist(int playlist, int tracknum, char *artist) ;
	virtual void getAlbum(int playlist, int tracknum, char *album) ;
	virtual void setShuffleMode(int shuffleMode) ;
	virtual int getShuffleMode() ;
	virtual void setRepeatMode(int shuffleMode) ;
	virtual int getRepeatMode() ;
	virtual void changeTrack(int tracknum) ;
	virtual void previousTrack() ;
	virtual void nextTrack() ;
	virtual void setPlaylist(int playlist) ;
	virtual void getPlaylistName(int pos, char *name) ;


protected:
	void setupHardware();
	int getCustomPlaylistPos();
	int getAdditionalPlaylistCount();
	void getCustomPlaylistName(int pos);
	void getCustomPlaylistName(int pos, char *name);
	void getCustomPlaylistSongTitle(int playlist, int tracknum, char *title);
	int getCustomPlaylistSongCount();


	PodModule *currentModule;
	PodModule *podMods[MAX_POD_MODS];
	int podModCnt;

	NVRam<struct HeadendProperties> headendProperties;
	struct HeadendProperties props;
	HeadendInitThread initThread;

	int customPlaylist;
	int customPlaylistPos;

	PodShieldMenuItem menuItems[MAX_MENU_ITEMS];
	int menuItemCnt;
};

#endif /* HEADEND_H_ */
