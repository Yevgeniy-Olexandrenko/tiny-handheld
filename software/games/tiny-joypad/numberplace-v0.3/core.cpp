#include "core.h"
#define SimpleWire_SCL_PORT B
#define SimpleWire_SCL_POS  2
#define SimpleWire_SDA_PORT B
#define SimpleWire_SDA_POS  0
#include "SimpleWire.h"
#ifdef ENABLE_EEPROM
#include <avr/eeprom.h>
#endif

/*  Defines  */

#define BUTTON_STATE_OPTIMIZE

#define PIN_X_AXIS      A0
#define PIN_Y_AXIS      A3
#define PIN_BUTTON      1
#define PIN_BIT_BUTTON  0b00000010
#define PIN_SOUND       4
#define PIN_BIT_SOUND   0b00010000

#define SIMPLEWIRE      SimpleWire<SimpleWire_1M>
#define SSD1306_ADDRESS 0x3C
#define SSD1306_COMMAND 0x00
#define SSD1306_DATA    0x40

#define SPRITES         8
#define STRINGS         10

#define EEPROM_TOKEN_SIZE 4

/*  Typedefs  */

typedef struct {
    int8_t  x;
    int8_t  y;
    uint8_t w;
    uint8_t h:6;
    uint8_t color:2;
    const uint8_t *pBitmap;
} SPRITE_T;

typedef struct {
    int8_t  x;
    uint8_t isPgm:1;
    uint8_t color:1;
    char    *pString;
} STRING_T;

/*  Macro functions  */

#define is88KRange(v)       ((v) <= 920 && (v) > 715)
#define is33KRange(v)       ((v) <= 715 && (v) > 573)
#define getAxisState(pin)   ((1024 - analogRead(pin)) / 150)
#define enableSoundTimer()  bitSet(TIMSK, OCIE1A)
#define disableSoundTimer() bitClear(TIMSK, OCIE1A)
#define checkSumSeed(v)     ((uint8_t)(((v) >> 24 & 0xFF) * 249 ^ ((v) >> 16 & 0xFF) * 251 ^ \
                                       ((v) >>  8 & 0xFF) * 253 ^ ((v)       & 0xFF) * 255))

/*  Local functions  */

#ifdef ENABLE_SOUND
static void forwardScore(void);
static void setupSoundTimer(uint16_t frequency, uint16_t duration);
#endif

/*  Global Variables  */

uint8_t counter;
bool    isInvalid;

/*  Local Variables  */

PROGMEM static const uint8_t ssd1306InitSequence[] = { // Initialization Sequence
    SSD1306_COMMAND,
    0xAE,           // Set Display ON/OFF - AE=OFF, AF=ON
    0xD5, 0xF0,     // Set display clock divide ratio/oscillator frequency, set divide ratio
    0xA8, 0x3F,     // Set multiplex ratio (1 to 64) ... (height - 1)
    0xD3, 0x00,     // Set display offset. 00 = no offset
    0x40 | 0x00,    // Set start line address, at 0.
    0x8D, 0x14,     // Charge Pump Setting, 14h = Enable Charge Pump
    0x20, 0x00,     // Set Memory Addressing Mode - 00=Horizontal, 01=Vertical, 10=Page, 11=Invalid
    0xA0 | 0x01,    // Set Segment Re-map
    0xC8,           // Set COM Output Scan Direction
    0xDA, 0x12,     // Set COM Pins Hardware Configuration - 128x32:0x02, 128x64:0x12
    0x81, 0x3F,     // Set contrast control register
    0xD9, 0x22,     // Set pre-charge period (0x22 or 0xF1)
    0xDB, 0x20,     // Set Vcomh Deselect Level - 0x00: 0.65 x VCC, 0x20: 0.77 x VCC (RESET), 0x30: 0.83 x VCC
    0xA4,           // Entire Display ON (resume) - output RAM to display
    0xA6,           // Set Normal/Inverse Display mode. A6=Normal; A7=Inverse
    0x2E,           // Deactivate Scroll command
    0xAF,           // Set Display ON/OFF - AE=OFF, AF=ON
    0xB0,           // Set page start, at 0.
    0x00, 0x10,     // Set column start, at 0.
};

