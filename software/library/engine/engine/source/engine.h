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

namespace th
{
	namespace engine
	{
		typedef u08 FPS;
		const FPS FPS_UNLIMITED = 0;
		const FPS FPS_60 = 1; // 62.5 FPS 
		const FPS FPS_30 = 2; // 31.3 FPS
		const FPS FPS_15 = 3; // 15.6 FPS
		const FPS FPS_02 = 6; // 1.95 FPS
		const FPS FPS_01 = 7; // 0.98 FPS
		
		void setFPS(FPS fps);
		u32  getCurrentTimeMillis();
	}
}