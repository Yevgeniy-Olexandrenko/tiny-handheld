#pragma once

#include "src/engine.h"
#include "src/assets/fonts.h"
#include "src/assets/logo.h"

const uint8_t picture [] IN_FLASH =
 {
	0x00,0x03,0x05,0x09,0x11,0xFF,0x11,0x89,0x05,0xC3,0x00,0xE0,0x00,0xF0,0x00,0xF8,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x44,0x28,0xFF,0x11,0xAA,0x44,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x83,0x01,0x38,0x44,0x82,0x92,
	0x92,0x74,0x01,0x83,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7C,0x44,0xFF,0x01,0x7D,
	0x7D,0x7D,0x01,0x7D,0x7D,0x7D,0x7D,0x01,0x7D,0x7D,0x7D,0x7D,0x7D,0x01,0xFF,0x00,
	0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x00,0x00,0x00,0x00,
	0x00,0x00,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,
	0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0x3F,0x03,0x03,
	0xF3,0x13,0x11,0x11,0x11,0x11,0x11,0x11,0x01,0xF1,0x11,0x61,0x81,0x01,0x01,0x01,
	0x81,0x61,0x11,0xF1,0x01,0x01,0x01,0x01,0x41,0x41,0xF1,0x01,0x01,0x01,0x01,0x01,
	0xC1,0x21,0x11,0x11,0x11,0x11,0x21,0xC1,0x01,0x01,0x01,0x01,0x41,0x41,0xF1,0x01,
	0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x11,0x11,0x11,0x11,0x11,0xD3,0x33,
	0x03,0x03,0x3F,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xE0,0xE0,0x00,0x00,
	0x7F,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00,0x7F,0x00,0x00,0x01,0x06,0x18,0x06,
	0x01,0x00,0x00,0x7F,0x00,0x00,0x00,0x00,0x40,0x40,0x7F,0x40,0x40,0x00,0x00,0x00,
	0x1F,0x20,0x40,0x40,0x40,0x40,0x20,0x1F,0x00,0x00,0x00,0x00,0x40,0x40,0x7F,0x40,
	0x40,0x00,0x00,0x00,0x00,0x60,0x00,0x00,0x00,0x00,0x40,0x30,0x0C,0x03,0x00,0x00,
	0x00,0x00,0xE0,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x07,0x06,0x06,
	0x06,0x06,0x04,0x04,0x04,0x84,0x44,0x44,0x44,0x84,0x04,0x04,0x84,0x44,0x44,0x44,
	0x84,0x04,0x04,0x04,0x84,0xC4,0x04,0x04,0x04,0x04,0x84,0x44,0x44,0x44,0x84,0x04,
	0x04,0x04,0x04,0x04,0x84,0x44,0x44,0x44,0x84,0x04,0x04,0x04,0x04,0x04,0x84,0x44,
	0x44,0x44,0x84,0x04,0x04,0x84,0x44,0x44,0x44,0x84,0x04,0x04,0x04,0x04,0x06,0x06,
	0x06,0x06,0x07,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x10,0x18,0x14,0x12,0x11,0x00,0x00,0x0F,0x10,0x10,0x10,
	0x0F,0x00,0x00,0x00,0x10,0x1F,0x10,0x00,0x00,0x00,0x08,0x10,0x12,0x12,0x0D,0x00,
	0x00,0x18,0x00,0x00,0x0D,0x12,0x12,0x12,0x0D,0x00,0x00,0x18,0x00,0x00,0x10,0x18,
	0x14,0x12,0x11,0x00,0x00,0x10,0x18,0x14,0x12,0x11,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x80,
	0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x7F,0x03,0x0C,0x30,0x0C,0x03,0x7F,0x00,0x00,0x38,0x54,0x54,0x58,0x00,0x00,
	0x7C,0x04,0x04,0x78,0x00,0x00,0x3C,0x40,0x40,0x7C,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xAA,0xAA,0xAA,
	0x28,0x08,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0x03,0x0C,0x30,0x0C,0x03,0x7F,
	0x00,0x00,0x26,0x49,0x49,0x49,0x32,0x00,0x00,0x7F,0x02,0x04,0x08,0x10,0x7F,0x00,
};

const uint8_t tile_empty[] PROGMEM =
{
	// bits A
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	// mask
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	// bits B
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
};

const uint8_t tile_4x8[] PROGMEM =
{
  // bits A
  0b11111111,
  0b10000001,
  0b10000001,
  0b11111111,
  // mask
  0b11111111,
  0b11111111,
  0b11111111,
  0b11111111,
  // bits B
  0b11111111,
  0b10011001,
  0b10011001,
  0b11111111,
};

const uint8_t tile_triangle[] PROGMEM =
{
	// bits A
	0b00000000,
	0b01000000,
	0b01100000,
	0b01010000,
	0b01001000,
	0b01000100,
	0b01111110,
	0b00000000,
	// mask
	0b10000000,
	0b11000000,
	0b11100000,
	0b11110000,
	0b11111000,
	0b11111100,
	0b11111110,
	0b11111111,
	// bits B
	0b00000000,
	0b01000000,
	0b01100000,
	0b01110000,
	0b01111000,
	0b01111100,
	0b01111110,
	0b00000000,
};

