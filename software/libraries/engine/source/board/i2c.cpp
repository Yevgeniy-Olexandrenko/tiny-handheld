#include "commons.h"
#include "i2c.h"

#define SCL_LO() I2C_PORT &= ~_BV(I2C_SCL)
#define SDA_LO() I2C_PORT &= ~_BV(I2C_SDA)
#define SCL_HI() I2C_PORT |=  _BV(I2C_SCL)
#define SDA_HI() I2C_PORT |=  _BV(I2C_SDA)

#define SCL_SDA_HI()  I2C_PORT |=  (_BV(I2C_SDA) | _BV(I2C_SCL))
#define SCL_SDA_OUT() I2C_DDR  |=  (_BV(I2C_SDA) | _BV(I2C_SCL))
#define SCL_SDA_IN()  I2C_DDR  &= ~(_BV(I2C_SDA) | _BV(I2C_SCL))

namespace th
{
	namespace i2c
	{
		void write(u08 data)
		{
			u08 i;
			u08 P = I2C_PORT & ~(_BV(I2C_SDA) | _BV(I2C_SCL));

			for (i = 8; i > 0; --i)
			{
				P &= ~_BV(I2C_SDA);
				if (data & 0x80) P |= _BV(I2C_SDA);

				I2C_PORT = P;
				SCL_HI();
				SCL_LO();
				data <<= 1;
			}
			I2C_PORT = P & ~_BV(I2C_SDA);
			SCL_HI();
			SCL_LO();
		}

		void write(p08 data, u08 size)
		{
			u08 i, b;
			u08 P = I2C_PORT & ~(_BV(I2C_SDA) | _BV(I2C_SCL));

			while (size--)
			{
				b = *data++;
				if (b == 0x00 || b == 0xff)
				{
					// special case can save time
					P &= ~_BV(I2C_SDA);
					if (b) P |= _BV(I2C_SDA);

					I2C_PORT = P;
					for (i = 8; i > 0; --i)
					{
						// just toggle SCL, SDA stays the same
						SCL_HI();
						SCL_LO();
					}
				}
				else
				{
					// normal byte needs every bit tested
					for (i = 8; i > 0; --i)
					{

						P &= ~_BV(I2C_SDA);
						if (b & 0x80) P |= _BV(I2C_SDA);

						I2C_PORT = P;
						SCL_HI();
						SCL_LO();
						b <<= 1;
					}
				}

				// ACK bit seems to need to be set to 0, but SDA line doesn't need to be tri-state
				SDA_LO();
				SCL_HI();
				SCL_LO();
			}
		}

		void begin(u08 addr)
		{
			SCL_SDA_HI();
			SCL_SDA_OUT();
			SDA_LO();
			SCL_LO();
			write(addr);
		}

		void end()
		{
			SDA_LO();
			SCL_HI();
			SDA_HI();
			SCL_SDA_IN();
		}
	}
}
