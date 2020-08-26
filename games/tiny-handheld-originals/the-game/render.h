#pragma once

namespace th
{
	namespace render
	{
		// rendering sequence type defs
		typedef void (*RenderLayerCallback)(uint8_t page, uint8_t column, uint8_t& bits, uint8_t& mask, bool isOddFrame);
		typedef const RenderLayerCallback * RenderSequence;

		// tile data access type defs
		enum TileStorage : uint8_t
		{
			TS_SRAM, TS_PROGMEM, TS_EEPROM, TS_UNCHANGED
		};
		typedef const uint8_t* TileBankAddr;
		typedef uint16_t TileDataAddr;

		// tile data type defs
		enum TileFlags : uint8_t
		{
			TF_EMPTY   = 0x00,
			TF_FLIP_Y  = 0x80,
			TF_FLIP_X  = 0x40,
			TF_INVERSE = 0x20,
			TF_WIDTH_B = 0x1F
		};

		void init();
		void update();
		void setRenderSequence(RenderSequence renderSequence);

		void setTileBank(TileStorage tileStorage, TileBankAddr tileBankAddr);
		void renderTile(TileDataAddr tileDataAddr, TileFlags tileF, uint8_t tileX, uint8_t tileY, uint8_t page, uint8_t column, uint8_t &bits);
		void renderTile(TileDataAddr tileDataAddr, TileFlags tileF, uint8_t tileX, uint8_t tileY, uint8_t page, uint8_t column, uint8_t &bits, uint8_t &mask, bool isOddFrame);
	}
}
