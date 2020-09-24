// Low level access to the hardware display

#pragma once

#ifndef SSD1306_SA
#define SSD1306_SA   0x78
#endif

namespace th
{
	namespace display
	{
		void init();
		void update();

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
