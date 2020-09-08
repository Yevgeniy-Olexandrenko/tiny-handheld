#pragma once

#include "commons.h"
#include "board/eeprom.h"

#define DISABLE_EXT_EEPROM 0

namespace th
{
	namespace memory
	{
		enum class Type : uint8_t
		{
			NONE,
			MCU_FLASH,
			EXT_EEPROM,

		#if DISABLE_EXT_EEPROM
			STORAGE = MCU_FLASH
		#else
			STORAGE = EXT_EEPROM
		#endif			
		};

		template <Type M, typename T>
		struct Wrapper;

		template <bool V, typename T, typename F>
		struct select
		{
			typedef T type;
		};

		template <typename T, typename F>
		struct select<false, T, F>
		{
			typedef F type;
		};

		template <Type M, typename T>
		struct MultiByte
		{
			T Read(const Wrapper<Type::MCU_FLASH, T> *data)
			{
				T ret;
				memcpy_P(&ret, data, sizeof(T));
				return ret;
			}

			T Read(const Wrapper<Type::EXT_EEPROM, T> *data)
			{
				T ret;
				eeprom::readBlock(&ret, data, sizeof(T));
				return ret;
			}
		};

		template <Type M, typename T>
		struct SingleByte
		{
			T Read(const Wrapper<Type::MCU_FLASH, T> *data)
			{
				return pgm_read_byte(data);
			}

			T Read(const Wrapper<Type::EXT_EEPROM, T> *data)
			{
				return eeprom::readByte(data);
			}
		};

		template <Type M, typename T>
		struct Wrapper
		{
			typedef typename select<sizeof(T) == 1, SingleByte<M, T>, MultiByte<M, T>>::type Reader;

			operator const T() const
			{
				return Reader().Read(this);
			}

			bool operator==(const T &in)
			{
				return Reader().Read(this) == in;
			}

			T t;
		};

	}
} 

// Flash memory data types
typedef const th::memory::Wrapper<th::memory::Type::MCU_FLASH, int8_t>   int8_f;
typedef const th::memory::Wrapper<th::memory::Type::MCU_FLASH, uint8_t>  uint8_f;
typedef const th::memory::Wrapper<th::memory::Type::MCU_FLASH, int16_t>  int16_f;
typedef const th::memory::Wrapper<th::memory::Type::MCU_FLASH, uint16_t> uint16_f;
typedef const th::memory::Wrapper<th::memory::Type::MCU_FLASH, int32_t>  int32_f;
typedef const th::memory::Wrapper<th::memory::Type::MCU_FLASH, uint32_t> uint32_f;
typedef const th::memory::Wrapper<th::memory::Type::MCU_FLASH, int64_t>  int64_f;
typedef const th::memory::Wrapper<th::memory::Type::MCU_FLASH, uint64_t> uint64_f;
typedef const th::memory::Wrapper<th::memory::Type::MCU_FLASH, bool>     bool_f;
typedef const th::memory::Wrapper<th::memory::Type::MCU_FLASH, float>    float_f;
typedef const th::memory::Wrapper<th::memory::Type::MCU_FLASH, double>   double_f;
typedef const th::memory::Wrapper<th::memory::Type::MCU_FLASH, size_t>   size_f;

// Data storage data types
typedef const th::memory::Wrapper<th::memory::Type::STORAGE, int8_t>   int8_s;
typedef const th::memory::Wrapper<th::memory::Type::STORAGE, uint8_t>  uint8_s;
typedef const th::memory::Wrapper<th::memory::Type::STORAGE, int16_t>  int16_s;
typedef const th::memory::Wrapper<th::memory::Type::STORAGE, uint16_t> uint16_s;
typedef const th::memory::Wrapper<th::memory::Type::STORAGE, int32_t>  int32_s;
typedef const th::memory::Wrapper<th::memory::Type::STORAGE, uint32_t> uint32_s;
typedef const th::memory::Wrapper<th::memory::Type::STORAGE, int64_t>  int64_s;
typedef const th::memory::Wrapper<th::memory::Type::STORAGE, uint64_t> uint64_s;
typedef const th::memory::Wrapper<th::memory::Type::STORAGE, bool>     bool_s;
typedef const th::memory::Wrapper<th::memory::Type::STORAGE, float>    float_s;
typedef const th::memory::Wrapper<th::memory::Type::STORAGE, double>   double_s;
typedef const th::memory::Wrapper<th::memory::Type::STORAGE, size_t>   size_s;

// Data definition helpers
#if DISABLE_EXT_EEPROM
	#define IN_FLASH   PROGMEM
	#define IN_STORAGE PROGMEM
#else
	#define IN_FLASH   PROGMEM
	#define IN_STORAGE EEMEM
#endif

#define BINARY_IN_FLASH(name)   uint8_f name[] IN_FLASH
#define BINARY_IN_STORAGE(name) uint8_s name[] IN_STORAGE
