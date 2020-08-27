#pragma once

namespace th
{
	namespace render
	{
		// rendering sequence type defs
		struct RenderContext
		{
			uint8_t page, pageY, column;
			uint8_t bits, mask, composed;
			uint8_t isOddFrame;
		};
		typedef void (*RenderLayerCallback)(RenderContext& renderContext);
		typedef const RenderLayerCallback * RenderSequence;

		// tile data access type defs
		enum TileStorage : uint8_t
		{
			TS_SRAM, TS_PROGMEM, TS_EEPROM, TS_UNCHANGED
		};
		typedef const uint8_t* TileBankAddr;
		typedef uint16_t TileDataAddr;
		typedef uint8_t TileIndex;

		// tile data type defs
		enum TileFlags : uint16_t
		{
			TF_EMPTY        = 0x0000,
			TF_CONFIG_BITS  = 0xF000,
			TF_CONTROL_BITS = 0x0F00,
			TF_WIDTH_BITS   = 0x00FF,

			TF_HAS_MASK     = 0x8000,
			TF_HAS_ODD_BM   = 0x4000,
			
			TF_FLIP_Y       = 0x0800,
			TF_FLIP_X       = 0x0400,
			TF_TRANSPARENT  = 0x2000,
			TF_INVERSE      = 0x0100
		};

		// font data type defs
		struct FontData
		{
			TileStorage tileStorage;
			TileBankAddr tileBankAddr;
			TileFlags tileFlags;
			uint8_t asciiOffset;
		};

		void init();
		void update();

		void setRenderSequence(RenderSequence renderSequence);
		void flushRenderContext();

		void setTileBank(TileStorage tileStorage, TileBankAddr tileBankAddr);
		TileDataAddr getTileDataAddr(TileFlags& tileF, TileIndex tileI);

		void renderTile(TileDataAddr tileDataAddr, TileFlags tileF, uint8_t tileX, uint8_t tileY);
		void renderText(const FontData& fontData, TileFlags tileF, uint8_t x, uint8_t y, const char* text, uint8_t length);
	}
}
