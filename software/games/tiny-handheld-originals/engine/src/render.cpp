#include "commons.h"
#include "render.h"
#include "board/display.h"

namespace th
{
	namespace render
	{
		RenderCallback m_renderCallback;
		uint8_t m_pageR;

		memory::Binary m_tileBank;
		uint8_t  m_tileWidth;
		uint8_t  m_fontFlags;
		uint8_t  m_asciiBase;

		uint8_t* m_renderBuffer;
		uint8_t  m_page;
		uint8_t  m_pageY;
		uint8_t  m_oddFrame;

		////////////////////////////////////////////////////////////////////////

		static bool isOnThisPage(uint8_t y)
		{ 
			return y >> 3 == m_page;
		}

		static bool isOnNextPage(uint8_t y)
		{
			return m_pageY >= y && m_pageY < y + 8;
		}

		static bool isNotVisibleForRender(uint8_t y, uint8_t h)
		{
			return m_pageY + 8 <= y || m_pageY >= y + h;
		}

		static uint8_t reverseBits(uint8_t b)
		{
			b = (((b & 0xAAu) >> 1) | ((b & 0x55u) << 1));
			b = (((b & 0xCCu) >> 2) | ((b & 0x33u) << 2));
			b = (((b & 0xF0u) >> 4) | ((b & 0x0Fu) << 4));
			return b;
		}

		static void fetchTileData(TileAddr ta, TileFlags tf, uint8_t bi, uint8_t &tb, uint8_t &tm)
		{
			if (tf & TF_FLIP_X)
				ta += m_tileWidth - 1 - bi;
			else
				ta += bi;

			tb = m_tileBank[ta];

			if (tf & TF_HAS_MASK)
				tm = m_tileBank[ta += m_tileWidth];
			else if (tf & TF_TRANSPARENT)
				tm = tb;
			else
				tm = 0xFF;

			if ((tf & TF_HAS_ODD_BM) && m_oddFrame)
				tb = m_tileBank[ta + m_tileWidth];

			if (tf & TF_FLIP_Y)
			{
				tb = reverseBits(tb);
				tm = reverseBits(tm);
			}

			if (tf & TF_INVERSE)
			{
				tb = ~tb;
				tb &= tm;
			}
		}

		static TileAddr getTileAddr(TileIndx ti, TileFlags tf)
		{
			uint16_t ts = m_tileWidth;
			if (tf & TF_HAS_MASK)   ts += m_tileWidth;
			if (tf & TF_HAS_ODD_BM)	ts += m_tileWidth;
			return ti * ts;
		}

		////////////////////////////////////////////////////////////////////////

		void init()
		{
			m_oddFrame = false;
			m_renderBuffer = NULL;
			setRenderCallback(NULL);
		}

		void update()
		{
			if (m_renderCallback)
			{
				// prepare rendering buffer
				uint8_t buf[129] = { 0x40 };
				m_renderBuffer = &buf[0x01];

				// prepare rendering configuration 
				uint8_t pageF = m_pageR >> 4;
				uint8_t pageL = m_pageR & 0x0F;
				RenderCallback renderCallback = m_renderCallback;

				// do actual rendering page by page
				for (m_page = pageF; m_page <= pageL; ++m_page)
				{
					m_pageY = m_page << 3;
					display::writeCmd(0xb0 | m_page);
		 			display::writeCmd(0x00);
		 			display::writeCmd(0x10);

					// call same rendering pipeline for every page
					renderCallback();
					display::writeBuf(buf, sizeof(buf));
				}
				m_renderBuffer = NULL;
			}
			m_oddFrame ^= true;
		}

		void setRenderCallback(RenderCallback renderCallback, uint8_t pageRange = 0x07)
		{
			// Rendering configuration may be changed at the end of rendering
			// pipeline. In this case it will be applied on the next frame.
			m_renderCallback = renderCallback;
			m_pageR = pageRange & 0x77;
		}

		void setTileBank(const memory::Binary& tileBank, uint8_t tileWidth)
		{
			m_tileBank  = tileBank;
			m_tileWidth = tileWidth;
		}

		void setFontData(const FontData &fontData)
		{
			m_tileBank  = memory::Binary::InFLASH(fontData.tileBank); // TODO !!!
			m_fontFlags = fontData.tileFlags & (TF_CONFIG_BITS);
			m_tileWidth = fontData.tileWidth;
			m_asciiBase = fontData.asciiBase;
		}

		void renderTile(TileFlags tf, uint8_t x, uint8_t y, TileAddr ta)
		{
			uint8_t bs, bi, tb, tm;
			if (isOnThisPage(y))
			{
				// Case #1 : top tile half
				bs = y & 0x07;
				for (bi = 0; bi < m_tileWidth && x < 128; ++bi, ++x)
				{
					fetchTileData(ta, tf, bi, tb, tm);
					m_renderBuffer[x] &= ~(tm << bs);
					m_renderBuffer[x] |= tb << bs;
				}
			}
			else if (isOnNextPage(y))
			{
				// Case #2 : bottom tile half
				bs = m_pageY - y;
				for (bi = 0; bi < m_tileWidth && x < 128; ++bi, ++x)
				{
					fetchTileData(ta, tf, bi, tb, tm);
					m_renderBuffer[x] &= ~(tm >> bs);
					m_renderBuffer[x] |= tb >> bs;
				}
			}
		}

		void renderChar(TileFlags tf, uint8_t x, uint8_t y, char ch)
		{
			tf &= TF_CONTROL_BITS;
			tf |= m_fontFlags;
			renderTile(tf, x, y, getTileAddr(ch - m_asciiBase, tf));
		}

		void renderText(TileFlags tf, uint8_t x, uint8_t y, const char *text, uint8_t len)
		{
			uint8_t i;
			if (isOnThisPage(y) || isOnNextPage(y))
			{
				tf &= TF_CONTROL_BITS;
				tf |= m_fontFlags;

				for (i = 0; i < len && text[i]; ++i, x += m_tileWidth)
				{
					renderTile(tf, x, y, getTileAddr(text[i] - m_asciiBase, tf));
				}
			}
		}

		// TODO: there must be a more efficient way
		void renderBitmap(TileFlags tf, uint8_t x, uint8_t y, uint8_t w, uint8_t h, const memory::Binary &bitmap)
		{
			uint8_t yh, ti;
			if (isNotVisibleForRender(y, h)) return;

			setTileBank(bitmap, w);
			for (yh = y + h, ti = 0; y < yh; y += 8, ++ti)
			{
				renderTile(tf, x, y, getTileAddr(ti, tf));
			}
		}

		void renderPattern(TileFlags tf, uint8_t x, uint8_t y, uint8_t w, uint8_t h, TileIndx ti)
		{
			TileAddr ta = getTileAddr(ti, tf);
			// TODO
		}

		////////////////////////////////////////////////////////////////////////

	}
} 
