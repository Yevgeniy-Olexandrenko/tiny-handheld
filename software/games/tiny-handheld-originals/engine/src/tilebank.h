#pragma once

#include "memory.h"

namespace th
{
	namespace video
	{
		typedef uint16_t TileAddr;

		struct TileBank
		{
			TileBank()
				: m_type(memory::Type::NONE)
				, m_addr(NULL)
				, m_format(0)
			{}

			template<memory::Type M> 
			TileBank(const memory::Wrapper<M, uint8_t> * addr, uint8_t format)
				: m_type(M)
				, m_addr(addr)
				, m_format(format)
			{}

			uint8_t operator[](TileAddr ta) const
			{
				if (m_type == memory::Type::STORAGE)
					return static_cast<uint8_s *>(m_addr)[ta];
				if (m_type == memory::Type::MCU_FLASH)
					return static_cast<uint8_f *>(m_addr)[ta];
				return 0xFF;
			}

			uint8_t m_format;
			uint8_t m_type;
			void*   m_addr;
		};

		struct FontBank : TileBank
		{
			template<memory::Type M> 
			FontBank(const memory::Wrapper<M, uint8_t> * addr, uint8_t format, uint8_t asciiBase)
				: TileBank(addr, format)
				, m_asciiBase(asciiBase)
			{}

			uint8_t m_asciiBase;
		};
	}
}