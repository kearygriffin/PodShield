#include "podshield.h"

#ifdef STANDALONE_PC
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

unsigned long millis() {
	struct timespec t;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t);
	unsigned long m = t.tv_sec*1000L;
	m += t.tv_nsec / 1000000L;
	//printf("m: %ld\n", m);
	return m;
}
void delay(long d) {
	long tim = millis() + d;
	while(millis() < tim)
		;
}

void init() {

}

#endif
