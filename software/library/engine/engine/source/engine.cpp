#include "engine.h"

// main program callbacks
extern void init();
extern void update();

namespace th
{
	namespace engine
	{
		static volatile struct
		{
			u16 count_l;
			u08 count_h;
			u08 scale_f;
		} m_frame;
		
		static const u08 F_SCALE_MSK = 0x0F;
		static const u08 F_SLEEP_BIT = 0x04; 

		static void waitNextFrame()
		{
#if !DISABLE_FPS_SYNC
			set_bit(m_frame.scale_f, F_SLEEP_BIT);
			while (is_bit_set(m_frame.scale_f, F_SLEEP_BIT))
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
			m_frame.count_l = 0;
			m_frame.count_h = 0;
			m_frame.scale_f = 0;
			setFrameTime(FT_32MS);
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

		void setFrameTime(FrameTime frameTime)
		{
			cli();
			mcu::wdtDisable();
			set_sleep_mode(SLEEP_MODE_IDLE);
			mcu::wdtEnable(WDT_MODE_INT, frameTime);
			m_frame.scale_f = frameTime & F_SCALE_MSK;
			sei();		
		}
		
		u32 getCurrentTimeMillis()
		{
			return (m_frame.count_h << 16 | m_frame.count_l) * (16 << (m_frame.scale_f & F_SCALE_MSK));
		}
	}
}

ISR(WDT_vect)
{
	using namespace th::engine;
	if (++m_frame.count_l == 0) ++m_frame.count_h;
	clear_bit(m_frame.scale_f, F_SLEEP_BIT);
}

int main(void)
{
	th::engine::init();
	for (;;) th::engine::update();
	return 0;
}
