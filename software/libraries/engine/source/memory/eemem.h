#pragma once

namespace th
{
	namespace memory
	{
		namespace eemem
		{
			template <typename T>
			struct is_integer
			{
				enum
				{
					V8    = is_same<T, char>::value || is_same<T, unsigned char>::value || is_same<T, signed char>::value,
					V16   = is_same<T, unsigned int>::value || is_same<T, signed int>::value,
					V32   = is_same<T, unsigned long>::value || is_same<T, signed long>::value,
					V64   = is_same<T, unsigned long long>::value || is_same<T, signed long long>::value,
					value = V8 || V16 || V32 || V64
				};
			};
		
			template <typename T>
			struct is_float
			{
				enum { value = is_same<T, float>::value || is_same<T, double>::value || is_same<T, long double>::value };
			};
		
			template <typename T>
			struct is_number
			{
				enum { value = is_float<T>::value || is_integer<T>::value };
			};
		
			template <typename T>
			struct is_bool
			{
				enum { value = is_same<T, bool>::value };
			};

			template <typename T>
			struct is_fundamental
			{
				enum { value = is_number<T>::value || is_bool<T>::value };
			};

			template <typename D, typename T> 
			class PrimitiveWrap
			{
			public:
				D &operator+=(const T &in) { return self().Write(self().Read() + in); }
				D &operator-=(const T &in) { return self().Write(self().Read() - in); }
				D &operator*=(const T &in) { return self().Write(self().Read() * in); }
				D &operator/=(const T &in) { return self().Write(self().Read() / in); }

				D &operator|=(const T &in) { return self().Write(self().Read() | in); }
				D &operator&=(const T &in) { return self().Write(self().Read() & in); }
				D &operator%=(const T &in) { return self().Write(self().Read() % in); }
				D &operator^=(const T &in) { return self().Write(self().Read() ^ in); }

				template <typename U>
				D &operator<<=(const U &in) { return self().Write(self().Read() << in); }

				template <typename U>
				D &operator>>=(const U &in) { return self().Write(self().Read() >> in); }

				D &operator++() { return self().Write(self().Read() + 1); }
				D &operator--() { return self().Write(self().Read() - 1); }

				T operator++(int)
				{
					T ret = self().Read();
					self().Write(ret + 1);
					return ret;
				}

				T operator--(int)
				{
					T ret = self().Read();
					self().Write(ret - 1);
					return ret;
				}

			protected:
				D& self() { return *static_cast< D* >( this ); }
				const D& self() const { return *static_cast< const D* >( this ); }
			};

			struct NoPrimitiveWrap {};

			template <typename D, typename T> 
			class MultiByte
			{
			public:
				T Read(void)
				{
					T ret;
					readBlockImpl(&self(), &ret, sizeof(T));
					return ret;
				}

				D& Write(const T &v)
				{
					writeBlockImpl(&v, &self(), sizeof(T));
					return self();
				}

			protected:
				D& self() { return *static_cast< D* >( this ); }
				const D& self() const { return *static_cast< const D* >( this ); }
			};

			template <typename D, typename T> 
			class SingleByte
			{
			public:
				T Read(void) 
				{ 
					return readByteImpl(&self());
				}

				D& Write(const T &v)
				{
					writeByteImpl(&self(), v);
					return self();
				}

			protected:
				D& self() { return *static_cast< D* >( this ); }
				const D& self() const { return *static_cast< const D* >( this ); }
			};
		
			template <typename T> 
			struct Wrap;
			
			template <typename T> 
			struct Mode
			{
				typedef typename select< sizeof(T) == 1, SingleByte<Wrap<T>, T>, MultiByte<Wrap<T>, T> >::type Interface;
				typedef typename select<is_fundamental<T>::value, PrimitiveWrap<Wrap<T>, T>, NoPrimitiveWrap >::type Extension;
			};

			template <typename T> 
			struct Wrap : Mode<T>::Interface, Mode<T>::Extension
			{
				T t;

				using Mode<T>::Interface::Read;
				using Mode<T>::Interface::Write;

				operator const T() { return Read(); }
				Wrap& operator=(const T &in) { return Write(in); }
				bool operator==(const T &in) { return Read() == in; }
			};
		}
	}
}

typedef th::memory::eemem::Wrap<char>               int8_e;
typedef th::memory::eemem::Wrap<unsigned char>      uint8_e;
typedef th::memory::eemem::Wrap<int>                int16_e;
typedef th::memory::eemem::Wrap<unsigned int>       uint16_e;
typedef th::memory::eemem::Wrap<long>               int32_e;
typedef th::memory::eemem::Wrap<unsigned long>      uint32_e;
typedef th::memory::eemem::Wrap<long long>          int64_e;
typedef th::memory::eemem::Wrap<unsigned long long> uint64_e;
typedef th::memory::eemem::Wrap<bool>               bool_e;
typedef th::memory::eemem::Wrap<float>              float_e;
typedef th::memory::eemem::Wrap<double>             double_e;
typedef th::memory::eemem::Wrap<long double>        long_double_e;