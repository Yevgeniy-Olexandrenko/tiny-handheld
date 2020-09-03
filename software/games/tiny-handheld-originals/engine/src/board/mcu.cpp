#include "../commons.h"
#include "mcu.h"

#if defined(__AVR_ATtiny85__) || defined(__AVR_ATtiny167__)
#define WDTCSR WDTCR
#endif

namespace th
{
	namespace mcu
	{
		void init()
		{
			//
		}

		void wdtEnable(uint8_t mode, uint8_t prescaler)
		{
			uint8_t wdtr = mode | ((prescaler > 7) ? 0x20 | (prescaler - 8) : prescaler);
			uint8_t sreg = SREG;
			cli();
			WDTCSR = ((1 << WDCE) | (1 << WDE));
			WDTCSR = wdtr;
			SREG = sreg;
		}

		void wdtDisable()
		{
			uint8_t sreg = SREG;
			cli();
			WDTCSR = ((1 << WDCE) | (1 << WDE));
			WDTCSR = 0x00;
			SREG = sreg;
		}

		void wdtReset()
		{
			asm volatile ("WDR");
		}
	}
}
