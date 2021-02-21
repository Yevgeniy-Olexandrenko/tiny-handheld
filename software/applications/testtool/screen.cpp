#include "common.h"

/*  Defines  */

#define STRBUF_COORDS   0
#define OFFS_X          2
#define OFFS_Y          6

/*  Local Variables  */

static int16_t  dx = WIDTH / 4;
static int16_t  dy = HEIGHT / 4;

/*---------------------------------------------------------------------------*/

void initScreenTest(void)
{
    setString(0, 0, F("[SCREEN]"), WHITE);
    setString(5, 43, setStringBuffer(STRBUF_COORDS, F("nnn,nnn")), WHITE);
    isInvalid = true;
}

MODE_T updateScreenTest(void)
{
    handleDpad();
    if (dpadX || dpadY || isInvalid) {
        dx = circulate(dx, dpadX, WIDTH);
        dy = circulate(dy, dpadY, HEIGHT);
        printNumber(STRBUF_COORDS, OFFS_X, dx, 10);
        printNumber(STRBUF_COORDS, OFFS_Y, dy, 10);
        isInvalid = true;
    }
    return (isButtonDown(A_BUTTON)) ? MODE_TOP : MODE_SCREEN;
}

void drawScreenTest(int16_t y, uint8_t *pBuffer)
{
    memset(pBuffer, (dy >= y && dy < y + 8) ? bit(dy - y) : 0, WIDTH);
    pBuffer[dx] = 0xFF;
}
