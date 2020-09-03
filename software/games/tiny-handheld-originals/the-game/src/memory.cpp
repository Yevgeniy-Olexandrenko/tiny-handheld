#include "commons.h"
#include "memory.h"
#include "board/eeprom.h"

namespace th
{
	namespace memory
	{
		static Binary Binary::InFLASH(const uint8_t* data)     { return Binary(data, 1); }
		static Binary Binary::InIntEEPROM(const uint8_t* data) { return Binary(data, 2); }
		static Binary Binary::InExtEEPROM(const uint8_t* data) { return Binary(data, 3); }

		Binary::Binary()
			: m_address(0), m_storage(0)
		{}

		Binary::Binary(const uint8_t* data, uint8_t storage)
			: m_address((uint16_t)data), m_storage(storage)
		{}

		uint8_t Binary::operator[](uint16_t index) const
		{
			switch(m_storage)
			{
				case  1: return pgm_read_byte(m_address + index);
				case  2: return eeprom_read_byte(m_address + index);
				case  3: return eeprom::readByte(m_address + index);
				default: return 0x00;
			}
		}
	}
}
