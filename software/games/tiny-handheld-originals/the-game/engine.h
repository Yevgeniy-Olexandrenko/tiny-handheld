#pragma once

#include "commons.h"

// Low level hardware support
#include "input.h"
#include "display.h"
#include "sound.h"
#include "eeprom.h"

// High level hardware support
#include "render.h"

namespace th
{
	namespace engine
	{
		void init();
	}
}
