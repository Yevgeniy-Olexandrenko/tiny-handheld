
#include "ssd1306.h"
#include "intf/i2c/ssd1306_i2c.h"
#include "intf/ssd1306_interface.h"
#include <avr/eeprom.h>

// Defines for OLED output
#define SSD1306_SCL PORTB2
#define SSD1306_SDA PORTB0
#define SSD1306_SA  0x78

// Defines for SOUND output
#define SOUND_PIN PORTB4

// Input for horizontal screen orientation
bool IsLeft()   { return (analogRead(A0) > 750) && (analogRead(A0) < 950); }
bool IsRight()  { return (analogRead(A0) > 500) && (analogRead(A0) < 750); }
bool IsDown()   { return (analogRead(A3) > 750) && (analogRead(A3) < 950); }
bool IsUp()     { return (analogRead(A3) > 500) && (analogRead(A3) < 750); }
bool IsAction() { return bitRead(PINB, PB1) == LOW; }
bool IsCenter() { return bitRead(PINB, PB3) == LOW; }

// Routines to set and clear bits (used in the sleep code)
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))

// General
#define LEN(d) (sizeof(d) / sizeof(d[0]))

// Engine options
#define INVERTED 0
#define FRAME_DURATION 14

#define WIDTH 128
#define HEIGHT 64

#define NORM_VALUE 10
#define MULTIPLIER_VALUE 11

// Gameplay options
#define DELAY_TICKS_GAME_OVER (500 / FRAME_DURATION)
#define MAX_LEVEL 20

#define GROUND_ROW 7

#define DINO_X (WIDTH / 5)
#define DINO_Y (HEIGHT - 16)
#define DINO_JUMP_VELOCITY 500
#define DINO_ACCEL 50
#define DINO_CHEAT_SIZE 2
#define DINO_REPLACE_SPRITE_TICKS 16

#define CACTUS_Y DINO_Y
#define CACTUSES_COUNT 6
#define CACTUS_VELOCITY 15

// For sampling from exponential distribution
#define EXPONENTIAL_RATE 8
#define EXP10 22026

enum gameState {
  gameOver,
  playing,
  startScreen
};

typedef struct {
  uint8_t x, y, width, velocity, delayTicks, nextUpdateAt;
} rockMeta;

typedef struct {
  SPRITE sprite;
  uint16_t x, velocity;
  uint8_t nextAppearAt, startDelay;
} cactusMeta;

typedef struct {
  SPRITE sprite;
  int16_t velocity;
  uint8_t isJumping, isLanded;
} dinoMeta;


const uint16_t exp_values[] = {3, 7, 20, 55, 148, 403, 1097, 2981, 8103, 22026};
const PROGMEM uint8_t dino_images[2][9] = {
  {
    0B00011000,
    0B00110000,
    0B11111000,
    0B01111100,
    0B01111111,
    0B10011101,
    0B00100111,
    0B00000111,
    0B00000011
  }, {
    0B00011000,
    0B00110000,
    0B11111000,
    0B01111100,
    0B11111111,
    0B00011101,
    0B00010111,
    0B00000111,
    0B00000011
  }
};

const PROGMEM uint8_t cactus_image[] = {
  0B00000111,
  0B00001111,
  0B00011000,
  0B11111110,
  0B11111111,
  0B00011000,
  0B00011000,
  0B00001100
};

uint8_t lastTimeStamp, tickNum, levelNum, delayToGameOver;
uint32_t maxScore, score, gameSpeed;
enum gameState state;

rockMeta rocks[3];
cactusMeta cactuses[CACTUSES_COUNT];
dinoMeta dino;


void setup() {
  randomSeed(analogRead(0)); // TODO
  ssd1306_setFixedFont(ssd1306xled_font5x7);

  sbi(DDRB, SOUND_PIN); // output
  cbi(DDRB, PB1);       // input
  cbi(DDRB, A0);        // input
  cbi(DDRB, A3);        // input
  
  ssd1306_128x64_i2c_initEx(SSD1306_SCL, SSD1306_SDA, 0);
  if (INVERTED) ssd1306_invertMode();
  ssd1306_clearScreen();

  loadMaxScore();
  state = startScreen;
}


