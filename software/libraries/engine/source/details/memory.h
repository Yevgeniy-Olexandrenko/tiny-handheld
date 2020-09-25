#pragma once

#include "commons.h"
#include "memory/access.h"
#include "memory/pgmem.h"
#include "memory/eemem.h"
#include "memory/storage.h"

#ifdef USE_EXT_STORAGE
	#define IN_FLASH   PROGMEM
	#define IN_EEPROM  EEMEM
	#define IN_STORAGE EEMEM
#else
	#define IN_FLASH   PROGMEM
	#define IN_EEPROM  EEMEM
	#define IN_STORAGE PROGMEM
#endif

#define BINARY_IN_FLASH(name)   uint8_p name[] IN_FLASH
#define BINARY_IN_STORAGE(name) uint8_s name[] IN_STORAGE
