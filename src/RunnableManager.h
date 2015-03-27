/*
 * RunnableManager.h
 *
 *  Created on: Aug 21, 2009
 *      Author: keary
 */

#ifndef RUNNABLEMANAGER_H_
#define RUNNABLEMANAGER_H_
#include "Runnable.h"

#define MAX_RUNNABLES 32

class RunnableManager {
public:
	RunnableManager();
	bool runOnce();
	void runWhileThreadsExist();
	bool removeRunnable(Runnable *runnable);
	bool addRunnable(Runnable *runnable);
protected:
	Runnable* runList[MAX_RUNNABLES];
};

extern RunnableManager& getRunnableManager();


#endif /* RUNNABLEMANAGER_H_ */
