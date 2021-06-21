#include "common.h"

/*  Defines  */

#define SPRITE_CURSOR   0
#define STRBUF_LAUNCH   0
#define OFFS_LAUNCH     14
#define IMG_CURSOR_W    7
#define IMG_CURSOR_H    5
#define MENUS           6

/*  Local Variables  */

PROGMEM static const char menuLabels[] = "BUTTON\0CHARACTER\0EEPROM\0SCREEN\0SOUND\0SPRITE";

static uint8_t menuPos = 0;

/*---------------------------------------------------------------------------*/

void initTopMenu(void)
{
    setString(0, 0, F("-- TEST MENU --"), WHITE);
    const char *p = menuLabels;
    for (int8_t i = 0; i < MENUS; i++) {
        setString(i + 2, 6, (__FlashStringHelper *)p, WHITE);
        p += strlen_P(p) + 1;
    }
#ifdef ENABLE_EEPROM
    setString(9, 0, setStringBuffer(STRBUF_LAUNCH, F("LAUNCH COUNTxxx")), WHITE);
    printNumber(STRBUF_LAUNCH, OFFS_LAUNCH, record.playCount, 10);
#endif
    setDpadSprite(SPRITE_CURSOR, DPAD_SPRITE_BUTTON, 0, menuPos * FONT_H + 14);
    isInvalid = true;
}

MODE_T updateTopMenu(void)
{
    handleDpad();
    if (dpadY && menuPos + dpadY >= 0 && menuPos + dpadY < MENUS) {
        menuPos += dpadY;
        moveSprite(SPRITE_CURSOR, 0, menuPos * FONT_H + 14);
        isInvalid = true;
    }
    return (isButtonDown(A_BUTTON)) ? (MODE_T)(MODE_BUTTON + menuPos) : MODE_TOP;
}
