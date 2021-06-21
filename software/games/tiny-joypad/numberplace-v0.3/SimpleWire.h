/*
  SimpleWire.h - I2C Single Master Mode Library for AVR

  Copyright (c) 2020 Sasapea's Lab. All right reserved.

  exsample:

    #define SimpleWire_SCL_PORT B
    #define SimpleWire_SCL_POS  1
    #define SimpleWire_SDA_PORT B
    #define SimpleWire_SDA_POS  3
    #include "SimpleWire.h"

    #define SIMPLEWIRE SimpleWire<SimpleWire_100K>
    #define SLAVE_ADDR 0x20

    void setup()
    {
      SIMPLEWIRE::begin();
    }

    void loop()
    {
      uint8_t len, buf[32];
      ...
      // write
      if (SIMPLEWIRE::write(SLAVE_ADDR, buf, len) != (int)len)
        // error
      ...
      // read
      if (SIMPLEWIRE::read(SLAVE_ADDR, buf, len) != (int)len)
        // error
      ...
      // device scan
      for (uint8_t addr = 0x08; addr <= 0x7F; ++addr)
      {
        if (SIMPLEWIRE::write(addr, 0, 0) == 0)
          // device found.
    }

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA
*/
#ifndef __SIMPLEWIRE_H
#define __SIMPLEWIRE_H

#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <util/delay_basic.h>

#ifndef SimpleWire_HS_MODE
  #define SimpleWire_HS_MODE 1
#endif

#define SimpleWire_100K 0
#define SimpleWire_400K 1
#define SimpleWire_1M   2

#define SimpleWire_WRITE 0
#define SimpleWire_READ  1

#define SimpleWire_CAT(a, b) a##b
#define SimpleWire_REG(r, p) SimpleWire_CAT(r, p)
#define SimpleWire_SCL(r) SimpleWire_REG(r, SimpleWire_SCL_PORT)
#define SimpleWire_SDA(r) SimpleWire_REG(r, SimpleWire_SDA_PORT)

#if SimpleWire_HS_MODE
  #define SimpleWire_SCL_INIT
  #define SimpleWire_SCL_HIGH do { \
    cli(); \
    SimpleWire_SCL(PORT) |=  _BV(SimpleWire_SCL_POS); \
    SimpleWire_SCL(DDR ) &= ~_BV(SimpleWire_SCL_POS); \
    sei(); \
  } while (0)
  #define SimpleWire_SCL_LOW  do { \
    cli(); \
    SimpleWire_SCL(PORT) &= ~_BV(SimpleWire_SCL_POS); \
    SimpleWire_SCL(DDR ) |=  _BV(SimpleWire_SCL_POS); \
    sei(); \
  } while (0);
  #define SimpleWire_SDA_INIT
  #define SimpleWire_SDA_HIGH do { \
    cli(); \
    SimpleWire_SDA(PORT) |=  _BV(SimpleWire_SDA_POS); \
    SimpleWire_SDA(DDR ) &= ~_BV(SimpleWire_SDA_POS); \
    sei(); \
  } while (0)
  #define SimpleWire_SDA_LOW  do { \
    cli(); \
    SimpleWire_SDA(PORT) &= ~_BV(SimpleWire_SDA_POS); \
    SimpleWire_SDA(DDR ) |=  _BV(SimpleWire_SDA_POS); \
    sei(); \
  } while (0)
#else
  #define SimpleWire_SCL_INIT SimpleWire_SCL(PORT) &= ~_BV(SimpleWire_SCL_POS)
  #define SimpleWire_SCL_HIGH SimpleWire_SCL(DDR)  &= ~_BV(SimpleWire_SCL_POS)
  #define SimpleWire_SCL_LOW  SimpleWire_SCL(DDR)  |=  _BV(SimpleWire_SCL_POS)
  #define SimpleWire_SDA_INIT SimpleWire_SDA(PORT) &= ~_BV(SimpleWire_SDA_POS)
  #define SimpleWire_SDA_HIGH SimpleWire_SDA(DDR)  &= ~_BV(SimpleWire_SDA_POS)
  #define SimpleWire_SDA_LOW  SimpleWire_SDA(DDR)  |=  _BV(SimpleWire_SDA_POS)
