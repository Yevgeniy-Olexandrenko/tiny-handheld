//      >>>>>  T-I-N-Y  T-R-I-C-K for ATTINY85  GPL v3 <<<<
//                      Programmer: Daniel C 2019
//              Contact EMAIL: electro_l.i.b@tinyjoypad.com
//                      https://WWW.TINYJOYPAD.COM
//           https://sites.google.com/view/arduino-collection

//  Tiny trick is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.

//  You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

//for TINY JOYPAD rev2 (attiny85)
//the code work at 16MHZ internal
//and use ssd1306xled Library for SSD1306 oled display 128x64
//Program chip with arduino uno as isp at 16 mhz internal!

#include <avr/pgmspace.h>

typedef struct Sprite{
uint8_t Xpos;
uint8_t Ypos;
uint8_t CumuleX;
uint8_t SpeedXpositif;
uint8_t SpeedXnegatif;
uint8_t CumuleY;
uint8_t SpeedYpositif;
uint8_t SpeedYnegatif;
}Sprite;

const int8_t  PLAYER_Puck [] PROGMEM= {
16,12,11,15,2,16,0,11,-2,1,4,-2,13,-2,14,2,
};

const uint8_t  PLAYER_DIM [] PROGMEM= {
6,6,1,6,6
};

const uint8_t  GOALER_WHITE [] PROGMEM= {
5,2,
0xFC,0x32,0x79,0x49,0xB6,0x00,0x01,0x02,0x02,0x01,
};

const uint8_t  GOALER_BLACK [] PROGMEM= {
5,2,
0x86,0xB7,0xB7,0xCE,0xFC,0x01,0x03,0x03,0x01,0x00,
};

const uint8_t  puck [] PROGMEM= {
3,1,
0x02,0x05,0x02,
};

const uint8_t  NUM [] PROGMEM= {
4,1,
0xF8,0x88,0xF8,0x00,
0x00,0xF8,0x00,0x00,
0xE8,0xA8,0xB8,0x00,
0x88,0xA8,0xF8,0x00,
0x38,0x20,0xF8,0x00,
0xB8,0xA8,0xE8,0x00,
0xF8,0xA8,0xE8,0x00,
0x08,0xE8,0x18,0x00,
0xF8,0xA8,0xF8,0x00,
0xB8,0xA8,0xF8,0x00,
};

const uint8_t  PLAYER_WHITE_B [] PROGMEM= {
17,3,
0x00,0x00,0x00,0x00,0x00,0x00,0xE0,0x90,0x50,0xF0,0x20,0x40,0x80,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0x24,0x2B,0x3F,0x13,0x0C,0x03,0x06,0xF8,0x70,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xC0,0xE0,0xD0,0x90,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x0C,0x12,0x17,0x1F,0x14,0x16,0x97,0xD8,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x80,0x40,0x40,0xC0,0x40,0x40,0xC0,0x40,0x40,0x80,0x00,0x00,0x00,0x00,0x00,0x40,0xC0,
0xC3,0xC6,0x4B,0x2A,0x37,0x17,0x12,0x0B,0x06,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0xE0,0x90,0xD0,0xE0,0xC0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x0C,
0x08,0x10,0x1F,0x18,0x17,0x16,0x14,0x1F,0x17,0x12,0x0C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x1C,0x3E,0xC0,0x80,0x60,0x90,0xF8,0xA8,0x48,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x03,0x04,0x09,0x1F,0x15,0x12,0x0F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0xF8,0x36,0xD3,0xD1,0x50,0xF0,0xD0,0x90,0x60,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x0F,0x12,0x17,0x0E,0x06,0x03,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x80,0xC0,0xA0,0x90,0xD0,0xD8,0xA8,0xA4,0xC6,0x86,0x06,0x04,0x00,0x00,0x00,0x00,
0x00,0x03,0x04,0x05,0x06,0x05,0x05,0x06,0x05,0x04,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x60,0x90,0xD0,0xF0,0x50,0xD0,0xD0,0x30,0xF0,0x10,0x20,0x60,0xC0,0x00,0x00,0x00,0x00,
0x00,0x00,0x01,0x03,0x06,0x0E,0x17,0x12,0x0F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};

