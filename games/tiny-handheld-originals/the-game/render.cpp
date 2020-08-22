#include "commons.h"
#include "render.h"
#include "display.h"

namespace th
{
	namespace render
	{
		const RenderLayerCallback *renderSequence;
		bool isOddFrame;

		void reverseBits(uint8_t& a)
		{
			if(a == 0x00 || a == 0xFF) return;
			a = ((a >> 1) & 0x55) | ((a << 1) & 0xAA);
			a = ((a >> 2) & 0x33) | ((a << 2) & 0xCC);
			asm volatile("swap %0":"=r"(a):"0"(a));
		}

		void init()
		{
			renderSequence = NULL;
			isOddFrame = false;
		}

		void update()
		{
			if (renderSequence)
			{
				uint8_t page, column, index, mask, bits, composed;
				for (page = 0; page < 8; ++page)
				{
					display::setPos(page, 0);
					display::startData();
					for (column = 0; column < 128; ++column)
					{
						composed = 0x00;
						for (index = 0; renderSequence[index]; ++index)
						{
							mask = 0xFF;
							bits = 0x00;
							renderSequence[index](page, column, bits, mask, isOddFrame);
							composed &= mask;
							composed |= bits;
						}
						display::write(composed);
					}
				}
				display::stop();
			}
			isOddFrame ^= true;
		}

		void setRenderSequence(const RenderLayerCallback *sequence)
		{
			renderSequence = sequence;
		}

		void getTileRenderData(const uint8_t *tileData, uint8_t tileF, uint8_t tileX, uint8_t tileY, uint8_t page, uint8_t column, uint8_t &bits)
		{
			uint8_t tileW = tileF & 0x3F;
			if (column >= tileX && column < tileX + (tileF & 0x3F))
			{
				uint8_t flipX = tileF & 0x40;
				uint8_t flipY = tileF & 0x80;

				uint8_t index = flipX ? tileW - 1 - (column - tileX) : column - tileX;
				if (tileY >> 3 == page)
				{
					// Case #1 : top tile half
					uint8_t shift = tileY & 0x07;
					uint8_t tileB = pgm_read_byte(&tileData[index]);

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
						uint8_t tileB = pgm_read_byte(&tileData[index]);

						if (flipY) reverseBits(tileB);
						bits |= tileB >> shift;
					}
				}
			}
		}

		void getTileRenderData(const uint8_t *tileData, uint8_t tileF, uint8_t tileX, uint8_t tileY, uint8_t page, uint8_t column, uint8_t &bits, uint8_t &mask, bool isOddFrame)
		{
			uint8_t tileW = tileF & 0x3F;
			if (column >= tileX && column < tileX + tileW)
			{
				uint8_t flipX = tileF & 0x40;
				uint8_t flipY = tileF & 0x80;

				uint8_t index = flipX ? tileW - 1 - (column - tileX) : column - tileX;
				if (tileY >> 3 == page)
				{
					// Case #1 : top tile half
					uint8_t shift = tileY & 0x07;
					uint8_t tileB = pgm_read_byte(&tileData[index + (isOddFrame ? tileW + tileW : 0)]);
					uint8_t tileM = pgm_read_byte(&tileData[index + tileW]);

					if (flipY)
					{
						reverseBits(tileB);
						reverseBits(tileM);
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
						uint8_t tileB = pgm_read_byte(&tileData[index + (isOddFrame ? tileW + tileW : 0)]);
						uint8_t tileM = pgm_read_byte(&tileData[index + tileW]);

						if (flipY)
						{
							reverseBits(tileB);
							reverseBits(tileM);
						}

						bits |= tileB >> shift;
						mask &= ~(tileM >> shift);
					}
				}
			}
		}
	} 
}
