//      >>>>>  T-I-N-Y  B-E-R-T for ATTINY85  GPL v3 <<<<
//                     Programmer: Daniel C 2019
//              Contact EMAIL: electro_l.i.b@tinyjoypad.com
//                     https://www.tinyjoypad.com   
//           https://sites.google.com/view/arduino-collection

//  Tiny Bert is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.

//  You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

//the code work at 16MHZ internal
//and use ssd1306xled Library for SSD1306 oled display 128x64
//Program chip with arduino uno as isp at 16 mhz internal!

#include <ssd1306xled.h>
#include "spritebank.h"

// var public
uint8_t INTERLACE=0;
uint8_t ANIM_LIFT;
uint8_t DESTROY_LIFT_R;
uint8_t DESTROY_LIFT_L;
uint8_t SEED=0;
uint8_t INGAME=0;
uint8_t MAIN_GHOST=0;
uint8_t BYPASS_DEAD;
uint8_t DIFICULT_PLATE;
uint8_t Dificulty=0;
uint8_t RENEW_SPRITE;
int8_t Extra_Live;
uint8_t CHANGE_LEVEL=0;
uint8_t D4=0,D3=0,D2=0,D1=0,D0=0;
uint8_t HD4=0,HD3=0,HD2=0,HD1=0,HD0=0;
// fin var public

void setup() {
SSD1306.ssd1306_init();
pinMode(1,INPUT);
pinMode(4,OUTPUT);
pinMode(A0,INPUT); 
pinMode(A3,INPUT);
}

void DIFICULTY_CHANGE(void){
if (Dificulty<20) {Dificulty++;}
MAX_SPEED=(DEFAULT_SPEED-(Dificulty));
MAX_RENEW=MAX_SPEED;
if ((Dificulty>4)) {DIFICULT_PLATE=1;}
}

void ResetDataVar(Sprite* DATA){
uint8_t x;
for (x=0;x<3;x++){
DEADIFY(x,DATA);
}}

void RESET_GAME(void){
ANIM_LIFT=0;
DESTROY_LIFT_R=0;
DESTROY_LIFT_L=0;
INGAME=0;
BYPASS_DEAD=1;
DIFICULT_PLATE=0;
Dificulty=0;
MAX_RENEW=DEFAULT_SPEED;
MAX_SPEED=DEFAULT_SPEED;
Extra_Live=3;
CHANGE_LEVEL=0;
D4=0;
D3=0;
D2=0;
D1=0;
D0=0; 
}

void SEED_RND(void){
if (SEED<29) {SEED++;}else{SEED=0;}  
}

uint8_t RANDOM(uint8_t STEP){
SEED_RND();   
if (STEP==2) {
return pgm_read_byte(&RAND2[SEED]);  
}else{
return pgm_read_byte(&RAND4[SEED]);   
}}


void Recupe_HighScore(void){
uint16_t Score1=HighScore(D3,D2,D1,D0);
uint16_t Score2=HighScore(HD3,HD2,HD1,HD0);
if (Score1>Score2){
HD0=D0;
HD1=D1;
HD2=D2;
HD3=D3;
HD4=D4;}
}

void Rst_Score(void){
D0=0;
D1=0;
D2=0;
D3=0;
D4=0;
}

void recupe_Score(void){
D0=HD0;
D1=HD1;
D2=HD2;
D3=HD3;
D4=HD4;
}

uint16_t HighScore(uint8_t D_3,uint8_t D_2,uint8_t D_1,uint8_t D_0){
return (D_0+(D_1*10)+(D_2*100)+(D_3*1000));
}

