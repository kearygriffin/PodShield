/*
 * Module.h
 *
 *  Created on: Aug 21, 2009
 *      Author: keary
 */

#ifndef MODULE_H_
#define MODULE_H_

#include "ProtoThread.h"


class Module : public ProtoThread {
public:
	Module(bool autoRegister=true, bool startSuspended = false);
	virtual void initModule() { };

	virtual char *getModuleProperty(char *str);
	virtual char *getModuleProperty_p(PROGMEM char *str);
};

#endif /* MODULE_H_ */
