#pragma once

namespace th
{
	namespace battery
	{
		void init();
		void update();

		uint16_t getVCC();
		uint16_t getVoltage();
		uint8_t  getPercent();
	}
}
