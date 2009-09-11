/*
 * TestModuleB.cpp
 *
 *  Created on: Sep 1, 2009
 *      Author: keary
 */

#include "podshield.h"

#include "TestModuleB.h"

static TestModuleB tb;

static char *playlists[] =  {
		"MB Test playlist #1",
		"MB Test playlist #2"
};

static char *songs[] = {
		"MB Song #1",
		"MB Song #2"
};
TestModuleB::TestModuleB()  {
	// TODO Auto-generated constructor stub
}


int TestModuleB::getPlaylistSongCount(int playlist)  {
	return sizeof(songs)/sizeof(char *);
}

static int pos = 0;
int TestModuleB::getCurrentPlaylistPosition() {
	return pos;
}

int TestModuleB::getPlaylistCount()  {
	return sizeof(playlists)/sizeof(char*) + 1;
}
bool TestModuleB::getGenre(int playlist, int tracknum, char *genre)  {
	*genre = 0;
	return false;

}
void TestModuleB::getSongTitle(int playlist, int tracknum, char *title)  {
	strcpy(title, songs[tracknum]);
}
void TestModuleB::getSongTitleForPlaylist(int playlist, int tracknum, char *title)  {
	strcpy(title, songs[tracknum]);
}
void TestModuleB::getArtist(int playlist, int tracknum, char *artist)  {
	*artist = 0;
}
void TestModuleB::getAlbum(int playlist, int tracknum, char *album)  {
	*album = 0;
}
void TestModuleB::setShuffleMode(int shuffleMode)  { }
int TestModuleB::getShuffleMode()  { return 0; }
void TestModuleB::setRepeatMode(int repeatMode)  { }
int TestModuleB::getRepeatMode() { return 0; }
void TestModuleB::changeTrack(int tracknum) {
	pos = tracknum;
}
void TestModuleB::previousTrack()  {
	pos--;
	if (pos <0)
		pos = sizeof(songs)/sizeof(char*)-1;
}
void TestModuleB::nextTrack()  {
	pos++;
	if (pos >= sizeof(songs)/sizeof(char*))
		pos = 0;
}
void TestModuleB::setPlaylist(int playlist)  {

}
void TestModuleB::getPlaylistName(int pos, char *name)  {
	if (pos > 0)
		pos--;
	strcpy(name, playlists[pos]);
}

bool TestModuleB::Run() {
	return false;
}


char *TestModuleB::getPodModuleName() {
	return "TestModule B";
}
char *TestModuleB::getPodModuleShortName() {
	return "TB";
}

bool TestModuleB::isInited() {
	return true;
}

bool TestModuleB::isRetryingInit() {
	return false;
}
