/*
 * 2048 game for the HIDIOT
 * by Saumil Shah @therealsaumil
 *
 * 5 October 2017
 * 
 * VCC -- 4.7k --+-- 10k --+-- 10k --+-- 10k --+-- 47k --+-- GND
 *               |         |         |         |         |
 *                / S1      / S2      / S3      / S4      / RESET
 *               |         |         |         |         |
 *     +---------+---------+---------+---------+---------+ 
 *     |                                                 S1 = Left
 *     |          +----v----+                            S2 = Down
 *     +----- PB5 |1  AT   8| VCC                        S3 = Up
 *         D+ PB3 |2 Tiny  7| PB2 -----------------+     S4 = Right
 *         D- PB4 |3  85   6| PB1 (LED1)           |
 *            GND |4       5| PB0 (S1) --------+   |
 *                +---------+                  |   |
 *                                    VCC GND SDA SCL
 *                                     |   |   |   |
 *                                     |   |   |   |
 *                                  +-----------------+
 *                                  | SSD1306 OLED    |
 *                                  |     128 x 64    |
 *                                  +-----------------+
 * 
 * Use PB5 as Analogue Input.
 * 
 * TODO:
 * detect deadlock: no more moves possible
 * display the score somehow
 *
 * Thanks:
 * Udayan Shah
 * Steve Lord   @RawHex
 */

#include "CompactDigisparkOLED.h"

const int WINNING PROGMEM = 2048;

const byte START_ROW PROGMEM = 0;
const byte START_COL PROGMEM = 0;

const char LEFT   PROGMEM = -1;
const char UP     PROGMEM = -1;
const char RIGHT  PROGMEM = +1;
const char DOWN   PROGMEM = +1;

#define S_LEFT    1
#define S_UP      2
#define S_DOWN    3
#define S_RIGHT   4

//#define DEBUGMODE

const byte redLED    PROGMEM = 1;
const byte PBFIVE    PROGMEM = 5;      // pinMode 5, analogRead 0
const int read_delay PROGMEM = 10;     // 10ms
const int debounce   PROGMEM = 5;      // 5ms debounce

// Global variables
boolean input_present = false;
boolean input_processed = false;
unsigned int grid[4][4];
unsigned int score;

// Input for horizontal screen orientation
bool IsLeft()   { return (analogRead(A0) > 750) && (analogRead(A0) < 950); }
bool IsRight()  { return (analogRead(A0) > 500) && (analogRead(A0) < 750); }
bool IsDown()   { return (analogRead(A3) > 750) && (analogRead(A3) < 950); }
bool IsUp()     { return (analogRead(A3) > 500) && (analogRead(A3) < 750); }
bool IsAction() { return bitRead(PINB, PB1) == LOW; }
bool IsCenter() { return bitRead(PINB, PB3) == LOW; }

bool HasDirectionInput()
{
   return IsLeft() || IsRight() || IsUp() || IsDown();
}

bool HasInput()
{
  return HasDirectionInput() || IsAction();
}

// Routines to set and clear bits (used in the sleep code)
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

void setup()
{
   cbi(DDRB, PB1);       // input
   cbi(DDRB, A0);        // input
   cbi(DDRB, A3);        // input
  
   // initialize OLED display
   oled.begin();

   moveto(0, 0);
   oled.print(F(
      "2048 - BY SAUMIL SHAH\n\n"
      "Merge adjoining cells\n"
      "having the same value\n"
      "using the 4 buttons\n"
      "LEFT, DOWN, UP, RIGHT\n"
      "GOAL: Reach 2048\n"
      "               READY?"
   ));

   // press any button to start, and seed RNG
   press_start();

   draw_borders();

   // set the first two cells
   init_grid();

   print_grid();

   // loop() is next
}

// input loop, keep running until RESET is pressed,
// in which case the program ends anyway
void loop()
{
   // check if there is an input present
   // and toggle the input_present flag
   // accordingly
   if(HasDirectionInput()) {
      delay(debounce);
      if(HasDirectionInput()) {
         input_present = true;
      }
   }
   else {
      if(input_present) {
         // input has gone above threshold
         // which means that there's no
         // connection. reset max and min
         input_present = false;
         input_processed = false;
      }
   }

   if(input_present && !input_processed) {
      processInput();
   }

   delay(read_delay);
}

// Press any button to start
// We shall also initialize the random number
// seed based on the time delay from RESET until
// any button is pressed.
void press_start()
{
   while(true) {
      if(HasInput()) {
         delay(debounce);
         if(HasInput()) {
            break;
         }
      }
   }

   randomSeed(millis());

   delay(100);
   oled.clear();
}

