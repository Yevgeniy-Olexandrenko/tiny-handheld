#include "common.h"
#include "data.h"

/*  Defines  */

#define COLUMNS         18
#define COLUMN_W        8
#define CAVE_WIDTH      (COLUMNS * COLUMN_W)
#define CAVE_BOTTOM_MIN 16
#define CAVE_BOTTOM_MAX 56
#define CAVE_BOTTOM_ADJ 34
#define CAVE_GAPMAX_MIN 32
#define CAVE_GAPMAX_MAX 255
#define CAVE_PHASE_MAX  256

#define PLAYER_X_MAX    56
#define PLAYER_W        8
#define PLAYER_H        8
#define PLAYER_SPEED    2

enum STATE_T : uint8_t {
    STATE_START = 0,
    STATE_PLAYING,
    STATE_OVER,
};

/*  Typedefs  */

typedef struct {
    uint8_t     top;
    uint8_t     bottom;
} COLUMN_T;

/*  Macro functions  */

#define divByColumnW(val)   ((val) >> 3)    // ((val) / COLUMN_W)
#define modByColumnW(val)   ((val) & 7)     // ((val) % COLUMN_W)
#define getColumnOfPos(pos) (divByColumnW(pos) % COLUMNS)

/*  Local Functions  */

static void     initCave(void);
static void     growCave(void);
static void     updateCave(void);
static void     initPlayer(void);
static uint8_t  updatePlayer(void);

/*  Global Variables  */

uint16_t score = 0;

/*  Local Variables  */

static COLUMN_T caveColumn[COLUMNS];
static STATE_T  state;
static uint8_t  playerX, cavePhase, caveOffset, caveGapMax, caveGapCurrent;
static int8_t   playerMove, playerJump, caveHollowDistance, caveBaseTop, caveBaseBottom;
static bool     isPlayerRight;

/*---------------------------------------------------------------------------*/
/*                               Main Functions                              */
/*---------------------------------------------------------------------------*/

void initGame(void)
{
    score = 0;
    initCave();
    initPlayer();
    initSprites();
    setSprite(1, (WIDTH - IMG_READY_W) / 2, 8, imgReady, IMG_READY_W, IMG_READY_H, DIRECT);
    playScore(soundStart);
    state = STATE_START;
    counter = FPS * 2;
    //isInvalid = true;
}

MODE_T updateGame(void)
{
    MODE_T ret = MODE_GAME;

    updateCave();
    uint8_t diff = updatePlayer();

    switch (state) {
    case STATE_START:
        if (--counter == 0) {
            clearSprite(1);
            updateScore();
            state = STATE_PLAYING;
        }
        break;
    case STATE_PLAYING:
        if (cavePhase++ == 0) { // cavePhase = (cavePhase + 1) % CAVE_PHASE_MAX
            if (diff < PLAYER_H / 2) {
                setSprite(1, (WIDTH - IMG_GAMEOVER_W) / 2, HEIGHT - IMG_GAMEOVER_H, imgGameOver,
                        IMG_GAMEOVER_W, IMG_GAMEOVER_H, DIRECT);
                playScore(soundOver);
                state = STATE_OVER;
                counter = FPS * 8;
            }
            if (caveGapMax < CAVE_GAPMAX_MAX) caveGapMax++;
        }
        break;
    case STATE_OVER:
        cavePhase += counter & 1; // cavePhase = (cavePhase + (counter & 1)) % CAVE_PHASE_MAX
        if (isButtonDown(A_BUTTON)) {
            initGame();
        } else if (--counter == 0) {
            ret = MODE_TITLE;
        }
        break;
    }

    isInvalid = true;
    return ret;
}

