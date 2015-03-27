
#include "podshield.h"
#include "PodModule.h"

PodModule::PodModule() {
	id_str[0] = 0;
	id_str[1] = 0;
}
 char *PodModule::getModuleProperty(char *str) {
	if (strcmp(str, "PODMODULE") == 0) {
		return "T";
	} else if (strcmp(str, "PNAME") == 0) {
		return getPodModuleName();
	} else if (strcmp(str, "PSHORTNAME") == 0) {
		return getPodModuleShortName();
	} else if (strcmp(str, "ID") == 0) {
		return	getPodModuleIdString();
	}
	return Module::getModuleProperty(str);
}

char *PodModule::getPodModuleIdString() {
	if (id_str[0] == 0) {
		int id = getPodModuleId();
		id_str[0] = id & 0xff;
		id_str[1] = id >> 8;
	}
	return id_str;
}
