#pragma once

#include "memory.h"

namespace th
{
	namespace video
	{
		// rendering sequence type defs and rendering context
		typedef void (*RenderCallback)();
		extern uint8_t* m_renderBuffer;
		extern uint8_t  m_page;
		extern uint8_t  m_pageY;
		extern uint8_t  m_oddFrame;

		enum RenderFlags : uint8_t
		{
			RF_FLIP_X  = 0x80,
			RF_FLIP_Y  = 0x40,
			RF_TRANSP  = 0x20,
			RF_INVERSE = 0x10,
			RF_EMPTY   = 0x00
		};

		// tile data access type defs
		typedef uint16_t TileAddr;
		typedef uint8_t  TileIndx;

		enum TileFormat : uint8_t
		{
			TF_BM              = 0x00,
			TF_BM_MASKBM       = 0x40,
			TF_BM_ODDBM        = 0x80,
			TF_BM_MASKBM_ODDBM = 0xC0,

			TF_BITS_FOR_TYPE  = 0xC0,
			TF_BITS_FOR_WIDTH = 0x3F
		};

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

		void init();
		void update();

		void setRenderCallback(RenderCallback renderCallback, uint8_t pageRange = 0x07);
		void setTileBank(const TileBank& tileBank, uint8_t tileWidth = 0);
		void setFontBank(const FontBank& fontBank);
		
		void renderTile(RenderFlags rf, uint8_t x, uint8_t y, TileIndx ti);
		void renderChar(RenderFlags rf, uint8_t x, uint8_t y, char ch);
		void renderText(RenderFlags rf, uint8_t x, uint8_t y, const char *text, uint8_t len);

		void renderPattern(RenderFlags rf, uint8_t x, uint8_t y, uint8_t w, uint8_t h, TileIndx ti);
		void renderBitmap(RenderFlags rf, uint8_t x, uint8_t y, uint8_t w, uint8_t h, const TileBank& bitmap);
		
	}
}
