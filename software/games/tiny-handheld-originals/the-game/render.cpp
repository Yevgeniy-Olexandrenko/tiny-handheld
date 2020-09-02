#include "commons.h"
#include "render.h"
#include "display.h"

namespace th
{
	namespace render
	{
		typedef uint8_t (*TileDataReader)(TileDataAddr tileDataAddr);

		RenderSequence m_renderSequence;
		RenderContext m_renderContext;

		TileBankAddr m_tileBankAddr;
		TileDataReader m_tileDataReader;

		////////////////////////////////////////////////////////////////////////

		void reverseBits(uint8_t &a)
		{
			if (a == 0x00 || a == 0xFF)
				return;
			a = ((a >> 1) & 0x55) | ((a << 1) & 0xAA);
			a = ((a >> 2) & 0x33) | ((a << 2) & 0xCC);
			asm volatile("swap %0"
						 : "=r"(a)
						 : "0"(a));
		}

		uint8_t tileDataReaderFromSRAM(TileDataAddr tileDataAddr)
		{
			return m_tileBankAddr[tileDataAddr];
		}

		uint8_t tileDataReaderFromPROGMEM(TileDataAddr tileDataAddr)
		{
			return pgm_read_byte_near(m_tileBankAddr + tileDataAddr);
		}

		uint8_t tileDataReaderFromEEPROM(TileDataAddr tileDataAddr)
		{
			return 0;
		}

		////////////////////////////////////////////////////////////////////////

		void init()
		{
			// init render sequence
			m_renderSequence = NULL;
			m_renderContext.isOddFrame = false;

			// init tile data reader
			setTileBank(TileStorage::TS_PROGMEM, 0);
		}

		void update()
		{
			if (m_renderSequence)
			{
				uint8_t buf[1 + 128] = {0x40};
				for (m_renderContext.page = 0; m_renderContext.page < 8; ++m_renderContext.page)
				{
					m_renderContext.pageY = m_renderContext.page << 3;
					display::oledWriteCommand(0xb0 | m_renderContext.page);
					display::oledWriteCommand(0x00);
					display::oledWriteCommand(0x10);

					for (m_renderContext.column = 0; m_renderContext.column < 128;)
					{
						m_renderContext.columnX = m_renderContext.column;
						m_renderContext.composed = 0x00;

						RenderSequence renderSequence = m_renderSequence;
						while (RenderLayerCallback renderLayerCallback = pgm_read_word_near(renderSequence++))
						{
							m_renderContext.mask = 0xFF;
							m_renderContext.bits = 0x00;
							renderLayerCallback(m_renderContext);
							m_renderContext.composed &= m_renderContext.mask;
							m_renderContext.composed |= m_renderContext.bits;
						}
						buf[++m_renderContext.column] = m_renderContext.composed;
					}
					display::oledWrite(buf, sizeof(buf));
				}
			}
			m_renderContext.isOddFrame ^= true;
		}

		void setRenderSequence(RenderSequence renderSequence)
		{
			m_renderSequence = renderSequence;
		}

		void flushRenderContext()
		{
			m_renderContext.composed &= m_renderContext.mask;
			m_renderContext.composed |= m_renderContext.bits;

			m_renderContext.mask = 0xFF;
			m_renderContext.bits = 0x00;
		}

		void setScrollX(int8_t sx)
		{
			m_renderContext.columnX = m_renderContext.column + sx;
		}

		void setTileBank(TileStorage tileStorage, TileBankAddr tileBankAddr)
		{
			switch (tileStorage)
			{
			case TileStorage::TS_SRAM:
				m_tileDataReader = &tileDataReaderFromSRAM;
				break;
			case TileStorage::TS_PROGMEM:
				m_tileDataReader = &tileDataReaderFromPROGMEM;
				break;
			case TileStorage::TS_EEPROM:
				m_tileDataReader = &tileDataReaderFromEEPROM;
				break;
			}
			m_tileBankAddr = tileBankAddr;
		}

