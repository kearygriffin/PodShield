/*
 * Module.cpp
 *
 *  Created on: Aug 21, 2009
 *      Author: keary
 */

#include "Module.h"
#include "ModuleManager.h"

Module::Module(bool autoRegister, bool startSuspended) : ProtoThread(startSuspended) {
	if (autoRegister) {
		getModuleManager().addModule(this);
	}
}
