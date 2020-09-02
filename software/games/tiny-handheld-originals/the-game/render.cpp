#include "commons.h"
#include "render.h"
#include "display.h"

namespace th
{
	namespace render
	{
		RenderSequence m_renderSequence;

		uint8_t m_page, m_column, m_pageY, m_columnX; 
		uint8_t m_bits, m_mask, m_composed;
		uint8_t m_oddFrame;

		memory::Binary m_tileBank;
		
		////////////////////////////////////////////////////////////////////////

		void reverseBits(uint8_t &a)
		{
			if (a == 0x00 || a == 0xFF)	return;
			a = ((a >> 1) & 0x55) | ((a << 1) & 0xAA);
			a = ((a >> 2) & 0x33) | ((a << 2) & 0xCC);
			asm volatile("swap %0"
						 : "=r"(a)
						 : "0"(a));
		}

		////////////////////////////////////////////////////////////////////////

		void init()
		{
			// init render sequence
			m_renderSequence = NULL;
			m_oddFrame = false;
		}

		void update()
		{
			if (m_renderSequence)
			{
				uint8_t buf[1 + 128] = {0x40};
				for (m_page = 0; m_page < 8; ++m_page)
				{
					m_pageY = m_page << 3;
					display::oledWriteCommand(0xb0 | m_page);
					display::oledWriteCommand(0x00);
					display::oledWriteCommand(0x10);

					for (m_column = 0; m_column < 128;)
					{
						m_columnX = m_column;
						m_composed = 0x00;

						RenderSequence renderSequence = m_renderSequence;
						while (RenderLayerCallback renderLayerCallback = pgm_read_word_near(renderSequence++))
						{
							m_mask = 0xFF;
							m_bits = 0x00;
							renderLayerCallback();
							m_composed &= m_mask;
							m_composed |= m_bits;
						}
						buf[++m_column] = m_composed;
					}
					display::oledWrite(buf, sizeof(buf));
				}
			}
			m_oddFrame ^= true;
		}

		void setRenderSequence(RenderSequence renderSequence)
		{
			m_renderSequence = renderSequence;
		}

		void flushRenderContext()
		{
			m_composed &= m_mask;
			m_composed |= m_bits;

			m_mask = 0xFF;
			m_bits = 0x00;
		}

		void setScrollX(int8_t sx)
		{
			m_columnX = m_column + sx;
		}

		void setTileBank(const memory::Binary& tileBank)
		{
			m_tileBank = tileBank;
		}

		TileAddr getTileAddr(TileFlags tf, TileIndx ti)
		{
			uint8_t tileW = tf & TF_WIDTH_BITS;
			uint8_t tileS = tileW;

			if (tf & TF_HAS_MASK) tileS += tileW;
			if (tf & TF_HAS_ODD_BM)	tileS += tileW;

			return ti * tileS;
		}

		void fetchTileData(TileAddr tileDataAddr, TileFlags tf, uint8_t x, uint8_t &tileB, uint8_t &tileM)
		{
			if (tf & TF_FLIP_X)
				tileDataAddr += ((tf & TF_WIDTH_BITS) - 1) - (m_columnX - x);
			else
				tileDataAddr += m_columnX - x;

			tileB = m_tileBank[tileDataAddr];

			if (tf & TF_HAS_MASK)
				tileM = m_tileBank[tileDataAddr += (tf & TF_WIDTH_BITS)];
			else if (tf & TF_TRANSPARENT)
				tileM = tileB;
			else
				tileM = 0xFF;

			if ((tf & TF_HAS_ODD_BM) && m_oddFrame)
				tileB = m_tileBank[tileDataAddr + (tf & TF_WIDTH_BITS)];

			if (tf & TF_FLIP_Y)
			{
				reverseBits(tileB);
				reverseBits(tileM);
			}

			if (tf & TF_INVERSE)
			{
				tileB = ~tileB;
				tileB &= tileM;
			}
		}

#define IS_ON_THIS_COLUMN(x, w) (m_columnX >= (x) && m_columnX < (x) + (w))
#define IS_ON_THIS_PAGE(y)      ((y) >> 3 == m_page)
#define IS_ON_NEXT_PAGE(y)      (m_pageY >= (y) && m_pageY < (y) + 8)

		void renderTile(TileAddr tileAddr, TileFlags tf, uint8_t x, uint8_t y)
		{
			uint8_t shift, tileB, tileM;
			if (IS_ON_THIS_COLUMN(x, tf & TF_WIDTH_BITS))
			{
				if (IS_ON_THIS_PAGE(y))
				{
					// Case #1 : top tile half
					fetchTileData(tileAddr, tf, x, tileB, tileM);
					shift = y & 0x07;

					m_bits |= tileB << shift;
					m_mask &= ~(tileM << shift);
				}
				else if (IS_ON_NEXT_PAGE(y))
				{
					// Case #2 : bottom tile half
					fetchTileData(tileAddr, tf, x, tileB, tileM);
					shift = m_pageY - y;

					m_bits |= tileB >> shift;
					m_mask &= ~(tileM >> shift);
				}
			}
		}

		void renderText(const FontData &fontData, TileFlags tf, int8_t x, int8_t y, const char *text, uint8_t len)
		{
			uint8_t tileW = fontData.tileFlags & TF_WIDTH_BITS;
			uint8_t dispW = tf & TF_WIDTH_BITS;

			if (IS_ON_THIS_COLUMN(x, dispW * len) && (IS_ON_THIS_PAGE(y) || IS_ON_NEXT_PAGE(y)))
			{
				setTileBank(memory::Binary::InFLASH(fontData.tileBank)); // !!!
				tf &= TF_CONTROL_BITS;
				tf |= fontData.tileFlags & (TF_CONFIG_BITS | TF_WIDTH_BITS);

				uint8_t i = (m_columnX - x) / dispW, tx = i * dispW + x;
				renderTile(getTileAddr(tf, text[i] - fontData.asciiBase), tf, tx, y);
			}
		}

	} // namespace render
} // namespace th