#endif
#define SimpleWire_SDA_READ (SimpleWire_SDA(PIN) & _BV(SimpleWire_SDA_POS))

#define SimpleWire_DELAY(t) \
do { \
  uint8_t n = (uint8_t)((((t) / 1000.0) * (F_CPU / 1000000) * 256) / 768); \
  if (n) _delay_loop_1(n); \
} while (0)

//
// Standard Mode
//
#define SimpleWire_100K_THDSTA SimpleWire_DELAY(4000) // 4000
#define SimpleWire_100K_TLOW   SimpleWire_DELAY(4400) // 4700
#define SimpleWire_100K_THIGH  SimpleWire_DELAY(4000) // 4000
#define SimpleWire_100K_TSUSTA SimpleWire_DELAY(4700) // 4700
#define SimpleWire_100K_THDDAT SimpleWire_DELAY( 200) //  300
#define SimpleWire_100K_TSUSTO SimpleWire_DELAY(4000) // 4000
#define SimpleWire_100K_TSUDAT SimpleWire_DELAY( 200) //  250
#define SimpleWire_100K_TBUF   SimpleWire_DELAY(4700) // 4700
//
// Fast Mode
//
#define SimpleWire_400K_THDSTA                       //  600
#define SimpleWire_400K_TLOW   SimpleWire_DELAY( 800) // 1300
#define SimpleWire_400K_THIGH  SimpleWire_DELAY( 200) //  600
#define SimpleWire_400K_TSUSTA SimpleWire_DELAY( 600) //  600
#define SimpleWire_400K_THDDAT                       //    0
#define SimpleWire_400K_TSUSTO SimpleWire_DELAY( 400) //  600
#define SimpleWire_400K_TSUDAT                       //  100
#define SimpleWire_400K_TBUF   SimpleWire_DELAY(1300) // 1300
//
// Hs Mode
//
#define SimpleWire_1M_THDSTA // 160
#define SimpleWire_1M_TLOW   // 160
#define SimpleWire_1M_THIGH  //  60
#define SimpleWire_1M_TSUSTA // 160
#define SimpleWire_1M_THDDAT //   0
#define SimpleWire_1M_TSUSTO // 160
#define SimpleWire_1M_TSUDAT //  10
#define SimpleWire_1M_TBUF   //  60

#define SimpleWire_DELAY_THDSTA(m) \
do { \
  if      ((m) == SimpleWire_1M  ) SimpleWire_1M_THDSTA;   \
  else if ((m) == SimpleWire_400K) SimpleWire_400K_THDSTA; \
  else                            SimpleWire_100K_THDSTA; \
} while (0)

#define SimpleWire_DELAY_TLOW(m) \
do { \
  if      ((m) == SimpleWire_1M  ) SimpleWire_1M_TLOW;   \
  else if ((m) == SimpleWire_400K) SimpleWire_400K_TLOW; \
  else                            SimpleWire_100K_TLOW; \
} while (0)

#define SimpleWire_DELAY_THIGH(m) \
do { \
  if      ((m) == SimpleWire_1M  ) SimpleWire_1M_THIGH;   \
  else if ((m) == SimpleWire_400K) SimpleWire_400K_THIGH; \
  else                            SimpleWire_100K_THIGH; \
} while (0)

#define SimpleWire_DELAY_TSUSTA(m) \
do { \
  if      ((m) == SimpleWire_1M  ) SimpleWire_1M_TSUSTA;   \
  else if ((m) == SimpleWire_400K) SimpleWire_400K_TSUSTA; \
  else                            SimpleWire_100K_TSUSTA; \
} while (0)

#define SimpleWire_DELAY_THDDAT(m) \
do { \
  if      ((m) == SimpleWire_1M  ) SimpleWire_1M_THDDAT;   \
  else if ((m) == SimpleWire_400K) SimpleWire_400K_THDDAT; \
  else                            SimpleWire_100K_THDDAT; \
} while (0)

#define SimpleWire_DELAY_TSUSTO(m) \
do { \
  if      ((m) == SimpleWire_1M  ) SimpleWire_1M_TSUSTO;   \
  else if ((m) == SimpleWire_400K) SimpleWire_400K_TSUSTO; \
  else                            SimpleWire_100K_TSUSTO; \
} while (0)

