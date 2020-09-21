#pragma once

#include "../video.h"

#include "font6x8.h"
#include "font6x8caps.h"
#include "font6x8digits.h"

#include "font4x8.h"
#include "font4x8caps.h"
#include "font4x8digits.h"

namespace th
{
	namespace assets
	{
		FONTBANK_IN_STORAGE(fb_font6x8) = { font6x8, video::TileFormat::TF_BM | 6, ' ' };
		FONTBANK_IN_STORAGE(fb_font6x8caps) = { font6x8_caps, video::TileFormat::TF_BM | 6, ' ' };
		FONTBANK_IN_STORAGE(fb_font6x8digits) = { font6x8_digits, video::TileFormat::TF_BM | 6, '0' };

		FONTBANK_IN_STORAGE(fb_font4x8) = { font4x8, video::TileFormat::TF_BM | 4, ' ' };
		FONTBANK_IN_STORAGE(fb_font4x8caps) = { font4x8_caps, video::TileFormat::TF_BM | 4, ' ' };
		FONTBANK_IN_STORAGE(fb_font4x8digits) = { font4x8_digits, video::TileFormat::TF_BM | 4, '0' };
	}
}
