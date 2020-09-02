#pragma once

#include "memory.h"

namespace th
{
	namespace render
	{
		// rendering sequence type defs and rendering context
		typedef void (*RenderLayerCallback)();
		typedef const RenderLayerCallback * RenderSequence;

		extern uint8_t m_page, m_column, m_pageY, m_columnX; 
		extern uint8_t m_bits, m_mask, m_composed;
		extern uint8_t m_oddFrame;

		// tile data access type defs
		typedef uint16_t TileAddr;
		typedef uint8_t  TileIndx;

		// tile data type defs
		enum TileFlags : uint16_t
		{
			TF_EMPTY        = 0x0000,
			TF_CONFIG_BITS  = 0xF000,
			TF_CONTROL_BITS = 0x0F00,
			TF_WIDTH_BITS   = 0x00FF,

			TF_HAS_MASK     = 0x8000,
			TF_HAS_ODD_BM   = 0x4000,
			
			TF_FLIP_Y       = 0x0800,
			TF_FLIP_X       = 0x0400,
			TF_TRANSPARENT  = 0x2000,
			TF_INVERSE      = 0x0100
		};

		// font data type defs
		struct FontData
		{
			const uint8_t* tileBank;
			TileFlags tileFlags;
			uint8_t asciiBase;
		};

		void init();
		void update();

		void setRenderSequence(RenderSequence renderSequence);
		void flushRenderContext();

		void setScrollX(int8_t sx);
		void setTileBank(const memory::Binary& tileBank);
		TileAddr getTileAddr(TileFlags tf, TileIndx ti);

		void renderTile(TileAddr tileAddr, TileFlags tf, uint8_t x, uint8_t y);
		void renderText(const FontData &fontData, TileFlags tf, int8_t x, int8_t y, const char *text, uint8_t len);
	}
}
