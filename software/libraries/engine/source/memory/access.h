#pragma once

#include "../board/eeprom.h"

namespace th
{
	namespace memory
	{
		namespace pgmem
		{
			inline uint8_t readByteImpl(const void* src)
			{
				return pgm_read_byte(reinterpret_cast<uint16_t>(src));
			}
			
			inline void readBlockImpl(const void* src, void* dst, size_t size)
			{
				memcpy_P(dst, src, size);
			}
		}
		
		namespace eemem
		{
			inline uint8_t readByteImpl(const void* src)
			{
				#ifdef USE_EXT_STORAGE
				return eeprom::readByte(reinterpret_cast<uint16_t>(src));
				#else
				return eeprom_read_byte(reinterpret_cast<const uint8_t*>(src));
				#endif
			}
			
			inline uint8_t writeByteImpl(void* dst, uint8_t data)
			{
				#ifdef USE_EXT_STORAGE
				eeprom::writeByte(reinterpret_cast<uint16_t>(dst), data);
				#else
				eeprom_update_byte(reinterpret_cast<uint8_t*>(dst), data);
				#endif
			}
			
			inline void readBlockImpl(const void* src, void* dst, size_t size)
			{
				#ifdef USE_EXT_STORAGE
				eeprom::readBlock(reinterpret_cast<uint16_t>(src), reinterpret_cast<uint8_t*>(dst), size);
				#else
				eeprom_read_block(dst, src, size);
				#endif
			}

			inline void writeBlockImpl(const void* src, void* dst, size_t size)
			{
				#ifdef USE_EXT_STORAGE
				eeprom::writeBlock(reinterpret_cast<const uint8_t*>(src), reinterpret_cast<uint16_t>(dst), size);
				#else
				eeprom_update_block(src, dst, size);
				#endif
			}
		}
		
		namespace storage
		{
			inline uint8_t readByteImpl(const void* src)
			{
				#ifdef USE_EXT_STORAGE
				return eeprom::readByte(reinterpret_cast<uint16_t>(src));
				#else
				return pgm_read_byte(reinterpret_cast<uint16_t>(src));
				#endif
			}
			
			inline void readBlockImpl(const void* src, void* dst, size_t size)
			{
				#ifdef USE_EXT_STORAGE
				eeprom::readBlock(reinterpret_cast<uint16_t>(src), reinterpret_cast<uint8_t*>(dst), size);
				#else
				memcpy_P(dst, src, size);
				#endif
			}
		}
	}
}