void resetGame() {
  lastTimeStamp = levelNum = tickNum = score = 0;
  gameSpeed = 10;

  dino.sprite = ssd1306_createSprite(DINO_X, DINO_Y, sizeof(dino_images[0]), dino_images[0]);
  dino.velocity = dino.isJumping = 0;
  dino.sprite.x = DINO_X;
  dino.sprite.y = DINO_Y;
  dino.isLanded = 1;

  rocks[0] = {WIDTH - 1, 1, 7, 3, 120, 50};
  rocks[1] = {WIDTH - 1, 3, 4, 2, 100, 20};
  rocks[2] = {WIDTH - 1, 6, 3, 1, 30,  10};

  for (int i = 0; i < LEN(cactuses); i++) {
    cactuses[i].sprite.eraseTrace();
    cactuses[i].sprite = ssd1306_createSprite(WIDTH - 1, CACTUS_Y, LEN(cactus_image), cactus_image);
    cactuses[i].x = (WIDTH - 1) * NORM_VALUE;
    cactuses[i].velocity = CACTUS_VELOCITY;
    cactuses[i].nextAppearAt = generateNextAppearAt();
    cactuses[i].startDelay = i == 0;
  }

  ssd1306_clearScreen();
}


void updateMaxScore(uint32_t s) {
  maxScore = s;
  eeprom_update_dword(0, s);
}


void loadMaxScore() {
  maxScore = eeprom_read_dword(0);
  if (maxScore == uint32_t(-1)) {
    updateMaxScore(0);
  }
}

/* Update functions */

void updateRocks() {
  for (int i = 0; i < LEN(rocks); i++) {
    if (rocks[i].nextUpdateAt) {
      rocks[i].nextUpdateAt--;
      continue;
    };

    if (rocks[i].x < rocks[i].velocity) {
      rocks[i].nextUpdateAt = rocks[i].delayTicks;
      rocks[i].x = WIDTH - rocks[i].width;
    } else {
      rocks[i].x -= rocks[i].velocity;
    }
  }
}


uint8_t sampleFromExpDistribution(uint8_t r) {
  uint16_t value;
  uint8_t lg;

  value = EXP10 - random(0, EXP10);

  for (int i = 0; i < LEN(exp_values); i++) {
    if (value < exp_values[i]) {
      lg = i;
      break;
    }
  }

  lg = -(lg - 10) * 8;

  return lg;
  
}


uint16_t generateNextAppearAt() {
  uint8_t lg, s;

  lg = sampleFromExpDistribution(EXPONENTIAL_RATE);
  s = gameSpeed / NORM_VALUE;

  return DINO_JUMP_VELOCITY * 2 * s / DINO_ACCEL + lg;
}


void updateCactuses() {
  for (int i = 0; i < LEN(cactuses); i++) {
    if (!cactuses[i].startDelay) continue;

    if (cactuses[i].nextAppearAt) {
      cactuses[i].nextAppearAt--;
      continue;
    }
    cactuses[(i + 1) % LEN(cactuses)].startDelay = 1;

    if (cactuses[i].x <= cactuses[i].velocity) {
      cactuses[i].nextAppearAt = generateNextAppearAt();
      cactuses[i].x = (WIDTH - cactuses[i].sprite.w) * NORM_VALUE;
      cactuses[i].sprite.x = WIDTH - 1;
      cactuses[i].startDelay = 0;
    } else {
      cactuses[i].x -= cactuses[i].velocity;
      cactuses[i].sprite.x = cactuses[i].x / NORM_VALUE;
    }
  }
}


uint8_t isOverlapping(uint8_t x1, uint8_t x2, uint8_t y1, uint8_t y2) {
  return max(x1, y1) <= min(x2, y2);
}


uint8_t isCollided() {
  uint8_t dinoStart, dinoEnd, cactusStart, cactusEnd;

  if (dino.sprite.y < DINO_Y - (LEN(cactus_image) + 1)) return 0;
  for (int i = 0; i < LEN(cactuses); i++) {
    if (cactuses[i].nextAppearAt) continue;

    dinoStart = dino.sprite.x;
    dinoEnd = dinoStart + LEN(dino_images[0]);
    cactusStart = cactuses[i].sprite.x + DINO_CHEAT_SIZE;
    cactusEnd = cactusStart + LEN(cactus_image) - DINO_CHEAT_SIZE;
    if (isOverlapping(dinoStart, dinoEnd, cactusStart, cactusEnd)) return 1;
  }

  return 0;
}


void updateDino() {
  uint8_t spriteNum;

  if (dino.isJumping) {
    dino.velocity = DINO_JUMP_VELOCITY;
    dino.isJumping = 0;
  }

  spriteNum = dino.isLanded * ((tickNum & (DINO_REPLACE_SPRITE_TICKS - 1)) > DINO_REPLACE_SPRITE_TICKS >> 1);
  ssd1306_replaceSprite(&dino.sprite, dino_images[spriteNum]);

  dino.sprite.y -= dino.velocity / (NORM_VALUE * NORM_VALUE);
  dino.velocity -= DINO_ACCEL;

  if (dino.sprite.y >= DINO_Y) {
    dino.sprite.y = DINO_Y;
    dino.velocity = 0;
    dino.isLanded = 1;
  }
}


