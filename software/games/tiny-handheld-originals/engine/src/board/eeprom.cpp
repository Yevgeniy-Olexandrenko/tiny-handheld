#include "../commons.h"
#include "eeprom.h"

namespace th
{
	namespace eeprom
	{
		void init()   {}
		void update() {}

		uint8_t readByte(const void *p)
		{
			// TODO
			return 0xAA;
		}

		void readBlock(void *dest, const void *src, size_t n)
		{
			// TODO
		}
	}
}
