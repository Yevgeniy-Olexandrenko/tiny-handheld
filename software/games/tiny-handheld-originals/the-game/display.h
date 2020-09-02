// Low level access to the hardware display

#pragma once

// #ifndef SSD1306_SCL
// #define SSD1306_SCL  PB2
// #endif

// #ifndef SSD1306_SDA
// #define SSD1306_SDA  PB0
// #endif

#ifndef SSD1306_SA
#define SSD1306_SA   0x78
#endif

#define I2CPORT PORTB
#define I2CDDR  DDRB
#define BB_SDA  PB0
#define BB_SCL  PB2

namespace th
{
	namespace display
	{
		void init();

		// Low level access to hardware
		void writeCmd(uint8_t  cmd);
		void writeCmd(uint8_t  cmd, uint8_t data);
		void writeBuf(uint8_t *buf, uint8_t size);

		// High level manipulation
		void tunrOn();
		void turnOff();
		void inverse(bool yes);
		void contrast(uint8_t data);
		void position(uint8_t page, uint8_t column);
		void fill(uint8_t data);
	}
}