const uint8_t  PLAYER_BLACK_B [] PROGMEM= {
17,3,
0x00,0x00,0x00,0x00,0x00,0x00,0xE0,0xF0,0x70,0xF0,0x20,0x40,0x80,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0x3C,0x3B,0x3F,0x13,0x0C,0x03,0x06,0xF8,0x70,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xC0,0xE0,0xE0,0xC0,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x0C,0x1E,0x1F,0x14,0x16,0x97,0xD8,0x3F,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x80,0xC0,0xC0,0xC0,0x40,0x40,0xC0,0xC0,0xC0,0x80,0x00,0x00,0x00,0x00,
0x00,0x40,0xC0,0xC3,0xC7,0x4B,0x2A,0x37,0x17,0x12,0x0B,0x07,0x03,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0xC0,0xE0,0xE0,0xC0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x06,0x0C,0x08,0x10,0x1F,0x18,0x17,0x16,0x14,0x1F,0x1E,0x0C,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x1C,0x3E,0xC0,0x80,0x60,0x90,0xF8,0xB8,0x78,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x03,0x04,0x09,0x1F,0x1D,0x1E,0x0F,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0xF8,0x36,0xD3,0xD1,0x50,0xF0,0xF0,0x60,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x07,0x0E,0x0F,0x06,0x02,0x01,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x80,0xC0,0xA0,0x90,0xD0,0xD8,0xA8,0xA4,0xC6,0x86,0x06,0x04,0x00,
0x00,0x00,0x00,0x00,0x03,0x07,0x07,0x06,0x05,0x05,0x06,0x07,0x07,0x03,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x60,0xF0,0xF0,0x50,0xD0,0xD0,0x30,0xF0,0x10,0x20,0x60,0xC0,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x02,0x06,0x0F,0x0E,0x07,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};

const uint8_t  patinoire1_2 [] PROGMEM= {
0xF0,0x1C,0x86,0xE2,0x33,0xD9,0xE9,0xED,0xE9,0xED,0xE1,0xEF,0xE9,0xED,0xE9,0xED,0xE9,0xED,0xE9,0xED,
0xE1,0xEF,0xE9,0xED,0xE9,0xED,0xE9,0xED,0xE9,0xED,0xE1,0xEF,0xE9,0xED,0xE9,0xED,0xE9,0xED,0x09,0xED,
0x01,0x0F,0xE9,0xED,0xE9,0xED,0xE9,0xED,0xE9,0xED,0xE1,0xEF,0xE9,0xED,0xE9,0xED,0xE9,0xED,0xE9,0xED,
0xE1,0x0F,0xE9,0x4D,0xFF,0x08,0xAA,0xEF,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0x00,0xFF,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0xFF,0x44,0xBF,0x80,0x6A,0x6F,0x00,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0xFF,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x7F,
0x3F,0x1F,0x0F,0x87,0xC7,0xC3,0xE3,0xE3,0xF1,0x00,0xFF,0x44,0xFF,0x00,0x66,0x99,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0xFF,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0x1F,0x03,0x01,0xE0,0xF8,0xFC,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0xFF,0x44,0xFF,0x00,0x66,0x99,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0xFF,0x00,0x00,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xF8,0xC0,0x80,0x07,0x1F,0x3F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0xFF,0x22,
0xFD,0x01,0x56,0xF6,0x00,0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0xFF,
0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFE,0xFC,0xF8,0xF0,0xE1,0xE3,0xC3,0xC7,0xC7,
0x8F,0x00,0xFF,0x22,0xFF,0x10,0x55,0xF7,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0x00,0xFF,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0xFF,0x22,0x0F,0x38,0x61,0x47,0xCC,0x9B,0x97,0xB7,0x97,0xB7,0x87,0xF7,
0x97,0xB7,0x97,0xB7,0x97,0xB7,0x97,0xB7,0x87,0xF7,0x97,0xB7,0x97,0xB7,0x97,0xB7,0x97,0xB7,0x87,0xF7,
0x97,0xB7,0x97,0xB7,0x97,0xB7,0x90,0xB7,0x80,0xF0,0x97,0xB7,0x97,0xB7,0x97,0xB7,0x97,0xB7,0x87,0xF7,
0x97,0xB7,0x97,0xB7,0x97,0xB7,0x97,0xB7,0x87,0xF0,0x97,0xB2,
};

