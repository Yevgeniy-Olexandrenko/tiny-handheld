#include "common.h"

/*  Defines  */

#define FREQ_MIN        1
#define FREQ_MAX        999
#define FREQ_INIT       44
#define STRBUF_FREQ     0
#define OFFS_FREQ       12

/*  Local Variables  */

static uint16_t freq = FREQ_INIT;

/*---------------------------------------------------------------------------*/

void initSoundTest(void)
{
    setString(0, 0, F("[SOUND]"), WHITE);
    setDpadSprite(0, DPAD_SPRITE_Y_AXIS, 0, 8);
    setString(1, 12, setStringBuffer(STRBUF_FREQ, F("FREQUENCY xxx0HZ")), WHITE);
    setDpadSprite(1, DPAD_SPRITE_X_AXIS, 0, 14);
    setString(2, 12, F("PLAY"), WHITE);
    isInvalid = true;
}

MODE_T updateSoundTest(void)
{
    handleDpad();
    if (dpadY && freq - dpadY >= FREQ_MIN && freq - dpadY <= FREQ_MAX || isInvalid) {
        freq -= dpadY;
        printNumber(STRBUF_FREQ, OFFS_FREQ, freq, 10);
        isInvalid = true;
    }
#ifdef ENABLE_SOUND
    if (isButtonDown(LEFT_BUTTON | RIGHT_BUTTON)) playTone(freq * 10, 500);
#endif
    return (isButtonDown(A_BUTTON)) ? MODE_TOP : MODE_SOUND;
}
