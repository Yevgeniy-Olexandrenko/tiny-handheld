// connect 4 engine with custom functions
// ported to ATTiny85 hardware....see pockeTETRIS
// works well- change computer skill with AISCANS (number of scenarios) and AIDEPTH (moves look ahead)
// sleep works (don't push the middle button to wake up, or you'll make a move!)

#include <EEPROM.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>

#define DIGITAL_WRITE_HIGH(PORT) PORTB |= (1 << PORT)
#define DIGITAL_WRITE_LOW(PORT) PORTB &= ~(1 << PORT)

// Routines to set and clear bits (used in the sleep code)
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))

// Defines for OLED output
#define SSD1306XLED_H
#define SSD1306_SCL   PORTB2
#define SSD1306_SDA   PORTB0
#define SSD1306_SA    0x78

// Input for vertical screen orientation
bool IsLeft()   { return (analogRead(A3) > 500) && (analogRead(A3) < 750); }
bool IsRight()  { return (analogRead(A3) > 750) && (analogRead(A3) < 950); }
bool IsDown()   { return (analogRead(A0) > 750) && (analogRead(A0) < 950); }
bool IsUp()     { return (analogRead(A0) > 500) && (analogRead(A0) < 750); }
bool IsAction() { return bitRead(PINB, PB1) == LOW; }
bool IsCenter() { return bitRead(PINB, PB3) == LOW; }

#define BOARDWIDTH 7
#define BOARDHEIGHT 6
#define BOARDTOWIN 4
#define AISCANS 20
#define AIDEPTH 5

char board[BOARDWIDTH][BOARDHEIGHT];
int mv=0;     //player, 1=computer

// Function prototypes - screen control modified from https://bitbucket.org/tinusaur/ssd1306xled
void ssd1306_init(void);
void ssd1306_xfer_start(void);
void ssd1306_xfer_stop(void);
void ssd1306_send_byte(uint8_t byte);
void ssd1306_send_command(uint8_t command);
void ssd1306_send_data_start(void);
void ssd1306_send_data_stop(void);
void ssd1306_setpos(uint8_t x, uint8_t y);
void ssd1306_fillscreen(uint8_t fill_Data);
void ssd1306_char_f8x8(uint8_t x, uint8_t y, const char ch[]);

static const byte font[][8] PROGMEM = {
  {12,18,33,33,33,18,12,0},     //0
  {8,12,8,8,8,8,28,0},     //1
  {30,33,32,24,6,1,63,0},     //2
  {30,33,32,28,32,33,30,0},     //3
  {24,20,18,17,63,16,16,0},     //4
  {63,1,31,33,32,33,30,0},     //5
  {28,2,1,31,33,33,30,0},     //6
  {63,32,16,8,8,4,4,0},     //7
  {30,33,33,30,33,33,30,0},     //8
  {30,33,33,62,32,16,12,0},     //9
  {
    0b11111111,
    0b10000001,
    0b10100101,
    0b10011001,
    0b10011001,
    0b10100101,
    0b10000001,
    0b11111111,
  },     //X
  {
    0b11111111,
    0b10000001,
    0b10011001,
    0b10100101,
    0b10100101,
    0b10011001,
    0b10000001,
    0b11111111,
  },     //O
  {0,0,0,0,0,0,0,0},     //blank
  {255,129,129,129,129,129,129,255},     //empty
  {39,41,41,39,33,33,225,0},     //PL
  {166,169,169,169,79,73,73,0},     //AY
  {81,85,85,85,85,91,81,0},     //WI
  {201,43,43,77,141,137,105,0},     //NS
  {95,68,68,68,68,68,68,0},     //TI
  {31,1,1,15,1,1,31,0}     //E
};

void setup() {
  ssd1306_init();
  ssd1306_fillscreen(0x00);
  boardinit(board);
  showboard(board);
 
  cbi(DDRB, PB1);       // input
  cbi(DDRB, A0);        // input
  cbi(DDRB, A3);        // input
  sbi(PCMSK, PCINT1);   // pin change mask: listen to PB1
  sbi(PCMSK, PCINT3);   // pin change mask: listen to PB3
  GIMSK |= 0b00100000;  // enable PCINT interrupt
}

