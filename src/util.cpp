#include "podshield.h"

#include <stdlib.h>
#include <stdarg.h>
#include "pgmspace.h"
#include "serports.h"
#include "ModuleManager.h"
#include "NVRamManager.h"

static char vsbuf[80];
char debug_buf1[80];
void debug(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vsprintf(vsbuf, fmt, args);
	DebugSerial->println(vsbuf);
	va_end(args);
}

extern "C" void setup() {

	// For NVRam modules to allocate memory
	getNVRamManager().chk_init();

	// Init modules here...
	if (DebugSerial != NULL) {
		DebugSerial->setBaud(DEBUG_BAUDRATE);
	}
	getModuleManager().init_modules();
}

extern "C" void loop() {
	getModuleManager().module_loop();
}
