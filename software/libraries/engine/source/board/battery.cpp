#include "commons.h"
#include "battery.h"

#define MIN_BAT_VOLTAGE 2000 // 2.0V
#define MAX_BAT_VOLTAGE 3000 // 3.0V

namespace th
{
	namespace battery
	{
		void init()   {}
		void update() {}

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

		uint16_t getVoltage()
		{
			uint16_t history = 0;
			history += getVCC();
			history += getVCC();
			history += getVCC();
			history += getVCC();
			return history >> 2;
		}

		uint8_t getPercent()
		{
			uint16_t bv = min(max(getVoltage(), MIN_BAT_VOLTAGE), MAX_BAT_VOLTAGE);
			return (100L * (bv - MIN_BAT_VOLTAGE) / (MAX_BAT_VOLTAGE - MIN_BAT_VOLTAGE));
		}
	}
}