// What to do when a button is pressed
void processInput()
{
   byte button;
   boolean grid_modified = false;

   input_processed = true;
   button = identify_button_pressed();

   switch(button) {
      case S_LEFT:
         grid_modified = sum_all_rows(LEFT);
         break;
      case S_RIGHT:
         grid_modified = sum_all_rows(RIGHT);
         break;
      case S_UP:
         grid_modified = sum_all_columns(UP);
         break;
      case S_DOWN:
         grid_modified = sum_all_columns(DOWN);
         break;
   }
   print_grid();
   // sometimes we get an erroneous button press
   // which results in button code = 0
   //
   // add a new number to the grid only if
   // the grid is modified and a valid button
   // is pressed
   if(grid_modified && button != 0) {
      plot_two_or_four();
      // remove the next two lines
      // if we don't want a delay for the
      // new number to appear
      delay(100);   // 100 ms
      print_grid();
   }
}

// identify which button is pressed
byte identify_button_pressed()
{
#ifdef DEBUGMODE
   moveto(7, 0);
   oled.print(input);
#endif
   if(IsLeft()) {
#ifdef DEBUGMODE
      oled.print(F(" left "));
#endif
      return(S_LEFT);   // 1
   }
   if(IsUp()) {
#ifdef DEBUGMODE
      oled.print(F(" up   "));
#endif
      return(S_UP);   // 2
   }
   if(IsDown()) {
#ifdef DEBUGMODE
      oled.print(F(" down "));
#endif
      return(S_DOWN);     // 3
   }
   if(IsRight()) {
#ifdef DEBUGMODE
      oled.print(F(" right"));
#endif
      return(S_RIGHT);  // 4
   }
   return(0);
}

// set up the 4x4 array
// of cells and place the first
// pair of numbers in it
void init_grid()
{
   score = 0;
   set_grid_to_zero();     // initialize all cells to 0
   plot_two_or_four();     // place a 2 or 4 in one cell
   plot_two_or_four();     // place a 2 or 4 in another cell
}

// initialize all cells to 0
void set_grid_to_zero()
{
   byte i, j;

   // set all the grid numbers to zero
   for(i = 0; i < 4; i++) {
      for(j = 0; j < 4; j++) {
         grid[i][j] = 0;
      }
   }
}

// boolean function to check if
// the grid is full
boolean grid_full()
{
   byte i, j;

   for(i = 0; i < 4; i++) {
      for(j = 0; j < 4; j++) {
         if(grid[i][j] == 0) {
            return(false);
         }
      }
   }
   return(true);
}

// place a 2 or a 4 in a single cell
// on the grid.
// a 2 is picked 90% of the time
// a 4 is picked 10% of the time
void plot_two_or_four()
{
   byte r, value;
   byte x, y;

   // first check if we have empty space in the grid
   if(grid_full()) {
      return;
   }

   // generate a random number
   // which would be either 2 or 4
   // 90% probability of a 2
   // 10% probability of a 4
   r = random(10);
   value = (r == 9) ? 4 : 2;

   // find a blank cell and plot the value
   do {
      // generate two sets of random co-ordinates
      // between 0 and 3
      x = random(4);
      y = random(4);

      if(grid[x][y] == 0) {
         grid[x][y] = value;
         break;
      }
   } while(true);
}

// prints the 4x4 grid cells
// this function only writes the values
// from the array. doesn't draw borders
void print_grid()
{
   byte i, j;
   byte row, col;

   row = START_ROW;

   // print the 4x4 grid
   for(i = 0; i < 4; i++) {
      col = START_COL + 1;

      // print the ith row
      for(j = 0; j < 4; j++) {
         moveto(row, col);
         if(grid[i][j] != 0) {
            printf4d(grid[i][j]);
         }
         else {
            oled.print(F("    "));
         }
         col += 5;
      }
      row += 2;
   }
}

// function to print a 4 digit
// integer, right justified
void printf4d(int x)
{
   if(x < 1000) {
      oled.print(F(" "));
   }
   if(x < 100) {
      oled.print(F(" "));
   }
   if(x < 10) {
      oled.print(F(" "));
   }
   oled.print(x);
}

// draw grid borders
void draw_borders()
{
   byte i, j;
   byte row, col;

   row = START_ROW;

   for(i = 0; i < 3; i++) {
      col = START_COL + 1;

      // print the ith row
      for(j = 0; j < 3; j++) {
         moveto(row, col);
         oled.print(F("    !"));
         col += 5;
      }
      row++;

      // draw a border line below each row
      col = START_COL + 1;
      for(j = 0; j < 3; j++) {
         moveto(row, col);
         oled.print(F("----+"));
         col += 5;
      }
      moveto(row, col);
      oled.print(F("----"));

      row++;
   }

   // draw the last row
   col = START_COL + 1;
   for(j = 0; j < 3; j++) {
      moveto(row, col);
      oled.print(F("    !"));
      col += 5;
   }
}

// wrapper around oled.setCursor
// oled.setCursor expects the column
// number in pixels and the row number in
// characters. we will normalise them both
// to characters, based on 6x8 char grids
void moveto(byte row, byte col)
{
   oled.setCursor(col * 6, row);
}

