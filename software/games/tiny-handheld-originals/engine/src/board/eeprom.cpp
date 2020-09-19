#include "../commons.h"
#include "eeprom.h"

namespace th
{
	namespace eeprom
	{
		uint16_t m_addr; // addres of next byte to be read
		bool     m_comm; // is I2C communication in progress

		static void startCommunication(uint16_t addr)
		{
			m_addr = addr;
			// start I2C communication
			m_comm = true;
		}

		static void stopCommunication()
		{
			if (m_comm)
			{
				// stop I2C communication
				m_comm = false;
			}
		}

		static uint8_t readNextByte()
		{
			++m_addr;
			return 0xAA;
		}

		void init()
		{
			m_addr = 0;
			m_comm = false;
		}

		void update() {}

		uint8_t readByte(uint16_t addr)
		{
			if (!m_comm || addr != m_addr)
			{
				stopCommunication();
				startCommunication(addr);
			}
			return readNextByte();
		}

		void readBlock(uint16_t addr, uint8_t *dest, size_t n)
		{
			if (!m_comm || addr != m_addr)
			{
				stopCommunication();
				startCommunication(addr);
			}
			while(n-- > 0) 
			{
				(*dest++) = readNextByte();
			}
		}
	}
}
