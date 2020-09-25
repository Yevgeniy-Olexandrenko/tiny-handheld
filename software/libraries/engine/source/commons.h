#pragma once

#define F_CPU 16000000

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

#define max(a,b) ((a) > (b) ? (a) : (b))
#define min(a,b) ((a) < (b) ? (a) : (b))
#define set_bit(sfr, bit) ((sfr) |= _BV(bit))
#define clear_bit(sfr, bit) ((sfr) &= ~_BV(bit))
#define is_bit_set(sfr, bit) ((sfr) & _BV(bit))
#define is_bit_clear(sfr, bit) (!((sfr) & _BV(bit)))

// Select different types based on a condition, an 'if statement' for types.
template< bool V, typename T, typename F > struct select { typedef T type; };
template< typename T, typename F > struct select< false, T, F > { typedef F type; };

// A basic comparison of two types
template < typename T, typename U > struct is_same { enum { value = false }; };
template < typename T > struct is_same< T, T > { enum { value = true }; };

typedef int8_t   s08;
typedef uint8_t  u08;
typedef int16_t  s16;
typedef uint16_t u16;
typedef int32_t  s32;
typedef uint32_t u32;
typedef int64_t  s64;
typedef uint64_t u64;
typedef uint8_t* p08;

