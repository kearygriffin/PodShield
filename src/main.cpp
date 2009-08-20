#ifndef	STANDALONE_PC
#include <WProgram.h>
#endif

extern void init();
extern void setup();
extern void loop();

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

