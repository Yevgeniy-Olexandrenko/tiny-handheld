#include "common.h"

/*  Defines  */

#define BUTTONS         5
#define STRBUF_PRESS    0
#define STRBUF_DOWN     1
#define STRBUF_UP       2
#define OFFS_INFO       6
#define OFFS_ANALOG     20

/*  Local Variables  */

/*---------------------------------------------------------------------------*/

void initButtonTest(void)
{
    setString(0, 0, F("[BUTTON]"), WHITE);
    setDpadSprite(0, DPAD_SPRITE_ALL, OFFS_INFO * FONT_W, 8);
    setString(2, 0, setStringBuffer(STRBUF_PRESS, F("PRESS xxxxx")), WHITE);
    setString(3, 0, setStringBuffer(STRBUF_DOWN,  F(" DOWN xxxxx   A0=nnnn")), WHITE);
    setString(4, 0, setStringBuffer(STRBUF_UP,    F("   UP xxxxx   A3=nnnn")), WHITE);
    setDpadSprite(1, DPAD_SPRITE_BUTTON, 27, 56);
    setString(9, 0, F("HOLD  TO EXIT"), WHITE);
    counter = 0;
}

MODE_T updateButtonTest(void)
{
    for (uint8_t i = 0; i < BUTTONS; i++) {
        uint8_t x = i + OFFS_INFO, b = bit(i);
        stringBuffer[STRBUF_PRESS][x] = isButtonPressed(b) ? '#' : '.';
        stringBuffer[STRBUF_DOWN ][x] = isButtonDown(b) ? '#' : '.';
        stringBuffer[STRBUF_UP   ][x] = isButtonUp(b) ? '#' : '.';
    }
    for (uint8_t i = 0; i < 2; i++) {
        uint16_t analog = analogRead(i * 3);
        printNumber(STRBUF_DOWN + i, OFFS_ANALOG, analog, 10);
        stringBuffer[STRBUF_DOWN + i][OFFS_ANALOG - 3] = (analog >= 1000) ? '1' : ' ';
    }
    isInvalid = true;
    counter = (isButtonPressed(A_BUTTON)) ? counter + 1 : 0;
    return (counter >= FPS) ? MODE_TOP : MODE_BUTTON;
}