const uint8_t tile_square[] IN_FLASH =
{
  // bits A
  0b00000000,
  0b01111111,
  0b01111111,
  0b01100000,
  0b01100000,
  0b01100000,
  0b01100000,
  0b01100000,
  // mask
  0b11111111,
  0b11111111,
  0b11111111,
  0b11111111,
  0b11110000,
  0b11110000,
  0b11110000,
  0b11110000,
  // bits B
  0b00000000,
  0b01111110,
  0b01000010,
  0b01011010,
  0b01011010,
  0b01000010,
  0b01111110,
  0b00000000,
};

const uint8_t tile_box_quater[] PROGMEM =
{
  // bits A
  0b00000000,
  0b01111111,
  0b01000000,
  0b01010101,
  0b01001011,
  0b01010101,
  0b01001011,
  0b01011111,
  // mask
  0b11111111,
  0b11111111,
  0b11111111,
  0b11111111,
  0b11111111,
  0b11111111,
  0b11111111,
  0b11111111,
  // bits B
  0b00000000,
  0b01111111,
  0b01000000,
  0b01001011,
  0b01010101,
  0b01001011,
  0b01010101,
  0b01011111,
};



int8_t s = 2;
int8_t y = 0, dy = 1;
int8_t x = 0, dx = 1;

uint32_t saved_time;
uint8_t frame_count;
uint8_t fps;

void RenderBackground()
{
#if 1
	th::render::TileFlags tileF = th::render::TF_EMPTY;
	// tileF |= th::render::TF_TRANSPARENT;
	// tileF |= th::render::TF_INVERSE;
	th::render::renderBitmap(tileF, 0, 0, 128, 64, th::memory::Binary::InFLASH(picture));
#else
	for (uint8_t col = 0; col < 128; ++col)
	{
		bool isSolid = (th::render::m_page & 0x01) ^ (col >> 3 & 0x01);
		th::render::m_renderBuffer[col] = isSolid ? ((col & 0x07) == 0 || (col & 0x07) == 7 ? 0xFF : ((col & 0x01 ^ th::render::m_oddFrame ? 0xAB : 0xD5))) : 0x00;
	}
#endif
}

void RenderSprite(uint8_t x, uint8_t y)
{
	th::render::TileFlags tileF;
	tileF |= th::render::TF_HAS_MASK;
	// tileF |= th::render::TF_HAS_ODD_BM;
	// tileF |= th::render::TF_TRANSPARENT;
	// tileF |= th::render::TF_INVERSE;

	th::render::setTileBank(th::memory::Binary::InFLASH(tile_square));
	for (uint8_t yy = 0; yy < s; ++yy)
	{
		for (uint8_t xx = 0; xx < s; ++xx)
		{
			th::render::TileFlags flipY = (yy & 0x01) ? th::render::TF_EMPTY : th::render::TF_FLIP_Y;
			th::render::TileFlags flipX = (xx & 0x01) ? th::render::TF_FLIP_X : th::render::TF_EMPTY;
			th::render::renderTile(tileF | flipX | flipY, x + xx * 8, y + yy * 8, 0);
		}
	}
}

void RenderLogo(uint8_t x, uint8_t y)
{
	th::render::TileFlags tileF;
	//tileF |= th::render::TF_TRANSPARENT;
	//tileF |= th::render::TF_INVERSE;
	th::render::renderBitmap(tileF, x, y, 48, 16, th::memory::Binary::InFLASH(th::assets::logo));
}

void RenderForeground()
{
	RenderSprite(x, y);
	RenderSomeText(128 - x, y + 4);
	RenderSprite(x + 4, 64 - y);
	RenderLogo(40, y);
}

void RenderSomeText(uint8_t x, uint8_t y)
{
	th::render::setFontData(th::assets::font6x8);
	th::render::renderText(th::render::TileFlags::TF_EMPTY, x, y, "Some text!", 10);
}

void RenderFPS()
{
	uint8_t f = fps;
	th::render::setFontData(th::assets::font6x8);
	for (uint8_t x = 0; x <= 12; x += 6)
	{
		th::render::renderChar(th::render::TileFlags::TF_EMPTY, 12 - x, 0, '0' + (f % 10));
		f /= 10;
	}
}

void RenderBattery()
{
	uint8_t bat = th::mcu::getBatteryPercent();
	th::render::setFontData(th::assets::font6x8);
	for (uint8_t x = 0; x <= 12; x += 6)
	{
		th::render::renderChar(th::render::TileFlags::TF_EMPTY, 12 - x, 64-8, '0' + (bat % 10));
		bat /= 10;
	}
	th::render::renderChar(th::render::TileFlags::TF_EMPTY, 12 + 6, 64-8, '%');
}

void ClearBuffer()
{
	memset(th::render::m_renderBuffer, 0x00, 128);
}

void RenderSequence()
{
  //ClearBuffer();
	RenderBackground();
	RenderForeground();
	RenderFPS();
}

void setup()
{
	saved_time = millis();
	frame_count = 0;
	fps = 0;

	th::render::setRenderCallback(&RenderSequence);
}

void loop()
{
	uint32_t current_time = millis();
	if (current_time >= saved_time + 1000)
	{
		fps = frame_count;
		saved_time += 1000;
		frame_count = 0;
	}
	frame_count++;

	y += dy * 1;
	x += dx * 1;

	if (y == 0 || y == 63) dy = -dy;
	if (x == 0 || x == 127) dx = -dx;
}
