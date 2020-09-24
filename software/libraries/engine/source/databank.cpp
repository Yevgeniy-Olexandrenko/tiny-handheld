#include "commons.h"
#include "databank.h"

namespace th
{
	namespace video
	{
		TileBank::TileBank()
			: m_type(memory::NONE)
			, m_addr(NULL)
			, m_format(0)
		{}

		void TileBank::connect()
		{
			//
		}

		void TileBank::disconnect()
		{
			//
		}

		void TileBank::get(TileAddr ta, u08& b0) const
		{
			if (m_type == memory::STORAGE)
			{
				b0 = reinterpret_cast<uint8_s *>(m_addr)[ta];
			}
			else if (m_type == memory::MCU_FLASH)
			{
				b0 = reinterpret_cast<uint8_f *>(m_addr)[ta];
			}
		}

		void TileBank::get(TileAddr ta, u08& b0, u08& b1) const
		{
			if (m_type == memory::STORAGE)
			{
				uint8_s *data = reinterpret_cast<uint8_s *>(m_addr) + ta;
				b0 = *data++;
				b1 = *data;
			}
			else if (m_type == memory::MCU_FLASH)
			{
				uint8_f *data = reinterpret_cast<uint8_f *>(m_addr) + ta;
				b0 = *data++;
				b1 = *data;
			}
		}

		void TileBank::get(TileAddr ta, u08& b0, u08& b1, u08& b2) const
		{
			if (m_type == memory::STORAGE)
			{
				uint8_s *data = reinterpret_cast<uint8_s *>(m_addr) + ta;
				b0 = *data++;
				b1 = *data++;
				b2 = *data;
			}
			else if (m_type == memory::MCU_FLASH)
			{
				uint8_f *data = reinterpret_cast<uint8_f *>(m_addr) + ta;
				b0 = *data++;
				b1 = *data++;
				b2 = *data;
			}
		}
	}
}