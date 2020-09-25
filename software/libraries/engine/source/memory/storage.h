#pragma once

namespace th
{
	namespace memory
	{
		namespace storage
		{
			template <typename T>
			struct Wrap;
			
			template <typename T>
			struct MultiByte
			{
				static T Read(const Wrap<T> *data)
				{
					T ret;
					readBlockImpl(data, &ret, sizeof(T));
					return ret;
				}
			};

			template <typename T>
			struct SingleByte
			{
				static T Read(const Wrap<T> *data)
				{
					return readByteImpl(data);
				}
			};
			
			template <typename T>
			struct Wrap
			{
				T t;
				typedef typename select< sizeof(T) == 1, SingleByte<T>, MultiByte<T> >::type Reader;

				operator const T() const { return Reader::Read(this); }
				bool operator==(const T &in) { return Reader::Read(this) == in; }
			};
		}
	}
}

typedef const th::memory::storage::Wrap<char>               int8_s;
typedef const th::memory::storage::Wrap<unsigned char>      uint8_s;
typedef const th::memory::storage::Wrap<int>                int16_s;
typedef const th::memory::storage::Wrap<unsigned int>       uint16_s;
typedef const th::memory::storage::Wrap<long>               int32_s;
typedef const th::memory::storage::Wrap<unsigned long>      uint32_s;
typedef const th::memory::storage::Wrap<long long>          int64_s;
typedef const th::memory::storage::Wrap<unsigned long long> uint64_s;
typedef const th::memory::storage::Wrap<bool>               bool_s;
typedef const th::memory::storage::Wrap<float>              float_s;
typedef const th::memory::storage::Wrap<double>             double_s;
typedef const th::memory::storage::Wrap<long double>        long_double_s;
