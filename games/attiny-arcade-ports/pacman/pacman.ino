/*  2015 / 2016 / 2017
 *  Pacman for Attiny Arcade by Andy Jackson - Twitter @andyhighnumber
 *  
 *  Inspired by http://webboggles.com/ and includes some code from the #AttinyArcade games on that site
 *  The code that does not fall under the licenses of sources listed below can be used non commercially with attribution.
 *  This software is supplied without warranty of any kind.
 *  
 *  **** BEFORE USE, BURN THE BOOTLOADER ON THE ATTINY85 WITH 16Mhz Internal Clock *  
 *  
 *  Controls:
 *  On the standard AttinyArcade:
 *  
 *  Tap left button to turn 90 degrees left (each tap cycles through directions, so if you are going left and 
 *  you tap once, you will go down, tap twice and you'll be going right)
 *  
 *  Tap right button to turn 90 degrees right (each tap cycles through directions, so if you are going left and 
 *  you tap once, you will go up, tap twice and you'll be going right)
 *  
 *  The arrow indicator on the far left of the screen indicates the currently-selected direction, which pac-man will take as soon as there's a gap big enough for him!
 *  
 *  Also, from standby....
 *  Press and hold left button to turn sound on and off
 *  Press and hold left button with the right button held to reset high score
 * 
 *  Everything you need to build and run this game is contained in this file and the font header (font6x8AJ3.h) - no additional libraries required
 *  
 *  Instructions for programming the Attiny85 can be found here: https://create.arduino.cc/projecthub/arjun/programming-attiny85-with-arduino-uno-afb829

 *  This sketch is using the screen control and font functions written by Neven Boyanov for the http://tinusaur.wordpress.com/ project
 *  Source code and font files available at: https://bitbucket.org/tinusaur/ssd1306xled
 *  **Note that this highly size-optimised version requires modified library functions (which are in this source code file) 
 *  and a modified font header
 * 
 *  As well as the above sources of inspiration, the pac-man screen artwork was insipred by elements of another clone found here https://sites.google.com/view/arduino-collection 
 * 
 *  Sleep code is based on this blog post by Matthew Little:
 *  http://www.re-innovation.co.uk/web12/index.php/en/blog-75/306-sleep-modes-on-attiny85
*/
#include <EEPROM.h>
#include "font6x8AJ3.h"
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <avr/interrupt.h> // needed for the additional interrupt

#define DIR_RIGHT 0
#define DIR_DOWN 1
#define DIR_LEFT 2
#define DIR_UP 3

#define PACMAN 1
#define GHOST1 2
#define GHOST2 4
#define GHOST3 8

// Probability of a ghost taking a branch to the side if it can
#define BRANCH_PROBABILITY 50     
// How many ghosts on level 1
#define INITIAL_GHOSTS 2          
// Initial number of cycles before a ghost is released - reduces according to next two defines down to a min level 
#define INITIAL_GHOST_DELAY 150  
#define GHOST_DELAY_REDUCTION 25
#define GHOST_DELAY_MINIMUM 50

// How long does powerUp mode last?
#define POWER_LENGTH 200

#define DIGITAL_WRITE_HIGH(PORT) PORTB |= (1 << PORT)
#define DIGITAL_WRITE_LOW(PORT) PORTB &= ~(1 << PORT)

// Routines to set and clear bits (used in the sleep code)
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))

// Defines for OLED output
#define SSD1306XLED_H
#define SSD1306_SCL   PORTB2  // SCL, Pin 4 on SSD1306 Board - for webbogles board
#define SSD1306_SDA   PORTB0  // SDA, Pin 3 on SSD1306 Board - for webbogles board
#define SSD1306_SA    0x78  // Slave address

// Defines for SOUND output
#define SOUND_PIN PORTB4

// Input for vertical screen orientation
bool IsLeft()   { return (analogRead(A0) > 750) && (analogRead(A0) < 950); }
bool IsRight()  { return (analogRead(A0) > 500) && (analogRead(A0) < 750); }
bool IsDown()   { return (analogRead(A3) > 750) && (analogRead(A3) < 950); }
bool IsUp()     { return (analogRead(A3) > 500) && (analogRead(A3) < 750); }
bool IsAction() { return bitRead(PINB, PB1) == LOW; }
bool IsCenter() { return bitRead(PINB, PB3) == LOW; }

/* ------ This section needs to be updated if you change the screen bitmap ------
 * ------------------------------------------------------------------------------*/

// Where on the screen to the ghosts appear when they pop out onto the playing screen?
#define GHOST_LAUNCH_X 64
#define GHOST_LAUNCH_Y 16

// How many pills are there on the screen?
#define NO_PILLS 56

// Where are the ghosts and pacman drawn initially?
// In each of these, the order is - Ghost1, Ghost2, Ghost3, PAC-MAN
static const byte startPosX[] = {66,55,77,55};
static const byte startPosY[] = {28,28,28,40};
static const byte startDirections[] = {DIR_UP,DIR_UP,DIR_UP,DIR_RIGHT};

