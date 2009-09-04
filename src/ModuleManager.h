/*
 * ModuleManager.h
 *
 *  Created on: Aug 22, 2009
 *      Author: keary
 */

#ifndef MODULEMANAGER_H_
#define MODULEMANAGER_H_
#include "Module.h"

#define MAX_MODULES 8

class ModuleManager  {
public:
	ModuleManager();
	void addModule(Module *mod);
	void module_loop();
	void init_modules();
private:
	int moduleCnt;
	Module* modules[MAX_MODULES];
};

extern ModuleManager& getModuleManager();

#endif /* MODULEMANAGER_H_ */
