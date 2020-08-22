#pragma once

namespace th
{
	namespace render
	{
		typedef void (*RenderLayerCallback)(uint8_t page, uint8_t column, uint8_t& bits, uint8_t& mask, bool isOddFrame);

		void init();
		void update();
		void setRenderSequence(const RenderLayerCallback* sequence);
		void getTileRenderData(const uint8_t* tileData, uint8_t tileF, uint8_t tileX, uint8_t tileY, uint8_t page, uint8_t column, uint8_t& bits);
		void getTileRenderData(const uint8_t* tileData, uint8_t tileF, uint8_t tileX, uint8_t tileY, uint8_t page, uint8_t column, uint8_t& bits, uint8_t& mask, bool isOddFrame);
	}
}
