#include "engine.h"

// main program callbacks
extern void init();
extern void update();

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
			buzzer::init();
			eeprom::init();
			
			// init software layer
			video::init();
			sound::init();

			// init main programm
			setFPS(FPS_HIGH);
			::init();
		}

		void update()
		{
			// update inputs
			battery::update();
			buttons::update();

			// update main program
			::update();

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
	th::engine::init();
	for (;;) th::engine::update();
	return 0;
}
