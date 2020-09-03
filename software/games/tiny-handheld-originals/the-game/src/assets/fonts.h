#pragma once

#include "../render.h"

namespace th
{
	namespace assets
	{
		#include "font6x8.h"
		#include "font6x8caps.h"
		#include "font6x8digits.h"

		const render::FontData font6x8 PROGMEM = { tile_bank_font6x8, render::TileFlags::TF_EMPTY, 6, ' ' };
		const render::FontData font6x8caps PROGMEM = { tile_bank_font6x8_caps, render::TileFlags::TF_EMPTY, 6, ' ' };
		const render::FontData font6x8digits PROGMEM = { tile_bank_font6x8_digits, render::TileFlags::TF_EMPTY, 6, '0' };
	}
}
