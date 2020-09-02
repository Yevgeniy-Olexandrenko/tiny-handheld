#include "commons.h"
#include "render.h"
#include "display.h"

#define IS_ON_THIS_PAGE(y) ((y) >> 3 == m_page)
#define IS_ON_NEXT_PAGE(y) (m_pageY >= (y) && m_pageY < (y) + 8)

namespace th
{
	namespace render
	{
		RenderSequence m_renderSequence;
		uint8_t  m_pageR;

		memory::Binary m_tileBank;
		uint8_t  m_tileWidth;
		uint8_t  m_fontFlags;
		uint8_t  m_asciiBase;

		uint8_t* m_renderBuffer;
		uint8_t  m_page;
		uint8_t  m_pageY;
		uint8_t  m_oddFrame;

		////////////////////////////////////////////////////////////////////////

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
			uint8_t ts = m_tileWidth;
			if (tf & TF_HAS_MASK)   ts += m_tileWidth;
			if (tf & TF_HAS_ODD_BM)	ts += m_tileWidth;
			return ti * ts;
		}

		static bool isRenderingActive()
		{
			return m_renderBuffer != NULL;
		}

		////////////////////////////////////////////////////////////////////////

		void init()
		{
			m_oddFrame = false;
			m_renderBuffer = NULL;
			setRenderSequence(NULL);
		}

		void update()
		{
			if(m_renderSequence)
			{
				uint8_t buf[129] = { 0x40 };
				m_renderBuffer = &buf[0x01];

				uint8_t pageF = m_pageR >> 0x4;
				uint8_t pageL = m_pageR &  0xF;
				for (m_page = pageF; m_page <= pageL; ++m_page)
				{
					m_pageY = m_page << 3;
					display::writeCmd(0xb0 | m_page);
		 			display::writeCmd(0x00);
		 			display::writeCmd(0x10);

					RenderSequence rs = m_renderSequence;
					while (RenderLayerCallback rlc = pgm_read_word(rs++)) rlc();
					display::writeBuf(buf, sizeof(buf));
				}
			}

			m_renderBuffer = NULL;
			m_oddFrame ^= true;
		}

		void setRenderSequence(RenderSequence renderSequence, uint8_t pageRange)
		{
			if (!isRenderingActive())
			{
				m_renderSequence = renderSequence;
				m_pageR = pageRange & 0x77;
			}
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
			if (IS_ON_THIS_PAGE(y))
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
			else if (IS_ON_NEXT_PAGE(y))
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
			if (IS_ON_THIS_PAGE(y) || IS_ON_NEXT_PAGE(y))
			{
				tf &= TF_CONTROL_BITS;
				tf |= m_fontFlags;

				for (i = 0; i < len && text[i]; ++i, x += m_tileWidth)
				{
					renderTile(tf, x, y, getTileAddr(text[i] - m_asciiBase, tf));
				}
			}
		}

		////////////////////////////////////////////////////////////////////////

	}
} 
