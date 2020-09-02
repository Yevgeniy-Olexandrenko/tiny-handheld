/*
 * SSD1306xLED - Drivers for SSD1306 controlled dot matrix OLED/PLED 128x64 displays
 *
 * @created: 2014-08-12
 * @author: Neven Boyanov
 *
 * Source code available at: https://bitbucket.org/tinusaur/ssd1306xled
 *
 * MINIMIZED BY SAUMIL SHAH
 */
#include <stdint.h>
#include <Arduino.h>
#include <util/delay.h>

#ifndef DIGISPARKOLED_H
#define DIGISPARKOLED_H

// ----------------------------------------------------------------------------

#define SSD1306_SCL  PB2
#define SSD1306_SDA  PB0
#define SSD1306_SA   0x78

// ----------------------------------------------------------------------------

class SSD1306Device: public Print {

    public:
      SSD1306Device(void);
      void begin(void);
      void ssd1306_send_command(uint8_t command);
      void ssd1306_send_data_byte(uint8_t byte);
      void ssd1306_send_data_start(void);
      void ssd1306_send_data_stop(void);
      void setCursor(uint8_t x, uint8_t y);
      void fill(uint8_t fill);
      void clear(void);
      void ssd1306_send_command_start(void);
      void ssd1306_send_command_stop(void);
      virtual size_t write(byte c);
      using Print::write;
};


extern SSD1306Device oled;

// ----------------------------------------------------------------------------

#endif