void loop() {
uint8_t Time_Management;
Sprite sprite[3];
NEW_GAME:;
Recupe_HighScore();
RESET_GAME();
NEW_LEVEL:;
DESTROY_LIFT_R=0;
DESTROY_LIFT_L=0;
DIFICULTY_CHANGE();
RENEW_SPRITE=0b00000111;
Time_Management=0;
ResetPlateGrid();
ResetDataVar(&sprite[0]);
MENU_LOOP:;
SEED_RND();
if (digitalRead(1)==LOW) {Rst_Score();INGAME=1;Sound(150,200);Sound(100,200);}else{if (INGAME==0){recupe_Score();}goto FLIP;}
START:;
while(1){
if (CHANGE_LEVEL!=0) {
FLIP_PLATE();
CHANGE_LEVEL--; 
if (CHANGE_LEVEL==0) {goto NEW_LEVEL;}else{Sound(200,40);BYPASS_DEAD=1;goto FLIP;}
}
Ball_move_update(1,&sprite[0]);
Ball_move_update(2,&sprite[0]);
if ((sprite[0].Joypad_Key==255)&&(sprite[0].DEAD==0)) {
if (sprite[0].moveTimer1==6) {sprite[0].Joypad_Key=0;sprite[0].moveTimer1=0;}else{sprite[0].moveTimer1++;}
}
if (((analogRead(A0)>=750)&&(analogRead(A0)<950))&&(sprite[0].Joypad_Key==0)) {SEED_RND();LimitCheck(0,4,&sprite[0]);}
if (((analogRead(A0)>500)&&(analogRead(A0)<750))&&(sprite[0].Joypad_Key==0)) {LimitCheck(0,2,&sprite[0]);}
if (((analogRead(A3)>=750)&&(analogRead(A3)<950))&&(sprite[0].Joypad_Key==0)) {LimitCheck(0,3,&sprite[0]);}
if (((analogRead(A3)>500)&&(analogRead(A3)<750))&&(sprite[0].Joypad_Key==0)) {SEED_RND();LimitCheck(0,1,&sprite[0]);}
if (Time_Management==1) {Renew_if_Posible(&sprite[0]);if (Extra_Live<0) {INGAME=0;goto NEW_GAME;}}
if (Time_Management<2) {
Time_Management++;
GamePlayUpdate(&sprite[0]);
for (uint8_t Count=0;Count<3;Count++){
 switch (sprite[Count].DEAD){
  case(0):
  case(1):UPDATE_MAIN_MOVE(Count,&sprite[0]);break;
  case(2):UPDATE_MAIN_DEAD(Count,&sprite[0]);break;
  case(3):UPDATE_LIFT_MAIN(Count,&sprite[0]);break;
  default:break;}}
  }else{Time_Management=0;goto FLIP;}
}
FLIP:;
if (ANIM_LIFT<2) {ANIM_LIFT++;}else{ANIM_LIFT=0;}
Tiny_Flip(INTERLACE,&sprite[0]);
COLLISION_CHECK(&sprite[0]);
if (INTERLACE<3) {INTERLACE++;}else{INTERLACE=0;}
if (INGAME==0) {goto MENU_LOOP;}
goto START;
}
////////////////////////////////// main end /////////////////////////////////

void DEAD_SOUND(void){
for(uint8_t s=200;s>100;s--){Sound(s,10);} 
}

void DEADIFY(uint8_t SPr,Sprite *DATA){
DATA[SPr].DEAD=254;
DATA[SPr].moveTimer1=0;
DATA[SPr].xPOS=200+(SPr*10);
DATA[SPr].yPOS=200+(SPr*10);
DATA[SPr].JumpFrame=0;
DATA[SPr].Joypad_Key=254;
DATA[SPr].GridXpos=4+SPr;
DATA[SPr].GridYpos=4+SPr;
}

void RenewSprite(uint8_t SPr,Sprite *DATA){
if ((SPr==0)&&(BYPASS_DEAD==0)) {Extra_Live--;}
BYPASS_DEAD=0;
DATA[SPr].DEAD=0;
DATA[SPr].moveTimer1=0;
DATA[SPr].xPOS=58;
DATA[SPr].yPOS=2;
DATA[SPr].JumpFrame=0;
DATA[SPr].Joypad_Key=0;
DATA[SPr].GridXpos=0;
DATA[SPr].GridYpos=0;
}

uint8_t Renew_if_Posible(Sprite *DATA){
if (RENEW_SPRITE==0) {return 0;}
if ((RENEW_SPRITE&0b00000001)!=0) {return Renew_Check_if_Posible(0,DATA);} 
if ((RENEW_SPRITE&0b00000010)!=0) {Renew_Check_if_Posible(1,DATA);return 0;} 
if ((RENEW_SPRITE&0b00000100)!=0) {Renew_Check_if_Posible(2,DATA);return 0;} 
return 0;}

