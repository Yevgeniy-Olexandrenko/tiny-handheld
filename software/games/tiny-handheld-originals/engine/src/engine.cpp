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
		volatile bool m_sleeping = false;

		void waitNextFrame()
		{
#if !DISABLE_FPS_SYNC			
			m_sleeping = true;
			while (m_sleeping)
			{
				sleep_enable();
				sleep_mode();
			}
#endif			
		}

		void init()
		{
			// init hardware layer
			mcu::init();
			battery::init();
			display::init();
			eeprom::init();
			input::init();
			sound::init();

			// init software layer
			render::init();
			setFPS(FPS_HIGH);

			// init main programm
			setup();
		}

		void update()
		{
			input::update();
			loop();
			render::update();
			waitNextFrame();
		}

		void setFPS(uint8_t fps)
		{
#if !DISABLE_FPS_SYNC
			cli();
			mcu::wdtDisable();
			set_sleep_mode(SLEEP_MODE_IDLE);
			mcu::wdtEnable(WDT_MODE_INT, fps);
			sei();
#endif			
		}
	}
}

#if !DISABLE_FPS_SYNC
ISR(WDT_vect)
{
	th::engine::m_sleeping = false;	
}
#endif

int main(void)
{
	init();
	initVariant();

	th::engine::init();
	for (;;) th::engine::update();
	return 0;
}
