/*
 * TestModuleA.h
 *
 *  Created on: Sep 1, 2009
 *      Author: keary
 */

#ifndef TESTMODULEB_H_
#define TESTMODULEB_H_

#include "PodModule.h"
#include "IPodInterface.h"

#define TEST_MODULEB_ID	MODULE_ID('T', 'B')


class TestModuleB: public PodModule {
public:
	TestModuleB();
	virtual bool Run();
	// PodModule
	virtual char *getPodModuleName();
	virtual char *getPodModuleShortName();
	virtual unsigned int getPodModuleId() { return TEST_MODULEB_ID; }
	virtual bool isInited();
	virtual bool isRetryingInit();


	// IPodInterface
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

};

#endif /* TESTMODULEB_H_ */