uint8_t Renew_Check_if_Posible(uint8_t SPr,Sprite *DATA){
  if ((SPr==2) &&(Dificulty<=5)) return 0;
for (uint8_t t=0;t<3;t++){
if (DATA[t].DEAD!=254) {
if (DATA[t].yPOS<12) {return 1;}
}}
  switch(SPr){
    case(0):RENEW_SPRITE=(RENEW_SPRITE&0b11111110);break;
    case(1):RENEW_SPRITE=(RENEW_SPRITE&0b11111101);break;
    case(2):RENEW_SPRITE=(RENEW_SPRITE&0b11111011);break;    
    default:break;
  }
  RenewSprite(SPr,DATA);
  return 0;
}

uint8_t COLLISION_CHECK(Sprite *DATA){
#define MargXMax 3
#define MargYMax 3
if ((MAIN_GHOST==1)||((DATA[0].DEAD!=0))) {return 0;}
for (uint8_t x=1;x<3;x++){
if (((DATA[0].xPOS-MargXMax)>(DATA[x].xPOS+MargXMax))||((DATA[0].xPOS+MargXMax)<(DATA[x].xPOS-MargXMax))||((DATA[0].yPOS-MargYMax)>(DATA[x].yPOS+MargYMax))||((DATA[0].yPOS+MargYMax)<(DATA[x].yPOS-MargYMax))) {}else{DEADIFY(0,DATA);RENEW_SPRITE=(RENEW_SPRITE|0b00000001);DEAD_SOUND();}
}
return 0;
}

void FLIP_PLATE(void){
uint8_t x,y,S=4,Q;
if (PlateGrid[0][0]==1) {Q=0;}else{Q=1;}
for (y=0;y<4;y++){
for (x=0;x<S;x++){
PlateGrid[y][x]=Q;
}S--;} 
}

void GamePlayUpdate(Sprite *DATA){
for (uint8_t Q=0;Q<3;Q++){
if (DATA[Q].DEAD==255) {
if (DATA[Q].Timer_new_Live>0) {DATA[Q].Timer_new_Live--;}else{ 
DATA[Q].Timer_new_Live=MAX_RENEW;
DEADIFY(Q,DATA);
renew_if_possible_add(Q);
}}}}

void renew_if_possible_add(uint8_t Sprite ){
switch (Sprite){
  case(0):RENEW_SPRITE=(RENEW_SPRITE|0b00000001);break;
  case(1):RENEW_SPRITE=(RENEW_SPRITE|0b00000010);break;
  case(2):RENEW_SPRITE=(RENEW_SPRITE|0b00000100);break;
  default:break;
}}

void ResetPlateGrid(void){
uint8_t x,y,S=4;
for (y=0;y<4;y++){
for (x=0;x<4;x++){
PlateGrid[y][x]=255;
}}
for (y=0;y<4;y++){
for (x=0;x<S;x++){
PlateGrid[y][x]=0;
}S--;}
}

void ScoreChange(void){
for (uint8_t r=0;r<13;r++){
if (D0<9) {D0++;}else{
D0=0;
if (D1<9) {D1++;}else{
D1=0;
if (D2<9) {D2++;}else{
if (Extra_Live<3) {Extra_Live++;
Sound(200,20);
}
D2=0;
 if (D3<9) {D3++;}else{D3=0;
 if (D4<9) {D4++;}}
}}}}}

