/*
 * PodModule.h
 *
 *  Created on: Sep 3, 2009
 *      Author: keary
 */

#ifndef PODMODULE_H_
#define PODMODULE_H_

#include "Module.h"
#include "IPodInterface.h"

#define MODULE_ID(x,y) (x + (y << 8))

class PodModule: public Module, public IPodInterface {
public:
	PodModule();
	virtual char *getModuleProperty(char *str);
	virtual char *getPodModuleName() = 0;
	virtual char *getPodModuleShortName() = 0;
	virtual char *getPodModuleIdString();
	virtual unsigned int getPodModuleId() = 0;
	virtual bool isInited() = 0;
	virtual bool isRetryingInit() = 0;
protected:
	char id_str[2];
};

#endif /* PODMODULE_H_ */