void drawGame(int16_t y, uint8_t *pBuffer)
{
    uint8_t col = divByColumnW(caveOffset);
    uint8_t odd = modByColumnW(caveOffset);
    uint8_t shiftTop = caveBaseTop & 7, shiftBottom = caveBaseBottom & 7;
    int8_t  caveTop, caveBottom;
    uint8_t maskTop, maskBottom;

    for (uint8_t x = 0, *pDest = pBuffer; x < WIDTH; x++, pDest++) {
        if (x == 0 || odd == 0) {
            caveTop = caveColumn[col].top + caveBaseTop - y;
            caveBottom = caveColumn[col].bottom + caveBaseBottom - y;
            maskTop = (caveTop < 8) ? 0xFF >> (8 - caveTop) : 0xFF;
            maskBottom = (caveBottom > 0) ? 0xFF << caveBottom : 0xFF;
        }
        COLUMN_T *pNeighbor;
        if (odd == 0 || odd == 7) {
            pNeighbor = &caveColumn[circulate(col, (odd == 0) ? -1 : 1, COLUMNS)];
        } else {
            pNeighbor = NULL;
        }
        uint8_t ptn = pgm_read_byte(&imgCave[odd]);

        /*  Top  */
        if (caveTop <= 0) {
            *pDest = 0;
        } else {
            int8_t edgeTop;
            if (pNeighbor) {
                edgeTop = pNeighbor->top + caveBaseTop - y;
                edgeTop = min(edgeTop, caveTop) - 1;
            } else {
                edgeTop = caveTop - 1;
            }
            uint8_t edgeTopPtn = (edgeTop > 0) ? 0xFF << edgeTop : 0xFF;
            *pDest = (ptn << shiftTop | ptn >> (8 - shiftTop) | edgeTopPtn) & maskTop;
        }

        /*  Bottom  */
        if (caveBottom < 8) {
            int8_t edgeBottom;
            if (pNeighbor) {
                edgeBottom = pNeighbor->bottom + caveBaseBottom - y;
                edgeBottom = max(edgeBottom, caveBottom) + 1;
            } else {
                edgeBottom = caveBottom + 1;
            }
            uint8_t edgeBottomPtn = (edgeBottom < 8) ? 0xFF >> (8 - edgeBottom) : 0xFF;
            *pDest |= (ptn << shiftBottom | ptn >> (8 - shiftBottom) | edgeBottomPtn) & maskBottom;
        }
        if (++odd == COLUMN_W) {
            col = (col + 1) % COLUMNS;
            odd = 0;
        }
    }
}

/*---------------------------------------------------------------------------*/
/*                             Control Functions                             */
/*---------------------------------------------------------------------------*/

static void initCave(void)
{
    caveOffset = 0;
    for (COLUMN_T *pColumn = caveColumn;
            pColumn <= &caveColumn[divByColumnW(PLAYER_X_MAX + COLUMN_W)];
            pColumn++) {
        pColumn->top = HEIGHT / 2;
        pColumn->bottom = HEIGHT / 2 + PLAYER_H;
    }
    caveHollowDistance = 2;
    for (caveOffset = PLAYER_X_MAX + COLUMN_W * 3;
            caveOffset <= WIDTH + COLUMN_W * 2;
            caveOffset += COLUMN_W) {
        growCave();
    }
    caveOffset = COLUMN_W;
    cavePhase = 0;
    caveGapMax = CAVE_GAPMAX_MIN;
}

static void growCave(void)
{
    uint8_t col = getColumnOfPos(caveOffset + WIDTH + COLUMN_W);
    COLUMN_T *pNewColumn = &caveColumn[col];
    COLUMN_T *pLastColumn = &caveColumn[circulate(col, -1, COLUMNS)];
    int8_t lastDiff = pLastColumn->bottom - pLastColumn->top;
    int8_t newDiff = random(PLAYER_H / 2 - 1);

    if (--caveHollowDistance <= 0) {
        newDiff = PLAYER_H - newDiff;
        caveHollowDistance = ((rand() + 32768) * score >> 22) + 2;
    }
    int8_t adjust = (pLastColumn->bottom - CAVE_BOTTOM_ADJ) / 5;
    int8_t change = random(PLAYER_H * 2 + 1 - abs(newDiff - lastDiff) - abs(adjust)) - PLAYER_H;
    if (newDiff > lastDiff) change += newDiff - lastDiff;
    if (adjust < 0) change -= adjust;

    pNewColumn->bottom = pLastColumn->bottom + change;
    pNewColumn->bottom = constrain(pNewColumn->bottom, CAVE_BOTTOM_MIN, CAVE_BOTTOM_MAX);
    pNewColumn->top = pNewColumn->bottom - newDiff;
}