// Where are the big pills? (their row is fixed as the last one)
static const byte bigPillLocations[2] PROGMEM = {14,119};

// How many pills on each row? for the six rows 1-7 that you are allowed pills
static const byte readRow[6][2] PROGMEM = {
  0,12,
  12,23,
  23,31,
  31,34,
  34,45,
  45,56
  };

// Where are the pills (x,y)
static const byte pillLocations[NO_PILLS][2] PROGMEM = {
14,8,
24,8,
36,8,
46,8,
55,8,
63,8,
75,8,
84,8,
93,8,
104,8,
111,8,
119,8,

14,20,
24,20,
36,20,
46,20,
55,20,
63,20,
75,20,
84,20,
93,20,
104,17,
119,17,

14,31,
24,31,
36,31,
46,31,
93,31,
104,25,
111,25,
119,25,

104,37,
111,37,
119,37,

14,43,
24,43,
36,43,
46,43,
55,43,
63,43,
75,43,
84,43,
93,43,
104,46,
119,46,

14,55,
24,55,
36,55,
46,55,
55,55,
63,55,
75,55,
84,55,
93,55,
104,55,
111,55
};

// Drawing functions - adapted from those at https://bitbucket.org/tinusaur/ssd1306xled
void ssd1306_init(void);
void ssd1306_xfer_start(void);
void ssd1306_xfer_stop(void);
void ssd1306_send_byte(uint8_t byte);
void ssd1306_send_command(uint8_t command);
void ssd1306_send_data_start(void);
void ssd1306_send_data_stop(void);
void ssd1306_setpos(uint8_t x, uint8_t y);
void ssd1306_fillscreen(uint8_t fill_Data);
void ssd1306_char_f6x8(uint8_t x, uint8_t y, const char ch[]);
void ssd1306_draw_bmp(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t bitmap[]);

// Other generic functions for games (both originated in code from webboggles.com and the sleep code is by Matthew Little - see above)
void beep(int,int);
void system_sleep(void);
void doNumber (int,int,int);

// Game functions
void playPacman(void);
void levelUp(int);
void showScore(void);
void displayTitle(void);
int checkCollision(byte xpos, byte ypos);
void initScreen(void);
void initLevel(void);
void newDirection(byte ghostNo);
byte hitGhosts(void);
void movePacman(void);
void pacDie(void);
void eatenGhost(void);
void moveGhosts(void);
byte drawPacman(byte c, byte r);


// Global variables - yes I know all these global vars is a lazy way to code but it makes it easier to prevent stack overflows when you're working with 512 bytes! 
// Most of these are initialised in the main game function (playPacman())
boolean stopAnimate;      // this is set to 1 when a collision is detected
int lives;                // Lives in the game - this can go negative to end the game, which is why it's a signed variable  
boolean clickLock;        // For debounce routine
int score;                // Obvious I hope
int topScore;             // High score
boolean newHigh;          // Is there a new high score?
boolean mute = 0;         // Mute the speaker
int mouth = 0;            // Is his mouth open?
int directions[4];        // Directions of the four possible characters (pacman = 0, ghosts are 1-3)
int commandDir;           // The desired direction set by input
byte pillsEaten = 0;      // How many pills have been eaten (there are 63 altogether)
int ghostCounter = 0;     // How many ghosts 
int ghostsActive[3];      // How many ghosts are out?
int count = 0;            // Counter
int oldDir;               // Holds previous direction
int level = 0;            // Obvious I hope
int releaseDelay;         // The delay between ghosts being let out
int maxGhosts;            // How many ghosts maximum on this level?
bool powerUp;             // Is powerUp move active?
int powerCounter;         // Counts the length of powerUp mode
byte locations[4][2];     // The x,y location of the sprites
byte pillsActive[NO_PILLS];    // Which of the pills is still on the screen?
byte bigPillsActive[2];   // Which of the two big pills is still active?

// Artwork 
static const byte pacOpen[4][8] PROGMEM = {
0x3C,0x7E,0xFF,0xFF, 0xE7,0xE7,0xC3,0x42, 
0x7C,0xFE,0x3F,0x0F,0x0F,0x3F,0xFE,0x7C,
0x42,0xC3,0xE7,0xE7,0xFF,0xFF,0x7E,0x3C,
0x3E,0x7F,0xFC,0xF0,0xF0,0xFC,0x7F,0x3E};

static const byte arrows[4][7] PROGMEM = {
0x10,0x10,0x10,0x7C,0x38,0x10,0x00,
0x00,0x20,0x60,0xFF,0x60,0x20,0x00,
0x10,0x38,0x7C,0x10,0x10,0x10,0x00,
0x00,0x04,0x06,0xFF,0x06,0x04,0x00
};

