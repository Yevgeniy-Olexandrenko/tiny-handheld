#include "../commons.h"
#include "mcu.h"

#if defined(__AVR_ATtiny85__) || defined(__AVR_ATtiny167__)
#define WDTCSR WDTCR
#endif

#define MIN_BAT_VOLTAGE 2000 // 2.0V
#define MAX_BAT_VOLTAGE 3000 // 3.0V

namespace th
{
	namespace mcu
	{
		void init()
		{
			//
		}

		////////////////////////////////////////////////////////////////////////

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

		////////////////////////////////////////////////////////////////////////

		// Vcc in millivolts
		// Source: http://blog.unlimite.net/?p=25
		uint16_t getVCC()
		{
			// Read 1.1V reference against AVcc
			// set the reference to Vcc and the measurement to the internal 1.1V reference
			ADMUX = _BV(MUX3) | _BV(MUX2);

			 // Wait for Vref to settle, start conversion and wait for measuring
			_delay_ms(75);			                  
			ADCSRA |= _BV(ADSC);
			while (bit_is_set(ADCSRA, ADSC));

			// read measurement form ADCL and ADCH
			uint8_t adcl = ADCL;
			uint8_t adch = ADCH;
			int32_t measurement = (adch << 8) | adcl;

			 // Calculate Vcc (in mV); 1125300 = 1.1 * 1023 * 1000
			return (uint16_t)(1119015L  / measurement);   
		}

		uint16_t getBatteryVoltage()
		{
			uint16_t history = 0;
			history += getVCC();
			history += getVCC();
			history += getVCC();
			history += getVCC();
			return history >> 2;
		}

		uint8_t getBatteryPercent()
		{
			uint16_t bv = getBatteryVoltage();
			bv = min(max(bv, MIN_BAT_VOLTAGE), MAX_BAT_VOLTAGE);
			return (100L * (bv - MIN_BAT_VOLTAGE) / (MAX_BAT_VOLTAGE - MIN_BAT_VOLTAGE));
		}

		////////////////////////////////////////////////////////////////////////
	}
}
