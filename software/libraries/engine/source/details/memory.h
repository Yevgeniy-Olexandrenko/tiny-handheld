#pragma once

#include "memory/access.h"
#include "memory/pgmem.h"
#include "memory/eemem.h"
#include "memory/storage.h"

//#define USE_EXT_STORAGE

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

namespace th
{
	namespace memory
	{
		class Binary
		{
			enum { Unknown, Flash, Storage };
			typedef const void* Address;
			typedef u08 MemType;
			typedef u16 Index;
			
		public:
			Binary();
			Binary(const uint8_p* addr);
			Binary(const uint8_s* addr);
			
			void startMultiAccess() const;
			void stopMultiAccess() const;
			
			void get(Index i, u08& b0) const;
			void get(Index i, u08& b0, u08& b1) const;
			void get(Index i, u08& b0, u08& b1, u08& b2) const;
			
		private:
			MemType m_type;
			Address m_addr;
		};
	}
}