void updateGameState() {
  uint8_t currentLevel;
  if (tickNum % 5 == 0) score += gameSpeed;

  currentLevel = score / (NORM_VALUE * 100);
  if (levelNum < MAX_LEVEL && currentLevel > levelNum) {
    for (int i = 0; i < LEN(rocks); i++) {
      rocks[i].velocity *= MULTIPLIER_VALUE;
      rocks[i].velocity /= NORM_VALUE;
      rocks[i].delayTicks *= NORM_VALUE;
      rocks[i].delayTicks /= MULTIPLIER_VALUE;
    }

    for (int i = 0; i < LEN(cactuses); i++) {
      cactuses[i].velocity *= MULTIPLIER_VALUE;
      cactuses[i].velocity /= NORM_VALUE;
    }
    gameSpeed *= MULTIPLIER_VALUE;
    gameSpeed /= NORM_VALUE;
    levelNum++;
  }
}


void updateState() {
  uint32_t scoreNorm;
  int buttonState;

//  buttonState = digitalRead(RESET_PIN);
//  if (!buttonState) {
//    updateMaxScore(0);
//    return;
//  }

  buttonState = IsUp() || IsAction() || IsCenter();

  if (state == gameOver || state == startScreen) {
    delayToGameOver -= (state == gameOver && delayToGameOver > 0);
    if (buttonState && (state != gameOver || !delayToGameOver)) {
        state = playing;
        resetGame();
    }
    return;
  }

  if (buttonState && dino.isLanded) {
    dino.isJumping = 1;
    dino.isLanded = 0;
  }

  updateRocks();
  updateDino();
  updateCactuses();
  updateGameState();

  if (isCollided()) {
    state = gameOver;
    scoreNorm = score / NORM_VALUE;
    if (scoreNorm > maxScore) updateMaxScore(scoreNorm);
    delayToGameOver = DELAY_TICKS_GAME_OVER;
  }
}

/* Draw functions */

void drawGround() {
  uint8_t bits;

  ssd1306_lcd.set_block(0, GROUND_ROW, WIDTH);
  for (int i = 0; i < WIDTH; i++) {
    bits = 0b00000001;
    for (int r = 0; r < LEN(rocks); r++) {
      if (rocks[r].nextUpdateAt) continue;
      if (i > rocks[r].x && i < rocks[r].x + rocks[r].width) {
        bits |= 1 << (8 - rocks[r].y);
      }
    }
    ssd1306_lcd.send_pixels1(bits);
  }
  ssd1306_intf.stop();
}


void drawDino() {
  dino.sprite.eraseTrace();
  dino.sprite.draw();
}


void drawCactuses() {
  for (int i = 0; i < LEN(cactuses); i++) {
    cactuses[i].sprite.eraseTrace();
    if (cactuses[i].nextAppearAt) continue;
    cactuses[i].sprite.draw();
  }
}


void drawStatus() {
  uint8_t chNums;
  char scoreBuff[32] = "score: ";
  uint32_t scoreNorm = score / NORM_VALUE;
  uint32_t scoreNorm_ = scoreNorm;

  ultoa(scoreNorm, scoreBuff + 7, 10);
  chNums = 0;
  do {
    chNums++;
  } while (scoreNorm_ /= 10);

  scoreBuff[7 + chNums] = '\\';
  ultoa(max(maxScore, scoreNorm), scoreBuff + 7 + 1 + chNums, 10);
  
  ssd1306_printFixed(1,  1, scoreBuff, STYLE_NORMAL);
}


void drawGameOver() {
  ssd1306_printFixed(32,  25, "Game over :(", STYLE_BOLD);
}


void drawStartScreen() {
  ssd1306_printFixed(52,  25, "Start", STYLE_BOLD);
}


void draw() {
  switch (state) {
    case playing:
      drawGround();
      drawDino();
      drawStatus();
      drawCactuses();
      break;
    case gameOver:
      drawGameOver();
      break;
    case startScreen:
      drawStartScreen();
      break;
  }
}


void loop() {
  uint8_t t, delta_t;

  t = millis();
  delta_t = t < lastTimeStamp ? t + (0xFF - lastTimeStamp) : t - lastTimeStamp;
  if (delta_t > FRAME_DURATION) {
    tickNum++;
    lastTimeStamp += FRAME_DURATION;

    updateState();
    draw();
  }
}
