#pragma once

#include "core.h"

/*  Defines  */

#define FPS             30
#define APP_CODE        "OBN-T00"
#define APP_VERSION     "0.5"
#define RECORD_TOKEN    0x004E424FUL // "OBN\x00"
#define RECORD_OFFSET   16

#define STRING_BUFFERS      4
#define STRING_BUFFER_SIZE  24
#define DPAD_REPEAT_DELAY   7

enum MODE_T : uint8_t {
    MODE_LOGO = 0,
    MODE_TOP,
    MODE_BUTTON,
    MODE_CHARACTER,
    MODE_EEPROM,
    MODE_SCREEN,
    MODE_SOUND,
    MODE_SPRITE,
};

enum DPAD_SPRITE_T : uint8_t {
    DPAD_SPRITE_ALL = 0,
    DPAD_SPRITE_X_AXIS,
    DPAD_SPRITE_Y_AXIS,
    DPAD_SPRITE_BUTTON,
};

/*  Typedefs  */

typedef struct {
    uint16_t    playCount;
} RECORD_T;

/*  Global Functions  */

void    setDpadSprite(uint8_t idx, DPAD_SPRITE_T type, int8_t x, int8_t y);
char    *(setStringBuffer)(uint8_t idx, const __FlashStringHelper *pFlashString);
void    printNumber(uint8_t idx, uint8_t offset, uint16_t number, uint8_t radix);
void    handleDpad(void);

void    initLogo(void);
MODE_T  updateLogo(void);

void    initTopMenu(void);
MODE_T  updateTopMenu(void);

void    initButtonTest(void);
MODE_T  updateButtonTest(void);

void    initCharacterTest(void);
MODE_T  updateCharacterTest(void);
void    drawCharacterTest(int16_t y, uint8_t *pBuffer);

void    initEEPROMTest(void);
MODE_T  updateEEPROMTest(void);

void    initScreenTest(void);
MODE_T  updateScreenTest(void);
void    drawScreenTest(int16_t y, uint8_t *pBuffer);

void    initSoundTest(void);
MODE_T  updateSoundTest(void);

void    initSpriteTest(void);
MODE_T  updateSpriteTest(void);
void    drawSpriteTest(int16_t y, uint8_t *pBuffer);

/*  Global Variables  */

extern RECORD_T record;
extern char     stringBuffer[STRING_BUFFERS][STRING_BUFFER_SIZE];
extern int8_t   dpadX, dpadY;
