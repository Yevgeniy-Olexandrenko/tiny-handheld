#include "commons.h"
#include "render.h"
#include "display.h"

namespace th
{
	namespace render
	{
		typedef uint8_t (*TileDataReader)(TileDataAddr tileDataAddr);

		RenderSequence m_renderSequence;
		bool m_isOddFrame;

		TileBankAddr m_tileBankAddr;
		TileDataReader m_tileDataReader;

		////////////////////////////////////////////////////////////////////////

		void reverseBits(uint8_t& a)
		{
			if(a == 0x00 || a == 0xFF) return;
			a = ((a >> 1) & 0x55) | ((a << 1) & 0xAA);
			a = ((a >> 2) & 0x33) | ((a << 2) & 0xCC);
			asm volatile("swap %0":"=r"(a):"0"(a));
		}

		uint8_t tileDataReaderFromSRAM(TileDataAddr tileDataAddr)
		{
			return m_tileBankAddr[tileDataAddr];
		}

		uint8_t tileDataReaderFromPROGMEM(TileDataAddr tileDataAddr)
		{
			return pgm_read_byte(m_tileBankAddr + tileDataAddr);
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
			m_isOddFrame = false;

			// init tile data reader
			setTileBank(TileStorage::TS_PROGMEM, 0);
		}

		void update()
		{
			if (m_renderSequence)
			{
				uint8_t page, column, index, mask, bits, composed;
				for (page = 0; page < 8; ++page)
				{
					display::setPos(page, 0);
					display::startData();
					for (column = 0; column < 128; ++column)
					{
						composed = 0x00;
						for (index = 0; m_renderSequence[index]; ++index)
						{
							mask = 0xFF;
							bits = 0x00;
							m_renderSequence[index](page, column, bits, mask, m_isOddFrame);
							composed &= mask;
							composed |= bits;
						}
						display::write(composed);
					}
				}
				display::stop();
			}
			m_isOddFrame ^= true;
		}

		void setRenderSequence(RenderSequence renderSequence)
		{
			m_renderSequence = renderSequence;
		}

		void setTileBank(TileStorage tileStorage, TileBankAddr tileBankAddr)
		{
			switch(tileStorage)
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

		void renderTile(TileDataAddr tileDataAddr, TileFlags tileF, uint8_t tileX, uint8_t tileY, uint8_t page, uint8_t column, uint8_t &bits)
		{
			uint8_t tileW = tileF & TF_WIDTH_B;
			if (column >= tileX && column < tileX + tileW)
			{
				uint8_t flipX = tileF & TF_FLIP_X;
				uint8_t flipY = tileF & TF_FLIP_Y;

				tileDataAddr += flipX ? tileW - 1 - (column - tileX) : column - tileX;
				if (tileY >> 3 == page)
				{
					// Case #1 : top tile half
					uint8_t shift = tileY & 0x07;
					uint8_t tileB = m_tileDataReader(tileDataAddr);

					if (flipY) reverseBits(tileB);
					bits |= tileB << shift;
				}
				else
				{
					// Case #2 : bottom tile half
					uint8_t pageY = page << 3;
					if (pageY >= tileY && pageY < tileY + 8)
					{
						uint8_t shift = pageY - tileY;
						uint8_t tileB = m_tileDataReader(tileDataAddr);

						if (flipY) reverseBits(tileB);
						bits |= tileB >> shift;
					}
				}
			}
		}

		void renderTile(TileDataAddr tileDataAddr, TileFlags tileF, uint8_t tileX, uint8_t tileY, uint8_t page, uint8_t column, uint8_t &bits, uint8_t &mask, bool isOddFrame)
		{
			uint8_t tileW = tileF & TF_WIDTH_B;
			if (column >= tileX && column < tileX + tileW)
			{
				uint8_t flipX   = tileF & TF_FLIP_X;
				uint8_t flipY   = tileF & TF_FLIP_Y;
				uint8_t inverse = tileF & TF_INVERSE;

				tileDataAddr += flipX ? tileW - 1 - (column - tileX) : column - tileX;
				if (tileY >> 3 == page)
				{
					// Case #1 : top tile half
					uint8_t shift = tileY & 0x07;
					uint8_t tileB = m_tileDataReader(tileDataAddr + (isOddFrame ? tileW << 1 : 0));
					uint8_t tileM = m_tileDataReader(tileDataAddr + tileW);

					if (flipY)
					{
						reverseBits(tileB);
						reverseBits(tileM);
					}

					if(inverse)
					{
						tileB = ~tileB;
						tileB &= tileM;
					}

					bits |= tileB << shift;
					mask &= ~(tileM << shift);
				}
				else
				{
					// Case #2 : bottom tile half
					uint8_t pageY = page << 3;
					if (pageY >= tileY && pageY < tileY + 8)
					{
						uint8_t shift = pageY - tileY;
						uint8_t tileB = m_tileDataReader(tileDataAddr + (isOddFrame ? tileW << 1 : 0));
						uint8_t tileM = m_tileDataReader(tileDataAddr + tileW);

						if (flipY)
						{
							reverseBits(tileB);
							reverseBits(tileM);
						}

						if(inverse)
						{
							tileB = ~tileB;
							tileB &= tileM;
						}

						bits |= tileB >> shift;
						mask &= ~(tileM >> shift);
					}
				}
			}
		}
	} 
}
