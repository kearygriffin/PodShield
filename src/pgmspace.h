#ifndef _PODPGMSPACE_H
#define _POD_PGMSPACE_H
#ifdef STANDALONE_PC

#define PROGMEM
#define PSTR(x) x
#define memcpy_P(x, y, s) memcpy(x, y, s)
#define strncpy_P(x, y, s) strncpy(x, y, s)
#define strcpy_P(x, y) strcpy(x,y)
#else
#include <avr/pgmspace.h>
#endif
#endif
