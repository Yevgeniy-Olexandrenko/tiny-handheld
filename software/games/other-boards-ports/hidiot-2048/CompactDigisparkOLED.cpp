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

// ----------------------------------------------------------------------------

#include <stdlib.h>
#include <avr/io.h>

#include <avr/pgmspace.h>

#include "CompactDigisparkOLED.h"
#include "CompactFont6x8.h"

// ----------------------------------------------------------------------------

#define SSD1306_COMMAND 0x00
#define SSD1306_DATA    0x40

#define I2CSW_H(PORT) PORTB |=  (1 << PORT)
#define I2CSW_L(PORT) PORTB &= ~(1 << PORT)

void i2csw_start() 
{
	DDRB |= (1 << SSD1306_SDA);  // Set port as output
	DDRB |= (1 << SSD1306_SCL);  // Set port as output
	I2CSW_H(SSD1306_SCL);        // Set to HIGH
	I2CSW_H(SSD1306_SDA);        // Set to HIGH
	I2CSW_L(SSD1306_SDA);        // Set to LOW
	I2CSW_L(SSD1306_SCL);        // Set to LOW
}

void i2csw_stop() 
{
	I2CSW_L(SSD1306_SCL);        // Set to LOW
	I2CSW_L(SSD1306_SDA);        // Set to LOW
	I2CSW_H(SSD1306_SCL);        // Set to HIGH
	I2CSW_H(SSD1306_SDA);        // Set to HIGH
	DDRB &= ~(1 << SSD1306_SDA); // Set port as input
}

void i2csw_byte(uint8_t b) 
{
	for (uint8_t i = 0; i < 8; i++) 
	{
	if ((b << i) & 0x80) 
		I2CSW_H(SSD1306_SDA); 
	else 
		I2CSW_L(SSD1306_SDA);
	I2CSW_H(SSD1306_SCL);
	I2CSW_L(SSD1306_SCL);
	}
	I2CSW_H(SSD1306_SDA);
	I2CSW_H(SSD1306_SCL);
	I2CSW_L(SSD1306_SCL);
}

// ----------------------------------------------------------------------------

// Some code based on "IIC_wtihout_ACK" by http://www.14blog.com/archives/1358

const uint8_t ssd1306_init_sequence [] PROGMEM = {   // Initialization Sequence
   0xAE,       // Display OFF (sleep mode)
   0x20, 0b00,    // Set Memory Addressing Mode
               // 00=Horizontal Addressing Mode; 01=Vertical Addressing Mode;
               // 10=Page Addressing Mode (RESET); 11=Invalid
   0xB0,       // Set Page Start Address for Page Addressing Mode, 0-7
   0xC8,       // Set COM Output Scan Direction
   0x00,       // ---set low column address
   0x10,       // ---set high column address
   0x40,       // --set start line address
   0x81, 0x3F,    // Set contrast control register
   0xA1,       // Set Segment Re-map. A0=address mapped; A1=address 127 mapped.
   0xA6,       // Set display mode. A6=Normal; A7=Inverse
   0xA8, 0x3F,    // Set multiplex ratio(1 to 64)
   0xA4,       // Output RAM to Display
               // 0xA4=Output follows RAM content; 0xA5,Output ignores RAM content
   0xD3, 0x00,    // Set display offset. 00 = no offset
   0xD5,       // --set display clock divide ratio/oscillator frequency
   0xF0,       // --set divide ratio
   0xD9, 0x22,    // Set pre-charge period
   0xDA, 0x12,    // Set com pins hardware configuration
   0xDB,       // --set vcomh
   0x20,       // 0x20,0.77xVcc
   0x8D, 0x14,    // Set DC-DC enable
   0xAF        // Display ON in normal mode

};

uint8_t oledFont, oledX, oledY = 0;

// Program:    5248 bytes

SSD1306Device::SSD1306Device(void){}

void SSD1306Device::begin(void)
{
   i2csw_start();
   for (uint8_t i = 0; i < sizeof (ssd1306_init_sequence); i++) {
      ssd1306_send_command(pgm_read_byte(&ssd1306_init_sequence[i]));
   }
   clear();
}

void SSD1306Device::ssd1306_send_command_start(void) {
   i2csw_start();
   i2csw_byte(SSD1306_SA);
   i2csw_byte(SSD1306_COMMAND);
}

void SSD1306Device::ssd1306_send_command_stop(void) {
   i2csw_stop();
}

void SSD1306Device::ssd1306_send_data_byte(uint8_t byte)
{
   ssd1306_send_data_stop();
   ssd1306_send_data_start();
   i2csw_byte(byte);
}

void SSD1306Device::ssd1306_send_command(uint8_t command)
{
   ssd1306_send_command_start();
   i2csw_byte(command);
   ssd1306_send_command_stop();
}

void SSD1306Device::ssd1306_send_data_start(void)
{
   i2csw_start();
   i2csw_byte(SSD1306_SA);
   i2csw_byte(SSD1306_DATA);
}

void SSD1306Device::ssd1306_send_data_stop(void)
{
   i2csw_stop();
}

void SSD1306Device::setCursor(uint8_t x, uint8_t y)
{
   ssd1306_send_command_start();
   i2csw_byte(0xb0 + y);
   i2csw_byte(((x & 0xf0) >> 4) | 0x10); // | 0x10
   i2csw_byte((x & 0x0f) | 0x01); // | 0x01
   ssd1306_send_command_stop();
   oledX = x;
   oledY = y;
}

void SSD1306Device::clear(void)
{
   fill(0x00);
}

void SSD1306Device::fill(uint8_t fill)
{
   uint8_t m,n;
   for (m = 0; m < 8; m++)
   {
      ssd1306_send_command(0xb0 + m);  // page0 - page1
      ssd1306_send_command(0x00);      // low column start address
      ssd1306_send_command(0x10);      // high column start address
      ssd1306_send_data_start();
      for (n = 0; n < 128; n++)
      {
         ssd1306_send_data_byte(fill);
      }
      ssd1306_send_data_stop();
   }
   setCursor(0, 0);
}

size_t SSD1306Device::write(byte c) {
   uint8_t i = 0;
   uint8_t ci = c - 32;
   if(c == '\r')
      return 1;
   if(c == '\n'){
      oledY++;
//       if ( oledY > 7) // tBUG
//          oledY = 7;
      setCursor(0, oledY);
      return 1;
   }

   if (oledX > 122)
   {
      oledX = 0;
      oledY++;
      if ( oledY > 7) // tBUG
         oledY = 7;
      setCursor(oledX, oledY);
   }

   ssd1306_send_data_start();
   for (i= 0; i < 6; i++)
   {
      ssd1306_send_data_byte(pgm_read_byte(&ssd1306xled_font6x8[ci * 6 + i]));
   }
   ssd1306_send_data_stop();
   setCursor(oledX+6, oledY);
   return 1;
}

SSD1306Device oled;

// ----------------------------------------------------------------------------