static const byte pacClosed[] PROGMEM = {0x3C,0x7E,0xFF,0xFF,0xFF,0xFF,0x7E,0x3C};

static const byte ghost[2][8] PROGMEM = {0x7C,0xFE,0x22,0xEB,0xFF,0x62,0xEA,0x7C,0x7C,0xC2,0x0A,0x83,0x83,0x0A,0x82,0x7C};

static const byte PAClives[] PROGMEM = {0x1C,0x3C,0x7E,0x66,0x66,0x62,0x40};

static const byte openBmp[] PROGMEM = {
0x00,0xC0,0xE0,0xF0,0xF8,0xFC,0xFC,0xFE,0xFE,0xFE,0xFF,0xFF,0xFF,0xFF,0xFE,0xFE,
0xFE,0xFC,0xFC,0xF8,0x70,0x60,0x00,
0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xEF,0xEF,0xC7,0x87,0x83,
0x83,0x01,0x00,0x00,0x10,0x38,0x7C,0x38,0x10,0x00,0x00,0x00,0x00,0x10,0x38,0x7C,
0x38,0x10,
0x00,0x07,0x0F,0x1F,0x3F,0x7F,0x7F,0x7F,0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x7F,
0x7F,0x3F,0x3F,0x3E,0x1E,0x1E
};


static const byte gameScreen[] PROGMEM = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF8,0x04,0xF4,0x14,0x14,0x14,0x14,0x14,0x14,
0x14,0x14,0x13,0x10,0x0F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0x10,0x13,
0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0xE4,0x04,0xE4,0x14,0x14,0x14,0x14,0x14,
0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,
0x14,0x14,0x14,0x14,0xE4,0x04,0xE4,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,
0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,
0x14,0xE4,0x04,0xE4,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,
0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0xE4,0x04,0xF8,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x40,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0x40,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x40,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0x40,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x7F,0x80,0x7F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,
0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x7F,0x80,0x7F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0x20,0xA0,0xA0,
0xA0,0xA0,0x20,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0xFF,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0x20,0x2F,0x2F,
0x2F,0x2F,0x20,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0xFF,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0xFE,0x01,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFE,0x01,0xFE,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x08,0x07,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0xFE,0x01,0xFD,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,
0x05,0x02,0x00,0x02,0x02,0x02,0x02,0x02,0x02,0x00,0x02,0x05,0x05,0x05,0x05,0x05,
0x05,0x05,0x05,0x05,0x05,0x05,0xFD,0x01,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0xFE,0x01,0x3E,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,
0x40,0x40,0x40,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0xFF,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x7F,0x80,0x7F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0x80,0x7F,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0x20,0xC0,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x7F,0x80,0x9F,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,
0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,
0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0x9F,0x80,0x7F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x7F,0x80,0x7E,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0xFF,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF8,0x04,0xF4,0xF4,
0xF4,0xF4,0x04,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0xFF,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x02,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x02,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x02,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x02,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0xFE,0x01,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,
0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0xFE,0x01,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x04,0x05,0x05,
0x05,0x05,0x04,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0xFF,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0x20,0x27,0x28,0x28,0x28,0x28,0x28,0x28,
0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28,
0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x27,0x20,0x27,0x28,0x28,0x28,0x28,0x28,
0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28,
0x28,0x28,0x28,0x28,0x27,0x20,0x27,0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28,
0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28,
0x28,0xC7,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0x08,0xC8,0x28,
0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x27,0x20,0x1F,0x00
};

void ReadInput()
{
    if(IsRight()) {
      commandDir = DIR_RIGHT;
    }
    else if(IsDown()) {
      commandDir = DIR_DOWN;
    }
    else if(IsLeft()) {
      commandDir = DIR_LEFT;
    }
    else if(IsUp()) {
      commandDir = DIR_UP;
    }
  
    else if (!clickLock) {
      if(IsCenter()) {
        if (commandDir > 0) commandDir--; else commandDir = 3;
      }
      else if(IsAction()) {
        if (commandDir < 3) commandDir++; else commandDir = 0;
      }
      clickLock = true;
    }
}

void displayTitle(void) {
  int incr = 0;
  for(int lxn = 3; lxn < 6; lxn++) {
    ssd1306_setpos(84,lxn); 
    ssd1306_send_data_start();
    for(int lxn2 = 0; lxn2 < 34; lxn2++) {
      ssd1306_send_byte(pgm_read_byte(&openBmp[incr]));      
      incr++;
      if ( (lxn == 3 || lxn == 5) && lxn2 > 21) lxn2 = 35;
    }
    ssd1306_send_data_stop();                    
  }
}

// Interrupt handlers
ISR(PCINT0_vect){ // PB1 Action button interrupt           
}

// Arduino stuff - setup
void setup() {
  sbi(DDRB, SOUND_PIN); // output
  cbi(DDRB, PB1);       // input
  cbi(DDRB, A0);        // input
  cbi(DDRB, A3);        // input
  sbi(PCMSK, PCINT1);   // pin change mask: listen to portb b1 Action button
  GIMSK |= 0b00100000;  // enable PCINT interrupt }
}

