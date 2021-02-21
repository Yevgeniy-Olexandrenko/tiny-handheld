#include "common.h"
#include <avr/eeprom.h>

/*  Defines  */

#define ADDRESS_MIN     0
#define ADDRESS_MAX     511
#define STRBUF_ADDRESS  0
#define STRBUF_VALUE    1
#define OFFS_ADDRESS    10
#define OFFS_VALUE      7

/*  Local Functions  */

static void storeValue(void);

/*  Local Variables  */

static uint16_t address = ADDRESS_MIN;
static uint8_t  value;
static bool     isDirty;

/*---------------------------------------------------------------------------*/

void initEEPROMTest(void)
{
    setString(0, 0, F("[EEPROM]"), WHITE);
    setDpadSprite(0, DPAD_SPRITE_Y_AXIS, 0, 8);
    setString(1, 12, setStringBuffer(STRBUF_ADDRESS, F("ADDRESS xxx")), WHITE);
    setDpadSprite(1, DPAD_SPRITE_X_AXIS, 0, 14);
    setString(2, 12, setStringBuffer(STRBUF_VALUE, F("VALUExxx")), WHITE);
    isDirty = false;
    isInvalid = true;
}

MODE_T updateEEPROMTest(void)
{
    handleDpad();
    if (dpadY && address - dpadY >= ADDRESS_MIN && address - dpadY <= ADDRESS_MAX || isInvalid) {
        if (isDirty) storeValue();
        address -= dpadY;
        printNumber(STRBUF_ADDRESS, OFFS_ADDRESS, address, 16);
        eeprom_busy_wait();
        value = eeprom_read_byte((const uint8_t *)address);
        isInvalid = true;
    }
    if (dpadX) {
        value += dpadX;
        isDirty = true;
        isInvalid = true;
    }
    if (isInvalid) printNumber(STRBUF_VALUE, OFFS_VALUE, value, 16);

    if (isButtonDown(A_BUTTON)) {
        if (isDirty) storeValue();
        return MODE_TOP;
    }
    return MODE_EEPROM;
}

static void storeValue(void)
{
    eeprom_busy_wait();
    eeprom_write_byte((uint8_t *)address, value);
}
