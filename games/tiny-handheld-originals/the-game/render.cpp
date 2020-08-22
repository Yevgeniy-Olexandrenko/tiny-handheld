#include "commons.h"
#include "render.h"
#include "display.h"

namespace th
{
	namespace render
	{
		const RenderLayerCallback* renderSequence;
		
		void init()
		{
			renderSequence = NULL;
		}

		void update()
		{
			if (renderSequence)
			{
				uint8_t page, column, index, mask, bits, composed;
				for (page = 0; page < 8; ++page)
				{
					display::setPos(page, 0);
					display::startData();
					for (column = 0; column < 128; ++column)
					{
						composed = 0x00;
						for (index = 0; renderSequence[index]; ++index)
						{
							mask = 0xFF;
							bits = 0x00;
							renderSequence[index](page, column, false, mask, bits);
							composed &= mask;
							composed |= bits;
						}
						display::write(composed);
					}
				}
				display::stop();
			}
		}

		void setRenderSequence(const RenderLayerCallback *sequence)
		{
			renderSequence = sequence;
		}
	}
}