static void updateCave(void)
{
    caveGapCurrent = (1.0 - cos(cavePhase * TWO_PI / (double)CAVE_PHASE_MAX)) * caveGapMax / 2;
    caveBaseTop = -(caveGapCurrent + 1) / 2;
    caveBaseBottom = caveGapCurrent / 2;
    if (cavePhase >= CAVE_PHASE_MAX - 12) {
        playTone(random(16) + 16, 50);
        if (cavePhase & 1) {
            caveBaseTop++;
            caveBaseBottom++;
        }
    }
}

/*---------------------------------------------------------------------------*/

static void initPlayer(void)
{
    playerX = 0;
    playerMove = 0;
    isPlayerRight = true;
}

static uint8_t updatePlayer(void)
{
    uint8_t col = getColumnOfPos(playerX + playerMove * isPlayerRight + caveOffset);
    COLUMN_T *pPlayerColumn = &caveColumn[col];

    /*  Button handling  */
    if (playerMove == 0) {
        int8_t vx;
        if (state == STATE_PLAYING) {
            vx = isButtonPressed(RIGHT_BUTTON) - isButtonPressed(LEFT_BUTTON);
        } else {
            vx = (state == STATE_START);
        }
        if (vx != 0) {
            isPlayerRight = (vx > 0);
            COLUMN_T *pNextColumn = &caveColumn[circulate(col, vx, COLUMNS)];
            int8_t nextGap = min(pPlayerColumn->bottom, pNextColumn->bottom)
                    - max(pPlayerColumn->top, pNextColumn->top);
            if (playerX + vx >= 0 && nextGap + caveGapCurrent >= PLAYER_H) {
                playerJump = pPlayerColumn->bottom - pNextColumn->bottom;
                playerMove = COLUMN_W;
                pPlayerColumn = pNextColumn;
                if (state == STATE_PLAYING) playTone(660, 20);
            }
        }
    }

    /*  Move and scroll  */
    if (playerMove > 0) {
        if (isPlayerRight && playerX == PLAYER_X_MAX) {
            if (playerMove == COLUMN_W) {
                growCave();
                score++;
                updateScore();
            }
            caveOffset = (caveOffset + PLAYER_SPEED) % CAVE_WIDTH;
        } else {
            playerX += (isPlayerRight) ? PLAYER_SPEED : -PLAYER_SPEED;
        }
        playerMove -= PLAYER_SPEED;
    }

    /*  Player's position  */
    int16_t playerY;
    if (state == STATE_OVER) {
        playerY = pPlayerColumn->top + caveBaseBottom;
    } else {
        playerY = pPlayerColumn->bottom + caveBaseBottom - PLAYER_H
                + divByColumnW(playerJump * playerMove);
        playerY = min(playerY, HEIGHT - PLAYER_H);
        playerY = max(playerY, pPlayerColumn->top + caveBaseTop);
    }
    setSprite(0, playerX, playerY, imgPlayer[isPlayerRight * 4 + playerMove / 2],
            PLAYER_W, PLAYER_H, WHITE);

    return pPlayerColumn->bottom - pPlayerColumn->top;
}

void updateScore(void)
{
    int8_t pos = (score >= 10) + (score >= 100) + (score >= 1000);
    for (uint16_t val = score; pos >= 0; pos--, val /= 10) {
        setSprite(pos + 2, pos * IMG_FIGURE_W, 0, imgFigure[val % 10],
                IMG_FIGURE_W, IMG_FIGURE_H, DIRECT);
    }
}
