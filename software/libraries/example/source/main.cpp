#include "engine.h"
#include "assets/fonts.h"
#include "assets/logo.h"
#include "data.h"


int8_t s = 2;
int8_t y = 0, dy = 1;
int8_t x = 0, dx = 1;

uint32_t saved_time;
uint8_t frame_count;
uint8_t fps;

//void FillBuffer(uint8_t column, uint8_t* buffer, uint8_t size)
//{
	//for (uint8_t col = column; col < column + size; ++col)
	//{
		//bool isSolid = (th::video::m_page & 0x01) ^ (col >> 3 & 0x01);
		//buffer[col - column] = isSolid ? ((col & 0x07) == 0 || (col & 0x07) == 7 ? 0xFF : ((col & 0x01 ^ th::video::m_oddFrame ? 0xAB : 0xD5))) : 0x00;
	//}
//}

void RenderBackground()
{
#if 1
	th::video::RenderFlags rf = th::video::RF_EMPTY;
	//rf |= th::video::RF_TRANSP;
	//rf |= th::video::RF_INVERSE;
	//rf |= th::video::RF_FLIP_Y;
	//rf |= th::video::RF_FLIP_X;
	th::video::renderBitmap(rf, 0, 0, 128, 64, tb_picture);
#else
	th::video::fillRenderBufferDirect(&FillBuffer);
#endif
}

void RenderSprite(uint8_t x, uint8_t y)
{
	th::video::RenderFlags rf = th::video::RF_EMPTY;
	rf |= th::video::RF_TRANSP;
	//rf |= th::video::RF_INVERSE;

	th::video::setTileBank(tb_tile_square);
	
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
	//rf |= th::video::RF_TRANSP;
	//rf |= th::video::RF_INVERSE;
	th::video::renderBitmap(rf, x, y, 48, 16, th::assets::tb_logo);
}

void RenderSomeText(uint8_t x, uint8_t y)
{
	th::video::setFontBank(&th::assets::fb_font4x8);
	th::video::renderText(th::video::RF_EMPTY, x, y, "Some text!", 10);
}

void RenderForeground()
{
	//RenderSprite(x, y);
	//RenderSomeText(128 - x, y + 4);
	//RenderSprite(x + 4, 64 - y);
	RenderLogo(40, y);
}

void RenderFPS()
{
	uint8_t f = fps;
	th::video::setFontBank(&th::assets::fb_font6x8);
	for (uint8_t x = 0; x <= 12; x += 6)
	{
		th::video::renderChar(th::video::RF_EMPTY, 12 - x, 0, '0' + (f % 10));
		f /= 10;
	}
}

void RenderBattery()
{
	uint8_t bat = th::battery::getPercent();
	th::video::setFontBank(&th::assets::fb_font6x8);
	for (uint8_t x = 0; x <= 12; x += 6)
	{
		th::video::renderChar(th::video::RF_EMPTY, 12 - x, 64-8, '0' + (bat % 10));
		bat /= 10;
	}
	th::video::renderChar(th::video::RF_EMPTY, 12 + 6, 64-8, '%');
}


void RenderSequence()
{
	th::video::fillRenderBuffer(0x00);
	RenderBackground();
	RenderForeground();
//	RenderFPS();

	//th::video::setRenderConfig(&RenderSequence, 0x07, 0x1F5F);
}

void init()
{
	saved_time = th::engine::getCurrentTimeMillis();
	frame_count = 0;
	fps = 0;

	th::video::setRenderConfig(&RenderSequence);
}

void update()
{
	uint32_t current_time = th::engine::getCurrentTimeMillis();
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

	//th::video::setScrollXY(0, 0);
	//th::display::writeCmd(0xD3, y);
}