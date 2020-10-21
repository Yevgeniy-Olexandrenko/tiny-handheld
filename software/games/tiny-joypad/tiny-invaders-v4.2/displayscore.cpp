#include <Arduino.h>
#include <avr/pgmspace.h>
#include "displayscore.h"
#include "smallFont.h"
#include <EEPROM.h>

const uint8_t HISCORE_MAGIC = 42;

/*--------------------------------------------------------------*/
void resetScore()
{
  score = 0;
}

/*--------------------------------------------------------------*/
void setScore( SCORE_TYPE points )
{
  score = points;
}

/*--------------------------------------------------------------*/
SCORE_TYPE getScore()
{
  return( score );
}

/*--------------------------------------------------------------*/
bool updateHighScorePoints()
{
  if ( score > getHighScorePoints() )
  {
    setHighScorePoints( score );
    return( true );
  }
  return( false );
}

/*--------------------------------------------------------------*/
void setHighScorePoints( SCORE_TYPE points )
{
  _hiScore.score = points;
}

/*--------------------------------------------------------------*/
SCORE_TYPE getHighScorePoints()
{
  return( _hiScore.score );
}

/*--------------------------------------------------------------*/
// simple checksum by adding all values which should result in 0x42, not a real CRC ;)
uint8_t calcHighScoreCRC()
{
  uint8_t sum = 0;
  uint8_t *hiScore = (uint8_t *)&_hiScore;
  uint8_t *hiScoreEnd = hiScore + sizeof( _hiScore );
  // sum up all values
  while ( hiScore < hiScoreEnd )
  {
    sum += *hiScore++;
  }

  return( sum );
}

/*--------------------------------------------------------------*/
// Attention: The returned string has a fixed size of 3 bytes 
// and is *not* terminated with zero!
uint8_t *getHighScoreName()
{
  return( _hiScore.name );
}

/*--------------------------------------------------------------*/
void initHighScoreStruct( uint16_t gameAddr )
{
  // read stored structure to _hiScore variable
  readHighScoreFromEEPROM( gameAddr );
  // structure not valid? (no former hiscore stored)
  if ( calcHighScoreCRC() != HISCORE_MAGIC )
  {
    // initialize with some default values
    _hiScore.score = 0;
    memset( _hiScore.name, '?', sizeof(_hiScore.name) );
    _hiScore.crcFix = 0;
    // and store it (CRC is calculated while storing)
    storeHighScoreToEEPROM( gameAddr );
  }
}

/*--------------------------------------------------------------*/
// read HISCORE struct from EEPROM
void readHighScoreFromEEPROM( uint16_t gameAddr )
{
  uint8_t *hiScore = (uint8_t *)&_hiScore;
  uint8_t *hiScoreEnd = hiScore + sizeof( _hiScore );

  while ( hiScore < hiScoreEnd )
  {
    *hiScore++ = EEPROM.read( gameAddr++ );
  }
}

/*--------------------------------------------------------------*/
// store HISCORE struct to EEPROM
void storeHighScoreToEEPROM( uint16_t gameAddr )
{
  // generate new value to adjust the sum of all bytes to 0x42,
  // so just set the crcFix to '0' and sum up the rest
  _hiScore.crcFix = 0;
  _hiScore.crcFix = HISCORE_MAGIC - calcHighScoreCRC();
  
  uint8_t *hiScore = (uint8_t *)&_hiScore;
  uint8_t *hiScoreEnd = hiScore + sizeof( _hiScore );
  
  while ( hiScore < hiScoreEnd )
  {
    EEPROM.update( gameAddr++, *hiScore++ );
  }
}

/*--------------------------------------------------------------*/
void addScore( SCORE_TYPE points )
{
  score += points;
}

/*--------------------------------------------------------------*/
// Converts 'value' to 6 decimal digits (if SCORE_TYPE equals uint16_t)
// Not the most elegant version, but the shortest for uint16_t
void convertValueToDigits( SCORE_TYPE value, uint8_t *digits )
{
  volatile static SCORE_TYPE dividerList[] = { 10000, 1000, 100, 10, 1, 0 };

  SCORE_TYPE *divider = dividerList;
  
  do
  {
    uint8_t digit = '0';
    while( value >= *divider )
    {
      digit++;
      value -= *divider;
    }
    // store digit
    *digits++ = digit;
    // next divider
    divider++;
  }
  while ( *divider != 0 );
}

/*--------------------------------------------------------------*/
// Displays a line of ascii character from the smallFont in the 
// top line of the screen. To save flash memory, the font ranges
// only from '0' to 'Z'.
uint8_t displayText( uint8_t x, uint8_t y )
{
  // top line?
  if ( y == 0 )
  {
    // find appropriate character in text array (font width is 4 px)
    uint8_t value = textBuffer[x >> 2];
    // is it a valid character?
    if ( value != 0 )
    {
      // get the column value
      return( pgm_read_byte( characterFont3x5 + ( ( value - '0' ) << 2 ) + ( x & 0x03) ) );
    }
  }
  return( 0x00 );
}

/*--------------------------------------------------------------*/
// Display zoomed ascii character from the smallFont in four
// lines of 16 characters. The zoom factor is fixed to '2'.
// If bit 7 is set, the character will be displayed inverted.
// To save flash memory, the font ranges only from '0' to 'Z'.
uint8_t displayZoomedText( uint8_t x, uint8_t y )
{
  // Find appropriate character in text array:
  // Font width is 4 px, zoom is 2x, so fetch a new character every 8 pixels
  uint8_t value = textBuffer[((y >> 1) << 4) + ( x >> 3)];
  // is it a valid character?
  if ( value != 0 )
  {
    // MSB set? -> inverse video
    uint8_t reverse = value & 0x80;
    // remove MSB from value
    value -= reverse;
    // return the column value (move the font 1 pixel down, lowest pixel returns at the top)
    value = ( pgm_read_byte( characterFont3x5 + ( ( value - '0' ) << 2 ) + ( ( x >> 1 ) & 0x03 ) ) );
    if ( ( y & 0x01 ) == 0 )
    {
      // upper line
      value = ( pgm_read_byte( nibbleZoom + ( value & 0x0f ) ) );
    }
    else
    {
      // lower line
      value = ( pgm_read_byte( nibbleZoom + ( value >> 4 ) ) );
    }
    // invert?
    if ( reverse )
    {
      // invert pixels
      value = value ^ 0xff;
    }
    return( value );
  }

  // Please move along, there is nothing to be seen here...
  return( 0x00 );
}

/*--------------------------------------------------------------*/
void clearTextBuffer()
{
  memset( textBuffer, 0x00, sizeof( textBuffer ) );
}

/*--------------------------------------------------------------*/
void printText( uint8_t x, uint8_t *text, uint8_t textLength )
{
  memcpy( textBuffer + x, text, textLength );
}

/*--------------------------------------------------------------*/
void pgm_printText( uint8_t x, uint8_t *text, uint8_t textLength )
{
  memcpy_P( textBuffer + x, text, textLength );
}

/*--------------------------------------------------------------*/
uint8_t *getTextBuffer()
{
  return( textBuffer );
}
