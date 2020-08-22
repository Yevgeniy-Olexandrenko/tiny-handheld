#include "commons.h"
#include "display.h"

#define SSD1306_COMMAND 0x00
#define SSD1306_DATA    0x40

#define I2CSW_H(PORT) PORTB |=  (1 << PORT)
#define I2CSW_L(PORT) PORTB &= ~(1 << PORT)

namespace th
{
	namespace display
	{
		// Some code based on "IIC_wtihout_ACK" by http://www.14blog.com/archives/1358
		const uint8_t initSequence [] PROGMEM = 
		{
			0xAE,             // Set Display ON/OFF - AE=OFF, AF=ON
			0xD5, 0xF0,       // Set display clock divide ratio/oscillator frequency, set divide ratio
			0xA8, 0x3F,       // Set multiplex ratio (1 to 64) ... (height - 1)
			0xD3, 0x00,       // Set display offset. 00 = no offset
			0x40 | 0x00,      // Set start line address, at 0.
			0x8D, 0x14,       // Charge Pump Setting, 14h = Enable Charge Pump
			0x20, 0x00,       // Set Memory Addressing Mode - 00=Horizontal, 01=Vertical, 10=Page, 11=Invalid
			0xA0 | 0x01,      // Set Segment Re-map
			0xC8,             // Set COM Output Scan Direction
			0xDA, 0x12,       // Set COM Pins Hardware Configuration - 128x32:0x02, 128x64:0x12
			0x81, 0x3F,       // Set contrast control register
			0xD9, 0x22,       // Set pre-charge period (0x22 or 0xF1)
			0xDB, 0x20,       // Set Vcomh Deselect Level - 0x00: 0.65 x VCC, 0x20: 0.77 x VCC (RESET), 0x30: 0.83 x VCC
			0xA4,             // Entire Display ON (resume) - output RAM to display
			0xA6,             // Set Normal/Inverse Display mode. A6=Normal; A7=Inverse
			0x2E,             // Deactivate Scroll command
			0xAF,             // Set Display ON/OFF - AE=OFF, AF=ON
			0x22, 0x00, 0x3f, // Set Page Address (start,end)
			0x21, 0x00,	0x7f, // Set Column Address (start,end)
		};

		void i2csw_start() 
		{
			DDRB |= (1 << SSD1306_SDA);  // Set port as output
			DDRB |= (1 << SSD1306_SCL);  // Set port as output
			I2CSW_H(SSD1306_SCL);        // Set to HIGH
			I2CSW_H(SSD1306_SDA);        // Set to HIGH
			I2CSW_L(SSD1306_SDA);        // Set to LOW
			I2CSW_L(SSD1306_SCL);        // Set to LOW
		}

		void i2csw_stop() 
		{
			I2CSW_L(SSD1306_SCL);        // Set to LOW
			I2CSW_L(SSD1306_SDA);        // Set to LOW
			I2CSW_H(SSD1306_SCL);        // Set to HIGH
			I2CSW_H(SSD1306_SDA);        // Set to HIGH
			DDRB &= ~(1 << SSD1306_SDA); // Set port as input
		}

		void i2csw_byte(uint8_t b) 
		{
			for (uint8_t i = 0; i < 8; i++) 
			{
				if ((b << i) & 0x80) 
					I2CSW_H(SSD1306_SDA); 
				else 
					I2CSW_L(SSD1306_SDA);
				I2CSW_H(SSD1306_SCL);
				I2CSW_L(SSD1306_SCL);
			}
			I2CSW_H(SSD1306_SDA);
			I2CSW_H(SSD1306_SCL);
			I2CSW_L(SSD1306_SCL);
		}

		void init()
		{
			// Wait to initialize itself after power-on
			_delay_ms(40);

			// Send initialization sequence
			startCommand();
			for (uint8_t i = 0; i < sizeof (initSequence); i++) 
			{
				write(pgm_read_byte(&initSequence[i]));
			}
			stop();
		}

		void startCommand()
		{
			i2csw_start();
			i2csw_byte(SSD1306_SA);
			i2csw_byte(SSD1306_COMMAND);
		}

		void startData()
		{
			i2csw_start();
			i2csw_byte(SSD1306_SA);
			i2csw_byte(SSD1306_DATA);
		}

		void write(uint8_t byte)
		{
			i2csw_byte(byte);
		}

		void stop()
		{
			i2csw_stop();
		}

		void setPos(uint8_t page, uint8_t column)
		{
			startCommand();
			write(0xb0 | (page & 0x07)); // Set page start address
			write(column & 0x0f);        // Set the lower nibble of the column start address
			write(0x10 | (column >> 4)); // Set the higher nibble of the column start address
			stop();
		}

		void clear()
		{
			fill(0x00);
		}

		void fill(uint8_t p)
		{
			fill(p, p, p, p);
		}

		void fill(uint8_t p1, uint8_t p2)
		{
			fill(p1, p2, p1, p2);
		}

		void fill(uint8_t p1, uint8_t p2, uint8_t p3, uint8_t p4)
		{
			setPos(0, 0);
			startData();
			for (uint16_t i = 0; i < 128 * 8 / 4; i++) 
			{
				write(p1);
				write(p2);
				write(p3);
				write(p4);
			}
			stop();
		}

		void drawBitmap(uint8_t page0, uint8_t column0, uint8_t page1, uint8_t column1, const uint8_t bitmap[])
		{
			for (uint8_t p = page0; p <= page1; ++p)
			{
				setPos(p, column0);
				startData();
				for (uint8_t c = column0; c <= column1; ++c)
				{
					write(pgm_read_byte(bitmap++));
				}
				stop();
			}
		}
	}
}
