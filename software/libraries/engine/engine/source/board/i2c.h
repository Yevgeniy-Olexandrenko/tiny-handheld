#pragma once

#define I2C_PORT PORTB
#define I2C_DDR  DDRB
#define I2C_SDA  PB0
#define I2C_SCL  PB2

namespace th
{
	
	namespace i2c
	{
		void begin(u08 addr);
		void write(u08 data);
		void write(p08 data, u08 size);
		void end();
	}
}