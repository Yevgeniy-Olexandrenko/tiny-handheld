#include "commons.h"
#include "video.h"
#include "board/display.h"

namespace th
{
	namespace video
	{
		RenderCallback m_renderCallback;
		uint8_t m_pageR;

		TileBank m_tileBank;
		uint8_t  m_tileWidth;
		uint8_t  m_asciiBase;

		uint8_t* m_renderBuffer;
		uint8_t  m_page;
		uint8_t  m_pageY;
		uint8_t  m_oddFrame;

		Axis m_scrollX;
		Axis m_scrollY;

		////////////////////////////////////////////////////////////////////////

		static bool isRenderable(Axis y)
		{
			// TODO: maybe it can be optimized?
			return y < m_pageY + 8 && y + 8 > m_pageY;
		}

		static bool isRenderable(Axis y, Size h)
		{
			return y < m_pageY + 8 && y + h > m_pageY;
		}

		static void applyScroll(int8_t& x, int8_t& y)
		{
			x -= m_scrollX;
			y -= m_scrollY;
		}

		static uint8_t reverseBits(uint8_t b)
		{
			b = (((b & 0xAAu) >> 1) | ((b & 0x55u) << 1));
			b = (((b & 0xCCu) >> 2) | ((b & 0x33u) << 2));
			b = (((b & 0xF0u) >> 4) | ((b & 0x0Fu) << 4));
			return b;
		}

		static void fetchTileData(TileAddr ta, RenderFlags rf, uint8_t bi, uint8_t &tb, uint8_t &tm)
		{
			ta += (rf & RF_FLIP_X) ? m_tileWidth - 1 - bi : bi;

			switch (m_tileBank.m_format & TF_BITS_FOR_TYPE)
			{
			default:
				tb = m_tileBank[ta];
				tm = (rf & RF_TRANSP) ? tb : 0xFF;
				break;

			case TF_BM_MASKBM:
				tb = m_tileBank[ta];
				tm = (rf & RF_TRANSP) ? m_tileBank[ta + m_tileWidth] : 0xFF;
				break;

			case TF_BM_ODDBM:
				tb = m_oddFrame ? m_tileBank[ta + m_tileWidth] : m_tileBank[ta];
				tm = (rf & RF_TRANSP) ? tb : 0xFF;
				break;

			case TF_BM_MASKBM_ODDBM:
				tb = m_oddFrame ? m_tileBank[ta + m_tileWidth + m_tileWidth] : m_tileBank[ta];
				tm = (rf & RF_TRANSP) ? m_tileBank[ta + m_tileWidth] : 0xFF;
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
			switch (m_tileBank.m_format & TF_BITS_FOR_TYPE)
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

		static void renderTileFromBank(RenderFlags rf, Axis x, Axis y, TileIndx ti)
		{
			uint8_t bs, bi, tb, tm; TileAddr ta;
			if (y >= 0 && y >> 3 == m_page)
			{
				// Case #1 : tile top half
				bs = y & 0x07;
				ta = getTileAddr(ti);
				for (bi = 0; bi < m_tileWidth && x < 128; ++bi, ++x)
				{
					if (x < 0) continue;
					fetchTileData(ta, rf, bi, tb, tm);
					m_renderBuffer[x] &= ~(tm << bs);
					m_renderBuffer[x] |= tb << bs;
				}
			}
			else if (m_pageY >= y && m_pageY < y + 8)
			{
				// Case #2 : tile bottom half
				bs = m_pageY - y;
				ta = getTileAddr(ti);
				for (bi = 0; bi < m_tileWidth && x < 128; ++bi, ++x)
				{
					if (x < 0) continue;
					fetchTileData(ta, rf, bi, tb, tm);
					m_renderBuffer[x] &= ~(tm >> bs);
					m_renderBuffer[x] |= tb >> bs;
				}
			}
		}

		////////////////////////////////////////////////////////////////////////

		void init()
		{
			m_oddFrame = false;
			m_renderBuffer = NULL;
			setRenderCallback(NULL);
			m_scrollX = m_scrollY = 0;
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

		void setTileBank(const TileBank& tileBank, Size tileWidth)
		{
			m_tileBank  = tileBank;
			m_tileWidth = tileBank.m_format & TF_BITS_FOR_WIDTH;

			if (tileWidth)
				// override tile width with custom value
				m_tileWidth = tileWidth;
			else if (!m_tileWidth)
				// if tile width is still not set, use default value 
				m_tileWidth = 8;
		}

		void setFontBank(const FontBank &fontBank)
		{
			setTileBank(fontBank);
			m_asciiBase = fontBank.m_asciiBase;
		}

		void setScrollXY(Axis sx, Axis sy)
		{
			m_scrollX = sx;
			m_scrollY = sy;
		}

		void renderTile(RenderFlags rf, Axis x, Axis y, TileIndx ti)
		{
			applyScroll(x, y);
			renderTileFromBank(rf, x, y, ti);
		}

		void renderChar(RenderFlags rf, Axis x, Axis y, char ch)
		{
			applyScroll(x, y);
			renderTileFromBank(rf, x, y, ch - m_asciiBase);
		}

		void renderText(RenderFlags rf, Axis x, Axis y, const char *text, uint8_t len)
		{
			applyScroll(x, y);
			if (isRenderable(y))
			{
				for (;len > 0 && (*text); --len, ++text, x += m_tileWidth)
				{
					renderTileFromBank(rf, x, y, (*text) - m_asciiBase);
				}
			}
		}

		void renderPattern(RenderFlags rf, Axis x, Axis y, Size w, Size h, TileIndx ti)
		{
			// TODO
		}

		void renderBitmap(RenderFlags rf, Axis x, Axis y, Size w, Size h, const TileBank& bitmap)
		{
			Axis yh; 
			TileIndx ti;
			applyScroll(x, y);
			if (isRenderable(y, h))
			{
				yh = y + h;
				setTileBank(bitmap, w);
				if (rf & RF_FLIP_Y)
				{
					for (ti = 0; y < yh; ++ti, yh -= 8)
					{
						renderTileFromBank(rf, x, yh - 8, ti);
					}
				}
				else
				{
					for (ti = 0; y < yh; ++ti, y += 8)
					{
						renderTileFromBank(rf, x, y, ti);
					}
				}
			}
		}

		////////////////////////////////////////////////////////////////////////

	}
} 
