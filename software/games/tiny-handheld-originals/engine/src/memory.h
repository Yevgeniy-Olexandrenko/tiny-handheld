#pragma once

#define IN_FLASH     PROGMEM
#define IN_IntEEPROM EEMEM
#define IN_ExtEEPROM EEMEM

namespace th
{
	namespace memory
	{
		class Binary final
		{
			Binary(const uint8_t* data, uint8_t storage);

		public:
			static Binary InFLASH(const uint8_t* data);
			static Binary InIntEEPROM(const uint8_t* data);
			static Binary InExtEEPROM(const uint8_t* data);

			Binary();
			uint8_t operator[](uint16_t index) const;

		private:
			uint16_t m_address;
			uint8_t  m_storage;
		};
	}
} 