// Arduino stuff - loop
void loop() { 
  ssd1306_init();
  ssd1306_fillscreen(0x00);

  ssd1306_char_f6x8(4, 2, "P A C-M A N");
  ssd1306_char_f6x8(0, 4, "andy jackson"); 
  
  ssd1306_setpos(0,1); 
  for (int incr = 0; incr < 76; incr++) {
    ssd1306_send_data_start();
    ssd1306_send_byte(B00111000);
    ssd1306_send_data_stop();                    
  }
  ssd1306_setpos(0,3); 
  for (int incr = 0; incr < 76; incr++) {
    ssd1306_send_data_start();
    ssd1306_send_byte(B00011100);
    ssd1306_send_data_stop();                    
  }

  displayTitle();

  ssd1306_char_f6x8(0, 6, "inspired by");
  ssd1306_char_f6x8(0, 7, "webboggles.com");
  delay(1200);

  long startT = millis();
  long nowT =0;
  boolean sChange = 0;

  while(IsAction()) {
    nowT = millis();
    if (nowT - startT > 2000) {
      sChange = 1;     
      if (IsLeft() || IsRight()) {
        EEPROM.write(0,0);
        EEPROM.write(1,0);
        ssd1306_char_f6x8(8, 0, "-HIGH SCORE RESET-");  
      } else if (mute == 0) { mute = 1; ssd1306_char_f6x8(32, 0, "-- MUTE --"); } else { mute = 0; ssd1306_char_f6x8(31, 0, "- SOUND ON -");  }    
      break;
    }
    if (sChange == 1) break;
  }  
  while(IsAction());

  if (sChange == 0) {
    delay(1000);

    ssd1306_init();
    ssd1306_fillscreen(0x00);

    playPacman(); 

    topScore = EEPROM.read(0);
    topScore = topScore << 8;
    topScore = topScore |  EEPROM.read(1);

    newHigh = 0;
    if (score > topScore) { 
      topScore = score;
      EEPROM.write(1,score & 0xFF); 
      EEPROM.write(0,(score>>8) & 0xFF); 
      newHigh = 1;
      }

    ssd1306_fillscreen(0x00);
    ssd1306_char_f6x8(11, 1, "----------------");
    ssd1306_char_f6x8(11, 2, "G A M E  O V E R");
    ssd1306_char_f6x8(11, 3, "----------------");
    showScore();
    if (!newHigh) {
      ssd1306_char_f6x8(21, 7, "HIGH SCORE:");
      doNumber(88, 7, topScore);
    }
    delay(1500);
    if (newHigh) {
      ssd1306_fillscreen(0x00);
      ssd1306_char_f6x8(10, 1, "----------------");
      ssd1306_char_f6x8(10, 3, " NEW HIGH SCORE ");
      ssd1306_char_f6x8(10, 7, "----------------");
      doNumber(50,5,topScore);
      for (int i = 700; i>200; i = i - 50){
        beep(30,i);
      }
      delay(1500);    
    } 
  }
  system_sleep();
}

void showScore(void) {
  ssd1306_char_f6x8(37, 5, "SCORE:");
  doNumber(75, 5, score);
}

// Handle what happens at the end of a level
void levelUp() {  
  initScreen();
  initLevel();
  level++;
  if ((level == 3)||(level==5)) lives++;
  if (maxGhosts < 3) maxGhosts++;
  if (releaseDelay > GHOST_DELAY_MINIMUM) releaseDelay -= GHOST_DELAY_REDUCTION;
  ssd1306_fillscreen(0x00);

  ssd1306_char_f6x8(35, 2, "-----------");
  ssd1306_char_f6x8(37, 3, "LEVEL:");
  doNumber(75,3,level);
  showScore();
  ssd1306_char_f6x8(35, 6, "-----------");

  delay(2500);    
  ssd1306_fillscreen(0x00);
  displayScreen();
}


void doNumber (int x, int y, int value) {
    char temp[10] = {0,0,0,0,0,0,0,0,0,0};
    itoa(value,temp,10);
    ssd1306_char_f6x8(x, y, temp);
}