#define SimpleWire_DELAY_TSUDAT(m) \
do { \
  if      ((m) == SimpleWire_1M  ) SimpleWire_1M_TSUDAT;   \
  else if ((m) == SimpleWire_400K) SimpleWire_400K_TSUDAT; \
  else                            SimpleWire_100K_TSUDAT; \
} while (0)

#define SimpleWire_DELAY_TBUF(m) \
do { \
  if      ((m) == SimpleWire_1M  ) SimpleWire_1M_TBUF;   \
  else if ((m) == SimpleWire_400K) SimpleWire_400K_TBUF; \
  else                            SimpleWire_100K_TBUF; \
} while (0)

template<uint8_t MODE = SimpleWire_100K>
class SimpleWire
{
  private:

    static void start(void)
    {
      SimpleWire_SDA_LOW;
      SimpleWire_DELAY_THDSTA(MODE);
      SimpleWire_SCL_LOW;
      SimpleWire_DELAY_THDDAT(MODE);
    }

    static void stop(void)
    {
      SimpleWire_SDA_LOW;
      SimpleWire_DELAY_TLOW(MODE);
      SimpleWire_SCL_HIGH;
      SimpleWire_DELAY_TSUSTO(MODE);
      SimpleWire_SDA_HIGH;
      SimpleWire_DELAY_TBUF(MODE);
    }

    static uint8_t write(uint8_t b)
    {
      for (uint8_t i = 0x80; i; i >>= 1)
      {
        if (b & i)
          SimpleWire_SDA_HIGH;
        else
          SimpleWire_SDA_LOW;
        SimpleWire_DELAY_TLOW(MODE);
        SimpleWire_SCL_HIGH;
        SimpleWire_DELAY_THIGH(MODE);
        SimpleWire_SCL_LOW;
        SimpleWire_DELAY_THDDAT(MODE);
      }
      SimpleWire_SDA_HIGH;
      SimpleWire_DELAY_TLOW(MODE);
      SimpleWire_SCL_HIGH;
      SimpleWire_DELAY_THIGH(MODE);
      b = SimpleWire_SDA_READ;
      SimpleWire_SCL_LOW;
      SimpleWire_DELAY_THDDAT(MODE);
      return b;
    }

    static uint8_t read(void)
    {
      uint8_t b = 0;
      SimpleWire_SDA_HIGH;
      for (uint8_t i = 0x80; i; i >>= 1)
      {
        SimpleWire_DELAY_TLOW(MODE);
        SimpleWire_SCL_HIGH;
        SimpleWire_DELAY_THIGH(MODE);
        if (SimpleWire_SDA_READ)
          b |= i;
        SimpleWire_SCL_LOW;
        SimpleWire_DELAY_THDDAT(MODE);
      }
      SimpleWire_SDA_LOW;
      SimpleWire_DELAY_TLOW(MODE);
      SimpleWire_SCL_HIGH;
      SimpleWire_DELAY_THIGH(MODE);
      SimpleWire_SCL_LOW;
      SimpleWire_DELAY_THDDAT(MODE);
      return b;
    }

  public:

    SimpleWire(void)
    {
    }

    virtual ~SimpleWire(void)
    {
    }

    static void begin(void)
    {
      SimpleWire_SCL_INIT;
      SimpleWire_SCL_HIGH;
      SimpleWire_SDA_INIT;
      SimpleWire_SDA_HIGH;
    }

    static int write(uint8_t addr, const uint8_t *buf, uint8_t len)
    {
      int cnt = -1;
      // start
      start();
      // write slave address
      if (write((addr << 1) | SimpleWire_WRITE) == 0)
      {
        // write data
        for (cnt = 0; cnt < len; ++cnt)
        {
          if (write(*buf++))
            break;
        }
      }
      // stop
      stop();
      return cnt;
    }

    static int read(uint8_t addr, uint8_t *buf, uint8_t len)
    {
      int cnt = -1;
      // start
      start();
      // write slave address
      if (write((addr << 1) | SimpleWire_READ) == 0)
      {
        // read data
        for (cnt = 0; cnt < len; ++cnt)
          *buf++ = read();
      }
      // stop
      stop();
      return cnt;
    }
};

