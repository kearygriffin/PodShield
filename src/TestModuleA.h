/*
 * TestModuleA.h
 *
 *  Created on: Sep 1, 2009
 *      Author: keary
 */

#ifndef TESTMODULEA_H_
#define TESTMODULEA_H_

#include "PodModule.h"
#include "IPodInterface.h"
#include "NVRam.h"

#define TEST_MODULEA_ID	MODULE_ID('T', 'A')

#define TEST_MODULEA_NVRAM	1
#define TEST_MODULEA_NVRAM_VER	1

class TestModuleA: public PodModule{
public:
	TestModuleA();
	virtual bool Run();
	// PodModule
	virtual char *getPodModuleName();
	virtual char *getPodModuleShortName();
	virtual unsigned int getPodModuleId() { return TEST_MODULEA_ID; }
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

	NVRam<int> nv;

protected:
};

#endif /* TESTMODULEA_H_ */