void loop() {
  ssd1306_setpos(112,3);
  ssd1306_putfont(14);
  ssd1306_setpos(112,4);
  ssd1306_putfont(15);
  ssd1306_setpos(112,6);
  if(mv==0){    
    ssd1306_putfont(10);
    playermove();     //player (X) to move
  }else{
    ssd1306_putfont(11);
    computermove();   //computer (O) to move
  }
  showboard(board);
  if(boardwin(board,'X')){      //player has won
    ssd1306_setpos(112,3);
    ssd1306_putfont(16);
    ssd1306_setpos(112,4);
    ssd1306_putfont(17);
    ssd1306_setpos(112,6);
    ssd1306_putfont(10);
    delay(5000);
    system_sleep();
    boardinit(board);         //reset board
    mv=0;
  }
  if(boardwin(board,'O')){      //computer has won
    ssd1306_setpos(112,3);
    ssd1306_putfont(16);
    ssd1306_setpos(112,4);
    ssd1306_putfont(17);
    ssd1306_setpos(112,6);
    ssd1306_putfont(11);
    delay(5000);
    system_sleep();
    boardinit(board);         //reset board
    mv=0;
  }
  if(boardtie(board)){          //board full, no moves left, tie
    ssd1306_setpos(112,3);
    ssd1306_putfont(18);
    ssd1306_setpos(112,4);
    ssd1306_putfont(19);
    ssd1306_setpos(112,6);
    ssd1306_putfont(10);
    delay(5000);
    system_sleep();
    boardinit(board);         //reset board
    mv=0;
  }
  showboard(board);
}

int boardtie(char board[BOARDWIDTH][BOARDHEIGHT]){         //returns 1 if no more moves are possible
  int m=0;              //available slots in top row
  for(int x=0;x<BOARDWIDTH;x++){
    if(board[x][0]<'@'){m++;}
  }
  if(m==0){return 1;}
  return 0;
}

void showplayer(char n,int p){
  for(int i=0;i<BOARDWIDTH;i++){
    ssd1306_setpos(16,i+1);
    if(i==p){
      ssd1306_putfont(n);
    }else{
      ssd1306_putfont(12);
    }
  }
}

void computermove(){
  showplayer(11,(BOARDWIDTH-1)/2);
  char cboard[BOARDWIDTH][BOARDHEIGHT];     //for computer to test moves on
  long mscore[BOARDWIDTH];                  //relative score for each move
  long smax=-99999999L;                    //for score comparison
  int mno=-1;
  int x,y,m;                                //index variable for scanning
  for(m=0;m<BOARDWIDTH;m++){
    mscore[m]=0;                            //clear scores
    for(int i=0;i<AISCANS;i++){             //more AISCANS = more games tested
      for(x=0;x<BOARDWIDTH;x++){for(y=0;y<BOARDHEIGHT;y++){cboard[x][y]=board[x][y];}}    //copy play board
      if(boardplay(cboard,m,'O')){
        for(int r=0;r<AIDEPTH;r++){                                                     //more depth = more moves tested per game
          boardplay(cboard,random(0,BOARDWIDTH),'X'+(r&1)*('O'-'X'));                   //make random alternating moves
        }
        mscore[m]=mscore[m]+boardwin(cboard,'O')-boardwin(cboard,'X')*2;                  //add results
      }else{
        break;                              //can't make any moves, so don't try this one
      }
    }
    if(mscore[m]>smax){smax=mscore[m];mno=m;}
  }
  if(mno>-1){
    showplayer(11,mno);
    delay(1000);
    if(boardplay(board,mno,'O')){
      mv=0;
    }else{                    //computer can't move, player has won
      ssd1306_setpos(112,3);
      ssd1306_putfont(16);
      ssd1306_setpos(112,4);
      ssd1306_putfont(17);
      ssd1306_setpos(112,6);
      ssd1306_putfont(11);
      delay(2000);
      boardinit(board);         //reset board
      mv=0;
    }
  }
}

int boardwin(char board[BOARDWIDTH][BOARDHEIGHT],char p){      //check if there is a row of pieces, returns number of winning positions (eg 5 in a row>2 wins)
  int result=0;            //count winning positions
  for(int x=0;x<BOARDWIDTH;x++){
    for(int y=0;y<BOARDHEIGHT;y++){
      int hwin=0;   //horizontal
      int vwin=0;   //vertical
      int dwin=0;   //diagonal down/right
      int dwin2=0;  //diagonal up/right
      for(int c=0;c<BOARDTOWIN;c++){
        if(x+c<BOARDWIDTH){                   //horizontal-don't look outside board
          if(board[x+c][y]==p){hwin++;}   //one piece
        }
        if(y+c<BOARDHEIGHT){                   //vertical-don't look outside board
          if(board[x][y+c]==p){vwin++;}   //one piece
        }
        if((y+c<BOARDHEIGHT)&&(x+c<BOARDWIDTH)){                   //diagonal-don't look outside board
          if(board[x+c][y+c]==p){dwin++;}   //one piece
          if(board[x+c][BOARDWIDTH-1-y-c]==p){dwin2++;}   //one piece
        }
        if(hwin==BOARDTOWIN){result++;}
        if(vwin==BOARDTOWIN){result++;}
        if(dwin==BOARDTOWIN){result++;}        
        if(dwin2==BOARDTOWIN){result++;}        
      }
    }
  }
  return result;
}

