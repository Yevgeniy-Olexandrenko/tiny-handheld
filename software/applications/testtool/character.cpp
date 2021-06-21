#include "common.h"

/*  Defines  */

#define CODE_MIN    ' '
#define CODE_MAX    '_'
#define STRBUF_INFO 0
#define OFFS_HEX    6
#define OFFS_CHAR   8
#define BOX_W       8

/*  Local Variables  */

static char     code = CODE_MIN;
static uint8_t  charPattern[FONT_W];

/*---------------------------------------------------------------------------*/

void initCharacterTest(void)
{
    setString(0, 0, F("[CHARACTER]"), WHITE);
    setDpadSprite(0, DPAD_SPRITE_Y_AXIS, 0, 8);
    setString(1, 12, setStringBuffer(STRBUF_INFO, F("CODExxx X")), WHITE);
    isInvalid = true;
}

MODE_T updateCharacterTest(void)
{
    handleDpad();
    if (dpadY && code - dpadY >= CODE_MIN && code - dpadY <= CODE_MAX || isInvalid) {
        code -= dpadY;
        printNumber(STRBUF_INFO, OFFS_HEX, code, 16);
        stringBuffer[STRBUF_INFO][OFFS_CHAR] = code;
        isInvalid = true;
    }
    return (isButtonDown(A_BUTTON)) ? MODE_TOP : MODE_CHARACTER;
}

void drawCharacterTest(int16_t y, uint8_t *pBuffer)
{
    if (y == 16) memcpy(charPattern, pBuffer + OFFS_CHAR * FONT_W + 12, FONT_W); // Trick!!
    uint8_t x = 0;
    if (y >= 16) {
        for (int8_t i = 0; i < FONT_W; i++) {
            memset(pBuffer + i * BOX_W, bitRead(charPattern[i], (y >> 3) - 2) * 0xFF, BOX_W);
        }
        x = BOX_W * FONT_W;
    }
    memset(pBuffer + x, 0, WIDTH - x);
}
