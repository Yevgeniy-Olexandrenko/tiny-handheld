#pragma once

#include "memory.h"

namespace th
{
	namespace video
	{
		typedef uint16_t TileAddr;

		struct TileBank
		{
			TileBank();

			template<memory::Type M> 
			TileBank(const memory::Wrapper<M, uint8_t> * addr, uint8_t format)
				: m_type(M)
				, m_addr(addr)
				, m_format(format)
			{}

			static void connect();
			static void disconnect();

			void get(TileAddr ta, uint8_t &b0) const;
			void get(TileAddr ta, uint8_t &b0, uint8_t &b1) const;
			void get(TileAddr ta, uint8_t &b0, uint8_t &b1, uint8_t &b2) const;

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