#include "engine.h"

namespace th
{
	namespace engine
	{
		void init()
		{
			input::init();
			display::init();
			sound::init();
			eeprom::init();

			render::init();
		}
	}
}
