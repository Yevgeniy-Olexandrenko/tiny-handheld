// Low level access to the hardware display

#pragma once

#ifndef SSD1306_SCL
#define SSD1306_SCL  PB2  // SCL, Pin 3 on SSD1306 Board
#endif

#ifndef SSD1306_SDA
#define SSD1306_SDA  PB0  // SDA, Pin 4 on SSD1306 Board
#endif

#ifndef SSD1306_SA
#define SSD1306_SA   0x78 // Slave address (or 0x78 = 0x3C << 1)
#endif

namespace th
{
	namespace display
	{
		void init();

		// Communication with hardware
		void startCommand();
		void startData();
		void write(uint8_t byte);	
		void stop();

		// Simple drawing
		void setPos(uint8_t page, uint8_t column);
		void clear();
		void fill(uint8_t p);
		void fill(uint8_t p1, uint8_t p2);
		void fill(uint8_t p1, uint8_t p2, uint8_t p3, uint8_t p4);
		void drawBitmap(uint8_t page0, uint8_t column0, uint8_t page1, uint8_t column1, const uint8_t bitmap[]);
	}
}
