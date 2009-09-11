

/*
 * TestModuleA.cpp
 *
 *  Created on: Sep 1, 2009
 *      Author: keary
 */

#include "podshield.h"

#include "TestModuleA.h"

static TestModuleA ta;

static char *playlists[] =  {
		"MA Test playlist #1",
		"MA Test playlist #2"
};

static char *songs[] = {
		"MA Song #1",
		"MA Song #2"
};

static int pos = 0;

TestModuleA::TestModuleA() : nv(256, TEST_MODULEA_ID, TEST_MODULEA_NVRAM, TEST_MODULEA_NVRAM_VER ){
}


int TestModuleA::getPlaylistSongCount(int playlist)  {
	return sizeof(songs)/sizeof(char *);
}

int TestModuleA::getCurrentPlaylistPosition() {
	return pos;
}

int TestModuleA::getPlaylistCount()  {
	return sizeof(playlists)/sizeof(char*) + 1;
}
bool TestModuleA::getGenre(int playlist, int tracknum, char *genre)  {
	*genre = 0;
	return false;

}
void TestModuleA::getSongTitle(int playlist, int tracknum, char *title)  {
	strcpy(title, songs[tracknum]);
}
void TestModuleA::getSongTitleForPlaylist(int playlist, int tracknum, char *title)  {
	strcpy(title, songs[tracknum]);
}
void TestModuleA::getArtist(int playlist, int tracknum, char *artist)  {
	*artist = 0;
}
void TestModuleA::getAlbum(int playlist, int tracknum, char *album)  {
	*album = 0;
}
void TestModuleA::setShuffleMode(int shuffleMode)  { }
int TestModuleA::getShuffleMode()  { return 0; }
void TestModuleA::setRepeatMode(int repeatMode)  { }
int TestModuleA::getRepeatMode() { return 0; }
void TestModuleA::changeTrack(int tracknum) {
	pos = tracknum;
}
void TestModuleA::previousTrack()  {
	pos--;
	if (pos <0)
		pos = sizeof(songs)/sizeof(char*)-1;
}
void TestModuleA::nextTrack()  {
	pos++;
	if (pos >= sizeof(songs)/sizeof(char*))
		pos = 0;
}
void TestModuleA::setPlaylist(int playlist)  {

}
void TestModuleA::getPlaylistName(int pos, char *name)  {
	if (pos > 0)
		pos--;
	strcpy(name, playlists[pos]);
}

bool TestModuleA::Run() {
	//debug("TestModuleA");
	return false;
}


char *TestModuleA::getPodModuleName() {
	int x;
	nv.get(0, &x);
	return "TestModule A";
}
char *TestModuleA::getPodModuleShortName() {
	return "TA";
}


bool TestModuleA::isInited() {
	return true;
}

bool TestModuleA::isRetryingInit() {
	return false;
}

