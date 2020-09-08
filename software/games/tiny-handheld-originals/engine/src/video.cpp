#include "commons.h"
#include "video.h"
#include "board/display.h"

namespace th
{
	namespace video
	{
		RenderCallback m_renderCallback;
		uint8_t m_pageR;

		TileBank   m_tileBank;
		TileFormat m_tileFormat;
		uint8_t    m_tileWidth;
		uint8_t    m_asciiBase;

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

		static uint8_t fetchData(TileAddr ta)
		{
			return m_tileBank[ta];
		}

		static void fetchTileData(TileAddr ta, RenderFlags rf, uint8_t bi, uint8_t &tb, uint8_t &tm)
		{
			ta += (rf & RF_FLIP_X) ? m_tileWidth - 1 - bi : bi;

			switch (m_tileFormat & TF_BITS_FOR_TYPE)
			{
			default:
				tb = m_tileBank[ta];
				tm = (rf & RF_TRANSPARENT) ? tb : 0xFF;
				break;

			case TF_BM_MASKBM:
				tb = m_tileBank[ta];
				tm = (rf & RF_TRANSPARENT) ? m_tileBank[ta + m_tileWidth] : 0xFF;
				break;

			case TF_BM_ODDBM:
				tb = m_oddFrame ? m_tileBank[ta + m_tileWidth] : m_tileBank[ta];
				tm = (rf & RF_TRANSPARENT) ? tb : 0xFF;
				break;

			case TF_BM_MASKBM_ODDBM:
				tb = m_oddFrame ? m_tileBank[ta + m_tileWidth + m_tileWidth] : m_tileBank[ta];
				tm = (rf & RF_TRANSPARENT) ? m_tileBank[ta + m_tileWidth] : 0xFF;
				break;
			}

			if (rf & RF_FLIP_Y)
			{
				tb = reverseBits(tb);
				tm = reverseBits(tm);
			}

			if (rf & RF_INVERSE)
			{
				tb = ~tb;
				tb &= tm;
			}
		}

		static TileAddr getTileAddr(TileIndx ti)
		{
			switch (m_tileFormat & TF_BITS_FOR_TYPE)
			{
			default:
				return ti * m_tileWidth;

			case TF_BM_MASKBM:
			case TF_BM_ODDBM:
				return ti * 2 * m_tileWidth;

			case TF_BM_MASKBM_ODDBM:
				return ti * 3 * m_tileWidth;
			}
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

		void setTileBank(const TileBank& tileBank, TileFormat tileFormat, uint8_t tileWidth)
		{
			m_tileBank   = tileBank;
			m_tileFormat = tileFormat;
			m_tileWidth  = tileFormat & TF_BITS_FOR_WIDTH;

			if (tileWidth)
				// override tile width with custom value
				m_tileWidth = tileWidth;
			else if (!m_tileWidth)
				// if tile width is still not set, use default value 
				m_tileWidth = 8;
		}

		void setFontData(const FontData &fontData)
		{
			setTileBank(fontData.tileBank, fontData.tileFormat);
			m_asciiBase = fontData.asciiBase;
		}

		void renderTile(RenderFlags rf, uint8_t x, uint8_t y, TileIndx ti)
		{
			uint8_t bs, bi, tb, tm; TileAddr ta;
			if (isOnThisPage(y))
			{
				// Case #1 : top tile half
				bs = y & 0x07;
				ta = getTileAddr(ti);
				for (bi = 0; bi < m_tileWidth && x < 128; ++bi, ++x)
				{
					fetchTileData(ta, rf, bi, tb, tm);
					m_renderBuffer[x] &= ~(tm << bs);
					m_renderBuffer[x] |= tb << bs;
				}
			}
			else if (isOnNextPage(y))
			{
				// Case #2 : bottom tile half
				bs = m_pageY - y;
				ta = getTileAddr(ti);
				for (bi = 0; bi < m_tileWidth && x < 128; ++bi, ++x)
				{
					fetchTileData(ta, rf, bi, tb, tm);
					m_renderBuffer[x] &= ~(tm >> bs);
					m_renderBuffer[x] |= tb >> bs;
				}
			}
		}

		void renderChar(RenderFlags rf, uint8_t x, uint8_t y, char ch)
		{
			renderTile(rf, x, y, ch - m_asciiBase);
		}

		void renderText(RenderFlags rf, uint8_t x, uint8_t y, const char *text, uint8_t len)
		{
			uint8_t i;
			if (isOnThisPage(y) || isOnNextPage(y))
			{
				for (i = 0; i < len && text[i]; ++i, x += m_tileWidth)
				{
					renderTile(rf, x, y, text[i] - m_asciiBase);
				}
			}
		}

		void renderPattern(RenderFlags rf, uint8_t x, uint8_t y, uint8_t w, uint8_t h, TileIndx ti)
		{
			// TODO
		}

		// TODO: there must be a more efficient way
		void renderBitmap(RenderFlags rf, uint8_t x, uint8_t y, uint8_t w, uint8_t h, const TileBank& bitmap)
		{
			uint8_t yh, ti;
			if (isNotVisibleForRender(y, h)) return;

			setTileBank(bitmap, TF_BM, w);
			if (rf & RF_FLIP_Y)
			{
				for (yh = y + h, ti = 0; yh > y; yh -= 8, ++ti)
				{
					renderTile(rf, x, yh - 8, ti);
				}
			}
			else
			{
				for (yh = y + h, ti = 0; y < yh; y += 8, ++ti)
				{
					renderTile(rf, x, y, ti);
				}
			}
		}

		
		

		////////////////////////////////////////////////////////////////////////

	}
} 
