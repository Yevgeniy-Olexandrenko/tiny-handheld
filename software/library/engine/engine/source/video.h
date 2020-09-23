#pragma once

#include "databank.h"

namespace th
{
	namespace video
	{
		typedef s08 Axis;
		typedef u08 Size;
		typedef u08 TileIndex;
		typedef u08 Bitmap;

		typedef void (*RenderCallback)();
		typedef void (*FillBufferCallback)(u08 column, p08 buffer, u08 size);
		
		extern bool m_oddFrame;
		extern Axis m_scrollX;
		extern Axis m_scrollY;
		
		typedef u08 RenderFlags;
		const RenderFlags RF_FLIP_X  = 0x80;
		const RenderFlags RF_FLIP_Y  = 0x40;
		const RenderFlags RF_TRANSP  = 0x20;
		const RenderFlags RF_INVERSE = 0x10;
		const RenderFlags RF_EMPTY   = 0x00;

		typedef u08 TileFormat;
		const TileFormat TF_BM             = 0x00;
		const TileFormat TF_BM_MSKBM       = 0x40;
		const TileFormat TF_BM_ODDBM       = 0x80;
		const TileFormat TF_BM_MSKBM_ODDBM = 0xC0;
		const TileFormat TF_BITS_FOR_TYPE  = 0xC0;
		const TileFormat TF_BITS_FOR_WIDTH = 0x3F;

		void init();
		void update();

		void setRenderConfig(RenderCallback renderCallback, u08 pageRange = 0x07, u16 columnRange = 0x007F);
		void setScrollXY(Axis sx, Axis sy);

		void setTileBank(const TileBank& tileBank, Size tileWidth = 0);
		void setFontBank(const FontBank& fontBank);

		void fillDisplay(u08 pattern);
		void fillRenderBuffer(u08 pattern);
		void fillRenderBufferDirect(FillBufferCallback fillBufferCallback);
		
		void renderTile(RenderFlags rf, Axis x, Axis y, TileIndex ti);
		void renderChar(RenderFlags rf, Axis x, Axis y, char ch);
		void renderText(RenderFlags rf, Axis x, Axis y, const char *text, u08 len);

		void renderPattern(RenderFlags rf, Axis x, Axis y, Size w, Size h, TileIndex ti);
		void renderBitmap(RenderFlags rf, Axis x, Axis y, Size w, Size h, const TileBank& bitmap);
		
	}
}