void refreshJump(uint8_t SpUSE,Sprite *DATA){
uint8_t backup=DATA[SpUSE].Joypad_Key;
if (DATA[SpUSE].Joypad_Key==2) {
DATA[SpUSE].xPOS=DATA[SpUSE].xPOS+pgm_read_byte(&Jump[DATA[SpUSE].JumpFrame*2]);
DATA[SpUSE].yPOS=DATA[SpUSE].yPOS+pgm_read_byte(&Jump[(DATA[SpUSE].JumpFrame*2)+1]);
}
if (DATA[SpUSE].Joypad_Key==3) {
DATA[SpUSE].xPOS=DATA[SpUSE].xPOS+(-pgm_read_byte(&Jump[DATA[SpUSE].JumpFrame*2]));
DATA[SpUSE].yPOS=DATA[SpUSE].yPOS+pgm_read_byte(&Jump[(DATA[SpUSE].JumpFrame*2)+1]);
}
if (DATA[SpUSE].Joypad_Key==1) {
DATA[SpUSE].xPOS=DATA[SpUSE].xPOS+pgm_read_byte(&Jump[32-(DATA[SpUSE].JumpFrame*2)]);
DATA[SpUSE].yPOS=DATA[SpUSE].yPOS+(-pgm_read_byte(&Jump[(32-(DATA[SpUSE].JumpFrame*2))+1]));
}
if (DATA[SpUSE].Joypad_Key==4) {
DATA[SpUSE].xPOS=DATA[SpUSE].xPOS+(-pgm_read_byte(&Jump[32-(DATA[SpUSE].JumpFrame*2)]));
DATA[SpUSE].yPOS=DATA[SpUSE].yPOS+(-pgm_read_byte(&Jump[(32-(DATA[SpUSE].JumpFrame*2))+1]));
}
DATA[SpUSE].JumpFrame=DATA[SpUSE].JumpFrame+1;
if (DATA[SpUSE].JumpFrame>15) {
Sound(160,10);
if (DIFICULT_PLATE==0) {
if ((PlateGrid[DATA[SpUSE].GridYpos][DATA[SpUSE].GridXpos]==0)&&(SpUSE==0)&&(DATA[SpUSE].DEAD==0)) {ScoreChange();PlateGrid[DATA[SpUSE].GridYpos][DATA[SpUSE].GridXpos]=1;if (Level_Completed()) {BYPASS_DEAD=1;CHANGE_LEVEL=18;}}  
}else{
if ((SpUSE==0)&&(DATA[SpUSE].DEAD==0)) {PlateGrid[DATA[SpUSE].GridYpos][DATA[SpUSE].GridXpos]=!PlateGrid[DATA[SpUSE].GridYpos][DATA[SpUSE].GridXpos];if (PlateGrid[DATA[SpUSE].GridYpos][DATA[SpUSE].GridXpos]==1) {ScoreChange();}if (Level_Completed()) {BYPASS_DEAD=1;CHANGE_LEVEL=18;}} 
} 
DATA[SpUSE].JumpFrame=0;
DATA[SpUSE].Joypad_Key=255;
if (DATA[SpUSE].sw==2) {DATA[SpUSE].sw=3;}
if (DATA[SpUSE].sw==0) {DATA[SpUSE].sw=1;}
if (DATA[SpUSE].DEAD==1) {
if ((DATA[SpUSE].GridXpos==3)&&(backup==1)&&(DESTROY_LIFT_R==0)) {DESTROY_LIFT_R=1;TELEPORT_MAIN(DATA);DATA[SpUSE].DEAD=3;}
else if ((DATA[SpUSE].GridYpos==3)&&(backup==4)&&(DESTROY_LIFT_L==0)) {DESTROY_LIFT_L=1;TELEPORT_MAIN(DATA);DATA[SpUSE].DEAD=3;}
else{DATA[SpUSE].DEAD=2;if (SpUSE==0) {DEAD_SOUND();}}
}}}

void TELEPORT_MAIN(Sprite *DATA){
BYPASS_DEAD=1; 
DATA[0].GridXpos=0;
DATA[0].GridYpos=0;
}

uint8_t UPDATE_LIFT_MAIN(uint8_t SpUSE,Sprite *DATA){
uint8_t x=0;
Sound(100-DATA[SpUSE].yPOS,10);
while(x<2) {
if ((DATA[SpUSE].xPOS==58)&&(DATA[SpUSE].yPOS==2)) {DATA[SpUSE].DEAD=0;BYPASS_DEAD=0; }
if (SpUSE!=0) {return 0;} 
if (DATA[SpUSE].yPOS>2) {DATA[SpUSE].yPOS--;}else{
if (DATA[SpUSE].xPOS>58) {DATA[SpUSE].xPOS--;}
if (DATA[SpUSE].xPOS<58) {DATA[SpUSE].xPOS++;}
}
x++;}
return 0;
}

