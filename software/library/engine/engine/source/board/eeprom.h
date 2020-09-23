// Low level access to the hardware external EEPROM chip

#pragma once

namespace th
{
	namespace eeprom
	{
		void init();
		void update();

		uint8_t readByte(uint16_t addr);
		void readBlock(uint16_t addr, uint8_t *dest, size_t n);
	}
}