PROGMEM static const uint32_t imgFont[] = { // '-' ~ 'Z'
                                                                0x04104104, 0x00000400, 0x01084210,
    0x0F45145E, 0x0001F040, 0x13555559, 0x0D5D5551, 0x087C928C, 0x0D555557, 0x0D55555E, 0x010C5251,
    0x0F55555E, 0x0F555556, 0x0000A000, 0x0000A400, 0x0028C200, 0x0028A280, 0x00086280, 0x000D5040,
    0x0755745E, 0x1F24929C, 0x0D5D555F, 0x1145149C, 0x0725145F, 0x1155555F, 0x0114515F, 0x1D55545E,
    0x1F10411F, 0x0045F440, 0x07210410, 0x1D18411F, 0x1041041F, 0x1F04F05E, 0x1F04109C, 0x0F45545E,
    0x0314925F, 0x1F45D45E, 0x1B34925F, 0x0D555556, 0x0105F041, 0x0721041F, 0x0108421F, 0x0F41E41F,
    0x1D184317, 0x0109C107, 0x114D5651
};

#ifdef ENABLE_SOUND
PROGMEM static const uint16_t noteFrquency[] = {
    8372, 8870, 9397, 9956, 10548, 11175, 11840, 12544, 13290, 14080, 14917, 15804
};
#endif

static SPRITE_T sprite[SPRITES];
static STRING_T string[STRINGS];
static uint8_t  buttonState = 0;
static uint8_t  lastButtonState;
static uint8_t  wireBuffer[WIDTH + 1];
#ifdef ENABLE_SOUND
static volatile uint32_t toneToggleCount;
static volatile const uint8_t *pPlayingScore;
#endif
#ifdef ENABLE_EEPROM
static bool     isRecordVirgin;
#endif

/*---------------------------------------------------------------------------*/
/*                                   Core                                    */
/*---------------------------------------------------------------------------*/

void initCore(void)
{
#ifdef BUTTON_STATE_OPTIMIZE
    // Default setting of every pin mode is INPUT.
#ifdef ENABLE_SOUND
    DDRB |= PIN_BIT_SOUND;
    PORTB &= ~PIN_BIT_SOUND;
#endif
#else
    pinMode(PIN_X_AXIS, INPUT);
    pinMode(PIN_Y_AXIS, INPUT);
    pinMode(PIN_BUTTON, INPUT);
#ifdef ENABLE_SOUND
    pinMode(PIN_SOUND, OUTPUT);
    digitalWrite(PIN_SOUND, LOW);
#endif
#endif
    SIMPLEWIRE::begin();
    memcpy_P(wireBuffer, ssd1306InitSequence, sizeof(ssd1306InitSequence));
    SIMPLEWIRE::write(SSD1306_ADDRESS, wireBuffer, sizeof(ssd1306InitSequence));
    initSprites();
    initStrings();
}

void refreshScreen(void (*func)(int16_t, uint8_t *))
{
    if (!isInvalid) return;

    wireBuffer[0] = SSD1306_DATA;
    for (int16_t y = 0; y < HEIGHT; y += 8) {
        (func) ? func(y, &wireBuffer[1]) : clearScreenBuffer();
        drawSprites(y);
        drawStrings(y);
        SIMPLEWIRE::write(SSD1306_ADDRESS, wireBuffer, WIDTH + 1);
    }
    isInvalid = false;
}

void clearScreenBuffer(void)
{
    memset(&wireBuffer[1], 0, WIDTH);
}

/*---------------------------------------------------------------------------*/
/*                               Button State                                */
/*---------------------------------------------------------------------------*/

void updateButtonState(void)
{
    lastButtonState = buttonState;
#ifdef BUTTON_STATE_OPTIMIZE
    buttonState =
            getAxisState(PIN_X_AXIS) |
            getAxisState(PIN_Y_AXIS) << 2 |
            (~PINB & PIN_BIT_BUTTON) << 3;
#else
    buttonState = 0;
    uint16_t xAxis = analogRead(PIN_X_AXIS), yAxis = analogRead(PIN_Y_AXIS);
    if (is88KRange(xAxis)) buttonState |= LEFT_BUTTON;
    if (is33KRange(xAxis)) buttonState |= RIGHT_BUTTON;
    if (is88KRange(yAxis)) buttonState |= DOWN_BUTTON;
    if (is33KRange(yAxis)) buttonState |= UP_BUTTON;
    if (digitalRead(PIN_BUTTON) == LOW) buttonState |= A_BUTTON;
#endif
}

