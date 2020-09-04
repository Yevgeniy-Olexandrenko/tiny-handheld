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

// Definitions
#define FPS_HIGH WDT_TIMEOUT_16MS
#define FPS_LOW  WDT_TIMEOUT_32MS
#define FPS_ULOW WDT_TIMEOUT_500MS

// Configuration
#define DISABLE_FPS_SYNC 1

namespace th
{
	namespace engine
	{
		void setFPS(uint8_t fps);
	}
}