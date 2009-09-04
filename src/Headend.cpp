/*
 * Headend.cpp
 *
 *  Created on: Aug 28, 2009
 *      Author: keary
 */

#include "podshield.h"
#include "Headend.h"
#include "serports.h"

Headend headend;

static char *playlists[] =  {
		"Test playlist #1",
		"Test playlist #2"
};

static char *songs[] = {
		"Song #1",
		"Song #2"
};
Headend::Headend() : IPodEmulator(HeadSerial, HEADEND_BAUDRATE, true) {
	// TODO Auto-generated constructor stub
}


int Headend::getPlaylistSongCount(int playlist)  {
	return sizeof(songs)/sizeof(char *);
}

int pos = 0;
int Headend::getCurrentPlaylistPosition() {
	return pos;
}

int Headend::getPlaylistCount()  {
	return sizeof(playlists)/sizeof(char*) + 1;
}
bool Headend::getGenre(int playlist, int tracknum, char *genre)  {
	*genre = 0;
	return false;

}
void Headend::getSongTitle(int playlist, int tracknum, char *title)  {
	strcpy(title, songs[tracknum]);
}
void Headend::getSongTitleForPlaylist(int playlist, int tracknum, char *title)  {
	strcpy(title, songs[tracknum]);
}
void Headend::getArtist(int playlist, int tracknum, char *artist)  {
	*artist = 0;
}
void Headend::getAlbum(int playlist, int tracknum, char *album)  {
	*album = 0;
}
void Headend::setShuffleMode(int shuffleMode)  { }
int Headend::getShuffleMode()  { return 0; }
void Headend::setRepeatMode(int repeatMode)  { }
int Headend::getRepeatMode() { return 0; }
void Headend::changeTrack(int tracknum) {
	pos = tracknum;
}
void Headend::previousTrack()  {
	pos--;
	if (pos <0)
		pos = sizeof(songs)/sizeof(char*)-1;
}
void Headend::nextTrack()  {
	pos++;
	if (pos >= sizeof(songs)/sizeof(char*))
		pos = 0;
}
void Headend::setPlaylist(int playlist)  {

}
void Headend::getPlaylistName(int pos, char *name)  {
	if (pos > 0)
		pos--;
	strcpy(name, playlists[pos]);
}
