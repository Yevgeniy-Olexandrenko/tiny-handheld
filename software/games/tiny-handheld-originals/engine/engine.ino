#pragma once

#include "src/engine.h"
#include "src/assets/fonts.h"
#include "src/assets/logo.h"

uint8_f picture [] IN_FLASH =
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

uint8_s tile_square[] IN_STORAGE =
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



int8_t s = 2;
int8_t y = 0, dy = 1;
int8_t x = 0, dx = 1;

uint32_t saved_time;
uint8_t frame_count;
uint8_t fps;

void RenderBackground()
{
#if 1
	th::video::RenderFlags rf = th::video::RF_EMPTY;
  //rf |= th::video::RF_TRANSPARENT;
  //rf |= th::video::RF_INVERSE;
  //rf |= th::video::RF_FLIP_Y;
  //rf |= th::video::RF_FLIP_X;
	th::video::renderBitmap(rf, 0, 0, 128, 64, picture);
#else
	for (uint8_t col = 0; col < 128; ++col)
	{
		bool isSolid = (th::video::m_page & 0x01) ^ (col >> 3 & 0x01);
		th::video::m_renderBuffer[col] = isSolid ? ((col & 0x07) == 0 || (col & 0x07) == 7 ? 0xFF : ((col & 0x01 ^ th::video::m_oddFrame ? 0xAB : 0xD5))) : 0x00;
	}
#endif
}

void RenderSprite(uint8_t x, uint8_t y)
{
	th::video::RenderFlags rf = th::video::RF_EMPTY;
	//rf |= th::video::RF_TRANSPARENT;
	//rf |= th::video::RF_INVERSE;

	th::video::setTileBank(tile_square, th::video::TF_BM_MASKBM);
	for (uint8_t yy = 0; yy < s; ++yy)
	{
		for (uint8_t xx = 0; xx < s; ++xx)
		{
			th::video::RenderFlags flipY = (yy & 0x01) ? th::video::RF_EMPTY  : th::video::RF_FLIP_Y;
			th::video::RenderFlags flipX = (xx & 0x01) ? th::video::RF_FLIP_X : th::video::RF_EMPTY;
			th::video::renderTile(rf | flipX | flipY, x + xx * 8, y + yy * 8, 0);
		}
	}
}

void RenderLogo(uint8_t x, uint8_t y)
{
	th::video::RenderFlags rf = th::video::RF_EMPTY;
	//rf |= th::video::RF_TRANSPARENT;
	//rf |= th::video::RF_INVERSE;
	th::video::renderBitmap(rf, x, y, 48, 16, th::assets::logo);
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
	th::video::setFontData(th::assets::font6x8);
	th::video::renderText(th::video::RF_EMPTY, x, y, "Some text!", 10);
}

void RenderFPS()
{
	uint8_t f = fps;
	th::video::setFontData(th::assets::font6x8);
	for (uint8_t x = 0; x <= 12; x += 6)
	{
		th::video::renderChar(th::video::RF_EMPTY, 12 - x, 0, '0' + (f % 10));
		f /= 10;
	}
}

void RenderBattery()
{
	uint8_t bat = th::battery::getPercent();
	th::video::setFontData(th::assets::font6x8);
	for (uint8_t x = 0; x <= 12; x += 6)
	{
		th::video::renderChar(th::video::RF_EMPTY, 12 - x, 64-8, '0' + (bat % 10));
		bat /= 10;
	}
	th::video::renderChar(th::video::RF_EMPTY, 12 + 6, 64-8, '%');
}

void ClearBuffer()
{
	memset(th::video::m_renderBuffer, 0x00, 128);
}

void RenderSequence()
{
  ClearBuffer();
	RenderBackground();
	RenderForeground();
	RenderFPS();
}

void setup()
{
	saved_time = millis();
	frame_count = 0;
	fps = 0;

	th::video::setRenderCallback(&RenderSequence);
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
