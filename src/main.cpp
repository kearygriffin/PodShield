#include "podshield.h"
#ifndef	STANDALONE_PC
#include <WProgram.h>
#endif

extern "C" void init();
extern "C" void setup();
extern "C" void loop();

#ifdef STANDALONE_PC
void init() {

}
#endif

extern "C" void __cxa_pure_virtual()
{
//cli();
for (;;);
}

/*
extern "C" void * operator new(size_t size)
{
  return malloc(size);
}

extern "C" void operator delete(void * ptr)
{
  free(ptr);
}
*/


int main(void) {

	init();

	/* Add setup code here */
	setup();

for (;;) {
	/* write main loop here */
	loop();
	}

}