bool isButtonPressed(uint8_t b)
{
    return (buttonState & b);
}

bool isButtonDown(uint8_t b)
{
    return (buttonState & ~lastButtonState & b);
}

bool isButtonUp(uint8_t b)
{
    return (~buttonState & lastButtonState & b);
}

/*---------------------------------------------------------------------------*/
/*                                  Sprites                                  */
/*---------------------------------------------------------------------------*/

void initSprites(void)
{
    memset(sprite, 0, sizeof(SPRITE_T) * SPRITES);
}

void setSprite(uint8_t idx, int8_t x, int8_t y, const uint8_t *pBitmap, uint8_t w, uint8_t h, uint8_t color)
{
    SPRITE_T *p = &sprite[idx];
    p->x = x;
    p->y = y;
    p->w = w;
    p->h = h;
    p->color = color;
    p->pBitmap = pBitmap;
}

void moveSprite(uint8_t idx, int8_t x, int8_t y)
{
    SPRITE_T *p = &sprite[idx];
    p->x = x;
    p->y = y;
}

void clearSprite(uint8_t idx)
{
    sprite[idx].pBitmap = NULL;
}

void drawSprites(int16_t y)
{
    for (SPRITE_T *p = sprite; p < &sprite[SPRITES]; p++) {
        if (p->pBitmap == NULL || p->y > y + 7 || p->y + p->h <= y ||
                p->x >= WIDTH || p->x + p->w <= 0) continue;
        int8_t row = (y + 7 - p->y) >> 3;
        int8_t odd = p->y & 7;
        int16_t x = p->x;
        const uint8_t *pSrc = p->pBitmap + p->w * row;
        uint8_t *pDest = &wireBuffer[1 + x];
        for (uint8_t w = p->w; w > 0; w--, x++, pSrc++, pDest++) {
            if (x < 0 || x >= WIDTH) continue;
            uint8_t ptn = 0;
            if (row < ((p->h + 7) >> 3)) ptn |= pgm_read_byte(pSrc) << odd;
            if (odd > 0 && row > 0) ptn |= pgm_read_byte(pSrc - p->w) >> (8 - odd);
            if (p->color == BLACK) {
                *pDest &= ~ptn;
            } else if (p->color == WHITE) {
                *pDest |= ptn;
            } else if (p->color == INVERT) {
                *pDest ^= ptn;
            } else {
                *pDest = ptn;
            }
        }
    }
}

/*---------------------------------------------------------------------------*/
/*                                  Strings                                  */
/*---------------------------------------------------------------------------*/

void initStrings(void)
{
    memset(string, 0, sizeof(STRING_T) * STRINGS);
}

void setString(uint8_t idx, int8_t x, const __FlashStringHelper *pFlashString, uint8_t color)
{
    STRING_T *p = &string[idx];
    p->x = x;
    p->color = color;
    p->isPgm = true;
    p->pString = (char *)pFlashString;
}

void setString(uint8_t idx, int8_t x, char *pString, uint8_t color)
{
    STRING_T *p = &string[idx];
    p->x = x;
    p->color = color;
    p->isPgm = false;
    p->pString = pString;
}

void clearString(uint8_t idx)
{
    string[idx].pString = NULL;
}

