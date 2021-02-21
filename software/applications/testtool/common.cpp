#include "common.h"

/*  Defines  */

#define NUMBER_DIGITS       3
#define DPAD_BUTTONS        (LEFT_BUTTON | RIGHT_BUTTON | DOWN_BUTTON | UP_BUTTON)

/*  Global Variables  */

RECORD_T    record;
char        stringBuffer[STRING_BUFFERS][STRING_BUFFER_SIZE];
int8_t      dpadX, dpadY;

/*  Local Variables  */

static PROGMEM const uint8_t imgButtons[] = { // 6x6 x5
    0x04, 0x04, 0x0E, 0x0E, 0x1F, 0x00, // left
    0x1F, 0x0E, 0x0E, 0x04, 0x04, 0x00, // right
    0x01, 0x07, 0x1F, 0x07, 0x01, 0x00, // down
    0x10, 0x1C, 0x1F, 0x1C, 0x10, 0x00, // up
    0x0E, 0x1F, 0x1F, 0x1F, 0x0E, 0x00, // A
};

static uint8_t dpadCounter;

/*---------------------------------------------------------------------------*/

void setDpadSprite(uint8_t idx, DPAD_SPRITE_T type, int8_t x, int8_t y)
{
    const uint8_t *p = imgButtons;
    uint8_t w = 12;
    switch (type) {
        case DPAD_SPRITE_ALL:    w = 30; break;
        case DPAD_SPRITE_X_AXIS: break;
        case DPAD_SPRITE_Y_AXIS: p += 12; break;
        case DPAD_SPRITE_BUTTON: p += 24; w = 6; break;
    }
    setSprite(idx, x, y, p, w, 8, WHITE);
}

char *(setStringBuffer)(uint8_t idx, const __FlashStringHelper *pFlashString)
{
    uint8_t len = strlen_P((const char *)pFlashString);
    char *p = stringBuffer[idx];
    memcpy_P(p, pFlashString, len + 1);
    return p;
}

void printNumber(uint8_t idx, uint8_t offset, uint16_t number, uint8_t radix)
{
    char *p = &stringBuffer[idx][offset];
    for (int8_t i = 0; i < NUMBER_DIGITS; i++) {
        uint8_t c = number % radix;
        *p-- = (number || i == 0) ? c + ((c < 10) ? '0' : 'A' - 10) : ' ';
        number /= radix;
    }
}

void handleDpad(void)
{
    dpadX = isButtonPressed(RIGHT_BUTTON) - isButtonPressed(LEFT_BUTTON);
    dpadY = isButtonPressed(DOWN_BUTTON) - isButtonPressed(UP_BUTTON);
    if (isButtonPressed(DPAD_BUTTONS)) {
        if (dpadCounter < DPAD_REPEAT_DELAY && dpadCounter++ > 0) dpadX = dpadY = 0;
    } else {
        dpadCounter = 0;
    }
}
