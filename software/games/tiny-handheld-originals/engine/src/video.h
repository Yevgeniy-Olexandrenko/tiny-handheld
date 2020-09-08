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

		// tile data access type defs
		typedef uint16_t TileAddr;
		typedef uint8_t  TileIndx;

		struct TileBank
		{
			uint8_f *m_faddr;
			uint8_s *m_saddr;

			TileBank() : m_faddr(NULL), m_saddr(NULL) {}
			//TileBank(uint8_f *addr) : m_faddr(addr), m_saddr(NULL) {}
			TileBank(uint8_s *addr) : m_faddr(NULL), m_saddr(addr) {}

			uint8_t operator[](TileAddr tileAddr) const
			{
				return (m_saddr ? m_saddr[tileAddr] : (m_faddr ? m_faddr[tileAddr] : 0xFF));
			}
		};

		

		enum TileFormat : uint8_t
		{
			TF_BM              = 0x00,
			TF_BM_MASKBM       = 0x40,
			TF_BM_ODDBM        = 0x80,
			TF_BM_MASKBM_ODDBM = 0xC0,

			TF_BITS_FOR_TYPE   = 0xC0,
			TF_BITS_FOR_WIDTH  = 0x3F
		};

		enum RenderFlags : uint8_t
		{
			RF_FLIP_X       = 0x80,
		 	RF_FLIP_Y       = 0x40,
			RF_TRANSPARENT  = 0x20,
			RF_INVERSE      = 0x10,
			RF_EMPTY        = 0x00
		};

		// font data type defs
		struct FontData
		{
			uint8_s *tileBank;
			TileFormat tileFormat;
			uint8_t asciiBase;
		};

		void init();
		void update();

		void setRenderCallback(RenderCallback renderCallback, uint8_t pageRange = 0x07);
		void setTileBank(uint8_s *tileBank, TileFormat tileFormat, uint8_t tileWidth = 0);
		void setFontData(const FontData &fontData);
		
		void renderTile(RenderFlags rf, uint8_t x, uint8_t y, TileIndx ti);
		void renderChar(RenderFlags rf, uint8_t x, uint8_t y, char ch);
		void renderText(RenderFlags rf, uint8_t x, uint8_t y, const char *text, uint8_t len);

		void renderPattern(RenderFlags rf, uint8_t x, uint8_t y, uint8_t w, uint8_t h, TileIndx ti);
		void renderBitmap(RenderFlags rf, uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_s *bitmap);
		
	}
}
