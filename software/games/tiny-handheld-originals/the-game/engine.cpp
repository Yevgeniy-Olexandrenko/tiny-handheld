#include "engine.h"

namespace th
{
	namespace engine
	{
		void init()
		{
			// init hardware layer
			display::init();
			sound::init();
			input::init();
			eeprom::init();

			// init software layer
			render::init();
		}
	}
}
