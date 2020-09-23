#include "engine.h"

// main program callbacks
extern void init();
extern void update();

namespace th
{
	namespace engine
	{
		static const u08 F_SCALE_MSK = 0x07;
		static const u08 F_FSYNC_BIT = 0x03;
		static const u08 F_SLEEP_BIT = 0x04;
		
		static volatile struct
		{
			u16 count_l;
			u08 count_h;
			u08 divider;
			u08 scale_f;
		} m_frame;
		
		static void reloadFrameDivider()
		{
			m_frame.divider = _BV(m_frame.scale_f & F_SCALE_MSK);
		}
		
		static void waitNextFrame()
		{
			set_bit(m_frame.scale_f, F_SLEEP_BIT);
			while (is_bit_set(m_frame.scale_f, F_SLEEP_BIT))
			{
				sleep_enable();
				sleep_mode();
			}			
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
			setFPS(FPS_UNLIMITED);
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
			
			if (is_bit_set(m_frame.scale_f, F_FSYNC_BIT))
			{
				waitNextFrame();
			}
		}

		void setFPS(FPS fps)
		{
			cli();
			mcu::wdtDisable();
			set_sleep_mode(SLEEP_MODE_IDLE);
			mcu::wdtEnable(WDT_MODE_INT, WDT_TIMEOUT_16MS);
			m_frame.scale_f = 0;
			if (fps)
			{
				m_frame.scale_f = (--fps & F_SCALE_MSK);
				set_bit(m_frame.scale_f, F_FSYNC_BIT);
			}
			reloadFrameDivider();
			sei();		
		}
		
		u32 getCurrentTimeMillis()
		{
			return (m_frame.count_h << 16 | m_frame.count_l) << 4;
		}
	}
}

ISR(WDT_vect)
{
	using namespace th::engine;
	if (++m_frame.count_l == 0) ++m_frame.count_h;
	if (--m_frame.divider == 0)
	{
		clear_bit(m_frame.scale_f, F_SLEEP_BIT);
		reloadFrameDivider();
	}
}

int main(void)
{
	th::engine::init();
	for (;;) th::engine::update();
	return 0;
}