// add up the cells when merging left to right
// or right to left
// returns true if the row was modified
boolean row_wise_summation(byte row, char shift_direction)
{
   byte col, start, finish;
   int this_cell, next_cell;
   boolean unchanged, grid_modified;

   if(shift_direction == -1) {
      start = 3;
      finish = 0;
   }
   else {
      start = 0;
      finish = 3;
   }
   grid_modified = false;

   // go in reverse squeezing the empty zeros
   grid_modified = squeeze_zeros_in_row(row, shift_direction);

   // add pairs of numbers (in reverse)
   unchanged = true;
   for(col = finish; col != start; col -= shift_direction) {
      this_cell = grid[row][col];
      next_cell = grid[row][col - shift_direction];

      if(this_cell > 0 && this_cell == next_cell) {
         grid[row][col] = merge_cells(this_cell, next_cell);
         grid[row][col - shift_direction] = 0;
         unchanged = false;
         grid_modified = true;
      }
   }

   // if any changes have been made to the row, we need to
   // squeeze the zeros again
   if(!unchanged) {
      squeeze_zeros_in_row(row, shift_direction);
   }

   return(grid_modified);
}

// add up the cells when merging top to bottom
// or bottom to top
// returns true if the column was modified
boolean column_wise_summation(byte col, char shift_direction)
{
   byte row, start, finish;
   int this_cell, next_cell;
   boolean unchanged, grid_modified;

   if(shift_direction == -1) {
      start = 3;
      finish = 0;
   }
   else {
      start = 0;
      finish = 3;
   }
   grid_modified = false;

   // go in reverse squeezing the empty zeros
   grid_modified = squeeze_zeros_in_column(col, shift_direction);

   // add pairs of numbers (in reverse)
   unchanged = true;
   for(row = finish; row != start; row -= shift_direction) {
      this_cell = grid[row][col];
      next_cell = grid[row - shift_direction][col];

      if(this_cell > 0 && this_cell == next_cell) {
         grid[row][col] = merge_cells(this_cell, next_cell);
         grid[row - shift_direction][col] = 0;
         unchanged = false;
         grid_modified = true;
      }
   }

   // if any changes have been made to the column, we need to
   // squeeze the zeros again
   if(!unchanged) {
      squeeze_zeros_in_column(col, shift_direction);
   }

   return(grid_modified);
}

// if two adjacent cells have the same value,
// add them up. update the score and check
// if the winning cell is reached
int merge_cells(int a, int b)
{
   int sum;

   sum = a + b;
   score += sum;
   if(sum == WINNING) {
      print_win();
   }
   return(sum);
}

// print winning message
// in the bottom most row
void print_win()
{
   moveto(7, 0);
   oled.print(F("     y o u  w i n"));
}

// when merging horizontally, shift all the cells
// left or right if there is an empty cell adjacent
// to them
// returns true if any empty cells were squeezed
boolean squeeze_zeros_in_row(byte row, char shift_direction)
{
   byte col, start, finish;
   int this_cell, next_cell;
   boolean unchanged, zeros_squeezed;

   if(shift_direction == -1) {
      start = 3;
      finish = 0;
   }
   else {
      start = 0;
      finish = 3;
   }

   zeros_squeezed = false;

   // go in reverse squeezing the empty zeros
   do {
      unchanged = true;
      for(col = finish; col != start; col -= shift_direction) {
         this_cell = grid[row][col];
         next_cell = grid[row][col - shift_direction];

         if(this_cell == 0 && next_cell > 0) {
            grid[row][col] = grid[row][col - shift_direction];
            grid[row][col - shift_direction] = 0;
            unchanged = false;
            zeros_squeezed = true;
         }
      }
   } while(!unchanged);

   return(zeros_squeezed);
}

// when merging vertically, shift all the cells
// up or down if there is an empty cell adjacent
// to them
// returns true if any empty cells were squeezed
boolean squeeze_zeros_in_column(byte col, char shift_direction)
{
   byte row, start, finish;
   int this_cell, next_cell;
   boolean unchanged, zeros_squeezed;

   if(shift_direction == -1) {
      start = 3;
      finish = 0;
   }
   else {
      start = 0;
      finish = 3;
   }

   zeros_squeezed = false;

   // go in reverse squeezing the empty zeros
   do {
      unchanged = true;
      for(row = finish; row != start; row -= shift_direction) {
         this_cell = grid[row][col];
         next_cell = grid[row - shift_direction][col];

         if(this_cell == 0 && next_cell > 0) {
            grid[row][col] = grid[row - shift_direction][col];
            grid[row - shift_direction][col] = 0;
            unchanged = false;
            zeros_squeezed = true;
         }
      }
   } while(!unchanged);

   return(zeros_squeezed);
}

// when merging horizontally, add up all the 4 rows
// returns true if the grid was modified, false if unchanged
boolean sum_all_rows(char shift_direction)
{
   byte row;
   byte grid_modified = false;

   for(row = 0; row < 4; row++) {
      grid_modified |= row_wise_summation(row, shift_direction);
   }

   return(grid_modified);
}

// when merging vertically, add up all the 4 columns
// returns true if the grid was modified, false if unchanged
boolean sum_all_columns(char shift_direction)
{
   byte col;
   boolean grid_modified = false;

   for(col = 0; col < 4; col++) {
      grid_modified |= column_wise_summation(col, shift_direction);
   }

   return(grid_modified);
}

// END
