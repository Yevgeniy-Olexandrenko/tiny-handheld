#include "commons.h"
#include "display.h"
#include "i2c.h"

#define SSD1306_COMMAND 0x00
#define SSD1306_DATA    0x40

namespace th
{
	namespace display
	{
		const uint8_t oled_initbuf[] PROGMEM =
		{
#if 1
			0x00, 0xae, 0xa8, 0x3f, 0xd3, 0x00, 0x40, 0xa1,
			0xc8, 0xda, 0x12, 0x81, 0x7f, 0xa4, 0xa6, 0xd5,
			0x80, 0x8d, 0x14, 0xaf, 0x20, 0x02
#else
			0x00,
			0xAE,			  // Set Display ON/OFF - AE=OFF, AF=ON
			0xD5, 0xF0,		  // Set display clock divide ratio/oscillator frequency, set divide ratio
			0xA8, 0x3F,		  // Set multiplex ratio (1 to 64) ... (height - 1)
			0xD3, 0x00,		  // Set display offset. 00 = no offset
			0x40 | 0x00,	  // Set start line address, at 0.
			0x8D, 0x14,		  // Charge Pump Setting, 14h = Enable Charge Pump
			0x20, 0x00,		  // Set Memory Addressing Mode - 00=Horizontal, 01=Vertical, 10=Page, 11=Invalid
			0xA0 | 0x01,	  // Set Segment Re-map
			0xC8,			  // Set COM Output Scan Direction
			0xDA, 0x12,		  // Set COM Pins Hardware Configuration - 128x32:0x02, 128x64:0x12
			0x81, 0x3F,		  // Set contrast control register
			0xD9, 0x22,		  // Set pre-charge period (0x22 or 0xF1)
			0xDB, 0x20,		  // Set Vcomh Deselect Level - 0x00: 0.65 x VCC, 0x20: 0.77 x VCC (RESET), 0x30: 0.83 x VCC
			0xA4,			  // Entire Display ON (resume) - output RAM to display
			0xA6,			  // Set Normal/Inverse Display mode. A6=Normal; A7=Inverse
			0x2E,			  // Deactivate Scroll command
			0xAF,			  // Set Display ON/OFF - AE=OFF, AF=ON
			0x22, 0x00, 0x3f, // Set Page Address (start,end)
			0x21, 0x00, 0x7f, // Set Column Address (start,end)
#endif
		};

		void init()
		{
			// Wait to initialize itself after power-on
			_delay_ms(40);

			// Send initialization sequence
			i2c::begin(SSD1306_SA);
			for (uint8_t i = 0; i < sizeof(oled_initbuf); i++)
			{
				i2c::write(pgm_read_byte(&oled_initbuf[i]));
			}
			i2c::end();

#if 0
			uint8_t uc[4];
			uc[0] = 0x00;
			uc[1] = 0xa0;
			oledWrite(uc, 2);
			uc[1] = 0xc0;
			oledWrite(uc, 2);
#endif

			// Clear display on power on
			fill(0x00);
		}

		void update() {}

		void writeCmd(uint8_t cmd)
		{
			uint8_t buf[] = { SSD1306_COMMAND, cmd };
			writeBuf(buf, sizeof(buf));
		}

		void writeCmd(uint8_t cmd, uint8_t data)
		{
			uint8_t buf[] = { SSD1306_COMMAND, cmd, data };
			writeBuf(buf, sizeof(buf));
		}

		void writeBuf(uint8_t *buf, uint8_t size)
		{
			i2c::begin(SSD1306_SA);
			i2c::write(buf, size);
			i2c::end();
		}

		////////////////////////////////////////////////////////////////////////

		void tunrOn()
		{
			writeCmd(0xAF);
		}

		void turnOff()
		{
			writeCmd(0xAE);
		}

		void inverse(bool yes)
		{
			writeCmd(yes ? 0xA7 : 0xA6);
		}

		void contrast(uint8_t data)
		{
			writeCmd(0x81, data ? data : 0x01);
		}

		void position(uint8_t page, uint8_t column)
		{
			writeCmd(0xb0 | page);	
			writeCmd(0x00 | (column & 0x0F));
			writeCmd(0x10 | (column >> 4 & 0x0F));
		}

		void fill(uint8_t data)
		{
			uint8_t x, y;
			uint8_t buf[17] = { SSD1306_DATA };

			memset(buf + 1, data, 16);
			for (y = 0; y < 8; y++)
			{
				position(y, 0);
				for (x = 0; x < 8; x++)
				{
					writeBuf(buf, sizeof(buf));
				}
			}
		}
		
		////////////////////////////////////////////////////////////////////////

	}
}
