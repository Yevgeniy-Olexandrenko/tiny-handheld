#pragma once

#include "../video.h"

namespace th
{
	namespace assets
	{
		BINARY_IN_STORAGE(logo) =
		{
			// 48 x 16 px
			0xfc, 0xc4, 0x14, 0xf4, 0x14, 0x04, 0xd4, 0x04,
			0xec, 0x2c, 0xcc, 0x0c, 0x6c, 0x4c, 0xec, 0x0c, 
			0x04, 0xf4, 0x44, 0xf4, 0x04, 0xcc, 0x2e, 0xed,
			0x0d, 0xee, 0x2c, 0xcc, 0x1c, 0xdc, 0x44, 0xf4, 
			0x04, 0xf4, 0x44, 0x9c, 0x1c, 0xcc, 0xac, 0x6c,
			0x04, 0xf4, 0x04, 0xdc, 0x44, 0xf4, 0x04, 0xfc, 
			0xff, 0x87, 0xa4, 0xb5, 0xb4, 0xb4, 0xb5, 0xb4,
			0xb5, 0x94, 0x85, 0xa4, 0xb5, 0xb5, 0xb4, 0xb6, 
			0xb4, 0xb5, 0x94, 0x85, 0xa4, 0xb4, 0xb5, 0xb5,
			0xb4, 0xb5, 0xb4, 0x95, 0x84, 0xa4, 0xb5, 0xb5, 
			0xb4, 0xb5, 0xb4, 0xb5, 0x94, 0x84, 0xa5, 0xb5,
			0xb4, 0xb5, 0xb4, 0xb4, 0xb5, 0x95, 0x84, 0xff
		};

		video::TileBank tb_logo(logo, video::TF_BM);
	}
}
