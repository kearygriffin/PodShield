/*
 * Headend.h
 *
 *  Created on: Aug 28, 2009
 *      Author: keary
 */

#ifndef HEADEND_H_
#define HEADEND_H_

#include "IPodEmulator.h"

class Headend: public IPodEmulator {
public:
	Headend();
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

};

#endif /* HEADEND_H_ */