const uint8_t  Intro [] PROGMEM= {
0xFF,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x15,0x15,0x15,0x15,0x15,
0xD5,0xD5,0xD5,0xD5,0xD5,0x55,0x15,0x15,0x15,0x15,0x01,0x81,0xD5,0xD5,0xD5,0xD5,0xD5,0x01,0x01,0x01,
0xD5,0xD5,0xD5,0xD5,0x55,0xD1,0xD1,0xC1,0x81,0x01,0xD5,0xD5,0xD5,0xD5,0x51,0x01,0x05,0x15,0x15,0x55,
0xD5,0xD1,0xD1,0xC1,0xC1,0xD1,0xD1,0xD5,0x55,0x15,0x15,0x05,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1E,0x1E,0x1E,0x1E,0x1E,0x1A,0x00,0x00,0x00,0x00,0x00,0x0A,
0x1E,0x1E,0x1E,0x1E,0x1E,0x00,0x00,0x02,0x1E,0x1E,0x1E,0x1E,0x00,0x00,0x00,0x02,0x0E,0x1E,0x1E,0x1E,
0x1E,0x1E,0x14,0x00,0x00,0x00,0x00,0x00,0x00,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x05,0x05,0x05,0x05,0x05,0xB5,0xB5,0xB5,0xB5,0xB5,0x05,0x05,0x05,0x05,0x01,0x00,
0xB5,0xB5,0xB5,0xB5,0xB5,0xB5,0x25,0xA5,0xB5,0xB5,0xB4,0xB4,0x94,0x00,0x00,0x00,0x00,0xB5,0xB5,0xB5,
0xB5,0x10,0x00,0x00,0x30,0xB4,0xB4,0xB4,0xB4,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x04,0x04,0x00,
0xA0,0xB5,0xB5,0xB5,0xB5,0xB5,0xB0,0xB4,0xB4,0x94,0x95,0x85,0x05,0x01,0x01,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x00,0x00,0xE0,0x10,0x00,0xE7,0xA7,0x07,
0x17,0xE7,0x00,0xD0,0x50,0x70,0x00,0xF0,0x17,0xF7,0x07,0xF7,0x07,0x76,0x50,0xF0,0x01,0x03,0x07,0x07,
0xF7,0x57,0x14,0x00,0xF0,0x07,0x07,0x07,0xF7,0x55,0x10,0x00,0xE0,0x11,0x13,0x03,0x17,0xF7,0x17,0x07,
0xF6,0x56,0xD6,0x66,0x07,0xE3,0x12,0x10,0xE2,0x07,0x07,0xF7,0x07,0x07,0x00,0x11,0xF3,0x13,0x07,0xF7,
0x57,0x57,0xA6,0x04,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0xFF,
0xFE,0xFC,0xB8,0xE1,0xB0,0xF8,0x5C,0x2C,0x3D,0x08,0x08,0x01,0x01,0x01,0x00,0x01,0x01,0x01,0x00,0x01,
0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x00,0x01,0x01,0x01,0x00,0x01,0x01,0x01,0x00,
0x00,0x01,0x01,0x00,0x00,0x01,0x00,0x00,0x01,0x00,0x00,0x01,0x00,0x00,0x01,0x01,0x00,0x00,0x00,0x01,
0x01,0x09,0x08,0x3D,0x2D,0x5D,0xF8,0xB1,0xE1,0xB9,0xFC,0xFE,0xFF,0x0F,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,
0xFF,0x6C,0x7E,0x56,0xFC,0xAA,0xFC,0xB4,0xDC,0xF8,0x54,0xF8,0xA8,0xF0,0xD8,0x70,0xD0,0x70,0xE0,0xB0,
0x70,0xD0,0x70,0xD8,0x7C,0x6C,0x9E,0x0B,0xBF,0xFF,0xFF,0xFD,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x04,0x2A,0x2A,0x10,0x00,0x02,0x3E,0x02,0x00,0x3C,0x0A,0x0A,0x3C,0x00,0x3E,0x0A,0x34,0x00,
0x02,0x3E,0x02,0x00,0x00,0x00,0x1C,0x22,0x2A,0x18,0x00,0x3C,0x0A,0x0A,0x3C,0x00,0x3E,0x04,0x08,0x04,
0x3E,0x00,0x3E,0x2A,0x22,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0xFD,0xFF,0xFF,0xBF,
0x0B,0x9E,0x6C,0x7C,0xD8,0x70,0xD0,0x70,0xB0,0xE0,0x70,0xD0,0x70,0xD8,0xF0,0xA8,0xF8,0x54,0xF8,0xDC,
0xB4,0xFC,0xAA,0xFC,0x56,0x7E,0x6C,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x81,0x01,0x81,
0x02,0x81,0x82,0x83,0x81,0x01,0x03,0x01,0x81,0x01,0x01,0x00,0x01,0x00,0x00,0x03,0x00,0x07,0x0F,0x3F,
0x7F,0xFF,0xFE,0xFC,0xF8,0xE8,0xF0,0xE0,0xC0,0xC0,0xC0,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0xC0,0xC0,0xC0,0xE0,0xF0,0xE8,0xF8,
0xFC,0xFE,0xFF,0x7F,0x3F,0x0F,0x07,0x00,0x03,0x00,0x00,0x01,0x00,0x01,0x01,0x81,0x01,0x03,0x01,0x81,
0x83,0x82,0x81,0x02,0x81,0x01,0x81,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x80,0x80,0x80,
0x80,0x80,0x80,0x82,0x80,0x8A,0x80,0xAA,0x8A,0xAA,0xAA,0xA0,0x80,0x9E,0x9D,0xBD,0xBA,0xF8,0xF5,0xF9,
0xF2,0xEA,0xE8,0xF4,0x88,0x80,0x80,0x80,0x80,0x80,0x80,0x81,0x83,0x83,0x87,0x8F,0x8F,0x9F,0x9F,0xBF,
0xBF,0xFF,0xFF,0xFE,0xFC,0x9C,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x9C,0xFC,0xFE,0xFF,0xFF,0xBF,
0xBF,0x9F,0x9F,0x8F,0x8F,0x87,0x83,0x83,0x81,0x80,0x80,0x80,0x80,0x80,0x80,0x88,0xF4,0xE8,0xEA,0xF2,
0xF9,0xF5,0xF8,0xBA,0xBD,0x9D,0x9E,0x80,0xA0,0xAA,0xAA,0x8A,0xAA,0x80,0x8A,0x80,0x82,0x80,0x80,0x80,
0x80,0x80,0x80,0xFF,
};