void UPDATE_MAIN_MOVE(uint8_t SpUSE,Sprite *DATA){
uint8_t x=0;
if ((DATA[SpUSE].Joypad_Key!=0)&&(DATA[SpUSE].Joypad_Key!=255)&&(DATA[SpUSE].JumpFrame==0)) {
DATA[SpUSE].JumpFrame=1;}
while(x<2){
if (DATA[SpUSE].JumpFrame!=0) {refreshJump(SpUSE,DATA);
}x++;
}}

void UPDATE_MAIN_DEAD(uint8_t SpUSE,Sprite *DATA){
uint8_t x=0;
while(x<2){
DATA[SpUSE].yPOS=DATA[SpUSE].yPOS+2;
x++;}
if (DATA[SpUSE].yPOS>120) {DATA[SpUSE].DEAD=255;}
}

void LimitCheck(uint8_t SpUSE,uint8_t JOYKEY,Sprite *DATA){
DATA[SpUSE].Joypad_Key=JOYKEY;
 switch(JOYKEY){
  case(4):DATA[SpUSE].sw=2;GridLimitTest(SpUSE,-1,0,DATA);break;
  case(2):DATA[SpUSE].sw=0;GridLimitTest(SpUSE,1,0,DATA);break;
  case(3):DATA[SpUSE].sw=2;GridLimitTest(SpUSE,0,1,DATA);break;
  case(1):DATA[SpUSE].sw=0;GridLimitTest(SpUSE,0,-1,DATA);break;
  default:break;
 }
 }

uint8_t GridLimitTest(uint8_t SpUSE,int8_t x,int8_t y,Sprite *DATA){
  if  ((x==-1)&&(DATA[SpUSE].GridXpos+x)>=0) {DATA[SpUSE].GridXpos--;return 0;}
  if  (((x==1)&&(DATA[SpUSE].GridXpos+x)<=3)&&(PlateGrid[DATA[SpUSE].GridYpos][DATA[SpUSE].GridXpos+1]!=255)) {DATA[SpUSE].GridXpos++;return 0;}
  if  ((y==-1)&&(DATA[SpUSE].GridYpos+y)>=0) {DATA[SpUSE].GridYpos--;return 0;}
  if  (((y==1)&&(DATA[SpUSE].GridYpos+y)<=3)&&(PlateGrid[DATA[SpUSE].GridYpos+1][DATA[SpUSE].GridXpos]!=255)) {DATA[SpUSE].GridYpos++;return 0;} 
DATA[SpUSE].DEAD=1;
return 0;
}

uint8_t Level_Completed(void){
uint8_t x,y,S=4;
for (y=0;y<4;y++){
for (x=0;x<S;x++){
if (PlateGrid[y][x]==0) {return 0;} 
}S--;}
return 1;
}

void Ball_move_update(uint8_t SpUSE,Sprite *sprite){
uint8_t SPEED=MAX_SPEED;
if (SpUSE==0) {SPEED=30;}
if ((sprite[SpUSE].Joypad_Key==255)&&(sprite[SpUSE].DEAD==0)) {sprite[SpUSE].Joypad_Key=0;}
if (sprite[SpUSE].Joypad_Key==0) {
sprite[SpUSE].moveTimer1++;
if (sprite[SpUSE].moveTimer1==SPEED){
sprite[SpUSE].moveTimer1=0;
uint8_t Q;
if ((SpUSE==1)||(sprite[SpUSE].GridXpos==0)||(sprite[SpUSE].GridYpos==0)) {Q=RANDOM(2);}else{Q=RANDOM(4);}
switch(Q){
case (0):LimitCheck(SpUSE,2,&sprite[0]);break;
case (1):LimitCheck(SpUSE,3,&sprite[0]);break;
case (2):LimitCheck(SpUSE,1,&sprite[0]);break;
case (3):LimitCheck(SpUSE,4,&sprite[0]);break;
default:break;
}}}}


