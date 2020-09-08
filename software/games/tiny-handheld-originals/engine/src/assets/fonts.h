#pragma once

#include "../video.h"

#include "font6x8.h"
#include "font6x8caps.h"
#include "font6x8digits.h"

namespace th
{
	namespace assets
	{
		const video::FontBank fb_font6x8 = { font6x8, video::TileFormat::TF_BM | 6, ' ' };
		const video::FontBank fb_font6x8caps = { font6x8_caps, video::TileFormat::TF_BM | 6, ' ' };
		const video::FontBank fb_font6x8digits = { font6x8_digits, video::TileFormat::TF_BM | 6, '0' };
	}
}
