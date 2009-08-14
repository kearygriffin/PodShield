#include <WProgram.h>

extern void init();
extern void setup();
extern void loop();

void __cxa_pure_virtual()
{
cli();
for (;;);
}

int main(void) {

	init();

	/* Add setup code here */
	setup();

for (;;) {
	/* write main loop here */
	loop();
	}

}


