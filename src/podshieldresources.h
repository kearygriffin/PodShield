
#ifndef PODSHIELD_RESOURCES_H
#define PODSHIELD_RESOURCES_H

#define	MIN_STACKSIZE	768

#if defined(SHIELD_REV_1)
#define NVRAM_SIZE	0
#define SHIELD_REVISION	1
#elif defined(SHIELD_REV_2)
#define SHIELD_REVISION	2
#define NVRAM_SIZE	16384
#else
#error Shield revision not defined.
#endif


#if !defined(SHIELD_1280) && !defined(SHIELD_168p) && !defined(SHIELD_328p) && !defined(STANDALONE_644p)
#if defined(__AVR_ATmega1280__)
#define SHIELD_1280	1
#elif defined(__AVR_ATmega168P__)
#define	SHIELD_168p 1
#elif defined(__AVR_ATmega328P__)
#define SHIELD_328p	1
#elif defined(__AVR_ATmega644P__)
#define STANDALONE_644p	1
#elif defined(SHIELD_REV_1)
#define SHIELD_1280	1
#else
#error ATmega revison not specified
#endif
#endif

#if	defined(SHIELD_1280)
#define EEPROM_SIZE (4*1024)
#define SRAM_SIZE	(8*1024)
#define FLASH_SIZE_TOTAL	(1024*128)
#define FLASH_SIZE	(FLASH_SIZE_TOTAL-(4*1024))
#elif defined(SHIELD_168p)
#define	EEPROM_SIZE	512
#define	SRAM_SIZE	1024
#define FLASH_SIZE_TOTAL	(16*1024)
#define FLASH_SIZE	(FLASH_SIZE_TOTAL-(2*1024))
#elif defined(SHIELD_328p)
#define	EEPROM_SIZE	1024
#define	SRAM_SIZE	(2*1024)
#define FLASH_SIZE_TOTAL	(32*1024)
#define FLASH_SIZE	(FLASH_SIZE_TOTAL-(2*1024))
#elif defined(STANDALONE_644p)
#define EEPROM_SIZE (2*1024)
#define SRAM_SIZE	(4*1024)
#define FLASH_SIZE_TOTAL	(1024*64)
#define FLASH_SIZE	(FLASH_SIZE_TOTAL-(4*1024))
#endif


#endif
