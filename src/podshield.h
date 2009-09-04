
#ifndef PODSHIELD_H
#define PODSHIELD_H
#include "podshieldconfig.h"
#include "podshieldresources.h"
#include "serports.h"
#include "pgmspace.h"

#include <stdarg.h>
#ifndef STANDALONE_PC
#ifndef	_SKIP_WPROGRAM_H
#include <WProgram.h>
#endif
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef STANDALONE_PC
#include "pc.h"
#endif

#endif

extern void debug(const char *fmt, ...);
extern char debug_buf1[];

#define debug_P(x, ...) debug(strcpy_P(debug_buf1, x), ## __VA_ARGS__)

#define DEBUG_BAUDRATE	57600

#ifdef STANDALONE_PC
#define fatal_error(x) do { puts(x); exit(-1); } while(0)
#else
#define	fatal_error(x)
#endif
