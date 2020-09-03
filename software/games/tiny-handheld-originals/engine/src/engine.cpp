#include "engine.h"

// following Arduino crap must be excluded from code!
extern "C" void init();
extern "C" void initVariant();

// main programm callbacks
extern "C" void setup();
extern "C" void loop();

namespace th
{
	namespace engine
	{
		void init()
		{
			// init hardware layer
			battery::init();
			display::init();
			sound::init();
			input::init();
			eeprom::init();

			// init software layer
			render::init();

			// init main programm
			setup();
		}

		void update()
		{
			// update user input
			input::update();

			// update main programm
			loop();

			// update programm output
			render::update();
		}
	}
}

int main(void)
{
	init();
	initVariant();

	th::engine::init();
	for (;;) th::engine::update();
	return 0;
}