		TileDataAddr getTileDataAddr(TileFlags tileF, TileIndex tileI)
		{
			uint8_t tileW = tileF & TF_WIDTH_BITS;
			uint8_t tileS = tileW;

			if (tileF & TF_HAS_MASK)
				tileS += tileW;
			if (tileF & TF_HAS_ODD_BM)
				tileS += tileW;

			return tileI * tileS;
		}

		void fetchTileData(TileDataAddr tileDataAddr, TileFlags tileF, uint8_t x, uint8_t &tileB, uint8_t &tileM)
		{
			if (tileF & TF_FLIP_X)
				tileDataAddr += ((tileF & TF_WIDTH_BITS) - 1) - (m_renderContext.columnX - x);
			else
				tileDataAddr += m_renderContext.columnX - x;

			tileB = m_tileDataReader(tileDataAddr);

			if (tileF & TF_HAS_MASK)
				tileM = m_tileDataReader(tileDataAddr += (tileF & TF_WIDTH_BITS));
			else if (tileF & TF_TRANSPARENT)
				tileM = tileB;
			else
				tileM = 0xFF;

			if ((tileF & TF_HAS_ODD_BM) && m_renderContext.isOddFrame)
				tileB = m_tileDataReader(tileDataAddr += (tileF & TF_WIDTH_BITS));

			if (tileF & TF_FLIP_Y)
			{
				reverseBits(tileB);
				reverseBits(tileM);
			}

			if (tileF & TF_INVERSE)
			{
				tileB = ~tileB;
				tileB &= tileM;
			}
		}

#define IS_ON_THIS_COLUMN(x, w) (m_renderContext.columnX >= (x) && m_renderContext.columnX < (x) + (w))
#define IS_ON_THIS_PAGE(y) ((y) >> 3 == m_renderContext.page)
#define IS_ON_NEXT_PAGE(y) (m_renderContext.pageY >= (y) && m_renderContext.pageY < (y) + 8)

		void renderTile(TileDataAddr tileDataAddr, TileFlags tileF, uint8_t x, uint8_t y)
		{
			uint8_t shift, tileB, tileM;
			if (IS_ON_THIS_COLUMN(x, tileF & TF_WIDTH_BITS))
			{
				if (IS_ON_THIS_PAGE(y))
				{
					// Case #1 : top tile half
					fetchTileData(tileDataAddr, tileF, x, tileB, tileM);
					shift = y & 0x07;

					m_renderContext.bits |= tileB << shift;
					m_renderContext.mask &= ~(tileM << shift);
				}
				else if (IS_ON_NEXT_PAGE(y))
				{
					// Case #2 : bottom tile half
					fetchTileData(tileDataAddr, tileF, x, tileB, tileM);
					shift = m_renderContext.pageY - y;

					m_renderContext.bits |= tileB >> shift;
					m_renderContext.mask &= ~(tileM >> shift);
				}
			}
		}

		void renderText(const FontData &fontData, TileFlags tileF, int8_t x, int8_t y, const char *text, uint8_t len)
		{
			uint8_t tileW = fontData.tileFlags & TF_WIDTH_BITS;
			uint8_t dispW = tileF & TF_WIDTH_BITS;

			if (IS_ON_THIS_COLUMN(x, dispW * len) && (IS_ON_THIS_PAGE(y) || IS_ON_NEXT_PAGE(y)))
			{
				setTileBank(fontData.tileStorage, fontData.tileBankAddr);
				tileF &= TF_CONTROL_BITS;
				tileF |= fontData.tileFlags & (TF_CONFIG_BITS | TF_WIDTH_BITS);

				uint8_t i = (m_renderContext.columnX - x) / dispW, tx = i * dispW + x;
				renderTile(getTileDataAddr(tileF, text[i] - fontData.asciiOffset), tileF, tx, y);
			}
		}

	} // namespace render
} // namespace th
