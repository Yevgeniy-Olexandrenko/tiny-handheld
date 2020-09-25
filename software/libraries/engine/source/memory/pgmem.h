#pragma once

namespace th
{
	namespace memory
	{		
		namespace pgmem
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

typedef const th::memory::pgmem::Wrap<char>               int8_p;
typedef const th::memory::pgmem::Wrap<unsigned char>      uint8_p;
typedef const th::memory::pgmem::Wrap<int>                int16_p;
typedef const th::memory::pgmem::Wrap<unsigned int>       uint16_p;
typedef const th::memory::pgmem::Wrap<long>               int32_p;
typedef const th::memory::pgmem::Wrap<unsigned long>      uint32_p;
typedef const th::memory::pgmem::Wrap<long long>          int64_p;
typedef const th::memory::pgmem::Wrap<unsigned long long> uint64_p;
typedef const th::memory::pgmem::Wrap<bool>               bool_p;
typedef const th::memory::pgmem::Wrap<float>              float_p;
typedef const th::memory::pgmem::Wrap<double>             double_p;
typedef const th::memory::pgmem::Wrap<long double>        long_double_p;
