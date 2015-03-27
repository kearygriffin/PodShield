/*
 * IPodInterface.h
 *
 *  Created on: Sep 1, 2009
 *      Author: keary
 */

#ifndef IPODINTERFACE_H_
#define IPODINTERFACE_H_

class IPodInterface {
public:
	// ipod emulation functions
		virtual int getPlaylistSongCount(int playlist) = 0;
		virtual int getCurrentPlaylistPosition() = 0;
		virtual int getPlaylistCount() = 0;
		virtual bool getGenre(int playlist, int tracknum, char *genre) = 0;
		virtual void getSongTitle(int playlist, int tracknum, char *title) = 0;
		virtual void getSongTitleForPlaylist(int playlist, int tracknum, char *title) = 0;
		virtual void getArtist(int playlist, int tracknum, char *artist) = 0;
		virtual void getAlbum(int playlist, int tracknum, char *album) = 0;
		virtual void setShuffleMode(int shuffleMode) = 0;
		virtual int getShuffleMode() = 0;
		virtual void setRepeatMode(int repeatMode) = 0;
		virtual int getRepeatMode() = 0;
		virtual void changeTrack(int tracknum) = 0;
		virtual void previousTrack() = 0;
		virtual void nextTrack() = 0;
		virtual void setPlaylist(int playlist) = 0;
		virtual void getPlaylistName(int pos, char *name) = 0;
};

#endif /* IPODINTERFACE_H_ */
