#include "XC4630d.c"
#include "bitmaps.c"
#include "Keyboard.h"

uint8_t kpKeys[]={KEY_KP_0,KEY_KP_1,KEY_KP_2,KEY_KP_3,KEY_KP_4,KEY_KP_5,KEY_KP_6,KEY_KP_7,KEY_KP_8,KEY_KP_9};

#define FGC CYAN
#define BGC BLACK
#define ROWS 3
#define COLUMNS 4
#define BUTTON_WIDTH 80
#define BUTTON_HEIGHT 80
#define BUTTON_COUNT 12

int codes[BUTTON_COUNT]={176,188,189,190,169,128,163,165,177,247,133,134};

const unsigned char* bitmaps[BUTTON_COUNT]={
  u0176_degrees,u0188_1_4,u0189_1_2,u0190_3_4,
  u0169_copyright,u0128_euro,u0163_pound,u0165_yen,
  u0177_plus_minus,u0247_divide,u0133_ellipsis,u0134_dagger
};

bool isPressed[BUTTON_COUNT];

void setup() {
  char i,w,h;
  int x,y;
  Serial.begin(115200); //USB-serial
  Keyboard.begin();
  XC4630_init();
  XC4630_rotate(2);
  XC4630_clear(BGC);          //Blank screen
  for(i=0;i<BUTTON_COUNT;i++){
    XC4630_mcimage(getX(i),getY(i),bitmaps[i],FGC,BGC);
  }
}

void loop() {
  int x,y;
  char i,w,h;
  bool t;
  x=XC4630_touchx();
  y=XC4630_touchy();
  for(i=0;i<BUTTON_COUNT;i++){
    t=getPressed(i,x,y);
    if(t!=isPressed[i]){
      isPressed[i]=t;
      if(t){
        XC4630_mcimage(getX(i),getY(i),bitmaps[i],BGC,FGC);
        sendUnicode(codes[i]);
        delay(50);    //to help with debouncing
      }else{
        XC4630_mcimage(getX(i),getY(i),bitmaps[i],FGC,BGC);
      }
    }
  }
}

void sendUnicode(int n){  //eg ALT-0163 requires n=163 = 0xA3
  Keyboard.press(KEY_LEFT_ALT);
  delay(1);
  Keyboard.write(kpKeys[(n/1000)%10]);
  delay(1);
  Keyboard.write(kpKeys[(n/100)%10]);
  delay(1);
  Keyboard.write(kpKeys[(n/10)%10]);
  delay(1);
  Keyboard.write(kpKeys[(n/1)%10]);
  delay(1);
  Keyboard.releaseAll();
}

int getX(char i){
  char w;
  w=XC4630_imagewidth(bitmaps[i]);
  return (i%COLUMNS)*BUTTON_WIDTH+(BUTTON_WIDTH-w)/2;
}

int getY(char i){
  char h;
  h=XC4630_imageheight(bitmaps[i]);
  return (i/COLUMNS)*BUTTON_HEIGHT+(BUTTON_HEIGHT-h)/2;
}

bool getPressed(char i,int tx, int ty){
  char w,h;
  int x,y;  
  w=XC4630_imagewidth(bitmaps[i]);
  h=XC4630_imageheight(bitmaps[i]);
  x=getX(i);
  y=getY(i);
  if(tx<x){return false;}
  if(tx>x+w){return false;}
  if(ty<y){return false;}
  if(ty>y+h){return false;}
  return true;
}
