// Low level access to the hardware external EEPROM chip

#pragma once

namespace th
{
	namespace eeprom
	{
		void init();
		void update();

		uint8_t readByte(uint16_t src);
		void readBlock(uint16_t src, uint8_t* dst, size_t size);
		
		void writeByte(uint16_t dst, uint8_t data);
		void writeBlock(const uint8_t* src, uint16_t dst, size_t size);
		
	}
}
