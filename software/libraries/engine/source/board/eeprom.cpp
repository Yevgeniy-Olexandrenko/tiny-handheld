#include "commons.h"
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

		uint8_t readByte(uint16_t src)
		{
			if (!m_comm || src != m_addr)
			{
				stopCommunication();
				startCommunication(src);
			}
			return readNextByte();
		}

		void readBlock(uint16_t src, uint8_t* dst, size_t size)
		{
			if (!m_comm || src != m_addr)
			{
				stopCommunication();
				startCommunication(src);
			}
			while(size-- > 0) 
			{
				(*dst++) = readNextByte();
			}
		}

		void writeByte(uint16_t dst, uint8_t data)
		{
			//
		}

		void writeBlock(const uint8_t* src, uint16_t dst, size_t size)
		{
			//
		}
	}
}
