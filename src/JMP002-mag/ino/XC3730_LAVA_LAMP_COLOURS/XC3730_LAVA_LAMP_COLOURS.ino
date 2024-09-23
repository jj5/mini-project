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
#define BACK_COLOUR 0x0010
//assume minimum is 0
#define BLOB_T_MAX 7
#define AMBIENT 5
char blobx[BLOB_COUNT];
char bloby[BLOB_COUNT];
char blobt[BLOB_COUNT];
char heatMap[ROWS][COLUMNS]={
  {-2,-2,-2,-2,-2},
  {-1, 0, 0, 0,-1},
  { 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0},
  { 0, 1, 1, 1, 0},
  { 1, 1, 2, 1, 1}
};
uint16_t tempColour[BLOB_T_MAX+1]={
  0x8B98,0x9B18,0xAA98,0xBA18,0xC998,0xD918,0xE898,0xF818  //hot=magenta, cool=purple
  //0x1BD5,0x3BD3,0x5BD1,0x7BCF,0x9BCD,0xBBCB,0xDBC9,0xFBC7  //hot=orange, cool=blue
  //MAGENTA,MAGENTA,MAGENTA,MAGENTA,MAGENTA,MAGENTA,MAGENTA,MAGENTA //all the same Magenta
};

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(COLUMNS, ROWS, 13,MATRIX_CFG);

const int button_pin = 7;
const int led_pin = 13;

void setup() {
  char i;
  matrix.begin();
  matrix.setBrightness(2);
  matrix.fillScreen(0);
  matrix.show();
  for(i=0;i<BLOB_COUNT;i++){
    blobx[i]=i%COLUMNS;
    bloby[i]=i/COLUMNS+ROWS/2;
    blobt[i]=0;
  }
  pinMode( button_pin, INPUT );
  pinMode( led_pin, OUTPUT );
}

void loop() {
  char i,j;
  int button_state = digitalRead( button_pin );
  if ( button_state == HIGH ) {
    digitalWrite( led_pin, HIGH );
  }
  else {
    digitalWrite( led_pin, LOW );
  }
  for(j=0;j<BLOB_COUNT;j++){
    updateBlob(j);
    matrix.fillScreen(BACK_COLOUR);
    for(i=0;i<BLOB_COUNT;i++){
      matrix.drawPixel(blobx[i],bloby[i],tempColour[blobt[i]]);
    }
    matrix.show();
    delay(150);
  }
}

void updateBlob(char j){
  char dx,dy;
  dx=0;
  dy=0;
  blobt[j]=blobt[j]+heatMap[bloby[j]][blobx[j]];
  if(blobt[j]<0){blobt[j]=0;}
  if(blobt[j]>BLOB_T_MAX){blobt[j]=BLOB_T_MAX;}
  dy=AMBIENT-blobt[j]+random(-1,2);
  if(dy<-1){dy=-1;}
  if(dy>1){dy=1;}
  if(dy==0){dx=random(-1,2);}
  if(testMove(j,dx,dy)==0){
    dx=random(-1,2);    //try a random shuffle
    dy=random(-1,2);
    if(testMove(j,dx,dy)==0){ //keep the same
      dx=0;
      dy=0;
    }
  }
  bloby[j]=bloby[j]+dy;
  blobx[j]=blobx[j]+dx;  
}

char testMove(char j, char dx, char dy){    //see if move valid (ie no blob in the way)
  char i;
  for(i=0;i<BLOB_COUNT;i++){
    if(i!=j){
      if(((blobx[j]+dx)==blobx[i])&&((bloby[j]+dy)==bloby[i])){
        return 0; //collision
      }
    }
  }
  if((blobx[j]+dx)<0){return 0;}    //OOB
  if((blobx[j]+dx)>(COLUMNS-1)){return 0;}    //OOB
  if((bloby[j]+dy)<0){return 0;}    //OOB
  if((bloby[j]+dy)>(ROWS-1)){return 0;}    //OOB
  return 1;
}