/*
 * Module.cpp
 *
 *  Created on: Aug 21, 2009
 *      Author: keary
 */

#include "podshield.h"
#include "Module.h"
#include "ModuleManager.h"

Module::Module(bool autoRegister, bool startSuspended) : ProtoThread(startSuspended) {
	if (autoRegister) {
		getModuleManager().addModule(this);
	}
}

char *Module::getModuleProperty(char *str) {
	return NULL;
}
char *Module::getModuleProperty_p(PROGMEM char *str) {
	char buf[32];
	strcpy_P(buf, str);
	return getModuleProperty(buf);
}
