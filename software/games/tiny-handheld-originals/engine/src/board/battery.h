#pragma once

namespace th
{
	namespace battery
	{
		void init()   DO_NOTHING
		void update() DO_NOTHING

		uint16_t getVCC();
		uint16_t getVoltage();
		uint8_t  getPercent();
	}
}