const uint8_t  cadre40_37 [] PROGMEM= {
47,2,
0xFE,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0xFE,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x00,};

const uint8_t  screen [] PROGMEM= {
48,3,
0x00,0x00,0x50,0xF8,0xFC,0x38,0x1C,0xD8,0x5C,0x58,0x9C,0x18,0xDC,0x18,0x1C,0x18,0x9C,0x58,0xDC,0x18,
0x1C,0x18,0x3C,0xF8,0xF8,0x3C,0x18,0x9C,0x58,0x5C,0x58,0x1C,0xD8,0x5C,0x58,0x9C,0x18,0xDC,0x18,0x1C,
0xD8,0x1C,0x38,0xFC,0xF8,0x50,0x00,0x00,0x00,0x00,0x55,0xFF,0xFF,0x30,0x20,0x0F,0x02,0x22,0x21,0x00,
0x0F,0x28,0x28,0x00,0x00,0x28,0x2F,0x08,0x00,0x20,0x30,0xFF,0xFF,0x30,0x20,0x07,0x08,0x28,0x28,0x00,
0x0F,0x22,0x22,0x01,0x00,0x27,0x28,0x08,0x07,0x20,0x30,0xFF,0xFF,0x55,0x00,0x00,0x40,0xA0,0x55,0xFF,
0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,
0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,
0xFF,0x55,0xA0,0x40,

0xF8,0xFE,0xAE,0x07,0x03,0xC7,0xE3,0x27,0xA3,0xA7,0x63,0xE7,0x23,0xE7,0xE3,0xE7,0x63,0xA7,0x23,0xE7,
0xE3,0xE7,0xC3,0x07,0x07,0xC3,0xE7,0x63,0xA7,0xA3,0xA7,0xE3,0x27,0xA3,0xA7,0x63,0xE7,0x23,0xE7,0xE3,
0x27,0xE3,0xC7,0x03,0x07,0xAE,0xFE,0xF8,0xFF,0xFF,0xAA,0x00,0x00,0xCF,0xDF,0xF0,0xFD,0xDD,0xDE,0xFF,
0xF0,0xD7,0xD7,0xFF,0xFF,0xD7,0xD0,0xF7,0xFF,0xDF,0xCF,0x00,0x00,0xCF,0xDF,0xF8,0xF7,0xD7,0xD7,0xFF,
0xF0,0xDD,0xDD,0xFE,0xFF,0xD8,0xD7,0xF7,0xF8,0xDF,0xCF,0x00,0x00,0xAA,0xFF,0xFF,0xBF,0x5F,0xAA,0x00,
0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,
0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,
0x00,0xAA,0x5F,0xBF,
};