void drawStrings(int16_t y)
{
    y += 4;
    STRING_T *p = &string[y / FONT_H - 1];
    for (int8_t shift = -(y % FONT_H); shift < 8; p++, shift += FONT_H) {
        if (p < string || p >= &string[STRINGS] || p->pString == NULL) continue;
        int8_t x = p->x;
        char c, *pChar = p->pString;
        while ((c = (p->isPgm) ? pgm_read_byte(pChar++) : *pChar++)) {
            if (c < '-' || c > 'Z') {
                x += FONT_W;
                continue;
            }
            uint32_t glyph = pgm_read_dword(&imgFont[c - '-']);
            uint8_t *pDest = &wireBuffer[1 + x];
            for (uint8_t w = FONT_W; w > 0; w--, x++, glyph >>= FONT_H, *pDest++) {
                if (x < 0 || x >= WIDTH) continue;
                uint8_t ptn = (glyph & 0x3F);
                if (shift > 0) ptn <<= shift;
                if (shift < 0) ptn >>= -shift;
                if (p->color) {
                    *pDest |= ptn;
                } else {
                    *pDest &= ~ptn;
                }
            }
        }
    }
}

/*---------------------------------------------------------------------------*/
/*                                   Sound                                   */
/*---------------------------------------------------------------------------*/

#ifdef ENABLE_SOUND

void playTone(uint16_t frequency, uint16_t duration)
{
    disableSoundTimer();
    pPlayingScore = NULL;
    setupSoundTimer(frequency, duration);
}

void playScore(const uint8_t *pScore)
{
    disableSoundTimer();
    pPlayingScore = pScore;
    if (pScore != NULL) forwardScore();
}

static void forwardScore(void)
{
    uint8_t note = pgm_read_byte(pPlayingScore++);
    if (bitRead(note, 7)) {
        pPlayingScore = NULL;
        return;
    }
    uint16_t frequency = pgm_read_word(&noteFrquency[note % 12]);
    frequency >>= (131 - note) / 12;
    setupSoundTimer(frequency, pgm_read_byte(pPlayingScore++) * 10);
}

static void setupSoundTimer(uint16_t frequency, uint16_t duration)
{
    frequency *= 2;
    toneToggleCount = (uint32_t)duration * frequency / 1000 + 1;
    uint32_t ocr = F_CPU / frequency;
    uint8_t prescalarBits = 0b0001;
    while (ocr > 0xff && prescalarBits < 0b1111) {
        prescalarBits++;
        ocr >>= 1;
    }
    TCCR1 = 0b10000000 | prescalarBits; // CTC1=1, PWM1A=0, COM1A=00
    OCR1C = ocr - 1;
    TCNT1 = 0;
    enableSoundTimer();
}

ISR(TIMER1_COMPA_vect)
{
    if (toneToggleCount > 0) {
        if (--toneToggleCount) {
            PORTB ^= PIN_BIT_SOUND;
        } else {
            PORTB &= ~PIN_BIT_SOUND;
            disableSoundTimer();
            if (pPlayingScore != NULL) forwardScore();
        }
    }
}

#endif

/*---------------------------------------------------------------------------*/
/*                                   EEPROM                                  */
/*---------------------------------------------------------------------------*/

#ifdef ENABLE_EEPROM

bool loadRecord(uint32_t signature, uint16_t address, void *pRecord, size_t size)
{
    isRecordVirgin = true;
    eeprom_busy_wait();
    if (eeprom_read_dword((const uint32_t *)address) == signature) {
        eeprom_read_block(pRecord, (const void *)(address + EEPROM_TOKEN_SIZE), size);
        uint8_t checkSum = eeprom_read_byte((const uint8_t *)(address + EEPROM_TOKEN_SIZE + size));
        for (uint8_t i = 0, *p = (uint8_t *)pRecord; i < size; i++, p++) {
            checkSum -= *p * ((i << 1) | 1);
        }
        if (checkSum == checkSumSeed(signature)) isRecordVirgin = false;
    }
    return isRecordVirgin;
}

void storeRecord(uint32_t signature, uint16_t address, void *pRecord, size_t size)
{
    uint8_t checkSum = checkSumSeed(signature);
    for (uint8_t i = 0, *p = (uint8_t *)pRecord; i < size; i++, p++) {
        checkSum += *p * ((i << 1) | 1);
    }
    eeprom_busy_wait();
    if (isRecordVirgin) eeprom_write_dword((uint32_t *)address, signature);
    eeprom_write_block(pRecord, (void *)(address + EEPROM_TOKEN_SIZE), size);
    eeprom_write_byte((uint8_t *)(address + EEPROM_TOKEN_SIZE + size), checkSum);
    isRecordVirgin = false;
}

#endif