template<uint8_t MODE = SimpleWire_100K, uint8_t BUFFER_LENGTH = 32>
class TwoWire
{
  private:

    uint8_t _buffer[BUFFER_LENGTH];
    uint8_t _count;
    uint8_t _index;
    uint8_t _error;
    uint8_t _addr;

  public:

    TwoWire(void)
    : _count(0)
    , _index(0)
    , _error(0)
    , _addr(0)
    {
    }

    virtual ~TwoWire(void)
    {
    }

    void begin(void)
    {
      SimpleWire<MODE>::begin();
    }

    void end(void)
    {
    }

    void beginTransmission(uint8_t address)
    {
      _addr  = address;
      _count = 0;
      _index = 0;
      _error = 0;
    }

    void beginTransmission(int address)
    {
      beginTransmission((uint8_t)address);
    }

    uint8_t endTransmission(uint8_t sendStop = true)
    {
      if (_error)
        return 1; // buffer overflow
      int rv = SimpleWire<MODE>::write(_addr, _buffer, _index);
      if (rv == _index)
        return 0; // success
      return (rv < 0 ? 2 : 3);
    }

    uint8_t requestFrom(uint8_t address, uint8_t quantity)
    {
      return requestFrom((uint8_t)address, (uint8_t)quantity, (uint8_t)true);
    }

    uint8_t requestFrom(uint8_t address, uint8_t quantity, uint8_t sendStop)
    {
      return requestFrom((uint8_t)address, (uint8_t)quantity, (uint32_t)0, (uint8_t)0, (uint8_t)sendStop);
    }

    uint8_t requestFrom(int address, int quantity)
    {
      return requestFrom((uint8_t)address, (uint8_t)quantity, (uint8_t)true);
    }

    uint8_t requestFrom(int address, int quantity, int sendStop)
    {
      return requestFrom((uint8_t)address, (uint8_t)quantity, (uint8_t)sendStop);
    }

    uint8_t requestFrom(uint8_t address, uint8_t quantity, uint32_t iaddress, uint8_t isize, uint8_t sendStop)
    {
      if (isize > 0)
      {
        // send internal address; this mode allows sending a repeated start to access
        // some devices' internal registers. This function is executed by the hardware
        // TWI module on other processors (for example Due's TWI_IADR and TWI_MMR registers)
        beginTransmission(address);
        // the maximum size of internal address is 3 bytes
        if (isize > 3)
          isize = 3;
        // write internal register address - most significant byte first
        switch (isize)
        {
          case 3:
            write((uint8_t)(iaddress >> 16));
          case 2:
            write((uint8_t)(iaddress >> 8));
          case 1:
            write((uint8_t)(iaddress >> 0));
            break;
        }
        endTransmission(false);
      }
      // clamp to buffer length
      if (quantity > BUFFER_LENGTH)
        quantity = BUFFER_LENGTH;
      // perform blocking read into buffer
      int read = SimpleWire<MODE>::read(address, _buffer, quantity);
      // set rx buffer iterator vars
      _count = read > 0 ? (uint8_t)read : 0;
      _index = 0;
      return _count;
    }

    virtual size_t write(uint8_t data)
    {
      // don't bother if buffer is full
      if (_index == 0xFF)
      {
        _error = 1;
        return 0;
      }
      // put byte in tx buffer
      _buffer[_index++] = data;
      return 1;
    }

    virtual size_t write(uint8_t *data, uint8_t len)
    {
      uint8_t cnt = 0;
      while (len--)
        cnt += write(*data++);
      return cnt;
    }

    virtual int available(void)
    {
      return (_index < _count ? _count - _index : 0);
    }

    virtual int read(void)
    {
      int value = -1;
      // get each successive byte on each call
      if(_index < _count)
        value = _buffer[_index++];
      return value;
    }

    virtual int peek(void)
    {
      int value = -1;
      if (_index < _count)
        value = _buffer[_index];
      return value;
    }

    virtual void flush(void)
    {
    }

    inline size_t write(unsigned long n)
    {
      return write((uint8_t)n);
    }

    inline size_t write(long n)
    {
      return write((uint8_t)n);
    }

    inline size_t write(unsigned int n)
    {
      return write((uint8_t)n);
    }

    inline size_t write(int n)
    {
      return write((uint8_t)n);
    }
};

#endif
