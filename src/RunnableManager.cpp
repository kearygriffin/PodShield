/*
 * RunnableManager.cpp
 *
 *  Created on: Aug 21, 2009
 *      Author: keary
 */

#include "cppfix.h"
#include "RunnableManager.h"


RunnableManager::RunnableManager() {
	for (int i=0;i<MAX_RUNNABLES;i++)
		runList[i] = 0;
}

bool RunnableManager::removeRunnable(Runnable *runnable) {
	for (int i=0;i<MAX_RUNNABLES;i++) {
		if (runList[i] == runnable) {
			runList[i] = 0;
			return true;
		}
	}
	return false;

}
bool RunnableManager::addRunnable(Runnable *runnable) {
	removeRunnable(runnable);
	for (int i=0;i<MAX_RUNNABLES;i++) {
		if (runList[i] == 0) {
			runList[i] = runnable;
			return true;
		}
	}
	return false;
}

bool RunnableManager::runOnce() {
	bool hasRunnables;
	 hasRunnables = false;
	 for (int i=0;i<MAX_RUNNABLES;i++) {
		 if (runList[i] != 0) {
			 if (runList[i]->isRunning()) {
				hasRunnables= true;
				if (!runList[i]->isSuspended())
				runList[i]->Run();
			 } else if (runList[i]->autoRemove()) {
				 runList[i] = 0;
			 }
		 }
	 }
	 return hasRunnables;
}

void RunnableManager::runWhileThreadsExist() {
	while(runOnce())
			;
}

RunnableManager& getRunnableManager() {
	static RunnableManager rm;
	return rm;
}

