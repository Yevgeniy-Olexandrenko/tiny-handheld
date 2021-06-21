#include "common.h"

/*  Defines  */

#define OBJECTS         4
#define IMG_OBJECT_W    8
#define IMG_OBJECT_H    8

/*  Local Variables  */

PROGMEM static const uint8_t imgObject[] = { 0x3C, 0x7E, 0xFF, 0xE7, 0xE7, 0xFF, 0x7E, 0x3C };

static int16_t  dx = WIDTH / 4;
static int16_t  dy = HEIGHT / 4;

/*---------------------------------------------------------------------------*/

void initSpriteTest(void)
{
    setString(0, 0, F("[SPRITE]"), WHITE);
    for (uint8_t i = 0; i < OBJECTS; i++) {
        setSprite(i, 0, 0, imgObject, IMG_OBJECT_W, IMG_OBJECT_W, i);
    }
    isInvalid = true;
}

MODE_T updateSpriteTest(void)
{
    handleDpad();
    if (dpadX || dpadY || isInvalid) {
        dx += dpadX;
        dy += dpadY;
        for (uint8_t i = 0; i < OBJECTS; i++) {
            moveSprite(i, dx + (i & 1) * IMG_OBJECT_W * 2, dy + (i >> 1) * IMG_OBJECT_H * 2);
        }
        isInvalid = true;
    }
    return (isButtonDown(A_BUTTON)) ? MODE_TOP : MODE_SPRITE;
}

void drawSpriteTest(int16_t y, uint8_t *pBuffer)
{
    memset(pBuffer, 0x00, 48);
    for (uint8_t *p = &pBuffer[48]; p < &pBuffer[88]; *p++ = 0x55, *p++ = 0xAA) { ; }
    memset(pBuffer + 88, 0xFF, 40);
}
