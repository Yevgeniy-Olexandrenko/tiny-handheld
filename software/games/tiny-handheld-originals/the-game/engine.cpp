#include "engine.h"

namespace th
{
	namespace engine
	{
		void init()
		{
			input::init();
			display::oledInit();
			sound::init();
			eeprom::init();

			render::init();
		}
	}
}