const uint8_t  lost [] PROGMEM= {
32,2,
0x00,0xFE,0x02,0x3A,0xE2,0x3A,0x02,0xFA,0x8A,0x7A,0x02,0xFA,0x82,0x7A,0x02,0x02,0xFA,0x82,0x02,0x72,
0x8A,0x72,0x02,0x9A,0xEA,0x02,0x0A,0xFA,0x0A,0x02,0xFE,0x00,0x00,0x03,0x02,0x02,0x02,0x02,0x02,0x02,
0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,
0x02,0x02,0x03,0x00,

0xFF,0x01,0xFD,0xC5,0x1D,0xC5,0xFD,0x05,0x75,0x85,0xFD,0x05,0x7D,0x85,0xFD,0xFD,0x05,0x7D,0xFD,0x8D,
0x75,0x8D,0xFD,0x65,0x15,0xFD,0xF5,0x05,0xF5,0xFD,0x01,0xFF,0x07,0x04,0x05,0x05,0x05,0x05,0x05,0x05,
0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,
0x05,0x05,0x04,0x07,
};

const uint8_t  win [] PROGMEM= {
32,2,
0x00,0xFE,0x02,0x3A,0xE2,0x3A,0x02,0xFA,0x8A,0x7A,0x02,0xFA,0x82,0x7A,0x02,0x02,0xFA,0x82,0xF2,0x82,
0x7A,0x02,0xFA,0x02,0xFA,0x12,0x22,0x42,0xFA,0x02,0xFE,0x00,0x00,0x03,0x02,0x02,0x02,0x02,0x02,0x02,
0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,
0x02,0x02,0x03,0x00,

0xFF,0x01,0xFD,0xC5,0x1D,0xC5,0xFD,0x05,0x75,0x85,0xFD,0x05,0x7D,0x85,0xFD,0xFD,0x05,0x7D,0x0D,0x7D,
0x85,0xFD,0x05,0xFD,0x05,0xED,0xDD,0xBD,0x05,0xFD,0x01,0xFF,0x07,0x04,0x05,0x05,0x05,0x05,0x05,0x05,
0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,
0x05,0x05,0x04,0x07,
};


