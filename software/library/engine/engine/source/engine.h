#pragma once

#include "commons.h"

// Low level hardware support
#include "board/battery.h"
#include "board/buttons.h"
#include "board/display.h"
#include "board/buzzer.h"
#include "board/eeprom.h"
#include "board/mcu.h"

// High level hardware support
#include "memory.h"
#include "video.h"
#include "sound.h"

// Configuration
#define DISABLE_FPS_SYNC   0

namespace th
{
	namespace engine
	{
		typedef u08 FrameTime;
		const FrameTime FT_16MS   = WDT_TIMEOUT_16MS;
		const FrameTime FT_32MS   = WDT_TIMEOUT_32MS;
		const FrameTime FT_500MS  = WDT_TIMEOUT_500MS;
		const FrameTime FT_1000MS = WDT_TIMEOUT_1S;
		
		void setFrameTime(FrameTime frameTime);
		u32  getCurrentTimeMillis();
	}
}