void ssd1306_init(void){
  DDRB |= (1 << SSD1306_SDA); // Set port as output
  DDRB |= (1 << SSD1306_SCL); // Set port as output

  ssd1306_send_command(0xAE); // display off
  ssd1306_send_command(0x00); // Set Memory Addressing Mode
  ssd1306_send_command(0x10); // 00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
  ssd1306_send_command(0x40); // Set Page Start Address for Page Addressing Mode,0-7
  ssd1306_send_command(0x81); // Set COM Output Scan Direction
  ssd1306_send_command(0xCF); // ---set low rowumn address
  ssd1306_send_command(0xA1); // ---set high rowumn address
  ssd1306_send_command(0xC8); // --set start line address
  ssd1306_send_command(0xA6); // --set contrast control register
  ssd1306_send_command(0xA8);
  ssd1306_send_command(0x3F); // --set segment re-map 0 to 127
  ssd1306_send_command(0xD3); // --set normal display
  ssd1306_send_command(0x00); // --set multiplex ratio(1 to 64)
  ssd1306_send_command(0xD5); // 
  ssd1306_send_command(0x80); // 0xa4,Output follows RAM content;0xa5,Output ignores RAM content
  ssd1306_send_command(0xD9); // -set display offset
  ssd1306_send_command(0xF1); // -not offset
  ssd1306_send_command(0xDA); // --set display clock divide ratio/oscillator frequency
  ssd1306_send_command(0x12); // --set divide ratio
  ssd1306_send_command(0xDB); // --set pre-charge period
  ssd1306_send_command(0x40); // 
  ssd1306_send_command(0x20); // --set com pins hardware configuration
  ssd1306_send_command(0x02);
  ssd1306_send_command(0x8D); // --set vcomh
  ssd1306_send_command(0x14); // 0x20,0.77xVcc
  ssd1306_send_command(0xA4); // --set DC-DC enable
  ssd1306_send_command(0xA6); // 
  ssd1306_send_command(0xAF); // --turn on oled panel 
}

void ssd1306_xfer_start(void){
  DIGITAL_WRITE_HIGH(SSD1306_SCL);  // Set to HIGH
  DIGITAL_WRITE_HIGH(SSD1306_SDA);  // Set to HIGH
  DIGITAL_WRITE_LOW(SSD1306_SDA);   // Set to LOW
  DIGITAL_WRITE_LOW(SSD1306_SCL);   // Set to LOW
}

void ssd1306_xfer_stop(void){
  DIGITAL_WRITE_LOW(SSD1306_SCL);   // Set to LOW
  DIGITAL_WRITE_LOW(SSD1306_SDA);   // Set to LOW
  DIGITAL_WRITE_HIGH(SSD1306_SCL);  // Set to HIGH
  DIGITAL_WRITE_HIGH(SSD1306_SDA);  // Set to HIGH
}

void ssd1306_send_byte(uint8_t byte){
  uint8_t i;
  for(i=0; i<8; i++)
  {
    if((byte << i) & 0x80)
      DIGITAL_WRITE_HIGH(SSD1306_SDA);
    else
      DIGITAL_WRITE_LOW(SSD1306_SDA);
    
    DIGITAL_WRITE_HIGH(SSD1306_SCL);
    DIGITAL_WRITE_LOW(SSD1306_SCL);
  }
  DIGITAL_WRITE_HIGH(SSD1306_SDA);
  DIGITAL_WRITE_HIGH(SSD1306_SCL);
  DIGITAL_WRITE_LOW(SSD1306_SCL);
}

void ssd1306_send_command(uint8_t command){
  ssd1306_xfer_start();
  ssd1306_send_byte(SSD1306_SA);  // Slave address, SA0=0
  ssd1306_send_byte(0x00);  // write command
  ssd1306_send_byte(command);
  ssd1306_xfer_stop();
}

void ssd1306_send_data_start(void){
  ssd1306_xfer_start();
  ssd1306_send_byte(SSD1306_SA);
  ssd1306_send_byte(0x40);  //write data
}

void ssd1306_send_data_stop(void){
  ssd1306_xfer_stop();
}

void ssd1306_setpos(uint8_t x, uint8_t y)
{
  if (y>7) return;
  ssd1306_xfer_start();
  ssd1306_send_byte(SSD1306_SA);  //Slave address,SA0=0
  ssd1306_send_byte(0x00);  //write command

  ssd1306_send_byte(0xb0+y);
  ssd1306_send_byte(((x&0xf0)>>4)|0x10); // |0x10
  ssd1306_send_byte((x&0x0f)|0x01); // |0x01

  ssd1306_xfer_stop();
}

void ssd1306_fillscreen(uint8_t fill_Data){
  uint8_t m,n;
  for(m=0;m<8;m++)
  {
    ssd1306_send_command(0xb0+m); //page0-page1
    ssd1306_send_command(0x00);   //low rowumn start address
    ssd1306_send_command(0x10);   //high rowumn start address
    ssd1306_send_data_start();
    for(n=0;n<128;n++)
    {
      ssd1306_send_byte(fill_Data);
    }
    ssd1306_send_data_stop();
  }
}

void ssd1306_char_f6x8(uint8_t x, uint8_t y, const char ch[]){
  uint8_t c,i,j=0;
  while(ch[j] != '\0')
  {
    c = ch[j] - 32;
    if (c >0) c = c - 12;
    if (c >15) c = c - 6;
    if (c>40) c=c-9;
    if(x>126)
    {
      x=0;
      y++;
    }
    ssd1306_setpos(x,y);
    ssd1306_send_data_start();
    for(i=0;i<6;i++)
    {
      ssd1306_send_byte(pgm_read_byte(&ssd1306xled_font6x8[c*6+i]));
    }
    ssd1306_send_data_stop();
    x += 6;
    j++;
  }
}

