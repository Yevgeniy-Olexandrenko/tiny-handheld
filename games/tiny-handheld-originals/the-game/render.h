#pragma once

namespace th
{
	namespace render
	{
		typedef void (*RenderLayerCallback)(uint8_t page, uint8_t column, bool isOddFrame, uint8_t& mask, uint8_t& bits);

		void init();
		void update();
		void setRenderSequence(const RenderLayerCallback* sequence);
	}
}
