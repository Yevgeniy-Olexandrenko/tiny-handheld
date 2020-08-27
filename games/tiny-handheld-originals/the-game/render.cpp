#include "commons.h"
#include "render.h"
#include "display.h"

namespace th
{
	namespace render
	{
		typedef uint8_t (*TileDataReader)(TileDataAddr tileDataAddr);

		RenderSequence m_renderSequence;
		RenderContext  m_renderContext;

		TileBankAddr   m_tileBankAddr;
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
				for (m_renderContext.page = 0; m_renderContext.page < 8; ++m_renderContext.page)
				{
					m_renderContext.pageY = m_renderContext.page << 3;
					display::setPos(m_renderContext.page, 0);
					display::startData();

					for (m_renderContext.column = 0; m_renderContext.column < 128; ++m_renderContext.column)
					{
						m_renderContext.composed = 0x00;
						for (uint8_t index = 0; m_renderSequence[index]; ++index)
						{
							m_renderContext.mask = 0xFF;
							m_renderContext.bits = 0x00;
							m_renderSequence[index](m_renderContext);
							m_renderContext.composed &= m_renderContext.mask;
							m_renderContext.composed |= m_renderContext.bits;
						}
						display::write(m_renderContext.composed);
					}
				}
				display::stop();
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

		TileDataAddr getTileDataAddr(TileFlags& tileF, TileIndex tileI)
		{
			uint8_t tileW = tileF & TF_WIDTH_BITS;
			uint8_t tileS = tileW;

			if (tileF & TF_HAS_MASK) tileS += tileW;
			if (tileF & TF_HAS_ODD_BM) tileS += tileW;

			return tileI * tileS;
		}

		void fetchTileData(TileDataAddr& tileDataAddr, TileFlags& tileF, uint8_t& tileB, uint8_t& tileM)
		{
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

		void renderTile(TileDataAddr tileDataAddr, TileFlags tileF, uint8_t tileX, uint8_t tileY)
		{
			uint8_t shift, tileB, tileM;
			if (m_renderContext.column >= tileX && m_renderContext.column < tileX + (tileF & TF_WIDTH_BITS))
			{
				if (tileF & TF_FLIP_X)
					tileDataAddr += ((tileF & TF_WIDTH_BITS) - 1) - (m_renderContext.column - tileX);
				else
					tileDataAddr += m_renderContext.column - tileX;

				if (tileY >> 3 == m_renderContext.page)
				{
					// Case #1 : top tile half
					shift = tileY & 0x07;
					fetchTileData(tileDataAddr, tileF, tileB, tileM);

					m_renderContext.bits |= tileB << shift;
					m_renderContext.mask &= ~(tileM << shift);
				}
				else if (m_renderContext.pageY >= tileY && m_renderContext.pageY < tileY + 8)
				{
					// Case #2 : bottom tile half
					shift = m_renderContext.pageY - tileY;
					fetchTileData(tileDataAddr, tileF, tileB, tileM);

					m_renderContext.bits |= tileB >> shift;
					m_renderContext.mask &= ~(tileM >> shift);
				}
			}
		}

		void renderText(const FontData& fontData, TileFlags tileF, uint8_t x, uint8_t y, const char *text, uint8_t length)
		{
			uint8_t tileW = fontData.tileFlags & TF_WIDTH_BITS;
			uint8_t dispW = tileF & TF_WIDTH_BITS;

			if (m_renderContext.column >= x && m_renderContext.column < x + length * dispW)
			{
				if (y >> 3 == m_renderContext.page || m_renderContext.pageY >= y && m_renderContext.pageY < y + 8)
				{
					setTileBank(fontData.tileStorage, fontData.tileBankAddr);

					tileF &= TF_CONTROL_BITS;
					tileF |= fontData.tileFlags & (TF_CONFIG_BITS | TF_WIDTH_BITS);

					for (uint8_t i = 0; i < length && text[i]; ++i, x += dispW)
					{
						renderTile(getTileDataAddr(tileF, text[i] - fontData.asciiOffset), tileF, x, y);
					}
				}
			}
		}

	} // namespace render
} // namespace th
