// Low level access to the hardware external EEPROM chip

#pragma once

namespace th
{
	namespace eeprom
	{
		void init();
		void update();

		uint8_t readByte(const uint8_t * p);
	}
}
