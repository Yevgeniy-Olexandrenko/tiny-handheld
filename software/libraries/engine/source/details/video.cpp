#include "commons.h"
#include "video.h"
#include "board/display.h"

namespace th
{
	namespace video
	{
		typedef u16 TileAddr;
		
		static RenderCallback m_renderCallback;
		static u08 m_pageR;
		static u16 m_columnR;
		
		static memory::Binary m_tileBank;
		static u08 m_tileFormat;
		static u08 m_tileWidth;
		static u08 m_asciiBase;

		static u08 m_page;
		static u08 m_pageY;
		static u08 m_columnF;
		static u08 m_bufferW;
		static p08 m_buffer;

		bool m_oddFrame;
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

		static void applyScroll(Axis& x, Axis& y)
		{
			x -= m_scrollX;
			y -= m_scrollY;
		}

		static u08 reverseBits(u08 b)
		{
			if (b == 0x00 || b == 0xFF) return b;
			
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
		static void fetchTileData(RenderFlags rf, TileAddr ta, Bitmap& tb, Bitmap& tm)
		{
			Bitmap b0, b1, b2;
			switch (m_tileFormat & TF_BITS_FOR_TYPE)
			{
			default:
				m_tileBank.get(ta, tb);
				tm = (rf & RF_TRANSP) ? tb : 0xFF;
				break;

			case TF_BM_MSKBM:
				m_tileBank.get(ta, tb, b1);
				tm = (rf & RF_TRANSP) ? b1 : 0xFF;
				break;

			case TF_BM_ODDBM:
				m_tileBank.get(ta, b0, b1);
				tb = m_oddFrame ? b1 : b0;
				tm = (rf & RF_TRANSP) ? tb : 0xFF;
				break;

			case TF_BM_MSKBM_ODDBM:
				m_tileBank.get(ta, b0, b1, b2);
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

		static u08 getTileBitmapSize()
		{
			switch (m_tileFormat & TF_BITS_FOR_TYPE)
			{
			default: return 1;
			case TF_BM_MSKBM:
			case TF_BM_ODDBM: return 2;
			case TF_BM_MSKBM_ODDBM: return 3;
			}
		}

		static void setRenderDirection(RenderFlags rf, Axis& x, Axis& dx)
		{
			dx = 1;
			if (rf & RF_FLIP_X)
			{
				x += m_tileWidth - 1;
				dx = -1;
			}
			x -= m_columnF;
		}

		static void renderTileFromBank(RenderFlags rf, Axis x, Axis y, TileIndex ti)
		{
			Bitmap tb, tm; Axis& dx = y; TileAddr ta;
			u08 bs, bi, tbs = getTileBitmapSize();
			
			if (y >= 0 && y >> 3 == m_page)
			{
				// Case #1 : tile top half
				bs = y & 0x07;
				ta = ti * m_tileWidth * tbs;
				setRenderDirection(rf, x, dx);

				for (bi = m_tileWidth; bi > 0; --bi, ta += tbs, x += dx)
				{
					if (x < 0 || x >= m_bufferW) continue;
					
					fetchTileData(rf, ta, tb, tm);
					m_buffer[x] &= ~(tm << bs);
					m_buffer[x] |= tb << bs;
				}
				
			}
			else if (m_pageY >= y && m_pageY < y + 8)
			{
				// Case #2 : tile bottom half
				bs = m_pageY - y;
				ta = ti * m_tileWidth * tbs;
				setRenderDirection(rf, x, dx);

				for (bi = m_tileWidth; bi > 0; --bi, ta += tbs, x += dx)
				{
					if (x < 0 || x >= m_bufferW) continue;
					
					fetchTileData(rf, ta, tb, tm);
					m_buffer[x] &= ~(tm >> bs);
					m_buffer[x] |= tb >> bs;
				}
			}
		}

		////////////////////////////////////////////////////////////////////////

		void init()
		{
			setRenderConfig(NULL);
			m_buffer = NULL;

			m_oddFrame = false;
			m_scrollX  = 0;
			m_scrollY  = 0;
		}

		void update()
		{
			if (m_renderCallback)
			{
				// prepare rendering configuration 
				u08 pageF = m_pageR >> 0x4;
				u08 pageL = m_pageR & 0x0F;
				m_columnF = (m_columnR >> 0x8);
				m_bufferW = (m_columnR & 0xFF) - m_columnF + 1;
				RenderCallback renderCallback = m_renderCallback;

				// prepare rendering buffer
				u08 buf[1 + m_bufferW] = { 0x40 };
				m_buffer = &buf[1];

				// do actual rendering page by page
				for (m_page = pageF; m_page <= pageL; ++m_page)
				{
					m_pageY = m_page << 3;

					// call same rendering pipeline for every page
					m_tileBank.startMultiAccess();
					renderCallback();
					m_tileBank.stopMultiAccess();

					// send buffer data to specific location on display
					display::position(m_page, m_columnF);
					display::writeBuf(buf, sizeof(buf));
				}
				m_buffer = NULL;
			}
			m_oddFrame ^= true;
		}

		void setRenderConfig(RenderCallback renderCallback, u08 pageRange, u16 columnRange)
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

		void setTileBank(const TileBank_f& tileBank, Size tileWidth)
		{
			m_tileBank = memory::Binary(tileBank.m_addr);
			m_tileFormat = tileBank.m_format;
			
			m_tileWidth = m_tileFormat & TF_BITS_FOR_WIDTH;
			if (tileWidth)
			// override tile width with custom value
			m_tileWidth = tileWidth;
			else if (!m_tileWidth)
			// if tile width is still not set, use default value
			m_tileWidth = 8;
		}

		void setTileBank(const TileBank_s& tileBank, Size tileWidth)
		{
			const uint8_s * addr = tileBank.m_addr;
			m_tileBank = memory::Binary(reinterpret_cast<const uint8_s *>(addr));
			m_tileFormat = tileBank.m_format;
			
			m_tileWidth = m_tileFormat & TF_BITS_FOR_WIDTH;
			if (tileWidth)
			// override tile width with custom value
			m_tileWidth = tileWidth;
			else if (!m_tileWidth)
			// if tile width is still not set, use default value
			m_tileWidth = 8;
		}

		void setFontBank(const FontBank_f* fontBank)
		{
			m_tileBank = memory::Binary(fontBank->m_addr);
			m_tileFormat = fontBank->m_format;
			m_tileWidth = m_tileFormat & TF_BITS_FOR_WIDTH;			
			m_asciiBase = fontBank->m_asciiBase;
		}

		void setFontBank(const FontBank_s* fontBank)
		{
			m_tileBank = memory::Binary(fontBank->m_addr);
			m_tileFormat = fontBank->m_format;
			m_tileWidth = m_tileFormat & TF_BITS_FOR_WIDTH;
			m_asciiBase = fontBank->m_asciiBase;
		}

		void fillDisplay(u08 pattern)
		{
			if (!m_buffer) display::fill(pattern);
		}

		void fillRenderBuffer(u08 pattern)
		{
			memset(m_buffer, pattern, m_bufferW);
		}

		void fillRenderBufferDirect(FillBufferCallback fillBufferCallback)
		{
			fillBufferCallback(m_columnF, m_buffer, m_bufferW);
		}

		void renderTile(RenderFlags rf, Axis x, Axis y, TileIndex ti)
		{
			applyScroll(x, y);
			renderTileFromBank(rf, x, y, ti);
		}

		void renderChar(RenderFlags rf, Axis x, Axis y, char ch)
		{
			applyScroll(x, y);
			renderTileFromBank(rf, x, y, ch - m_asciiBase);
		}

		void renderText(RenderFlags rf, Axis x, Axis y, const char *text, u08 len)
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

		void renderPattern(RenderFlags rf, Axis x, Axis y, Size w, Size h, TileIndex ti)
		{
			// TODO
		}

		void renderBitmap(RenderFlags rf, Axis x, Axis y, Size w, Size h, const TileBank_f& bitmap)
		{
			Axis yh; 
			TileIndex ti;
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
		
		void renderBitmap(RenderFlags rf, Axis x, Axis y, Size w, Size h, const TileBank_s& bitmap)
		{
			Axis yh;
			TileIndex ti;
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
