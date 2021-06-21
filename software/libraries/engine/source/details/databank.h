#pragma once

#include "memory.h"

namespace th
{
	namespace video
	{
		struct TileBank_f
		{
			uint8_p* m_addr;
			uint8_p  m_format;
		};

		struct TileBank_s
		{
			const uint8_s* m_addr;
			const uint8_s  m_format;
		};

		struct FontBank_f
		{
			uint8_p* m_addr;
			uint8_p  m_format;
			uint8_p  m_asciiBase;
		};
		
		struct FontBank_s
		{
			uint8_s* m_addr;
			uint8_s  m_format;
			uint8_s  m_asciiBase;
		};
	}
}

#define TILEBANK_IN_FLASH(name)   const th::video::TileBank_f name IN_FLASH
#define TILEBANK_IN_STORAGE(name) const th::video::TileBank_s name IN_STORAGE

#define FONTBANK_IN_FLASH(name)   const th::video::FontBank_f name IN_FLASH
#define FONTBANK_IN_STORAGE(name) const th::video::FontBank_s name IN_STORAGE
