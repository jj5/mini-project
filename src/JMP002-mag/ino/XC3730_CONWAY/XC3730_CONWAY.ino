#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

#define COLUMNS 5
#define ROWS 8
#define MATRIX_CFG NEO_MATRIX_TOP+NEO_MATRIX_RIGHT+NEO_MATRIX_COLUMNS+NEO_MATRIX_PROGRESSIVE,NEO_GRB+NEO_KHZ800
//some colours with nice names
#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0  
#define WHITE 0xFFFF
#define GREY 0x8410

#define BLOB_COUNT 11
#define FORE_COLOUR WHITE
#define BACK_COLOUR BLACK

char oldState[ROWS][COLUMNS]={
  {0,0,0,0,0},
  {0,0,0,0,0},
  {0,0,1,0,0},
  {1,0,1,0,0},
  {0,1,1,0,0},
  {0,0,0,0,0},
  {0,0,0,0,0},
  {0,0,0,0,0}
};

char newState[ROWS][COLUMNS];

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(COLUMNS, ROWS, 13,MATRIX_CFG);

void setup() {
  matrix.begin();
  matrix.setBrightness(4);
  matrix.fillScreen(0);
  matrix.show();
}

void loop() {
  char i,j;
  for(i=0;i<COLUMNS;i++){
    for(j=0;j<ROWS;j++){
      if(oldState[j][i]>0){
        matrix.drawPixel(i,j,FORE_COLOUR);  
      }else{
        matrix.drawPixel(i,j,BACK_COLOUR);
      }
    }
  }
  matrix.show();
  updateState();
  delay(100);
}

char getCell(char x, char y){
  char n=0;
  char i,j;
  for(i=-1;i<2;i++){
    for(j=-1;j<2;j++){
      if((i!=0)||(j!=0)){ //don't count self
        if(oldState[(y+j+ROWS)%ROWS][(x+i+COLUMNS)%COLUMNS]>0){n=n+1;}
      }
    }
  }
  if(oldState[y][x]>0){
    if((n==2)||(n==3)){return 1;}
  }else{
    if(n==3){return 1;}
  }
  return 0;
}

void updateState(){   //update into newState then copy to oldState
  char i,j;
  for(i=0;i<COLUMNS;i++){
    for(j=0;j<ROWS;j++){
      newState[j][i]=getCell(i,j);
    }
  }
  for(i=0;i<COLUMNS;i++){
    for(j=0;j<ROWS;j++){
      oldState[j][i]=newState[j][i];
    }
  }
}