void playermove(){      //player has 'X'
  int ppos=(BOARDWIDTH-1)/2;      //start location
  showplayer(10,ppos);
  while(mv==0){
    int a;
    if(IsLeft()){      //is left pressed?
      delay(10);
      if(IsLeft()){    //debounce
        ppos=ppos-1;
        if(ppos<0){ppos=0;}           //move left and check
        showplayer(10,ppos);
        delay(200);                   //wait a bit
      }
    }
    if(IsRight()){      //is right pressed?
      delay(10);
      if(IsRight()){    //debounce
        ppos=ppos+1;
        if(ppos>=BOARDWIDTH){ppos=BOARDWIDTH-1;}           //move right and check
        showplayer(10,ppos);
        delay(200);                   //wait a bit
      }
    }

    if(IsAction() || IsCenter()){      //is middle pressed?
      delay(10);
      if(IsAction() || IsCenter()){    //debounce
        if(!boardplay(board,ppos,'X')){
          //move not allowed
        }else{
          mv=1;
          showboard(board);             //update display
          showplayer(10,-1);
          while(IsAction() || IsCenter()){}      //wait till button released
          delay(100);
        }
      }
    }
  }  
}


void boardinit(char board[BOARDWIDTH][BOARDHEIGHT]){
  for(int x=0;x<BOARDWIDTH;x++){
    for(int y=0;y<BOARDHEIGHT;y++){
       board[x][y]='.';     //empty, use O and X for pieces
    }
  }  
}

void showboard(char board[BOARDWIDTH][BOARDHEIGHT]){
  for(int y=0;y<BOARDHEIGHT;y++){
    for(int x=0;x<BOARDWIDTH;x++){
      ssd1306_setpos(y*8+40,x+1);
      switch(board[x][y]){
        case 'O': ssd1306_putfont(11);break;
        case 'X': ssd1306_putfont(10);break;
        default : ssd1306_putfont(13);break;
      }
    }
  }
}

int boardplay(char board[BOARDWIDTH][BOARDHEIGHT],int x,char p){   //play piece p in column x, returns 0 if illegal (no change), 1 if OK (play made)
  if(x<0){return 0;}           //outside board
  if(x>=BOARDWIDTH){return 0;} //outside board
  for(int y=BOARDHEIGHT-1;y>-1;y--){    //check from bottom up
    if(board[x][y]<'@'){                //anything below '@' is empty
      board[x][y]=p;
      return 1;
    }
  }
  return 0;      //no legal move 
}

// Screen control functions
void ssd1306_putfont(char n){   //nth character
  ssd1306_send_data_start();
  for(int i=0;i<8;i++){
    ssd1306_send_byte(pgm_read_byte(&font[n][i]));
  }
  ssd1306_send_data_stop();
}

void ssd1306_init(void) {
  DDRB |= (1 << SSD1306_SDA); // Set port as output
  DDRB |= (1 << SSD1306_SCL); // Set port as output

  ssd1306_send_command(0xAE); // display off
  ssd1306_send_command(0x00); // Set Memory Addressing Mode
  ssd1306_send_command(0x10); // 00,Horizontal Addressing Mode;01,VERTDRAWical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
  ssd1306_send_command(0x40); // Set Page Start Address for Page Addressing Mode,0-7
  ssd1306_send_command(0x81); // Set COM Output Scan Direction
  ssd1306_send_command(0xCF); // ---set low column address
  //ssd1306_send_command(0xA1); // ---set high column address
  ssd1306_send_command(0xA0); // ---set high column address
  ssd1306_send_command(0xC8); // --set start line address
  ssd1306_send_command(0xA6); // --set contrast control register
  ssd1306_send_command(0xA8);
  ssd1306_send_command(0x3F); // --set segment re-map 0 to 127
  ssd1306_send_command(0xD3); // --set normal display
  ssd1306_send_command(0x00); // --set multiplex ratio(1 to 64)
  ssd1306_send_command(0xD5); //
  ssd1306_send_command(0x80); // 0xa4,Output follows RAM content;0xa5,Output ignores RAM content
  ssd1306_send_command(0xD9); // -set display offset
  ssd1306_send_command(0xF1); // -not offset
  ssd1306_send_command(0xDA); // --set display clock divide ratio/oscillator frequency
  ssd1306_send_command(0x12); // --set divide ratio
  ssd1306_send_command(0xDB); // --set pre-charge period
  ssd1306_send_command(0x40); //
  ssd1306_send_command(0x20); // --set com pins hardware configuration
  ssd1306_send_command(0x02);
  ssd1306_send_command(0x8D); // --set vcomh
  ssd1306_send_command(0x14); // 0x20,0.77xVcc
  ssd1306_send_command(0xA4); // --set DC-DC enable
  ssd1306_send_command(0xA6); //
  ssd1306_send_command(0xAF); // --turn on oled panel
}

