#pragma once

#include "memory.h"

namespace th
{
	namespace render
	{
		// rendering sequence type defs and rendering context
		typedef void (*RenderLayerCallback)();
		typedef const RenderLayerCallback * RenderSequence;

		extern uint8_t* m_renderBuffer;
		extern uint8_t  m_page;
		extern uint8_t  m_pageY;
		extern uint8_t  m_oddFrame;

		// tile data access type defs
		typedef uint16_t TileAddr;
		typedef uint8_t  TileIndx;

		// tile data type defs
		enum TileFlags : uint8_t
		{
			TF_EMPTY        = 0x00,
			TF_CONFIG_BITS  = 0x0F,
			TF_CONTROL_BITS = 0xF0,

			TF_HAS_MASK     = 0x08, // tile has additional bitmap for masking opaque pixels
			TF_HAS_ODD_BM   = 0x04, // tile has additional bitmap for odd rendering frames
			TF_TRANSPARENT  = 0x02,
			TF_CONFIG_BIT0  = 0x01,
			
			TF_FLIP_X       = 0x80,
			TF_FLIP_Y       = 0x40,
			TF_INVERSE      = 0x20,
			TF_CONTROL_BIT4 = 0x10
		};

		// font data type defs
		struct FontData
		{
			const uint8_t* tileBank;
			TileFlags tileFlags;
			uint8_t tileWidth;
			uint8_t asciiBase;
		};

		void init();
		void update();

		void setRenderSequence(RenderSequence renderSequence, uint8_t pageRange = 0x07);
		void setTileBank(const memory::Binary& tileBank, uint8_t tileWidth = 8);
		void setFontData(const FontData &fontData);
		
		void renderTile(TileFlags tf, uint8_t x, uint8_t y, TileAddr ta);
		void renderChar(TileFlags tf, uint8_t x, uint8_t y, char ch);
		void renderText(TileFlags tf, uint8_t x, uint8_t y, const char *text, uint8_t len);
	}
}
