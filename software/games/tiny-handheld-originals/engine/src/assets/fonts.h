#pragma once

#include "../video.h"

namespace th
{
	namespace assets
	{
		#include "font6x8.h"
		#include "font6x8caps.h"
		#include "font6x8digits.h"

		const video::FontData font6x8 PROGMEM = { tile_bank_font6x8, video::TileFlags::TF_EMPTY, 6, ' ' };
		const video::FontData font6x8caps PROGMEM = { tile_bank_font6x8_caps, video::TileFlags::TF_EMPTY, 6, ' ' };
		const video::FontData font6x8digits PROGMEM = { tile_bank_font6x8_digits, video::TileFlags::TF_EMPTY, 6, '0' };
	}
}
