#pragma once

#include "databank.h"

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
		
		typedef uint8_t RenderFlags;
		const RenderFlags RF_FLIP_X  = 0x80;
		const RenderFlags RF_FLIP_Y  = 0x40;
		const RenderFlags RF_TRANSP  = 0x20;
		const RenderFlags RF_INVERSE = 0x10;
		const RenderFlags RF_EMPTY   = 0x00;

		typedef uint8_t TileFormat;
		const TileFormat TF_BM             = 0x00;
		const TileFormat TF_BM_MSKBM       = 0x40;
		const TileFormat TF_BM_ODDBM       = 0x80;
		const TileFormat TF_BM_MSKBM_ODDBM = 0xC0;
		const TileFormat TF_BITS_FOR_TYPE  = 0xC0;
		const TileFormat TF_BITS_FOR_WIDTH = 0x3F;

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