uint8_t blitzSprite(int8_t xPos,int8_t yPos,uint8_t xPASS,uint8_t yPASS,uint8_t FRAME,const uint8_t *SPRITES){
uint8_t OUTBYTE;
uint8_t WSPRITE=(pgm_read_byte(&SPRITES[0]));
uint8_t HSPRITE=(pgm_read_byte(&SPRITES[1]));
uint8_t Wmax=((HSPRITE*WSPRITE)+1);
uint8_t PICBYTE=FRAME*(Wmax-1);
int8_t RECUPELINEY=RecupeLineY(yPos);
if ((xPASS>((xPos+(WSPRITE-1))))||(xPASS<xPos)||((RECUPELINEY>yPASS)||((RECUPELINEY+(HSPRITE))<yPASS))) {return 0x00;}
int8_t SPRITEyLINE=(yPASS-(RECUPELINEY));
uint8_t SPRITEyDECALAGE=(RecupeDecalageY(yPos));
uint8_t ScanA=(((xPASS-xPos)+(SPRITEyLINE*WSPRITE))+2);
uint8_t ScanB=(((xPASS-xPos)+((SPRITEyLINE-1)*WSPRITE))+2);
if (ScanA>Wmax) {OUTBYTE=0x00;}else{OUTBYTE=SplitSpriteDecalageY(SPRITEyDECALAGE,pgm_read_byte(&SPRITES[ScanA+(PICBYTE)]),1);}
if ((SPRITEyLINE>0)) {
uint8_t OUTBYTE2=SplitSpriteDecalageY(SPRITEyDECALAGE,pgm_read_byte(&SPRITES[ScanB+(PICBYTE)]),0);
if (ScanB>Wmax) {return OUTBYTE;}else{return OUTBYTE|OUTBYTE2;}
}else{return OUTBYTE;}
}

uint8_t GridPlate(uint8_t xPASS,uint8_t yPASS){
uint8_t Byte=0x00;
if ((yPASS==0)||(yPASS==1)) {
if (PlateGrid[0][0]==1) {Byte|=blitzSprite(54,6,xPASS,yPASS,0,Plate); }
}else if ((yPASS==2)||(yPASS==3)) {
if (PlateGrid[0][1]==1) {Byte|=blitzSprite(64,20,xPASS,yPASS,0,Plate); }
if (PlateGrid[1][0]==1) {Byte|=blitzSprite(44,20,xPASS,yPASS,0,Plate); }
}else if ((yPASS==4)||(yPASS==5)) {
if (PlateGrid[0][2]==1) {Byte|=blitzSprite(74,34,xPASS,yPASS,0,Plate); }
if (PlateGrid[1][1]==1) {Byte|=blitzSprite(54,34,xPASS,yPASS,0,Plate); }
if (PlateGrid[2][0]==1) {Byte|=blitzSprite(34,34,xPASS,yPASS,0,Plate); }
}else if ((yPASS==6)) {
if (PlateGrid[0][3]==1) {Byte|=blitzSprite(84,48,xPASS,yPASS,0,Plate); } 
if (PlateGrid[1][2]==1) {Byte|=blitzSprite(64,48,xPASS,yPASS,0,Plate); }
if (PlateGrid[2][1]==1) {Byte|=blitzSprite(44,48,xPASS,yPASS,0,Plate); }
if (PlateGrid[3][0]==1) {Byte|=blitzSprite(24,48,xPASS,yPASS,0,Plate); }
}
return Byte;
}

uint8_t Recupe(uint8_t xPASS,uint8_t yPASS,Sprite *DATA){
uint8_t R,L;
if (INGAME==0) {
return (pgm_read_byte(&INTRO[xPASS+(yPASS*128)])|Police_Print(xPASS,yPASS)); 
}
if (DESTROY_LIFT_R==0) {R=blitzSprite(96,40,xPASS,yPASS,ANIM_LIFT,LIFT_PLATE);}else{R=0;}
if (DESTROY_LIFT_L==0) {L=blitzSprite(16,40,xPASS,yPASS,ANIM_LIFT,LIFT_PLATE);}else{L=0;}
uint8_t black=(0xff-Tiny_Bert_Live_Print(xPASS,yPASS));
if (DATA[0].DEAD!=2) {black&=(0xff-(blitzSprite(DATA[0].xPOS,DATA[0].yPOS,xPASS,yPASS,DATA[0].sw,TINYBERT_BLACK)));}
if (DATA[1].DEAD!=2) {black&=(0xff-(blitzSprite(DATA[1].xPOS,DATA[1].yPOS,xPASS,yPASS,DATA[1].sw,BALL_BLACK)));}
if (DATA[2].DEAD!=2) {black&=(0xff-(blitzSprite(DATA[2].xPOS,DATA[2].yPOS,xPASS,yPASS,DATA[2].sw,SNACK_BLACK)));}
return 
(blitzSprite(DATA[0].xPOS,DATA[0].yPOS,xPASS,yPASS,DATA[0].sw,TINYBERT)
|blitzSprite(DATA[1].xPOS,DATA[1].yPOS,xPASS,yPASS,DATA[1].sw,BALL)
|blitzSprite(DATA[2].xPOS,DATA[2].yPOS,xPASS,yPASS,DATA[2].sw,SNACK)
|Police_Print(xPASS,yPASS)
|GridPlate(xPASS,yPASS)
|R
|L
|pgm_read_byte(&BACK[xPASS+(yPASS*128)]))&(black);
;}

