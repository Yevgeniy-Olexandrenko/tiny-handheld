#include "common.h"

/*  Defines  */

#define DPAD_BUTTONS        (LEFT_BUTTON | RIGHT_BUTTON | DOWN_BUTTON | UP_BUTTON)
#define DPAD_REPEAT_DELAY   (FPS / 4)

/*  Global Variables  */

int16_t puzzleSeed;
int8_t  dpadX, dpadY;
uint8_t level;

/*---------------------------------------------------------------------------*/

void handleDpad(void)
{
    static uint8_t dpadCounter = 0;
    dpadX = isButtonPressed(RIGHT_BUTTON) - isButtonPressed(LEFT_BUTTON);
    dpadY = isButtonPressed(DOWN_BUTTON) - isButtonPressed(UP_BUTTON);
    if (isButtonPressed(DPAD_BUTTONS)) {
        if (dpadCounter < DPAD_REPEAT_DELAY && dpadCounter++ > 0) dpadX = dpadY = 0;
    } else {
        dpadCounter = 0;
    }
}

void playSoundTick(void)
{
    playTone(440, 10);
}

void playSoundClick(void)
{
    playTone(587, 20);
}
