/*
 * Headend.cpp
 *
 *  Created on: Aug 28, 2009
 *      Author: keary
 */

#include "podshield.h"
#include "Headend.h"
#include "serports.h"
#include "ModuleManager.h"
#include "RunnableManager.h"

Headend headend;

Headend::Headend() : IPodEmulator(HeadSerial, HEADEND_BAUDRATE, true),
	headendProperties(1, HEADEND_MODULE_ID, HEADEND_PROPERTIES_ID, HEADEND_PROPERTIES_VERSION),
	initThread(this)
{
	setIPodEnabled(false);
}

void Headend::disableHeadend() {
#ifndef	STANDALONE_PC

	digitalWrite(37, LOW);
	digitalWrite(31, LOW);
#endif
}

void Headend::enableHeadend() {
#ifndef	STANDALONE_PC
	digitalWrite(37, LOW);
	digitalWrite(31, LOW);
#endif
}

void Headend::setupHardware() {
#ifndef	STANDALONE_PC
	pinMode(37, OUTPUT);
	pinMode(31, OUTPUT);
#endif
}

void Headend::initModule() {
	IPodEmulator::initModule();
	setupHardware();
	disableHeadend();

	this->headendProperties.get(0, &props);

	podModCnt = 0;
	currentModule = NULL;
	int moduleCnt = getModuleManager().getModuleCnt();
	for (int i=0;i<moduleCnt;i++) {
		Module *m = getModuleManager().getModule(i);
		if (m->getModuleProperty_p(PSTR("PODMODULE")) != NULL) {
			// This is a pod module
			PodModule *pm = (PodModule*)m;
			podMods[podModCnt++] = pm;
			if (pm->getPodModuleId() == props.savedModuleId) {
				currentModule = pm;
			}
			menuItems[menuItemCnt++].menuName = m->getModuleProperty_p(PSTR("PNAME"));
		}
	}
	// Just pick a random one
	if (currentModule == NULL && podModCnt > 0) {
		currentModule = podMods[0];
		if (podModCnt > 1 && currentModule->getPodModuleId() == IPOD_PASSTHRU_MODULE) {
			currentModule = podMods[1];
		}
	}

	customPlaylist = -1;
	customPlaylistPos = -1;
	menuItemCnt = 0;

	getRunnableManager().addRunnable(&initThread);
}

int Headend::getPlaylistSongCount(int playlist)  {
	if (playlist >= getCustomPlaylistPos())
		return getCustomPlaylistSongCount();
	else
		return currentModule->getPlaylistSongCount(playlist);
}

int Headend::getCurrentPlaylistPosition() {
	if (customPlaylist < 0)
		return currentModule->getCurrentPlaylistPosition();
	else
		return 0;
}

int Headend::getPlaylistCount()  {
	return currentModule->getPlaylistCount() + getAdditionalPlaylistCount();
}
bool Headend::getGenre(int playlist, int tracknum, char *genre)  {
	if (customPlaylist < 0)
		return currentModule->getGenre(playlist, tracknum, genre);
	else {
		*genre = NULL;
		return false;
	}

}
void Headend::getSongTitle(int playlist, int tracknum, char *title)  {
	if (playlist >= getCustomPlaylistPos()) {
		getCustomPlaylistSongTitle(playlist, tracknum, title);
	}
	else
		currentModule->getSongTitle(playlist, tracknum, title);
}
void Headend::getSongTitleForPlaylist(int playlist, int tracknum, char *title)  {
	if (playlist >= getCustomPlaylistPos()) {
		getCustomPlaylistSongTitle(playlist, tracknum, title);
	}
	else
		currentModule->getSongTitleForPlaylist(playlist, tracknum, title);
}
void Headend::getArtist(int playlist, int tracknum, char *artist)  {
	if (playlist >= getCustomPlaylistPos()) {
		*artist = NULL;
	}
	else
		currentModule->getArtist(playlist, tracknum, artist);
}
void Headend::getAlbum(int playlist, int tracknum, char *album)  {
	if (playlist >= getCustomPlaylistPos()) {
		*album = NULL;
	}
	else
		currentModule->getArtist(playlist, tracknum, album);
}

void Headend::setShuffleMode(int shuffleMode)  {
	if (customPlaylist < 0)
		currentModule->setShuffleMode(shuffleMode);
}
int Headend::getShuffleMode()  {
	if (customPlaylist < 0)
	return currentModule->getShuffleMode();
}
void Headend::setRepeatMode(int repeatMode) {
	if (customPlaylist < 0)
		currentModule->setRepeatMode(repeatMode);
}

int Headend::getRepeatMode() {
	if (customPlaylist < 0)
		return currentModule->getRepeatMode();
	else
		return 0;
}

void Headend::changeTrack(int tracknum) {
	if (customPlaylist < 0)
		currentModule->changeTrack(tracknum);
}
void Headend::previousTrack()  {
	if (customPlaylist < 0)
		currentModule->previousTrack();
}
void Headend::nextTrack()  {
	if (customPlaylist < 0)
		currentModule->nextTrack();
}
void Headend::setPlaylist(int playlist)  {
	if (playlist >= getCustomPlaylistPos()) {
		this->customPlaylist = playlist;
	} else {
		this->customPlaylist = -1;
		currentModule->setPlaylist(playlist);
	}
}
void Headend::getPlaylistName(int pos, char *name)  {
	if (pos >= getCustomPlaylistPos())
		getCustomPlaylistName(pos, name);
	else
		currentModule->getPlaylistName(pos, name);
}

void Headend::setCurrentModule(PodModule *m) {
	currentModule = m;
}

int Headend::getAdditionalPlaylistCount() {
	return 1;
}

void Headend::getCustomPlaylistName(int pos, char *name) {
	strcpy_P(name, PODSHIELD_PLAYLIST_NAME);
}

int Headend::getCustomPlaylistPos() {
	if (customPlaylistPos >= 0)
		return customPlaylistPos;
	else {
		customPlaylistPos =currentModule->getPlaylistCount();
		return customPlaylistPos;
	}
}

void Headend::getCustomPlaylistSongTitle(int playlist, int tracknum, char *title) {
	strcpy(title, menuItems[tracknum].menuName);
}

int Headend::getCustomPlaylistSongCount() {
	return menuItemCnt;
}

bool HeadendInitThread::Run() {
	PT_BEGIN();
	while(1) {
		if (head->getCurrentModule()->isInited()) {
			head->enableHeadend();
			head->setIPodEnabled(true);
			PT_EXIT();
		}
		if (head->getCurrentModule()->isRetryingInit()) {
			// We failed the first init, try to find another module to switch to
			PodModule *goodModule = NULL;
			for (int i=0;i<head->getPodModCount();i++) {
				PodModule *m;
				m = head->getPodModule(i);
				if (m == head->getCurrentModule())
					continue;
				if (m->isInited()) {
					if (m->getPodModuleId() != IPOD_PASSTHRU_MODULE) {
						head->setCurrentModule(m);
						break;
					}
					goodModule = m;
				}
			}
			if (goodModule != NULL)
				head->setCurrentModule(goodModule);
		}
	}
	PT_END();
}
