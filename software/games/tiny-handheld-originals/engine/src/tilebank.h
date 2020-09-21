#pragma once

#include "memory.h"

namespace th
{
	namespace video
	{
		struct TileBank_f
		{
			uint8_f* m_addr;
			uint8_f  m_format;
		};

		struct TileBank_s
		{
			uint8_s* m_addr;
			uint8_s  m_format;
		};

		struct FontBank_s
		{
			uint8_s* m_addr;
			uint8_s  m_format;
			uint8_s  m_asciiBase;
		};

		struct FontBank_f
		{
			uint8_f* m_addr;
			uint8_f  m_format;
			uint8_f  m_asciiBase;
		};

		typedef uint16_t TileAddr;

		struct TileBank
		{
			TileBank();

			TileBank(const TileBank_f& tb);
			TileBank(const TileBank_s& tb);

			template<memory::Type M> 
			TileBank(const memory::Wrapper<M, uint8_t>* addr, uint8_t format)
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
			FontBank(const FontBank_f& fb);
			FontBank(const FontBank_s& fb);

			template<memory::Type M> 
			FontBank(const memory::Wrapper<M, uint8_t>* addr, uint8_t format, uint8_t asciiBase)
				: TileBank(addr, format)
				, m_asciiBase(asciiBase)
			{}

			uint8_t m_asciiBase;
		};
	}
}

#define TILEBANK_IN_FLASH(name)   const th::video::TileBank_f name IN_FLASH
#define TILEBANK_IN_STORAGE(name) const th::video::TileBank_s name IN_STORAGE

#define FONTBANK_IN_FLASH(name)   const th::video::FontBank_f name IN_FLASH
#define FONTBANK_IN_STORAGE(name) const th::video::FontBank_s name IN_STORAGE
