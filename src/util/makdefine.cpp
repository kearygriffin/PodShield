#include <stdio.h>
#include <stdlib.h>
#include "../podshieldconfig.h"
#include "../podshieldresources.h"

int main(int argc, char * argv[]) {

	printf("SHIELD_REVISION=%d\n", SHIELD_REVISION);
#if defined(SHIELD_1280)
	printf("MCU=atmega1280\n");
#elif defined(SHIELD_168p)
	printf("MCU=atmega168p\n");
#elif defined(SHIELD_328p)
	printf("MCU=atmega328p\n");
#elif defined(STANDALONE_644p)
	printf("MCU=atmega644p\n");
	printf("SHIELD_STANDALONE=1\n");
	#else
#error "Shield type undefined.  Can't make"
#endif

}

