//        >>>>>  T-I-N-Y  M-I-S-S-I-L-E for ATTINY85  GPL v3 <<<<<
//                    Programmer: Daniel C 2020
//             Contact EMAIL: electro_l.i.b@tinyjoypad.com
//                    https://WWW.TINYJOYPAD.COM
//          https://sites.google.com/view/arduino-collection

//  tiny-Missile is free software: you can redistribute it and/or modify
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

#include <Arduino.h>

#define TINYJOYPAD_LEFT  (analogRead(A0)>=750)&&(analogRead(A0)<950)
#define TINYJOYPAD_RIGHT (analogRead(A0)>500)&&(analogRead(A0)<750)
#define TINYJOYPAD_DOWN (analogRead(A3)>=750)&&(analogRead(A3)<950)
#define TINYJOYPAD_UP  (analogRead(A3)>500)&&(analogRead(A3)<750)
#define BUTTON_DOWN (digitalRead(1)==0)
#define BUTTON_UP (digitalRead(1)==1)

int8_t Mymap(int8_t x, int8_t in_min, int8_t in_max, int8_t out_min, int8_t out_max);
uint8_t blitzSprite(int8_t xPos,int8_t yPos,uint8_t xPASS,uint8_t yPASS,uint8_t FRAME,const uint8_t *SPRITES);
uint8_t SPEED_BLITZ(int8_t xPos,int8_t yPos,uint8_t xPASS,uint8_t yPASS,uint8_t FRAME,const uint8_t *SPRITES);
uint8_t SplitSpriteDecalageY(uint8_t decalage,uint8_t Input,uint8_t UPorDOWN);
int8_t RecupeLineY(int8_t Valeur);
uint8_t RecupeDecalageY(uint8_t Valeur);
uint8_t BACKGROUND(uint8_t xPASS,uint8_t yPASS,const uint8_t *BITMAP);
uint8_t Trace_LINE(uint8_t DESACTIVE_,int8_t x1,int8_t y1,int8_t x2,int8_t y2,uint8_t xPASS,uint8_t yPASS);
uint8_t DIRECTION_LINE(uint8_t DESACTIVE_,int8_t x1,int8_t y1,int8_t x2,int8_t y2,uint8_t xPASS,uint8_t yPASS);
uint8_t Return_Full_Byte(int8_t x1,int8_t y1,int8_t x2,int8_t y2,uint8_t xPASS,uint8_t yPASS);
void Universal_Swap(int8_t *A_,int8_t *B_);
uint8_t RECONSTRUCT_BYTE(uint8_t Min_,uint8_t Max_);
void TinySound(uint8_t freq_,uint8_t dur);
void TINYJOYPAD_INIT(void);
