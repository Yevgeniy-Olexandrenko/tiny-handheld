#include "common.h"
#include "data.h"

/*  Defines  */

#define BOARD_SIZE      9
#define SECTION_SIZE    3
#define SECTIONS        (BOARD_SIZE / SECTION_SIZE)
#define DEFAULT_POS     (BOARD_SIZE / 2)

#define DIGITS          (BOARD_SIZE + 1)
#define DEFAULT_NUMBER  1

#define CELL_PX         7
#define SECTION_PX      (SECTION_SIZE * CELL_PX)
#define BOARD_PX        (BOARD_SIZE * CELL_PX)
#define BOARD_LEFT      28

#define SHORT_PRESS     (FPS / 2)
#define LONG_PRESS      (FPS * 3)

enum : uint8_t {
    SPR_ID_STATE = 0,
    SPR_ID_TOOL,
    SPR_ID_NUMBER,
    SPR_ID_CURSOR,
    SPR_ID_LOCKED
};

enum : uint8_t {
    SEGMENT_COLUMN = 0,
    SEGMENT_ROW,
    SEGMENT_SECTION,
    SEGMENT_MAX
};

/*  Typedefs  */

typedef struct {
    uint8_t number : 4;
    uint8_t locked : 1;
} CELL_T;

/*  Macro functions  */

#define getUnitIndex(x, y)  ((y) / SECTION_SIZE * SECTION_SIZE + (x) / SECTION_SIZE)

/*  Local Functions  */

static void     initPuzzle(void);
static void     resetPuzzle(void);
static void     placeNumber(uint8_t x, uint8_t y, uint8_t n);
static void     removeNumber(uint8_t x, uint8_t y);
static void     updateState(uint8_t x, uint8_t y, uint8_t n, bool isPlace);
static int32_t  powDigits(uint8_t n);

static void     setStateSprite(void);
static void     setToolSprite(void);
static void     setCursorSprite(void);
static void     setSelectingSprite(void);

/*  Local Variables  */

static CELL_T   board[BOARD_SIZE][BOARD_SIZE];
static int32_t  boardState[SEGMENT_MAX][BOARD_SIZE], boardFailure;
static uint8_t  selectedNumber = DEFAULT_NUMBER, cursorX = DEFAULT_POS, cursorY = DEFAULT_POS;
static uint8_t  placedCount, presetCount;
static bool     isSelecting;

/*---------------------------------------------------------------------------*/
/*                               Main Functions                              */
/*---------------------------------------------------------------------------*/

void initGame(void)
{
    initPuzzle();
    isSelecting = false;
    counter = LONG_PRESS;

    setToolSprite();
    setCursorSprite();
    setString(1, 93, (const __FlashStringHelper *)&levelString[level * 7],WHITE);

    playScore(soundStart);
    isInvalid = true;
}

MODE_T updateGame(void)
{
    /*  Buttons handling  */
    handleDpad();
    bool isShortPress = false;
    if (isButtonPressed(A_BUTTON)) {
        /*  Reset puzzle  */
        if (counter < LONG_PRESS && ++counter == LONG_PRESS && placedCount > presetCount) {
            clearSprite(SPR_ID_STATE);
            initGame();
        }
    } else {
        if (counter > 0 && counter < SHORT_PRESS) isShortPress = true;
        counter = 0;
    }

    if (isSelecting) {
        /*  Select tool  */
        if (dpadX != 0) {
            selectedNumber = circulate(selectedNumber, dpadX, DIGITS);
            setToolSprite();
            playSoundTick();
            isInvalid = true;
        }
        /*  Exit selecting mode  */
        if (isButtonDown(A_BUTTON)) {
            isSelecting = false;
            counter = LONG_PRESS;
            playSoundClick();
            isInvalid = true;
        }
    } else {
        /*  Move cursor  */
        if (dpadX != 0 || dpadY != 0) {
            cursorX = circulate(cursorX, dpadX, BOARD_SIZE);
            cursorY = circulate(cursorY, dpadY, BOARD_SIZE);
            playSoundTick();
            isInvalid = true;
        }
        /*  Place or remove number  */
        if (isShortPress) {
            if (board[cursorY][cursorX].locked) {
                playSoundClick();
            } else {
                uint8_t n = board[cursorY][cursorX].number;
                if (selectedNumber > 0 && n != selectedNumber) {
                    playScore(soundPlace);
                    placeNumber(cursorX, cursorY, selectedNumber);
                } else if (selectedNumber == 0 && n > 0 ||
                        selectedNumber > 0 && n == selectedNumber) {
                    playScore(soundRemove);
                    removeNumber(cursorX, cursorY);
                }
                const uint8_t *pImg = NULL;
                if (boardFailure != 0) {
                    pImg = imgState[1];
                } else if (placedCount == BOARD_SIZE * BOARD_SIZE) {
                    pImg = imgState[0];
                    playScore(soundComplete);
                }
                setSprite(SPR_ID_STATE, 8, 24, pImg, IMG_STATE_W, IMG_STATE_H, DIRECT);
                isInvalid = true;
            }
        }
        /*  Enter selecting mode  */
        if (counter == SHORT_PRESS) {
            isSelecting = true;
            setSelectingSprite();
            playSoundClick();
            isInvalid = true;
        }
    }
    if (!isSelecting) setCursorSprite();

    return MODE_GAME;
}

