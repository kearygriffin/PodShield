/*
 * ModuleManager.cpp
 *
 *  Created on: Aug 22, 2009
 *      Author: keary
 */

#include "podshield.h"

#include "cppfix.h"
#include "ModuleManager.h"
#include "RunnableManager.h"

ModuleManager::ModuleManager() {
	moduleCnt = 0;
}

void ModuleManager::addModule(Module *m) {
	modules[moduleCnt++] = m;
	getRunnableManager().addRunnable(m);
}

ModuleManager& getModuleManager() {
	static ModuleManager mm;
	return mm;
}

void ModuleManager::init_modules() {
	for (int i=0;i<moduleCnt;i++)
		modules[i]->initModule();
}

void ModuleManager::module_loop() {
	bool moreThreads;
	do {
		moreThreads = getRunnableManager().runOnce();
	} while(moreThreads);
	// Should never fall through here!
}
