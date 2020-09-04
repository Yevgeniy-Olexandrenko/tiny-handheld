// Low level access to the hardware external EEPROM chip

#pragma once

namespace th
{
	namespace eeprom
	{
		void init()   DO_NOTHING
		void update() DO_NOTHING

		uint8_t readByte(const uint8_t * p);
	}
}