void drawGame(int16_t y, uint8_t *pBuffer)
{
    clearScreenBuffer();

    /*  Grid  */
    int8_t b = 7 - (y + 7) % SECTION_PX;
    if (b >= 0) {
        memset(pBuffer + BOARD_LEFT + 1, bit(b), BOARD_PX - 1);
    }
    for (uint8_t x = BOARD_LEFT; x <= BOARD_LEFT + BOARD_PX; x += SECTION_PX) {
        *(pBuffer + x) = 0xFF;
    }

    /*  Digits  */
    uint8_t by = y >> 3;
    for (uint8_t bx = 0; bx < BOARD_SIZE; bx++) {
        uint8_t n1 = board[by][bx].number;
        uint8_t n2 = board[by + 1][bx].number;
        uint8_t *p = pBuffer + BOARD_LEFT + 2 + bx * CELL_PX;
        const uint8_t *pImg1 = imgDigit[n1];
        const uint8_t *pImg2 = imgDigit[n2];
        for (uint8_t w = 0; w < IMG_DIGIT_W; w++) {
            *p++ |= pgm_read_byte(pImg1++) >> by | pgm_read_byte(pImg2++) << (CELL_PX - by);
        }
    }
}

/*---------------------------------------------------------------------------*/
/*                             Control Functions                             */
/*---------------------------------------------------------------------------*/

static void initPuzzle(void)
{
    /*  Generate shuffle table  */
    randomSeed(puzzleSeed);
    uint8_t table[SEGMENT_MAX][BOARD_SIZE];
    uint8_t *pTable = (uint8_t *)table;
    for (uint8_t segment = 0; segment < SEGMENT_MAX; segment++) {
        uint8_t n1 = random(SECTIONS) + SECTIONS;
        int8_t v1 = random(2) * 2 - 1;
        for (uint8_t j = 0; j < SECTIONS; j++, n1 += v1) {
            uint8_t n2 = random(SECTION_SIZE) + SECTION_SIZE;
            int8_t v2 = random(2) * 2 - 1;
            for (uint8_t k = 0; k < SECTION_SIZE; k++, n2 += v2) {
                *pTable++ = n1 % SECTIONS * SECTION_SIZE + n2 % SECTION_SIZE;
            }
        }
    }

    /*  Place preset numbers  */
    memset(board, 0, sizeof(board));
    memset(boardState, 0, sizeof(boardState));
    placedCount = 0;
    boardFailure = 0;
    const int32_t *pData = puzzleData[random(PUZZLES_PER_LEVEL) + level * PUZZLES_PER_LEVEL];
    for (uint8_t y = 0; y < BOARD_SIZE; y++) {
        int32_t rowData = pgm_read_dword(pData++);
        for (uint8_t x = 0; x < BOARD_SIZE; x++) {
            uint8_t n = rowData % DIGITS;
            if (n > 0) {
                uint8_t bx = table[SEGMENT_COLUMN][x], by = table[SEGMENT_ROW][y];
                placeNumber(bx, by, table[SEGMENT_SECTION][n - 1] + 1);
                board[by][bx].locked = true;
            }
            rowData /= (int32_t)DIGITS;
        }
    }
    presetCount = placedCount;
}

static void placeNumber(uint8_t x, uint8_t y, uint8_t n)
{
    removeNumber(x, y);
    board[y][x].number = n;
    placedCount++;
    updateState(x, y, n, true);
}

static void removeNumber(uint8_t x, uint8_t y)
{
    uint8_t n = board[y][x].number;
    if (n == 0) return;
    board[y][x].number = 0;
    placedCount--;
    updateState(x, y, n, false);
}

static void updateState(uint8_t x, uint8_t y, uint8_t n, bool isPlace)
{
    int32_t q = powDigits(n);
    for (uint8_t segment = 0; segment < SEGMENT_MAX; segment++) {
        uint8_t idx = (segment == SEGMENT_SECTION) ?
                getUnitIndex(x, y) : ((segment == SEGMENT_ROW) ? y : x);
        boardState[segment][idx] += (isPlace) ? q : -q;
        uint8_t a = boardState[segment][idx] / q % DIGITS;
        uint8_t b = segment * BOARD_SIZE + idx;
        if (isPlace && a >= 2) {
            playScore(soundFailure);
            bitSet(boardFailure, b);
        } else if (!isPlace && a <= 1) {
            bitClear(boardFailure, b);
        }
    }
}

static int32_t powDigits(uint8_t n)
{
    int32_t q = 1;
    while (--n > 0) {
        q *= (int32_t)DIGITS;
    }
    return q;
}

/*---------------------------------------------------------------------------*/

static void setToolSprite(void)
{
    setSprite(SPR_ID_TOOL, 104, 24, imgTool[!selectedNumber], IMG_TOOL_W, IMG_TOOL_H, DIRECT);
    setSprite(SPR_ID_NUMBER, 108, 25, imgDigit[selectedNumber], IMG_DIGIT_W, IMG_DIGIT_H, WHITE);
}

static void setCursorSprite(void)
{
    setSprite(SPR_ID_CURSOR, BOARD_LEFT + cursorX * CELL_PX, cursorY * CELL_PX, imgCursor,
            IMG_CURSOR_W, IMG_CURSOR_H, WHITE);
    setSprite(SPR_ID_LOCKED, 106, 40, (board[cursorY][cursorX].locked) ? imgLocked : NULL,
            IMG_LOCKED_W, IMG_LOCKED_H, DIRECT);
}

static void setSelectingSprite(void)
{
    setSprite(SPR_ID_CURSOR, 100, 24, imgSelecting, IMG_SELECTING_W, IMG_SELECTING_H, WHITE);
}
