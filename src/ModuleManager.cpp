/*
 * ModuleManager.cpp
 *
 *  Created on: Aug 22, 2009
 *      Author: keary
 */

#include "cppfix.h"
#include "ModuleManager.h"
#include "RunnableManager.h"

ModuleManager::ModuleManager() {
	// TODO Auto-generated constructor stub

}

void ModuleManager::addModule(Module *m) {
	getRunnableManager().addRunnable(m);
}

ModuleManager& getModuleManager() {
	static ModuleManager mm;
	return mm;
}

#ifndef SKIP_MODULE_RUN
extern "C" void setup() {
	// Init modules here...
}

extern "C" void loop() {
	bool moreThreads;
	do {
		moreThreads = getRunnableManager().runOnce();
	} while(moreThreads);
	// Should never fall through here!
}
#endif