void ssd1306_xfer_start(void) {
  DIGITAL_WRITE_HIGH(SSD1306_SCL);  // Set to HIGH
  DIGITAL_WRITE_HIGH(SSD1306_SDA);  // Set to HIGH
  DIGITAL_WRITE_LOW(SSD1306_SDA);   // Set to LOW
  DIGITAL_WRITE_LOW(SSD1306_SCL);   // Set to LOW
}

void ssd1306_xfer_stop(void) {
  DIGITAL_WRITE_LOW(SSD1306_SCL);   // Set to LOW
  DIGITAL_WRITE_LOW(SSD1306_SDA);   // Set to LOW
  DIGITAL_WRITE_HIGH(SSD1306_SCL);  // Set to HIGH
  DIGITAL_WRITE_HIGH(SSD1306_SDA);  // Set to HIGH
}

void ssd1306_send_byte(uint8_t byte) {
  uint8_t i;
  for (i = 0; i < 8; i++)
  {
    if ((byte << i) & 0x80)
      DIGITAL_WRITE_HIGH(SSD1306_SDA);
    else
      DIGITAL_WRITE_LOW(SSD1306_SDA);

    DIGITAL_WRITE_HIGH(SSD1306_SCL);
    DIGITAL_WRITE_LOW(SSD1306_SCL);
  }
  DIGITAL_WRITE_HIGH(SSD1306_SDA);
  DIGITAL_WRITE_HIGH(SSD1306_SCL);
  DIGITAL_WRITE_LOW(SSD1306_SCL);
}

void ssd1306_send_command(uint8_t command) {
  ssd1306_xfer_start();
  ssd1306_send_byte(SSD1306_SA);  // Slave address, SA0=0
  ssd1306_send_byte(0x00);  // write command
  ssd1306_send_byte(command);
  ssd1306_xfer_stop();
}

void ssd1306_send_data_start(void) {
  ssd1306_xfer_start();
  ssd1306_send_byte(SSD1306_SA);
  ssd1306_send_byte(0x40);  //write data
}

void ssd1306_send_data_stop(void) {
  ssd1306_xfer_stop();
}

void ssd1306_setpos(uint8_t x, uint8_t y)
{
  if (y > 7) return;
  ssd1306_xfer_start();
  ssd1306_send_byte(SSD1306_SA);  //Slave address,SA0=0
  ssd1306_send_byte(0x00);  //write command

  ssd1306_send_byte(0xb0 + y);
  ssd1306_send_byte(((x & 0xf0) >> 4) | 0x10); // |0x10
  ssd1306_send_byte((x & 0x0f) | 0x01); // |0x01

  ssd1306_xfer_stop();
}

void ssd1306_fillscreen(uint8_t fill_Data) {
  uint8_t m, n;
  for (m = 0; m < 8; m++)
  {
    ssd1306_send_command(0xb0 + m); //page0-page1
    ssd1306_send_command(0x00);   //low column start address
    ssd1306_send_command(0x10);   //high column start address
    ssd1306_send_data_start();
    for (n = 0; n < 128; n++)
    {
      ssd1306_send_byte(fill_Data);
    }
    ssd1306_send_data_stop();
  }
}

// Sleep code from http://www.re-innovation.co.uk/web12/index.php/en/blog-75/306-sleep-modes-on-attiny85
void system_sleep() {
  ssd1306_fillscreen(0x00);
  ssd1306_send_command(0xAE);
  cbi(ADCSRA, ADEN);                   // switch Analog to DigitalconVERTDRAWer OFF
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
  sleep_enable();
  sleep_mode();                        // System actually sleeps here
  sleep_disable();                     // System continues execution here when watchdog timed out
  sbi(ADCSRA, ADEN);                   // switch Analog to DigitalconVERTDRAWer ON
  ssd1306_send_command(0xAF);
}

ISR(PCINT0_vect) { // PB0 pin button interrupt, all it does is wake up
}