void system_sleep(void) {
  ssd1306_fillscreen(0x00);
  ssd1306_send_command(0xAE);
  cbi(ADCSRA,ADEN);                    // switch analog to digital converter off
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
  sleep_enable();
  sleep_mode();                        // system actually sleeps here
  sleep_disable();                     // system continues execution here when watchdog timed out 
  sbi(ADCSRA,ADEN);                    // switch analog to digital converter on
  ssd1306_send_command(0xAF);
}

void beep(int bCount,int bDelay){
  if (mute) return;
  for (int i = 0; i<=bCount; i++){
    digitalWrite(SOUND_PIN,HIGH);
    for(int i2=0; i2<bDelay; i2++){
      __asm__("nop\n\t"); // 62.5ns delay @ 16MHz
      }
    digitalWrite(SOUND_PIN,LOW);
    for(int i2=0; i2<bDelay; i2++) {
      __asm__("nop\n\t"); // 62.5ns delay @ 16Mhz
    }
  }
}

byte drawPacman(byte c, byte r) {
  byte out = 0;
  if ( (r >= locations[3][0]) && (r < locations[3][0] + 8) ) { // draw the pacMan if any of his body is on this column
    if (c == locations[3][1]/8) {
      if (mouth == 1) {
        out = (pgm_read_byte(&pacOpen[directions[3]][r-locations[3][0]]) << locations[3][1] % 8);            
      } else {
        out = (pgm_read_byte(&pacClosed[r-locations[3][0]]) << locations[3][1] % 8);
      }
    } else if (c == locations[3][1]/8+1) {
      if (mouth == 1) {
        out = (pgm_read_byte(&pacOpen[directions[3]][r-locations[3][0]]) >> (8- locations[3][1] % 8) );
      } else {
        out = (pgm_read_byte(&pacClosed[r-locations[3][0]]) >> (8- locations[3][1] % 8) );
      }
    } 
  }       
  return out;
}

void displayScreen(void) {
  int incr = 0;
  int pacDir = directions[3];
  byte out;
  
  for(byte c = 0; c < 8; c++) {
    ssd1306_setpos(0,c); 
    ssd1306_send_data_start();
    for (byte r = 0; r < 128; r++) {
      out = (pgm_read_byte(&gameScreen[incr]));

      out |= drawPacman(c,r);

      // Draw the bits in the far left column - lives and the little direction arrow
      if (r < 7) {
        if (c == 1) out |= (pgm_read_byte(&arrows[commandDir][r]));
        for (byte lxn = 3; lxn < 7; lxn++) {         
          if (c == lxn && lives > 6-lxn) out |= (pgm_read_byte(&PAClives[r]));
        }
      } 
      
      for (byte lxn = 0; lxn < 3; lxn++) {
        if ( (r >= locations[lxn][0]) && (r < locations[lxn][0] + 8) ) { // draw the ghost if any of his body is on this column
          if (c == locations[lxn][1]/8) {
            out |= (pgm_read_byte(&ghost[powerUp][r-locations[lxn][0]]) << locations[lxn][1] % 8);            
          } else if (c == locations[lxn][1]/8+1) {
            out |= (pgm_read_byte(&ghost[powerUp][r-locations[lxn][0]]) >> (8- locations[lxn][1] % 8) );
          } 
        }     
      }

      if (c>0 && c < 7) {  
        for (byte lxn = pgm_read_byte(&readRow[c-1][0]); lxn < pgm_read_byte(&readRow[c-1][1]); lxn++) {        
          if (pillsActive[lxn]) { // this pill is active
            if (r == pgm_read_byte(&pillLocations[lxn][0])) { // draw the pills if they appear here
              // check if pac-man has eaten a pill
              if (pgm_read_byte(&pillLocations[lxn][0]) > locations[3][0] && pgm_read_byte(&pillLocations[lxn][0]) < locations[3][0] + 8 && pgm_read_byte(&pillLocations[lxn][1]) > locations[3][1] && pgm_read_byte(&pillLocations[lxn][1]) < locations[3][1] + 8) {pillsActive[lxn] = 0; pillsEaten++;score+=level; break;}
              if (c == pgm_read_byte(&pillLocations[lxn][1])/8) {
                out |= ((B00000001) << pgm_read_byte(&pillLocations[lxn][1]) % 8);            
              } else if (c == pgm_read_byte(&pillLocations[lxn][1])/8 +1) {
                out |= ((B00000001) >> (8- pgm_read_byte(&pillLocations[lxn][1]) % 8) );
              } 
            }     
          }
        }
      }
      for (byte lxn = 0; lxn < 2; lxn++) {
        if (bigPillsActive[lxn] == 1 && c == 6 && (r==pgm_read_byte(&bigPillLocations[lxn]) || r==(pgm_read_byte(&bigPillLocations[lxn])+1) )) {
          out |= B11100000;
        }        
      }
      
      if (locations[3][1] == 51) {
        if ((locations[3][0] < 14 && bigPillsActive[0] == 1) || (locations[3][0] > 111 && bigPillsActive[1] == 1)) {
          if (locations[3][0] < 14) bigPillsActive[0] = 0; else bigPillsActive[1] = 0;
          powerUp = 1; 
          powerCounter = 0;
        }
      } 
      
      ssd1306_send_byte(out);
      incr++;
      } // end for 128     

    ssd1306_send_data_stop();                    
  }
}

