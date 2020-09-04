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
			buttons::init();
			display::init();
			buzzer::init()
			eeprom::init();
			
			// init software layer
			video::init();
			sound::init();

			// init main programm
			setFPS(FPS_HIGH);
			setup();
		}

		void update()
		{
			// update inputs
			battery::update();
			buttons::update();

			// update main program
			loop();

			// update outputs
			video::update();
			sound::update();
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
