#include "commons.h"
#include "tilebank.h"

namespace th
{
	namespace video
	{
		TileBank::TileBank()
			: m_type(memory::Type::NONE)
			, m_addr(NULL)
			, m_format(0)
		{}

		TileBank::TileBank(const TileBank_f& tilebank)
			: TileBank(tilebank.m_addr, tilebank.m_format)
		{}

		TileBank::TileBank(const TileBank_s& tilebank)
			: TileBank(tilebank.m_addr, tilebank.m_format)
		{}

		void TileBank::connect()
		{
			//
		}

		void TileBank::disconnect()
		{
			//
		}

		void TileBank::get(TileAddr ta, uint8_t &b0) const
		{
			if (m_type == memory::Type::STORAGE)
			{
				b0 = static_cast<uint8_s *>(m_addr)[ta];
			}
			else if (m_type == memory::Type::MCU_FLASH)
			{
				b0 = static_cast<uint8_f *>(m_addr)[ta];
			}
		}

		void TileBank::get(TileAddr ta, uint8_t &b0, uint8_t &b1) const
		{
			if (m_type == memory::Type::STORAGE)
			{
				uint8_s *data = static_cast<uint8_s *>(m_addr);
				b0 = data[ta++];
				b1 = data[ta];
			}
			else if (m_type == memory::Type::MCU_FLASH)
			{
				uint8_f *data = static_cast<uint8_f *>(m_addr);
				b0 = data[ta++];
				b1 = data[ta];
			}
		}

		void TileBank::get(TileAddr ta, uint8_t &b0, uint8_t &b1, uint8_t &b2) const
		{
			if (m_type == memory::Type::STORAGE)
			{
				uint8_s *data = static_cast<uint8_s *>(m_addr);
				b0 = data[ta++];
				b1 = data[ta++];
				b2 = data[ta];
			}
			else if (m_type == memory::Type::MCU_FLASH)
			{
				uint8_f *data = static_cast<uint8_f *>(m_addr);
				b0 = data[ta++];
				b1 = data[ta++];
				b2 = data[ta];
			}
		}

		FontBank::FontBank(const FontBank_f& fontbank)
			: FontBank(fontbank.m_addr, fontbank.m_format, fontbank.m_asciiBase)
		{}

		FontBank::FontBank(const FontBank_s& fontbank)
			: FontBank(fontbank.m_addr, fontbank.m_format, fontbank.m_asciiBase)
		{}
	}
}