int checkCollision(byte xpos, byte ypos) {
  int incr=0;
  int returnValue = 0;
  
  for(byte c = 0; c < 2; c++) { // for each of the two lines that the item might be on
    incr = (128 * (ypos/8+c)) + xpos;
    for (byte r = 0; r < 8; r++) { // for each of the eight bytes in the sprite
      if (c == 0) {
        if ((pgm_read_byte(&gameScreen[incr]) & (0xFF << ypos % 8) ) != 0) returnValue = 1;
      } else {
        if ((pgm_read_byte(&gameScreen[incr]) & (0xFF >> (8- ypos % 8) ) ) != 0) returnValue = 1;
      }  
      incr++;
      }    
  }
  return returnValue;
}

void moveGhosts(void) {
  int travelDirection;  // which way's the ghost going?
  byte travelParam;     // zero for travelling in x, one for y
  int searchDirection;  // +1 to search up, -1 to search down for side exits
  byte paramSelection;  // zero for xPosition, one for yPosition
  byte switchDirection; // which direction to switch to if there's a switch
  
  ghostCounter++;

  // Pop the next ghost out, if it's time
  if (ghostCounter >= releaseDelay) {
    ghostCounter = 0;
    for (byte lxn = 0; lxn < maxGhosts; lxn++) {
      if (ghostsActive[lxn] == 0) {
        locations[lxn][0] = GHOST_LAUNCH_X;
        locations[lxn][1] = GHOST_LAUNCH_Y;    
        ghostsActive[lxn] = 1;
        break;
      }
    }
  }

  for (byte lxn = 0; lxn < 3; lxn++) {
    if (ghostsActive[lxn] == 1) {
      char clash = 0;  
      switch(directions[lxn]) {
          case(DIR_RIGHT):
            travelDirection = 1;
            travelParam = 0;
            searchDirection = -1;
            paramSelection = 1;
            switchDirection = DIR_UP;
          break;
          
          case(DIR_LEFT):
            travelDirection = -1;
            travelParam = 0;
            searchDirection = 1;
            paramSelection = 1;
            switchDirection = DIR_DOWN; 
          break;     
    
          case(DIR_UP):
            travelDirection = -1;
            travelParam = 1;
            searchDirection = -1;
            paramSelection = 0;
            switchDirection = DIR_LEFT; 
          break;
          
          case(DIR_DOWN):
            travelDirection = 1;
            travelParam = 1;
            searchDirection = 1;
            paramSelection = 0;
            switchDirection = DIR_RIGHT; 
          break;      
        }
         
      locations[lxn][travelParam]+=travelDirection;
      // Move the ghosts along according to which way they are heading - if there's a clash, then backtrack
      if (checkCollision(locations[lxn][0], locations[lxn][1]) == 1) {locations[lxn][travelParam]-=travelDirection; newDirection(lxn);} else {            
        clash = 0;
        for (byte lxn2 = 0; lxn2 < 3; lxn2++) {
          locations[lxn][paramSelection]+=searchDirection;
          if (checkCollision(locations[lxn][0], locations[lxn][1]) == 1) clash = 1; 
        }
        if (clash == 0 && random(100) > BRANCH_PROBABILITY) directions[lxn] = switchDirection;
        locations[lxn][paramSelection]-=(searchDirection*3);            
      }  
    }  
  }
}

void newDirection(byte ghostNo) {
  directions[ghostNo] = random(4);
}

void eatenGhost(void) {
  for (int i = 700; i>200; i = i - 50){
    beep(30,i);
  }
}

byte hitGhosts(void){
  byte returnCode = 0;
  
  for (int lxn = 0; lxn < 3 ;lxn++) {
    if ((locations[lxn][0] > locations[3][0] - 8) && (locations[lxn][0] < locations[3][0] + 8) && (locations[lxn][1] > locations[3][1] - 8) && (locations[lxn][1] < locations[3][1] + 8)) {
      if (powerUp == 1) {
        locations[lxn][0] = startPosX[lxn];
        locations[lxn][1] = startPosY[lxn];
        ghostsActive[lxn] =0;
        eatenGhost();
        score += 10;
        ghostCounter = 0;
      } else returnCode = 1; 
    }
  }
  return returnCode;
}

