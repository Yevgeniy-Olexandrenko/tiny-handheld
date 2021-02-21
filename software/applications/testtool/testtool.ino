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
    { initLogo,         updateLogo,         NULL                },
    { initTopMenu,      updateTopMenu,      NULL                },
    { initButtonTest,   updateButtonTest,   NULL                },
    { initCharacterTest,updateCharacterTest,drawCharacterTest   },
    { initEEPROMTest,   updateEEPROMTest,   NULL                },
    { initScreenTest,   updateScreenTest,   drawScreenTest      },
    { initSoundTest,    updateSoundTest,    NULL                },
    { initSpriteTest,   updateSpriteTest,   drawSpriteTest      },
};

static MODE_T   mode = MODE_LOGO;
static uint32_t targetTime;

/*---------------------------------------------------------------------------*/

void setup(void)
{
    initCore();
#ifdef ENABLE_EEPROM
    if (loadRecord(RECORD_TOKEN, RECORD_OFFSET, &record, sizeof(RECORD_T))) {
        record.playCount = 0; //  initialize record
    }
    record.playCount++;
    storeRecord(RECORD_TOKEN, RECORD_OFFSET, &record, sizeof(RECORD_T));
#endif
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
