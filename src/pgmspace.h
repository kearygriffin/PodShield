#ifndef _PODPGMSPACE_H
#define _POD_PGMSPACE_H
#ifdef STANDALONE_PC

#define PROGMEM
#define memcpy_P(x, y, s) memcpy(x, y, s)
#else
#include <avr/pgmspace.h>
#endif
#endif
