#include "commons.h"
#include "video.h"
#include "board/display.h"

namespace th
{
	namespace video
	{
		RenderCallback m_renderCallback;
		uint8_t  m_pageR;
		uint16_t m_columnR;
		
		TileBank m_tileBank;
		uint8_t  m_tileSize;
		uint8_t  m_tileWidth;
		uint8_t  m_asciiBase;

		uint8_t* m_renderBuffer;
		uint8_t  m_page;
		uint8_t  m_pageY;
		uint8_t  m_columnF;
		uint8_t  m_columnW;
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

		// Tile data fetching procedure optimized to read bytes from storage:
		// 1. Strictly in sequence.
		// 2. Without skipping any data bytes.
		// 3. In the direction of increasing the data address.
		// It is needed for efficient memory access (especially for i2c EEPROM)
		static void fetchTileData(RenderFlags rf, TileAddr ta, uint8_t &tb, uint8_t &tm)
		{
			uint8_t b0, b1, b2;
			switch (m_tileBank.m_format & TF_BITS_FOR_TYPE)
			{
			default:
				tb = m_tileBank[ta];
				tm = (rf & RF_TRANSP) ? tb : 0xFF;
				break;

			case TF_BM_MASKBM:
				tb = m_tileBank[ta++];
				b1 = m_tileBank[ta];
				tm = (rf & RF_TRANSP) ? b1 : 0xFF;
				break;

			case TF_BM_ODDBM:
				b0 = m_tileBank[ta++];
				b1 = m_tileBank[ta];
				tb = m_oddFrame ? b1 : b0;
				tm = (rf & RF_TRANSP) ? tb : 0xFF;
				break;

			case TF_BM_MASKBM_ODDBM:
				b0 = m_tileBank[ta++];
				b1 = m_tileBank[ta++];
				b2 = m_tileBank[ta];
				tb = m_oddFrame ? b2 : b0;
				tm = (rf & RF_TRANSP) ? b1 : 0xFF;
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

		static uint8_t getTileBitmapSize()
		{
			switch (m_tileBank.m_format & TF_BITS_FOR_TYPE)
			{
			default:
				return 1;

			case TF_BM_MASKBM:
			case TF_BM_ODDBM:
				return 2;

			case TF_BM_MASKBM_ODDBM:
				return 3;
			}
		}

		static void getRenderDirection(RenderFlags rf, Axis x, int8_t &xx, int8_t &dx)
		{
			xx = x, dx = 1;
			if (rf & RF_FLIP_X)
			{
				xx += m_tileWidth - 1;
				dx = -1;
			}
			xx -= m_columnF;
		} 

		static void renderTileFromBank(RenderFlags rf, Axis x, Axis y, TileIndx ti)
		{
			uint8_t bi, bs, tb, tm; int8_t xx, dx; // ???
			TileAddr ta; uint8_t tbs = getTileBitmapSize();
			
			if (y >= 0 && y >> 3 == m_page)
			{
				// Case #1 : tile top half
				bs = y & 0x07;
				ta = ti * m_tileWidth * tbs;
				getRenderDirection(rf, x, xx, dx);

				for (bi = m_tileWidth; bi > 0; --bi, ta += tbs, xx += dx)
				{
					if (xx < 0 || xx >= m_columnW) continue;
					
					fetchTileData(rf, ta, tb, tm);
					m_renderBuffer[xx] &= ~(tm << bs);
					m_renderBuffer[xx] |= tb << bs;
				}
				
			}
			else if (m_pageY >= y && m_pageY < y + 8)
			{
				// Case #2 : tile bottom half
				bs = m_pageY - y;
				ta = ti * m_tileWidth * tbs;
				getRenderDirection(rf, x, xx, dx);

				for (bi = m_tileWidth; bi > 0; --bi, ta += tbs, xx += dx)
				{
					if (xx < 0 || xx >= m_columnW) continue;
					
					fetchTileData(rf, ta, tb, tm);
					m_renderBuffer[xx] &= ~(tm >> bs);
					m_renderBuffer[xx] |= tb >> bs;
				}
			}
		}

		////////////////////////////////////////////////////////////////////////

		void init()
		{
			m_oddFrame = false;
			m_renderBuffer = NULL;
			setRenderConfig(NULL);
			m_scrollX = m_scrollY = 0;
		}

		void update()
		{
			if (m_renderCallback)
			{
				// prepare rendering configuration 
				uint8_t pageF = m_pageR >> 0x4;
				uint8_t pageL = m_pageR & 0x0F;
				m_columnF = (m_columnR >> 0x8);
				m_columnW = (m_columnR & 0xFF) - m_columnF + 1;
				RenderCallback renderCallback = m_renderCallback;

				// prepare rendering buffer
				uint8_t buf[1 + m_columnW] = { 0x40 };
				m_renderBuffer = &buf[1];

				// do actual rendering page by page
				for (m_page = pageF; m_page <= pageL; ++m_page)
				{
					m_pageY = m_page << 3;
					display::position(m_page, m_columnF);

					// call same rendering pipeline for every page
					renderCallback();
					display::writeBuf(buf, sizeof(buf));
				}
				m_renderBuffer = NULL;
			}
			m_oddFrame ^= true;
		}

		void setRenderConfig(RenderCallback renderCallback, uint8_t pageRange, uint16_t columnRange)
		{
			// Rendering configuration may be changed at the end of rendering
			// pipeline. In this case it will be applied on the next frame.
			m_renderCallback = renderCallback;
			m_pageR = pageRange & 0x77;
			m_columnR = columnRange & 0x7F7F;
		}

		void setScrollXY(Axis sx, Axis sy)
		{
			m_scrollX = sx;
			m_scrollY = sy;
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

		void fillDisplay(uint8_t pattern)
		{
			if (!m_renderBuffer) display::fill(pattern);
		}

		void fillRenderBuffer(uint8_t pattern)
		{
			memset(m_renderBuffer, pattern, m_columnW);
		}

		void fillRenderBufferDirect(FillBufferCallback fillBufferCallback)
		{
			fillBufferCallback(m_columnF, m_renderBuffer, m_columnW);
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
