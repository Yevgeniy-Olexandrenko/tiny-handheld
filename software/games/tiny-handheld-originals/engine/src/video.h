#pragma once

#include "tilebank.h"

namespace th
{
	namespace video
	{
		typedef int8_t  Axis;
		typedef uint8_t Size;
		typedef uint8_t TileIndx;

		typedef void (*RenderCallback)();
		typedef void (*FillBufferCallback)(uint8_t column, uint8_t* buffer, uint8_t size);
		
		extern uint8_t m_page;
		extern uint8_t m_pageY;

		extern bool m_oddFrame;
		extern Axis m_scrollX;
		extern Axis m_scrollY;

		enum RenderFlags : uint8_t
		{
			RF_FLIP_X  = 0x80,
			RF_FLIP_Y  = 0x40,
			RF_TRANSP  = 0x20,
			RF_INVERSE = 0x10,
			RF_EMPTY   = 0x00
		};

		enum TileFormat : uint8_t
		{
			TF_BM             = 0x00,
			TF_BM_MSKBM       = 0x40,
			TF_BM_ODDBM       = 0x80,
			TF_BM_MSKBM_ODDBM = 0xC0,
			TF_BITS_FOR_TYPE  = 0xC0,
			TF_BITS_FOR_WIDTH = 0x3F
		};

		void init();
		void update();

		void setRenderConfig(RenderCallback renderCallback, uint8_t pageRange = 0x07, uint16_t columnRange = 0x007F);
		void setScrollXY(Axis sx, Axis sy);

		void setTileBank(const TileBank& tileBank, Size tileWidth = 0);
		void setFontBank(const FontBank& fontBank);

		void fillDisplay(uint8_t pattern);
		void fillRenderBuffer(uint8_t pattern);
		void fillRenderBufferDirect(FillBufferCallback fillBufferCallback);
		
		void renderTile(RenderFlags rf, Axis x, Axis y, TileIndx ti);
		void renderChar(RenderFlags rf, Axis x, Axis y, char ch);
		void renderText(RenderFlags rf, Axis x, Axis y, const char *text, uint8_t len);

		void renderPattern(RenderFlags rf, Axis x, Axis y, Size w, Size h, TileIndx ti);
		void renderBitmap(RenderFlags rf, Axis x, Axis y, Size w, Size h, const TileBank& bitmap);
		
	}
}