uint8_t SplitSpriteDecalageY(uint8_t decalage,uint8_t Input,uint8_t UPorDOWN){
if (UPorDOWN) {return Input<<decalage;}
return Input>>(8-decalage);
}

int8_t RecupeLineY(int8_t Valeur){
return (Valeur>>3); 
}

uint8_t RecupeDecalageY(uint8_t Valeur){
return (Valeur-((Valeur>>3)<<3));
}


uint8_t Tiny_Bert_Live_Print(uint8_t xPASS,uint8_t yPASS){
if (yPASS>1) {return 0x00;}
if (xPASS>34) {return 0x00;}
uint8_t ret=0x00;
#define A blitzSprite(5,1,xPASS,yPASS,0,Live)
#define B blitzSprite(14,1,xPASS,yPASS,0,Live)
#define C blitzSprite(23,1,xPASS,yPASS,0,Live)

switch(Extra_Live){
  case(1):ret|=A;break;
  case(2):ret|=A;ret|=B;break;
  case(3):ret|=A;ret|=B;ret|=C;break;
  default:break;
}return ret;
};

uint8_t Police_Print(uint8_t xPASS,uint8_t yPASS){
if (xPASS<94) return 0x00;
if (yPASS>0) return 0x00;
if ((xPASS>=99)&&(xPASS<=102)) {return pgm_read_byte(&police[(xPASS-99)+(D4*4)]);} //digit 0 
if ((xPASS>=103)&&(xPASS<=106)) {return pgm_read_byte(&police[(xPASS-103)+(D3*4)]);} //digit 0 
if ((xPASS>=107)&&(xPASS<=110)) {return pgm_read_byte(&police[(xPASS-107)+(D2*4)]);} //digit 0 
if ((xPASS>=111)&&(xPASS<=114)) {return pgm_read_byte(&police[(xPASS-111)+(D1*4)]);} //digit 0 
if ((xPASS>=115)&&(xPASS<=118)) {return pgm_read_byte(&police[(xPASS-115)+(D0*4)]);} //digit 0 
return 0x00;
};

void Tiny_Flip(uint8_t Scan,Sprite *DATA){
uint8_t y,x,MAXx,MAXy; 
if (Scan==0) {MAXx=109;MAXy=7;}else{MAXx=128;MAXy=8;}
for (y = 0; y < MAXy; y++){ 
    SSD1306.ssd1306_send_command(0xb0 + y);
    SSD1306.ssd1306_send_command(0x00);
    SSD1306.ssd1306_send_command(0x10);
    SSD1306.ssd1306_send_data_start();
for (x = 0; x < MAXx; x++){
SSD1306.ssd1306_send_byte(Recupe(x,y,DATA));}
SSD1306.ssd1306_send_data_stop();
}}

void Sound(uint8_t freq,uint8_t dur){
if (freq==0) {delay(dur);goto END;}
for (uint8_t t=0;t<dur;t++){
if (freq!=0)digitalWrite(4,HIGH); 
for (uint8_t t=0;t<(255-freq);t++){
_delay_us(1);}
digitalWrite(4,LOW);
for (uint8_t t=0;t<(255-freq);t++){
_delay_us(1); }}
END:;
}