void pacDie(void) {
  int i;
  directions[3] = DIR_UP;
  mouth = 1;
  displayScreen();
  for (i = 0; i<500; i = i+ 50){  
    beep(50,i);
  }
  mouth = 0;
  displayScreen();
  for (i = 500; i<1000; i = i+ 50){  
    beep(50,i);
  }
  delay(1200);
  stopAnimate = 1;
}

void movePacman(void) {
  ReadInput();
    
  count++;
  if (count == 2) {
    if (mouth == 0) {mouth = 1; beep(20,400+(powerUp*100));} else {mouth = 0; beep(20,420);}
    count = 0;
  }

  if (powerUp == 1) {
    powerCounter++;
    if (powerCounter >= POWER_LENGTH) {powerUp = 0; powerCounter = 0;}
  }
  
  oldDir = directions[3];
  directions[3] = commandDir;
  
  switch(directions[3]) {
  case(DIR_RIGHT):
    for (int lxn = 0; lxn < 3; lxn++) {
      locations[3][0]++;
      if (checkCollision(locations[3][0], locations[3][1]) == 1) {
        directions[3] = oldDir;
      } 
    }  
    locations[3][0]-=3;
  break;
  
  case(DIR_LEFT):
    for (int lxn = 0; lxn < 3; lxn++) {        
      locations[3][0]--;
      if (checkCollision(locations[3][0], locations[3][1]) == 1) {
        directions[3] = oldDir;
      }
    } 
    locations[3][0]+=3;
  break;
  
  case(DIR_UP):
    for (int lxn = 0; lxn < 3; lxn++) {
      locations[3][1]--;
      if (checkCollision(locations[3][0], locations[3][1]) == 1) {  
        directions[3] = oldDir;
      }       
    }
    locations[3][1]+=3;
  break;
  
  case(DIR_DOWN):
    for (int lxn = 0; lxn < 3; lxn++) {
      locations[3][1]++;
      if (checkCollision(locations[3][0], locations[3][1]) == 1) {
        directions[3] = oldDir;
      }
    }
    locations[3][1]-=3;              
  break;      
  }
  
  switch(directions[3]) {
    case(DIR_RIGHT):
      locations[3][0]+=1;
      if (checkCollision(locations[3][0], locations[3][1]) == 1) locations[3][0]-=1;
    break;
    
    case(DIR_LEFT):
      locations[3][0]-=1;
      if (checkCollision(locations[3][0], locations[3][1]) == 1) locations[3][0]+=1;
    break;
    
    case(DIR_UP):
      locations[3][1]-=1;
      if (checkCollision(locations[3][0], locations[3][1]) == 1) locations[3][1]+=1;
    break;
    
    case(DIR_DOWN):
      locations[3][1]+=1;
      if (checkCollision(locations[3][0], locations[3][1]) == 1) locations[3][1]-=1;          
    break;      
  }
}

void initLevel(void) {
  clickLock = false;
  pillsEaten = 0;
  stopAnimate = 0;

  for(byte lxn = 0; lxn < NO_PILLS; lxn++) pillsActive[lxn] = 1;  
  bigPillsActive[0] = 1;
  bigPillsActive[1] = 1;
}

void initScreen(void) {
  powerUp = 0;
  powerCounter = 0;
  ghostsActive[0] = 0;
  ghostsActive[1] = 0;
  ghostsActive[2] = 0;
  ghostCounter = 0;

  // Set up the initial directions of the ghosts and pacman
  for (byte lxn = 0; lxn<4; lxn++) {
    locations[lxn][0] = startPosX[lxn];
    locations[lxn][1] = startPosY[lxn];
    directions[lxn] = startDirections[lxn];
  }

  // Initially set pacman off in the direction he is facing
  commandDir = startDirections[3];  
}

/* ------------------------
 *  Pacman main game code
 */
void playPacman(){
  score = 0;
  lives = 2;
  level = 1;
  releaseDelay = INITIAL_GHOST_DELAY;
  maxGhosts = INITIAL_GHOSTS;

  initLevel();
  initScreen();

  while(lives >= 0) {
    
    if (pillsEaten >= NO_PILLS) levelUp();

    moveGhosts();
    movePacman();

    // Check for ghosrs or pacman going thru the tunnel teleport thing
    for (byte lxn2 = 0; lxn2 < 4; lxn2 ++) {
      if (locations[lxn2][1] <= 0) {locations[lxn2][1] = 56; locations[lxn2][0] = 100;} else if (locations[lxn2][1] >= 56) {locations[lxn2][1] = 0; locations[lxn2][0] = 21;}
    }

    if (hitGhosts() != 0) {
      pacDie();
      lives--;
      initScreen();      
    }
    
    if (clickLock && !(IsAction() || IsCenter())) clickLock = false; // normal debounce
         
    displayScreen();
  }   
}
