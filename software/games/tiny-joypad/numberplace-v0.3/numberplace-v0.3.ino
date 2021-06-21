#include "common.h"

/*  Typedefs  */

typedef struct {
    void    (*initFunc)(void);
    MODE_T  (*updateFunc)(void);
    void    (*drawFunc)(int16_t, uint8_t *);
} MODULE_FUNCS_T;

/*  Macro functions  */

#define callInitFunc(idx)       ((void (*)(void)) pgm_read_word(&moduleTable[idx].initFunc))()
#define callUpdateFunc(idx)     ((MODE_T (*)(void)) pgm_read_word(&moduleTable[idx].updateFunc))()
#define pointerDrawFunc(idx)    ((void (*)(int16_t, uint8_t *)) pgm_read_word(&moduleTable[idx].drawFunc))

/*  Local Variables  */

PROGMEM static const MODULE_FUNCS_T moduleTable[] = {
    { initLogo,  updateLogo,  NULL      },
    { initTitle, updateTitle, NULL      },
    { initGame,  updateGame,  drawGame  },
};

static MODE_T   mode = MODE_LOGO;
static uint32_t targetTime;

/*---------------------------------------------------------------------------*/

void setup(void)
{
    initCore();
    callInitFunc(mode);
    targetTime = millis();
}

void loop(void)
{
    updateButtonState();
    MODE_T nextMode = callUpdateFunc(mode);
    refreshScreen(pointerDrawFunc(mode));
    if (mode != nextMode) {
        mode = nextMode;
        initSprites();
        initStrings();
        callInitFunc(mode);
    }
    targetTime += 1000 / FPS;
    uint32_t delayTime = targetTime - millis();
    if (!bitRead(delayTime, 31)) delay(delayTime);
}
