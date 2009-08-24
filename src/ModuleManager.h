/*
 * ModuleManager.h
 *
 *  Created on: Aug 22, 2009
 *      Author: keary
 */

#ifndef MODULEMANAGER_H_
#define MODULEMANAGER_H_
#include "Module.h"

class ModuleManager  {
public:
	ModuleManager();
	void addModule(Module *mod);
};

extern ModuleManager& getModuleManager();

#endif /* MODULEMANAGER_H_ */
