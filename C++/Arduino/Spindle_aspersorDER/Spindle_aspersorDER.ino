#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;
#include <TouchScreen.h>
#define MINPRESSURE 200
#define MAXPRESSURE 1000
#include "bitmaps.h"

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define GRAY    222222

// ALL Touch panels and wiring is DIFFERENT
// copy-paste results from TouchScreen_Calibr_native.ino
const int XP = 8, XM = A2, YP = A3, YM = 9; //240x320 ID=0x9341
const int TS_LEFT = 140, TS_RT = 860, TS_TOP = 125, TS_BOT = 880;
const int Orientation = 1; //0 and 2 are portrait and 1 and 3 landscape

#include <AccelStepper.h>

#include <DueFlashStorage.h>
DueFlashStorage dueFlashStorage;


/*     Secuencia para hacer aspersores */
/*     Definicion relevadores */
const int relayA = 14; // relay aspiradora
const int spindleD = 15; // Spindle derecho
const int spindleC = 16; //Spindle central
const int spindleI = 17; // Spindle izquierdo

/* Definir pines de prensa fija*/
const int stepPinPF = 18;
const int dirPinPF = 19;
const int enablePF = 20;
const int finish_PF = A5;

/* Definir pines de prensa movil*/
const int stepPinPM = 26 ;
const int dirPinPM = 28;
const int enablePM = 30;
const int finish_PM = A6;

/* Definir pines de desplazamiento prensa movil*/
const int stepPinM = 27;
const int dirPinM = 29;
AccelStepper stepperM(AccelStepper::DRIVER, stepPinM, dirPinM);
const int enableM = 31;
const int finish_M = A7;

/* Definir pines para mover spindle izquierdo */
const int stepPinSPi = 33;
const int dirPinSPi = 35;
const int enableSPi = 37;
const int finish_SPi = A8;

/* Definir pines para mover spindle central */
const int stepPinSPc = 39;
const int dirPinSPc = 41;
const int enableSPc = 43;
const int finish_SPc = A9;

/* Definir pines para mover spindle derecho */
const int stepPinSPd = 45;
const int dirPinSPd = 47;
const int enableSPd = 49;
const int finish_SPd = A10;

/* Botón para paro de emergencia del perforador*/
const int StartStop = 14;
volatile bool AuxStartStop = false;

/* Variables auxiliares */
int aux=0;
int central_pos=0;
int left_pos=0;
int aux_left;
int right_pos=0;
int aux_right;
const int PressMMRev = 8;
const int SpindlesMMRev = 8;
const int MotorSteps = 200;
const int MicroSteps = 16;
const int moveMicroSteps = 4;
const float PressMMSteps = (MotorSteps*MicroSteps)/PressMMRev;
const float movePressMMSteps = -100;
const float SpindlesMMSteps = (MotorSteps*MicroSteps)/SpindlesMMRev;


//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
//**************************************************************************************

struct Configuration {
  float a;
  float b;
  float c;
  float d;
  float e;

};

  /*
  float Centralmm1; //= 14.45; // +0.15 cambio cortador 3/09/2022 SSG
  float Leftmm1; //= 27.3;
  float Rightmm1; //= 29; //-.5
  float PressFmm1; //= 6;
  float PressMmm1; //= 7.5;

float Centralmm;
float Leftmm;
float Rightmm;
float PressFmm;
float PressMmm;
*/
//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
// initialize one struct
Configuration configuration;

// Serial comunication
String inputstring = "";
bool input_string_complete = false;

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

Adafruit_GFX_Button on_btn, off_btn;

int xpos, ypos;     //Touch_getXY() updates global vars

//Screen auxs
bool homeInit = false;
bool setupInit = false;
bool motorsInit = false;
int waiting = 0;

byte screens = 0;
byte pressedButtonsPress = 0;

//Ending point of sliders
const int maxSliderSize = 300;

// Size slider vars
int sizeSliderX = 300;
const int sizeSliderY = 65;
const int sizeSliderInit = 100;
int tubeSize = 6000;

// Spacing Quantity slider vars
int spacingQtySliderX = 215;
const int  spacingQtySliderY = 98;
const int spacingQtySliderInit = 215;
int spacingQty = 1;

//Spacings
int activeColor = CYAN;
int spaceSlider1[] = {15, 145, 70, 145 + 70, 215};
int spaceSlider2[] = {95, 145, 70, 145 + 70, 145};
int spaceSlider3[] = {174, 145, 70, 145 + 70, 145};
int spaceSlider4[] = {253, 145, 70, 145 + 70, 145};

int space1 = 100;
int space1Qty = 59;
int space2 = 50;
int space2Qty = 0;
int space3 = 50;
int space3Qty = 0;
int space4 = 50;
int space4Qty = 0;

/************************************************************************ TOUCH S *****************************************************************************************/
/************************************************************************ TOUCH S *****************************************************************************************/
/************************************************************************ TOUCH S *****************************************************************************************/
bool Touch_getXY(void){
  TSPoint tp = ts.getPoint();
  pinMode(YP, OUTPUT);      //restore shared pins
  pinMode(XM, OUTPUT);
  digitalWrite(YP, HIGH);   //because TFT control pins
  digitalWrite(XM, HIGH);
  bool pressed = (tp.z > MINPRESSURE && tp.z < MAXPRESSURE);
  if (pressed) {
    switch (Orientation) {
      case 0:
        xpos = map(tp.x, TS_LEFT, TS_RT, 0, tft.width());
        ypos = map(tp.y, TS_TOP, TS_BOT, 0, tft.height());
        break;
      case 1:
        xpos = map(tp.y, TS_TOP, TS_BOT, 0, tft.width());
        ypos = map(tp.x, TS_RT, TS_LEFT, 0, tft.height());
        break;
      case 2:
        xpos = map(tp.x, TS_RT, TS_LEFT, 0, tft.width());
        ypos = map(tp.y, TS_BOT, TS_TOP, 0, tft.height());
        break;
      case 3:
        xpos = map(tp.y, TS_BOT, TS_TOP, 0, tft.width());
        ypos = map(tp.x, TS_LEFT, TS_RT, 0, tft.height());
        break;
    }
  }
  return pressed;
}

void homeDisplay(){
    tft.fillScreen(BLACK);
    tft.drawRoundRect(3, 3, 314, 234, 6, CYAN);
    tft.setCursor(102,15);
    tft.setTextSize(2);
    tft.println("PERFORADOR");


    int x = 35, y = 50, w = 64, h = 64;

    int SZ = homeSetupImg[2]*homeSetupImg[3]/8;
    uint8_t sram[SZ];
    memcpy_P(sram, setupImg, SZ );
    tft.drawBitmap(homeSetupImg[0],homeSetupImg[1], setupImg, homeSetupImg[2], homeSetupImg[3], BLACK, WHITE);

    SZ = homeToolBoxImg[2]*homeToolBoxImg[3]/8;
    //uint8_t sram[SZ];
    memcpy_P(sram, toolBoxImg, SZ);
    tft.drawBitmap(homeToolBoxImg[0], homeToolBoxImg[1], toolBoxImg, homeToolBoxImg[2], homeToolBoxImg[3], BLACK, WHITE);

    SZ = homeSteppersImg[2]*homeSteppersImg[3]/8;
    //uint8_t sram[SZ];
    memcpy_P(sram, steppersImg, SZ);
    tft.drawBitmap(homeSteppersImg[0], homeSteppersImg[1], steppersImg, homeSteppersImg[2], homeSteppersImg[3], BLACK, WHITE);

    SZ = homeStartImg[2]*homeStartImg[3]/8;
    //uint8_t sram[SZ];
    memcpy_P(sram, startImg, SZ);
    tft.drawBitmap(homeStartImg[0], homeStartImg[1], startImg, homeStartImg[2], homeStartImg[3], BLACK, WHITE);

    tft.setTextSize(1);
    tft.setCursor(30,120);
    tft.print("Configuraci"); tft.print(char(162)); tft.print("n");
    tft.print("      ");
    tft.print("Motores");
    tft.print("         ");
    tft.print("Empezar");
    tft.setCursor(30,220);
    tft.print(" Herramientas");

    tft.setCursor(125,170);
    tft.setTextSize(1);
    tft.print("Largo del tubo ");
    tft.setTextColor(RED);
    tft.print(tubeSize);
    tft.setTextColor(WHITE);
    tft.print(" mm");
    tft.setCursor(125,185);
    tft.print("Usando ");
    tft.print(spacingQty);
    tft.println(" espaciados diferentes");
    switch(spacingQty){
      case 1:
        tft.setCursor(125,200);
        tft.print("1 - ");
        tft.setTextColor(RED);
        tft.print(space1);
        tft.setTextColor(WHITE);
        tft.print(" mm");
        break;
      case 2:
        tft.setCursor(125,200);
        tft.print("1 - ");
        tft.setTextColor(RED);
        tft.print(space1);
        tft.setTextColor(WHITE);
        tft.print(" mm");
        tft.setCursor(225,200);
        tft.print("2 - ");
        tft.setTextColor(RED);
        tft.print(space2);
        tft.setTextColor(WHITE);
        tft.print(" mm");
        break;
      case 3:
        tft.setCursor(125,200);
        tft.print("1 - ");
        tft.setTextColor(RED);
        tft.print(space1);
        tft.setTextColor(WHITE);
        tft.print(" mm");
        tft.setCursor(225,200);
        tft.print("2 - ");
        tft.setTextColor(RED);
        tft.print(space2);
        tft.setTextColor(WHITE);
        tft.print(" mm");
        tft.setCursor(125,215);
        tft.print("3 - ");
        tft.setTextColor(RED);
        tft.print(space3);
        tft.setTextColor(WHITE);
        tft.print(" mm");
        break;
      case 4:
        tft.setCursor(125,200);
        tft.print("1 - ");
        tft.setTextColor(RED);
        tft.print(space1);
        tft.setTextColor(WHITE);
        tft.print(" mm");
        tft.setCursor(225,200);
        tft.print("2 - ");
        tft.setTextColor(RED);
        tft.print(space2);
        tft.setTextColor(WHITE);
        tft.print(" mm");
        tft.setCursor(125,215);
        tft.print("3 - ");
        tft.setTextColor(RED);
        tft.print(space3);
        tft.setTextColor(WHITE);
        tft.print(" mm");
        tft.setCursor(225,215);
        tft.print("4 - ");
        tft.setTextColor(RED);
        tft.print(space4);
        tft.setTextColor(WHITE);
        tft.print(" mm");
        break;
    }
    homeInit=true;
}

void pressedButtons(){
  int SZ = homeSetupImg[2]*homeSetupImg[3]/8;
  uint8_t sram[SZ];
  switch (pressedButtonsPress){
    case 0:
      SZ = homeSetupImg[2]*homeSetupImg[3]/8;
      //uint8_t sram[SZ];
      memcpy_P(sram, setupImg, SZ);
      tft.drawBitmap(homeSetupImg[0],homeSetupImg[1], setupImg, homeSetupImg[2], homeSetupImg[3], CYAN, BLACK);
      break;
    case 1:
      SZ = homeToolBoxImg[2]*homeToolBoxImg[3]/8;
      //uint8_t sram[SZ];
      memcpy_P(sram, toolBoxImg, SZ);
      tft.drawBitmap(homeToolBoxImg[0], homeToolBoxImg[1], toolBoxImg, homeToolBoxImg[2], homeToolBoxImg[3], CYAN, BLACK);
      break;
    case 2:
      SZ = homeSteppersImg[2]*homeSteppersImg[3]/8;
      //uint8_t sram[SZ];
      memcpy_P(sram, steppersImg, SZ);
      tft.drawBitmap(homeSteppersImg[0], homeSteppersImg[1], steppersImg, homeSteppersImg[2], homeSteppersImg[3], CYAN, BLACK);
      break;
    case 3:
      SZ = homeStartImg[2]*homeStartImg[3]/8;
      //uint8_t sram[SZ];
      memcpy_P(sram, startImg, SZ);
      tft.drawBitmap(homeStartImg[0], homeStartImg[1], startImg, homeStartImg[2], homeStartImg[3], CYAN, BLACK);
      break;
    case 4:
      SZ = setupHomeImg[2]*setupHomeImg[3]/8;
      //uint8_t sram[SZ];
      memcpy_P(sram, homeImg, SZ);
      tft.drawBitmap(setupHomeImg[0], setupHomeImg[1], homeImg, setupHomeImg[2], setupHomeImg[3], CYAN, BLACK);
      break;
  }
  delay(500);
}

void setupDisplay(){
    tft.fillScreen(BLACK);
    tft.drawRoundRect(2, 2, 316, 236, 6, CYAN);
    tft.setCursor(102,15);
    tft.setTextSize(2);
    tft.println("PERFORADOR");

    tft.setTextSize(1);
    tft.println("");
    tft.print(" Establece el patr"); tft.print(char(162)); tft.print("n de perforaci"); tft.print(char(162)); tft.println("n");
    tft.println("");
    tft.println("");
    tft.print(" Tama"); tft.print(char(164)); tft.println("o de tubo");
    tft.println("");
    tft.println("");
    tft.println("");
    tft.println(" Cantidad de espaciados diferentes");
    tft.println("");
    tft.println("");
    tft.println("");
    tft.print("  Espaciado 1");
    tft.print(" ");
    tft.print(" Espaciado 2");
    tft.print(" ");
    tft.print(" Espaciado 3");
    tft.print(" ");
    tft.print(" Espaciado 4");
    tft.setCursor(20,225);
    tft.print(space1Qty);
    tft.print(" x ");
    tft.print(space1);
    if (space1*space1Qty<tubeSize){
      tft.setTextColor(GREEN);
    }else{tft.setTextColor(RED);}
    tft.fillRect(260,225,40,8,BLACK);
    tft.setCursor(260,225);
    tft.print(space1*space1Qty);
    tft.setTextColor(WHITE);
    tft.print(" mm");
    if(spacingQty>1){
      tft.setCursor(70,225);
      tft.print(" + ");
      tft.print(space2Qty);
      tft.print(" x ");
      tft.print(space2);
      if (space1*space1Qty+space2*space2Qty<tubeSize){
        tft.setTextColor(GREEN);
      }else{tft.setTextColor(RED);}
      tft.fillRect(260,225,40,8,BLACK);
      tft.setCursor(260,225);
      tft.print(space1*space1Qty+space2*space2Qty);
      tft.setTextColor(WHITE);
      tft.print(" mm");
      if(spacingQty>2){
        tft.setCursor(125,225);
        tft.print(" + ");
        tft.print(space3Qty);
        tft.print(" x ");
        tft.print(space3);
        if (space1*space1Qty+space2*space2Qty+space3*space3Qty<tubeSize){
          tft.setTextColor(GREEN);
        }else{tft.setTextColor(RED);}
        tft.fillRect(260,225,40,8,BLACK);
        tft.setCursor(260,225);
        tft.print(space1*space1Qty+space2*space2Qty+space3*space3Qty);
        tft.setTextColor(WHITE);
        tft.print(" mm");
        if(spacingQty>3){
          tft.setCursor(180,225);
          tft.print(" + ");
          tft.print(space4Qty);
          tft.print(" x ");
          tft.print(space4);
          if (space1*space1Qty+space2*space2Qty+space3*space3Qty+space4*space4Qty<tubeSize){
            tft.setTextColor(GREEN);
          }else{tft.setTextColor(RED);}
          tft.fillRect(260,225,40,8,BLACK);
          tft.setCursor(260,225);
          tft.print(space1*space1Qty+space2*space2Qty+space3*space3Qty+space4*space4Qty);
          tft.setTextColor(WHITE);
          tft.print(" mm");
        }
      }
    }


    //Home img
    int x = 280, y=10, w = 30, h = 30;
    const int SZ = w * h / 8;
    uint8_t sram[SZ];
    memcpy_P(sram, homeImg, SZ);
    tft.drawBitmap(x,y, homeImg, 30,30, BLACK, WHITE);

    //TubeSizeSlider
    tft.setCursor(185, 74);
    tft.print(tubeSize);
    tft.println(" mm");

    tft.drawFastHLine(sizeSliderInit,sizeSliderY,maxSliderSize-sizeSliderInit,CYAN);
    tft.fillCircle(sizeSliderX,sizeSliderY,3,CYAN);
    tft.drawCircle(sizeSliderX,sizeSliderY,4,WHITE);
    tft.drawCircle(sizeSliderInit,sizeSliderY,4,WHITE);
    tft.drawCircle(maxSliderSize,sizeSliderY,4,WHITE);

    //SpacingAmount Slider
    tft.setCursor(255, 107);
    tft.println(spacingQty);

    tft.drawFastHLine(spacingQtySliderInit,spacingQtySliderY,maxSliderSize-spacingQtySliderInit,CYAN);
    tft.fillCircle(spacingQtySliderX,spacingQtySliderY,3,CYAN);
    tft.drawCircle(spacingQtySliderX,spacingQtySliderY,4,WHITE);
    tft.drawCircle(spacingQtySliderInit,spacingQtySliderY,4,WHITE);
    tft.drawCircle(maxSliderSize,spacingQtySliderY,4,WHITE);

    //Spacing slider
    activeColor=CYAN;
    tft.setTextColor(WHITE);
    tft.drawFastVLine(spaceSlider1[0], spaceSlider1[1], spaceSlider1[2], activeColor);
    tft.drawCircle(spaceSlider1[0], spaceSlider1[1], 4, WHITE);
    tft.drawCircle(spaceSlider1[0],spaceSlider1[3],4,WHITE);
    tft.drawCircle(spaceSlider1[0],spaceSlider1[4] ,4 ,WHITE);
    tft.fillCircle(spaceSlider1[0],spaceSlider1[4] ,3 ,CYAN);
    tft.setCursor(spaceSlider1[0]+10,spaceSlider1[1]+spaceSlider1[2]/2-4);
    tft.print(space1);
    tft.setCursor(spaceSlider1[0]+10,spaceSlider1[1]+spaceSlider1[2]/2+5);
    tft.print("mm");
    tft.setCursor(spaceSlider1[0]+35,spaceSlider1[1]+spaceSlider1[2]/2-25);
    tft.print("Cant");
    tft.setCursor(spaceSlider1[0]+45,spaceSlider1[1]+spaceSlider1[2]/2-6);
    tft.print("+");
    tft.setCursor(spaceSlider1[0]+45,spaceSlider1[1]+spaceSlider1[2]/2+8);
    tft.print(space1Qty);
    tft.setCursor(spaceSlider1[0]+45,spaceSlider1[1]+spaceSlider1[2]/2+23);
    tft.print("-");

    if (spacingQty>1){activeColor=CYAN; tft.setTextColor(WHITE);} else {activeColor=GRAY; tft.setTextColor(GRAY);}
    tft.drawFastVLine(spaceSlider2[0], spaceSlider2[1], spaceSlider2[2], activeColor);
    tft.drawCircle(spaceSlider2[0], spaceSlider2[1], 4, WHITE);
    tft.drawCircle(spaceSlider2[0],spaceSlider2[3],4,WHITE);
    tft.drawCircle(spaceSlider2[0],spaceSlider2[4] ,4 ,WHITE);
    tft.fillCircle(spaceSlider2[0],spaceSlider2[4] ,3 ,activeColor);
    tft.setCursor(spaceSlider2[0]+10,spaceSlider2[1]+spaceSlider2[2]/2-4);
    tft.print(space2);
    tft.setCursor(spaceSlider2[0]+10,spaceSlider2[1]+spaceSlider2[2]/2+5);
    tft.print("mm");
    tft.setCursor(spaceSlider2[0]+35,spaceSlider2[1]+spaceSlider2[2]/2-25);
    tft.print("Cant");
    tft.setCursor(spaceSlider2[0]+45,spaceSlider2[1]+spaceSlider2[2]/2-6);
    tft.print("+");
    tft.setCursor(spaceSlider2[0]+45,spaceSlider2[1]+spaceSlider2[2]/2+8);
    tft.print(space2Qty);
    tft.setCursor(spaceSlider2[0]+45,spaceSlider2[1]+spaceSlider2[2]/2+23);
    tft.print("-");

    if (spacingQty>2){activeColor=CYAN; tft.setTextColor(WHITE);} else {activeColor=GRAY; tft.setTextColor(GRAY);}
    tft.drawFastVLine(spaceSlider3[0], spaceSlider3[1], spaceSlider3[2], activeColor);
    tft.drawCircle(spaceSlider3[0], spaceSlider3[1], 4, WHITE);
    tft.drawCircle(spaceSlider3[0],spaceSlider3[3],4,WHITE);
    tft.drawCircle(spaceSlider3[0],spaceSlider3[4] ,4 ,WHITE);
    tft.fillCircle(spaceSlider3[0],spaceSlider3[4] ,3 ,activeColor);
    tft.setCursor(spaceSlider3[0]+10,spaceSlider3[1]+spaceSlider3[2]/2-4);
    tft.print(space3);
    tft.setCursor(spaceSlider3[0]+10,spaceSlider3[1]+spaceSlider3[2]/2+5);
    tft.print("mm");
    tft.setCursor(spaceSlider3[0]+35,spaceSlider3[1]+spaceSlider3[2]/2-25);
    tft.print("Cant");
    tft.setCursor(spaceSlider3[0]+45,spaceSlider3[1]+spaceSlider3[2]/2-6);
    tft.print("+");
    tft.setCursor(spaceSlider3[0]+45,spaceSlider3[1]+spaceSlider3[2]/2+8);
    tft.print(space3Qty);
    tft.setCursor(spaceSlider3[0]+45,spaceSlider3[1]+spaceSlider3[2]/2+23);
    tft.print("-");

    if (spacingQty>3){activeColor=CYAN; tft.setTextColor(WHITE);}  else {activeColor=GRAY; tft.setTextColor(GRAY);}
    tft.drawFastVLine(spaceSlider4[0], spaceSlider4[1], spaceSlider4[2], activeColor);
    tft.drawCircle(spaceSlider4[0], spaceSlider4[1], 4, WHITE);
    tft.drawCircle(spaceSlider4[0],spaceSlider4[3],4,WHITE);
    tft.drawCircle(spaceSlider4[0],spaceSlider4[4] ,4 ,WHITE);
    tft.fillCircle(spaceSlider4[0],spaceSlider4[4] ,3 ,activeColor);
    tft.setCursor(spaceSlider4[0]+10,spaceSlider4[1]+spaceSlider4[2]/2-4);
    tft.print(space4);
    tft.setCursor(spaceSlider4[0]+10,spaceSlider4[1]+spaceSlider4[2]/2+5);
    tft.print("mm");
    tft.setCursor(spaceSlider4[0]+35,spaceSlider4[1]+spaceSlider4[2]/2-25);
    tft.print("Cant");
    tft.setCursor(spaceSlider4[0]+45,spaceSlider4[1]+spaceSlider4[2]/2-6);
    tft.print("+");
    tft.setCursor(spaceSlider4[0]+45,spaceSlider4[1]+spaceSlider4[2]/2+8);
    tft.print(space4Qty);
    tft.setCursor(spaceSlider4[0]+45,spaceSlider4[1]+spaceSlider4[2]/2+23);
    tft.print("-");

    setupInit=true;
}

void slideSpacingSliders(int sliding, bool active = false){
  switch (sliding){
    case 1:
      if (active == true){
        tft.fillCircle(spaceSlider1[0],spaceSlider1[4] ,4 ,BLACK);
        spaceSlider1[4] = ypos;
        space1=int(2.1429*ypos-260.71);
      }
      activeColor=CYAN;
      tft.setTextColor(WHITE);
      tft.drawFastVLine(spaceSlider1[0], spaceSlider1[1], spaceSlider1[2], activeColor);
      tft.drawCircle(spaceSlider1[0], spaceSlider1[1], 4, WHITE);
      tft.drawCircle(spaceSlider1[0],spaceSlider1[3],4,WHITE);
      tft.drawCircle(spaceSlider1[0],spaceSlider1[4] ,4 ,WHITE);
      tft.fillCircle(spaceSlider1[0],spaceSlider1[4] ,3 ,CYAN);
      tft.setCursor(spaceSlider1[0]+10,spaceSlider1[1]+spaceSlider1[2]/2-4);
      tft.fillRect(spaceSlider1[0]+10,spaceSlider1[1]+spaceSlider1[2]/2-4,20,8,BLACK);
      tft.print(space1);
      tft.setCursor(spaceSlider1[0]+10,spaceSlider1[1]+spaceSlider1[2]/2+5);
      tft.print("mm");
      tft.setCursor(spaceSlider1[0]+35,spaceSlider1[1]+spaceSlider1[2]/2-25);
      tft.print("Cant");
      tft.setCursor(spaceSlider1[0]+45,spaceSlider1[1]+spaceSlider1[2]/2-6);
      tft.print("+");
      tft.fillRect(spaceSlider1[0]+45,spaceSlider1[1]+spaceSlider1[2]/2+8,20,10,BLACK);
      tft.setCursor(spaceSlider1[0]+45,spaceSlider1[1]+spaceSlider1[2]/2+8);
      tft.print(space1Qty);
      tft.setCursor(spaceSlider1[0]+45,spaceSlider1[1]+spaceSlider1[2]/2+23);
      tft.print("-");
      if (spacingQty<2){
        tft.fillRect(20,225,310-20,8,BLACK);
      } else{tft.fillRect(20,225,55,8,BLACK);}
      tft.setCursor(20,225);
      tft.print(space1Qty);
      tft.print(" x ");
      tft.print(space1);
      if (space1*space1Qty<tubeSize){
        tft.setTextColor(GREEN);
      }else{tft.setTextColor(RED);}
      tft.fillRect(260,225,40,8,BLACK);
      tft.setCursor(260,225);
      tft.print(space1*space1Qty);
      tft.setTextColor(WHITE);
      tft.print(" mm");
      break;
    case 2:
      if (active == true){
        tft.fillCircle(spaceSlider2[0],spaceSlider2[4] ,4 ,BLACK);
        spaceSlider2[4] = ypos;
        space2=int(2.1429*ypos-260.71);
      }
      if (spacingQty>1){activeColor=CYAN; tft.setTextColor(WHITE);} else {activeColor=GRAY; tft.setTextColor(GRAY);}
      tft.drawFastVLine(spaceSlider2[0], spaceSlider2[1], spaceSlider2[2], activeColor);
      tft.drawCircle(spaceSlider2[0], spaceSlider2[1], 4, WHITE);
      tft.drawCircle(spaceSlider2[0],spaceSlider2[3],4,WHITE);
      tft.drawCircle(spaceSlider2[0],spaceSlider2[4] ,4 ,WHITE);
      tft.fillCircle(spaceSlider2[0],spaceSlider2[4] ,3 ,activeColor);
      tft.setCursor(spaceSlider2[0]+10,spaceSlider2[1]+spaceSlider2[2]/2-4);
      tft.fillRect(spaceSlider2[0]+10,spaceSlider2[1]+spaceSlider2[2]/2-4,20,8,BLACK);
      tft.print(space2);
      tft.setCursor(spaceSlider2[0]+10,spaceSlider2[1]+spaceSlider2[2]/2+5);
      tft.print("mm");
      tft.setCursor(spaceSlider2[0]+35,spaceSlider2[1]+spaceSlider2[2]/2-25);
      tft.print("Cant");
      tft.setCursor(spaceSlider2[0]+45,spaceSlider2[1]+spaceSlider2[2]/2-6);
      tft.print("+");
      tft.fillRect(spaceSlider2[0]+45,spaceSlider2[1]+spaceSlider2[2]/2+8,20,10,BLACK);
      tft.setCursor(spaceSlider2[0]+45,spaceSlider2[1]+spaceSlider2[2]/2+8);
      tft.print(space2Qty);
      tft.setCursor(spaceSlider2[0]+45,spaceSlider2[1]+spaceSlider2[2]/2+23);
      tft.print("-");
      if(spacingQty>1){
        if (spacingQty<3){
          tft.fillRect(70,225,310-70,8,BLACK);
        } else {tft.fillRect(70,225,60,8,BLACK);}
        tft.setCursor(70,225);
        tft.setTextColor(WHITE);
        tft.print(" + ");
        tft.print(space2Qty);
        tft.print(" x ");
        tft.print(space2);
        if (space1*space1Qty+space2*space2Qty<tubeSize){
          tft.setTextColor(GREEN);
        }else{tft.setTextColor(RED);}
        tft.fillRect(260,225,40,8,BLACK);
        tft.setCursor(260,225);
        tft.print(space1*space1Qty+space2*space2Qty);
        tft.setTextColor(WHITE);
        tft.print(" mm");
      }
      break;
    case 3:
      if (active == true){
        tft.fillCircle(spaceSlider3[0],spaceSlider3[4] ,4 ,BLACK);
        spaceSlider3[4] = ypos;
        space3=int(2.1429*ypos-260.71);
      }
      if (spacingQty>2){activeColor=CYAN; tft.setTextColor(WHITE);} else {activeColor=GRAY; tft.setTextColor(GRAY);}
      tft.drawFastVLine(spaceSlider3[0], spaceSlider3[1], spaceSlider3[2], activeColor);
      tft.drawCircle(spaceSlider3[0], spaceSlider3[1], 4, WHITE);
      tft.drawCircle(spaceSlider3[0],spaceSlider3[3],4,WHITE);
      tft.drawCircle(spaceSlider3[0],spaceSlider3[4] ,4 ,WHITE);
      tft.fillCircle(spaceSlider3[0],spaceSlider3[4] ,3 ,activeColor);
      tft.setCursor(spaceSlider3[0]+10,spaceSlider3[1]+spaceSlider3[2]/2-4);
      tft.fillRect(spaceSlider3[0]+10,spaceSlider3[1]+spaceSlider3[2]/2-4,20,8,BLACK);
      tft.print(space3);
      tft.setCursor(spaceSlider3[0]+10,spaceSlider3[1]+spaceSlider3[2]/2+5);
      tft.print("mm");
      tft.setCursor(spaceSlider3[0]+35,spaceSlider3[1]+spaceSlider3[2]/2-25);
      tft.print("Cant");
      tft.setCursor(spaceSlider3[0]+45,spaceSlider3[1]+spaceSlider3[2]/2-6);
      tft.print("+");
      tft.fillRect(spaceSlider3[0]+45,spaceSlider3[1]+spaceSlider3[2]/2+8,20,10,BLACK);
      tft.setCursor(spaceSlider3[0]+45,spaceSlider3[1]+spaceSlider3[2]/2+8);
      tft.print(space3Qty);
      tft.setCursor(spaceSlider3[0]+45,spaceSlider3[1]+spaceSlider3[2]/2+23);
      tft.print("-");
      if(spacingQty>2){
        if (spacingQty<4){
          tft.fillRect(130,225,310-130,8,BLACK);
        } else {tft.fillRect(130,225,60,8,BLACK);}
        tft.setCursor(130,225);
        tft.setTextColor(WHITE);
        tft.print(" + ");
        tft.print(space3Qty);
        tft.print(" x ");
        tft.print(space3);
        if (space1*space1Qty+space2*space2Qty+space3*space3Qty<tubeSize){
          tft.setTextColor(GREEN);
        }else{tft.setTextColor(RED);}
        tft.fillRect(260,225,40,8,BLACK);
        tft.setCursor(260,225);
        tft.print(space1*space1Qty+space2*space2Qty+space3*space3Qty);
        tft.setTextColor(WHITE);
        tft.print(" mm");
      }
      break;
    case 4:
      if (active == true){
        tft.fillCircle(spaceSlider4[0],spaceSlider4[4] ,4 ,BLACK);
        spaceSlider4[4] = ypos;
        space4=int(2.1429*ypos-260.71);
      }
      if (spacingQty>3){activeColor=CYAN; tft.setTextColor(WHITE);}  else {activeColor=GRAY; tft.setTextColor(GRAY);}
      tft.drawFastVLine(spaceSlider4[0], spaceSlider4[1], spaceSlider4[2], activeColor);
      tft.drawCircle(spaceSlider4[0], spaceSlider4[1], 4, WHITE);
      tft.drawCircle(spaceSlider4[0],spaceSlider4[3],4,WHITE);
      tft.drawCircle(spaceSlider4[0],spaceSlider4[4] ,4 ,WHITE);
      tft.fillCircle(spaceSlider4[0],spaceSlider4[4] ,3 ,activeColor);
      tft.setCursor(spaceSlider4[0]+10,spaceSlider4[1]+spaceSlider4[2]/2-4);
      tft.fillRect(spaceSlider4[0]+10,spaceSlider4[1]+spaceSlider4[2]/2-4,20,8,BLACK);
      tft.print(space4);
      tft.setCursor(spaceSlider4[0]+10,spaceSlider4[1]+spaceSlider4[2]/2+5);
      tft.print("mm");
      tft.setCursor(spaceSlider4[0]+35,spaceSlider4[1]+spaceSlider4[2]/2-25);
      tft.print("Cant");
      tft.setCursor(spaceSlider4[0]+45,spaceSlider4[1]+spaceSlider4[2]/2-6);
      tft.print("+");
      tft.fillRect(spaceSlider4[0]+45,spaceSlider4[1]+spaceSlider4[2]/2+8,316-(spaceSlider4[0]+45),10,BLACK);
      tft.setCursor(spaceSlider4[0]+45,spaceSlider4[1]+spaceSlider4[2]/2+8);
      tft.print(space4Qty);
      tft.setCursor(spaceSlider4[0]+45,spaceSlider4[1]+spaceSlider4[2]/2+23);
      tft.print("-");
      if(spacingQty>3){
        tft.fillRect(190,225,310-190,8,BLACK);
        tft.setCursor(190,225);
        tft.setTextColor(WHITE);
        tft.print(" + ");
        tft.print(space4Qty);
        tft.print(" x ");
        tft.print(space4);
        if (space1*space1Qty+space2*space2Qty+space3*space3Qty+space4*space4Qty<tubeSize){
          tft.setTextColor(GREEN);
        }else{tft.setTextColor(RED);}
        tft.fillRect(260,225,40,8,BLACK);
        tft.setCursor(260,225);
        tft.print(space1*space1Qty+space2*space2Qty+space3*space3Qty+space4*space4Qty);
        tft.setTextColor(WHITE);
        tft.print(" mm");
      }
      break;
  }
}

void adjustSizeSlider(){
  tft.fillCircle(sizeSliderX,sizeSliderY,4,BLACK);
  sizeSliderX = xpos;
  tft.drawFastHLine(sizeSliderInit,sizeSliderY,200,CYAN);
  tft.fillCircle(sizeSliderX,sizeSliderY,3,CYAN);
  tft.drawCircle(sizeSliderX,sizeSliderY,4,WHITE);
  tft.drawCircle(sizeSliderInit,sizeSliderY,4,WHITE);
  tft.drawCircle(maxSliderSize,sizeSliderY,4,WHITE);

  tubeSize = int(28.5*sizeSliderX - 2550);
  tft.fillRect(sizeSliderInit,74,200,8,BLACK);
  tft.setCursor(185, 74);
  tft.setTextColor(WHITE);
  tft.print(tubeSize);
  tft.println(" mm");
}

void adjustSpacingQtySlider(){
  tft.fillCircle(spacingQtySliderX,spacingQtySliderY,4,BLACK);
  spacingQtySliderX = xpos;
  tft.drawFastHLine(spacingQtySliderInit,spacingQtySliderY,maxSliderSize-spacingQtySliderInit,CYAN);
  tft.fillCircle(spacingQtySliderX,spacingQtySliderY,3,CYAN);
  tft.drawCircle(spacingQtySliderX,spacingQtySliderY,4,WHITE);
  tft.drawCircle(spacingQtySliderInit,spacingQtySliderY,4,WHITE);
  tft.drawCircle(maxSliderSize,spacingQtySliderY,4,WHITE);

  spacingQty = int(.0453*spacingQtySliderX - 8.70);
  tft.fillRect(spacingQtySliderInit,107,100,8,BLACK);
  tft.setCursor(255, 107);
  tft.setTextColor(WHITE);
  tft.print(spacingQty);
}

void motorsDisplay(){
    tft.fillScreen(BLACK);
    tft.drawRoundRect(3, 3, 314, 234, 6, CYAN);
    tft.setCursor(102,15);
    tft.setTextSize(2);
    tft.println("PERFORADOR");

    int x = 35, y = 50, w = 64, h = 64;

    int SZ = motorsSpindleLeftImg[2]*motorsSpindleLeftImg[3]/8;
    uint8_t sram[SZ];
    memcpy_P(sram, spindleLeftImg, SZ );
    tft.drawBitmap(motorsSpindleLeftImg[0],motorsSpindleLeftImg[1], spindleLeftImg, motorsSpindleLeftImg[2], motorsSpindleLeftImg[3], BLACK, WHITE);

    SZ = motorsSpindleCentreImg[2]*motorsSpindleCentreImg[3]/8;
    memcpy_P(sram, spindleCentreImg, SZ);
    tft.drawBitmap(motorsSpindleCentreImg[0], motorsSpindleCentreImg[1], spindleCentreImg, motorsSpindleCentreImg[2], motorsSpindleCentreImg[3], BLACK, WHITE);

    SZ = motorsSpindleRightImg[2]*motorsSpindleRightImg[3]/8;
    memcpy_P(sram, spindleRightImg, SZ);
    tft.drawBitmap(motorsSpindleRightImg[0], motorsSpindleRightImg[1], spindleRightImg, motorsSpindleRightImg[2], motorsSpindleRightImg[3], BLACK, WHITE);

    SZ = motorsPressFImg[2]*motorsPressFImg[3]/8;
    memcpy_P(sram, pressFImg, SZ);
    tft.drawBitmap(motorsPressFImg[0], motorsPressFImg[1], pressFImg, motorsPressFImg[2], motorsPressFImg[3], BLACK, WHITE);

    SZ = motorsPressMImg[2]*motorsPressMImg[3]/8;
    memcpy_P(sram, pressMImg, SZ);
    tft.drawBitmap(motorsPressMImg[0], motorsPressMImg[1], pressMImg, motorsPressMImg[2], motorsPressMImg[3], BLACK, WHITE);

    SZ = motorsMovePressImg[2]*motorsMovePressImg[3]/8;
    memcpy_P(sram, movePressImg, SZ);
    tft.drawBitmap(motorsMovePressImg[0], motorsMovePressImg[1], movePressImg, motorsMovePressImg[2], motorsMovePressImg[3], BLACK, WHITE);

    tft.setTextSize(1);
    tft.setCursor(30,120);
    tft.print("Spindle Izq");
    tft.print("      ");
    tft.print("Spindle Cen");
    tft.print("     ");
    tft.print("Spindle Der");
    tft.setCursor(30,220);
    tft.print("Prensa Fija");
    tft.print("   ");
    tft.print("Movimiento Prensa");
    tft.print("   ");
    tft.print("Prensa M"); tft.print(char(162)); tft.print("vil");

    x = 280, y=10, w = 30, h = 30;
    SZ = w * h / 8;
    memcpy_P(sram, homeImg, SZ);
    tft.drawBitmap(x,y, homeImg, 30,30, BLACK, WHITE);

    motorsInit=true;
}
/************************************************************************ TOUCH S *****************************************************************************************/
/************************************************************************ TOUCH S *****************************************************************************************/
/************************************************************************ TOUCH S *****************************************************************************************/


/************************************************************************ MOTORES *****************************************************************************************/
/************************************************************************ MOTORES *****************************************************************************************/
/************************************************************************ MOTORES *****************************************************************************************/
void cerosPF(){
  digitalWrite(enablePF,LOW);
  digitalWrite(dirPinPF,LOW);
  bool setup_motors=false;
  while(setup_motors==false){
  /*** Cero Prensa Fija ***/
    if(digitalRead(finish_PF)==HIGH){
      digitalWrite(stepPinPF,HIGH);
      delayMicroseconds(75);
      digitalWrite(stepPinPF,LOW);
      delayMicroseconds(75);
    }
    if(digitalRead(finish_PF)==LOW){
      setup_motors=true;
    }
  }
  delay(100);
  digitalWrite(dirPinPF,HIGH);
  while(setup_motors==true){
    if(digitalRead(finish_PF)==LOW){
      digitalWrite(stepPinPF,HIGH);
      delayMicroseconds(75);
      digitalWrite(stepPinPF,LOW);
      delayMicroseconds(75);
    }
    if(digitalRead(finish_PF)==HIGH){
      setup_motors=false;
    }
  }
  digitalWrite(enablePF,HIGH);
}
void cerosPM(){
  digitalWrite(enablePM,LOW);
  digitalWrite(dirPinPM,LOW);
  bool setup_motors=false;
  while(setup_motors==false){
  /*** Cero Prensa Fija ***/
    if(digitalRead(finish_PM)==HIGH){
      digitalWrite(stepPinPM,HIGH);
      delayMicroseconds(75);
      digitalWrite(stepPinPM,LOW);
      delayMicroseconds(75);
    }
    if(digitalRead(finish_PM)==LOW){
      setup_motors=true;
    }
  }
  delay(100);
  digitalWrite(dirPinPM,HIGH);
  while(setup_motors==true){
    if(digitalRead(finish_PM)==LOW){
      digitalWrite(stepPinPM,HIGH);
      delayMicroseconds(75);
      digitalWrite(stepPinPM,LOW);
      delayMicroseconds(75);
    }
    if(digitalRead(finish_PM)==HIGH){
      setup_motors=false;
    }
  }
  digitalWrite(enablePM,HIGH);
}
void cerosM(){
  digitalWrite(enableM,LOW);
  bool setup_motors=false;
  stepperM.moveTo(88000); //steps para mover 220mm para asegurar cero
  while(setup_motors==false){
  /*** Cero movimiento de Prensa movil ***/
    if(digitalRead(finish_M)==HIGH){
      stepperM.run();
    }
    else if(digitalRead(finish_M)==LOW){
      setup_motors=true;
      stepperM.stop();
      stepperM.setCurrentPosition(0);
      delay(100);
      stepperM.moveTo(-88000); //steps para mover 5mm para salir de fin de carrera    
    }
  }
  while(setup_motors==true){
    if(digitalRead(finish_M)==LOW){
      stepperM.run();
    }
    else if(digitalRead(finish_M)==HIGH){
      setup_motors=false;
      stepperM.stop();
      stepperM.setCurrentPosition(0);
    }
  }
  digitalWrite(enableM,HIGH);
}

void cerosSPc(){
  digitalWrite(enableSPc,LOW);
  digitalWrite(dirPinSPc,LOW);
  bool setup_motors=false;
  while(setup_motors==false){
  /*** Cero Prensa Fija ***/
    if(digitalRead(finish_SPc)==HIGH){
      digitalWrite(stepPinSPc,HIGH);
      delayMicroseconds(75);
      digitalWrite(stepPinSPc,LOW);
      delayMicroseconds(75);
    }
    if(digitalRead(finish_SPc)==LOW){
      setup_motors=true;
    }
  }
  delay(100);
  digitalWrite(dirPinSPc,HIGH);
  while(setup_motors==true){
    if(digitalRead(finish_SPc)==LOW){
      digitalWrite(stepPinSPc,HIGH);
      delayMicroseconds(75);
      digitalWrite(stepPinSPc,LOW);
      delayMicroseconds(75);
    }
    if(digitalRead(finish_SPc)==HIGH){
      setup_motors=false;
    }
  }
  digitalWrite(enableSPc,HIGH);
}
void cerosSPd(){
  digitalWrite(enableSPd,LOW);
  digitalWrite(dirPinSPd,LOW);
  bool setup_motors=false;
  while(setup_motors==false){
  /*** Cero Prensa Fija ***/
    if(digitalRead(finish_SPd)==HIGH){
      digitalWrite(stepPinSPd,HIGH);
      delayMicroseconds(75);
      digitalWrite(stepPinSPd,LOW);
      delayMicroseconds(75);
    }
    if(digitalRead(finish_SPd)==LOW){
      setup_motors=true;
    }
  }
  delay(100);
  digitalWrite(dirPinSPd,HIGH);
  while(setup_motors==true){
    if(digitalRead(finish_SPd)==LOW){
      digitalWrite(stepPinSPd,HIGH);
      delayMicroseconds(75);
      digitalWrite(stepPinSPd,LOW);
      delayMicroseconds(75);
    }
    if(digitalRead(finish_SPd)==HIGH){
      Serial.println("cero");
      serialEvent();
      setup_motors=false;
    }
  }
  digitalWrite(enableSPd,HIGH);
}
void cerosSPi(){
  digitalWrite(enableSPi,LOW);
  digitalWrite(dirPinSPi,LOW);
  bool setup_motors=false;
  while(setup_motors==false){
  /*** Cero Prensa Fija ***/
    if(digitalRead(finish_SPi)==HIGH){
      digitalWrite(stepPinSPi,HIGH);
      delayMicroseconds(75);
      digitalWrite(stepPinSPi,LOW);
      delayMicroseconds(75);
    }
    if(digitalRead(finish_SPi)==LOW){
      setup_motors=true;
    }
  }
  delay(100);
  digitalWrite(dirPinSPi,HIGH);
  while(setup_motors==true){
    if(digitalRead(finish_SPi)==LOW){
      digitalWrite(stepPinSPi,HIGH);
      delayMicroseconds(75);
      digitalWrite(stepPinSPi,LOW);
      delayMicroseconds(75);
    }
    if(digitalRead(finish_SPi)==HIGH){
      setup_motors=false;
    }
  }
  digitalWrite(enableSPi,HIGH);
}

void PressF (bool state, float movement = 0){
  cerosPF();
  if (state == HIGH){
    digitalWrite(enablePF, LOW);
    digitalWrite(dirPinPF, state);
    for ( int steps = 0; steps < movement*PressMMSteps; steps++){
      digitalWrite(stepPinPF,HIGH);
      delayMicroseconds(100);
      digitalWrite(stepPinPF,LOW);
      delayMicroseconds(100);
    }
    digitalWrite(enablePF, HIGH);
  }
} //(HIGH, mm) to close press (LOW) to open
void PressM (bool state, float movement = 0){
  cerosPM();
  if (state == HIGH){
    digitalWrite(enablePM, LOW);
    digitalWrite(dirPinPM, state);
    for ( int steps = 0; steps < movement*PressMMSteps; steps++){
      digitalWrite(stepPinPM,HIGH);
      delayMicroseconds(100);
      digitalWrite(stepPinPM,LOW);
      delayMicroseconds(100);
    }
    digitalWrite(enablePM, HIGH);
  }
} //(HIGH, mm) to close press (LOW) to open
void drill(){
    /* read configuration struct from flash */
  byte* b = dueFlashStorage.readAddress(4); // byte array which is read from flash at adress 4
  Configuration configurationFromFlash; // create a temporary struct
  memcpy(&configurationFromFlash, b, sizeof(Configuration)); // copy byte array to temporary struct
  cerosSPd();
  cerosSPi();
  int vel1=100;
  //Movimiento de spindle central
  cerosSPc();
  int CentralSteps=configurationFromFlash.a*SpindlesMMSteps;
  digitalWrite(enableSPc, LOW);
  digitalWrite(dirPinSPc, HIGH);
  for ( int steps = 0; steps < CentralSteps; steps++){
    if(steps>=0 && steps<CentralSteps-6000){
     vel1=100;
    }
    if(steps>=6000 && steps<CentralSteps-1000){
     vel1=100;
    }
    if(steps>=CentralSteps-1000){
     vel1=800;
    }
    /*if(steps>=CentralSteps-700){
     vel1=3000;
    }*/
    if(steps==CentralSteps-1600){
     digitalWrite(spindleC,LOW); // Conectado a relevador con lógica inversa
    }
    if(steps==CentralSteps-1000){
     digitalWrite(relayA,LOW); // Conectado a relevador con lógica inversa
    }
    digitalWrite(stepPinSPc,HIGH);
    delayMicroseconds(vel1);
    digitalWrite(stepPinSPc,LOW);
    delayMicroseconds(vel1);
  }
  
  digitalWrite(dirPinSPc, LOW);
  vel1=500; 
  for ( int steps = 0; steps < 2*SpindlesMMSteps; steps++){
    digitalWrite(stepPinSPc,HIGH);
    delayMicroseconds(vel1);
    digitalWrite(stepPinSPc,LOW);
    delayMicroseconds(vel1);
  }
  digitalWrite(spindleC,HIGH);
  digitalWrite(relayA,HIGH);
  cerosSPc();

  //Movimiento de spindle izquierdo
  cerosSPi();
  int LeftSteps=configurationFromFlash.b*SpindlesMMSteps;
  digitalWrite(enableSPi, LOW);
  digitalWrite(dirPinSPi, HIGH);
  for ( int steps = 0; steps < LeftSteps; steps++){
    if(steps>=0 && steps<LeftSteps-4000){
     vel1=100;
    }
    if(steps>=LeftSteps-2400){
     vel1=250;
    }
    if(steps>=LeftSteps-1200){
     vel1=750;
    }
    if(steps==LeftSteps-2400){
     digitalWrite(spindleI,LOW); // Conectado a relevador con lógica inversa
    }
    if(steps==LeftSteps-1000){
     digitalWrite(relayA,LOW); // Conectado a relevador con lógica inversa
    }

    digitalWrite(stepPinSPi,HIGH);
    delayMicroseconds(vel1);
    digitalWrite(stepPinSPi,LOW);
    delayMicroseconds(vel1);
  }
  digitalWrite(dirPinSPi, LOW);
  vel1=500;
  for ( int steps = 0; steps < 5*SpindlesMMSteps; steps++){
    digitalWrite(stepPinSPi,HIGH);
    delayMicroseconds(vel1);
    digitalWrite(stepPinSPi,LOW);
    delayMicroseconds(vel1);
  }
  digitalWrite(spindleI,HIGH);
  digitalWrite(relayA,HIGH);
  cerosSPi();

  //Movimiento de spindle derecho
  cerosSPd();
  int RightSteps=configurationFromFlash.c*SpindlesMMSteps;
  digitalWrite(enableSPd, LOW);
  digitalWrite(dirPinSPd, HIGH);
  for ( int steps = 0; steps < RightSteps; steps++){
    if(steps>=0 && steps<RightSteps-4000){
     vel1=100;
    }
    if(steps>=RightSteps-2400){
     vel1=250;
    }
    if(steps>=RightSteps-1200){
     vel1=1000;
    }
    if(steps>=RightSteps-800){
     vel1=2500;
    }
    if(steps==RightSteps-2400){
     digitalWrite(spindleD,LOW); // Conectado a relevador
    }
    if(steps==RightSteps-1000){
     digitalWrite(relayA,LOW); // Conectado a relevador
    }

    digitalWrite(stepPinSPd,HIGH);
    delayMicroseconds(vel1);
    digitalWrite(stepPinSPd,LOW);
    delayMicroseconds(vel1);
  }
  digitalWrite(dirPinSPd, LOW);
  vel1=500;
  for ( int steps = 0; steps < 5*SpindlesMMSteps; steps++){
    digitalWrite(stepPinSPd,HIGH);
    delayMicroseconds(vel1);
    digitalWrite(stepPinSPd,LOW);
    delayMicroseconds(vel1);
  }
  digitalWrite(spindleD,HIGH);
  digitalWrite(relayA,HIGH);
  cerosSPd();
}

void movePressM(float space){
    /* read configuration struct from flash */
  byte* b = dueFlashStorage.readAddress(4); // byte array which is read from flash at adress 4
  Configuration configurationFromFlash; // create a temporary struct
  memcpy(&configurationFromFlash, b, sizeof(Configuration)); // copy byte array to temporary struct
  
  PressM(LOW);
  cerosM();
  digitalWrite(enableM, LOW);
  stepperM.moveTo(space*movePressMMSteps);
  while (stepperM.distanceToGo() != 0){
    stepperM.run();
  }
  PressM(HIGH, configurationFromFlash.d);
  PressF(LOW);
  stepperM.moveTo(50);
  while (stepperM.distanceToGo() != 0){
    stepperM.run();
  }
  cerosM();
}
/************************************************************************ MOTORES *****************************************************************************************/
/************************************************************************ MOTORES *****************************************************************************************/
/************************************************************************ MOTORES *****************************************************************************************/


void setup(void){
  Serial.begin(9600);
  uint16_t ID = tft.readID();
  if (ID == 0xD3D3) ID = 0x9486; // write-only shield
  tft.begin(ID);
  tft.setRotation(Orientation);            //PORTRAIT
  tft.fillScreen(BLACK);
  waiting = millis();


  /* Flash is erased every time new code is uploaded. Write the default configuration to flash if first time */
  // running for the first time?
  uint8_t codeRunningForTheFirstTime = dueFlashStorage.read(0); // flash bytes will be 255 at first run
  Serial.print("codeRunningForTheFirstTime: ");
  if (codeRunningForTheFirstTime) {
    Serial.println("yes");
    /* OK first time running, set defaults */
    configuration.a = 14.45; // +0.15 cambio cortador 3/09/2022 SSG
    configuration.b = 27.3;
    configuration.c = 29; //-.5
    configuration.d = 6;
    configuration.e = 7.5;
    
    // write configuration struct to flash at adress 4
    byte b2[sizeof(Configuration)]; // create byte array to store the struct
    memcpy(b2, &configuration, sizeof(Configuration)); // copy the struct to the byte array
    dueFlashStorage.write(4, b2, sizeof(Configuration)); // write byte array to flash

    // write 0 to address 0 to indicate that it is not the first time running anymore
    dueFlashStorage.write(0, 0); 
  }
  else {
    Serial.println("no");
  }

  pinMode(relayA,OUTPUT);
  
  pinMode(stepPinSPc,OUTPUT);
  pinMode(dirPinSPc,OUTPUT);
  pinMode(enableSPc,OUTPUT);
  pinMode(spindleC,OUTPUT);

  pinMode(stepPinSPd,OUTPUT);
  pinMode(dirPinSPd,OUTPUT);
  pinMode(enableSPd,OUTPUT);
  pinMode(spindleD,OUTPUT);

  pinMode(stepPinSPi,OUTPUT);
  pinMode(dirPinSPi,OUTPUT);
  pinMode(enableSPi,OUTPUT);
  pinMode(spindleI,OUTPUT);

  pinMode(stepPinPF,OUTPUT);
  pinMode(dirPinPF,OUTPUT);
  pinMode(enablePF,OUTPUT);

  pinMode(stepPinPM,OUTPUT);
  pinMode(dirPinPM,OUTPUT);
  pinMode(enablePM,OUTPUT);

  stepperM.setMaxSpeed(10000);
  stepperM.setAcceleration(3000);
  pinMode(enableM,OUTPUT);

  //attachInterrupt(digitalPinToInterrupt(StartStop), pause, CHANGE);
  pinMode(finish_SPc, INPUT_PULLUP);
  pinMode(finish_SPd, INPUT_PULLUP);
  pinMode(finish_SPi, INPUT_PULLUP);
  pinMode(finish_PF, INPUT_PULLUP);
  pinMode(finish_PM, INPUT_PULLUP);
  pinMode(finish_M, INPUT_PULLUP);

  // Desactivar Nema17
  digitalWrite(enableSPc,HIGH);
  digitalWrite(enableSPd, HIGH);
  digitalWrite(enableSPi,HIGH);
  digitalWrite(enablePF,HIGH);
  digitalWrite(enablePM,HIGH);
  digitalWrite(enableM,HIGH);
  

  // Desactivar relays 
  digitalWrite(relayA,HIGH);
  digitalWrite(spindleC,HIGH);
  digitalWrite(spindleD,HIGH);
  digitalWrite(spindleI,HIGH);

  /****************** Secuencia de calibración en ceros ****************************/
  cerosSPc();
  cerosSPi();
  cerosSPd();
  cerosPF();
  cerosPM();
  cerosM();

  digitalWrite(enablePF,HIGH);
  digitalWrite(enablePM, HIGH);
  digitalWrite(enableM ,HIGH);
  digitalWrite(enableSPc,HIGH);
  digitalWrite(enableSPd, HIGH);
  digitalWrite(enableSPi,HIGH);
  /****************** Secuencia de calibración en ceros ****************************/
}

void loop(void) {
  ///VariablesFlash
  byte* b = dueFlashStorage.readAddress(4); // byte array which is read from flash at adress 4
  Configuration configurationFromFlash; // create a temporary struct
  memcpy(&configurationFromFlash, b, sizeof(Configuration)); // copy byte array to temporary struct  
  
  bool down = Touch_getXY();
  switch (screens) {
    case 0:
      if (homeInit == false) {
        homeDisplay();
      }
      if (down == 1 && abs(xpos - homeSetupImg[4]) < homeSetupImg[2] / 2 && abs(ypos - homeSetupImg[5]) < homeSetupImg[3] / 2) {
        homeInit = false;
        pressedButtonsPress = 0;
        pressedButtons();
        screens = 1;
      }
      if (down == 1 && abs(xpos - homeSteppersImg[4]) < homeSteppersImg[2] / 2 && abs(ypos - homeSteppersImg[5]) < homeSteppersImg[3] / 2) {
        homeInit = false;
        pressedButtonsPress = 2;
        pressedButtons();
        screens = 2;
      }
      if (down == 1 && abs(xpos - homeStartImg[4]) < homeStartImg[2] / 2 && abs(ypos - homeStartImg[5]) < homeStartImg[3] / 2) {
        //homeInit=false;
        pressedButtonsPress = 3;
        pressedButtons();
        delay(100);
        PressF(HIGH, configurationFromFlash.e);
        PressM(HIGH, configurationFromFlash.d);
        for (int i=1; i<=space1Qty; i++){
          drill();
          movePressM(space1);
          PressF(HIGH, configurationFromFlash.e);
        }
        
        
      }
      if (down == 1 && abs(xpos - homeToolBoxImg[4]) < homeToolBoxImg[2] / 2 && abs(ypos - homeToolBoxImg[5]) < homeToolBoxImg[3] / 2) {
        pressedButtonsPress = 1;
        pressedButtons();
      }
      if (millis() - waiting > 750) {
        //tft.fillRect(160,140,120,40,BLACK);
        tft.setCursor(160, 140);
        tft.setTextColor(CYAN);
        tft.setTextSize(2);
        tft.println("Esperando");
        tft.setTextColor(WHITE);
      }
      if (millis() - waiting > 1500) {
        //tft.fillRect(160,140,120,40,BLACK);
        tft.setCursor(160, 140);
        tft.setTextSize(2);
        tft.println("Esperando");
        waiting = millis();
      }
      break;
    case 1:
      if (setupInit == false) {
        setupDisplay();
      }
      if (down == 1 && abs(xpos - sizeSliderX) < 8 && abs(ypos - sizeSliderY) < 8  && xpos >= sizeSliderInit && xpos <= maxSliderSize) {
        adjustSizeSlider();
      }
      if (down == 1 && abs(xpos - spacingQtySliderX) < 8 && abs(ypos - spacingQtySliderY) < 8  && xpos >= spacingQtySliderInit && xpos <= maxSliderSize) {
        adjustSpacingQtySlider();
        slideSpacingSliders(1);
        slideSpacingSliders(2);
        slideSpacingSliders(3);
        slideSpacingSliders(4);
      }
      if (down == 1 && abs(xpos - setupHomeImg[4]) < setupHomeImg[2] / 2 && abs(ypos - setupHomeImg[5]) < setupHomeImg[3] / 2) {
        setupInit = false;
        pressedButtonsPress = 4;
        pressedButtons();
        screens = 0;
      }
      if (down == 1 && abs(xpos - spaceSlider1[0]) < 8 && abs(ypos - spaceSlider1[4]) < 8  && ypos >= spaceSlider1[1] && ypos <= spaceSlider1[1]+spaceSlider1[2]) {
        slideSpacingSliders(1, true);
      }
      if (spacingQty>1 && down == 1 && abs(xpos - spaceSlider2[0]) < 8 && abs(ypos - spaceSlider2[4]) < 8  && ypos >= spaceSlider2[1] && ypos <= spaceSlider2[1]+spaceSlider2[2]) {
        slideSpacingSliders(2, true);
      }
      if (spacingQty>2 && down == 1 && abs(xpos - spaceSlider3[0]) < 8 && abs(ypos - spaceSlider3[4]) < 8  && ypos >= spaceSlider3[1] && ypos <= spaceSlider3[1]+spaceSlider3[2]) {
        slideSpacingSliders(3, true);
      }
      if (spacingQty>3 && down == 1 && abs(xpos - spaceSlider4[0]) < 8 && abs(ypos - spaceSlider4[4]) < 8  && ypos >= spaceSlider4[1] && ypos <= spaceSlider4[1]+spaceSlider4[2]) {
        slideSpacingSliders(4, true);
      }
      if (down == 1 && abs(xpos - (spaceSlider1[0]+45+3)) < 8 && abs(ypos - (spaceSlider1[1]+spaceSlider1[2]/2-6+3)) < 8) {
        if (space1Qty<120){
          space1Qty+=1;
        }
        slideSpacingSliders(1);
        delay(100);
      }
      if (down == 1 && abs(xpos - (spaceSlider1[0]+45+3)) < 8 && abs(ypos - (spaceSlider1[1]+spaceSlider1[2]/2+23+3)) < 8) {
        if (space1Qty>0){
          space1Qty-=1;
        }
        slideSpacingSliders(1);
        delay(100);
      }

      if (spacingQty>1 && down == 1 && abs(xpos - (spaceSlider2[0]+45+3)) < 8 && abs(ypos - (spaceSlider2[1]+spaceSlider2[2]/2-6+3)) < 8) {
          if (space2Qty<120){
            space2Qty+=1;
          }
        slideSpacingSliders(2);
        delay(100);
      }
      if (spacingQty>1 && down == 1 && abs(xpos - (spaceSlider2[0]+45+3)) < 8 && abs(ypos - (spaceSlider2[1]+spaceSlider2[2]/2+23+3)) < 8) {
        if (space2Qty>0){
          space2Qty-=1;
        }
        slideSpacingSliders(2);
        delay(100);
      }

      if (spacingQty>2 && down == 1 && abs(xpos - (spaceSlider3[0]+45+3)) < 8 && abs(ypos - (spaceSlider3[1]+spaceSlider3[2]/2-6+3)) < 8) {
        if (space3Qty<120){
          space3Qty+=1;
        }
        slideSpacingSliders(3);
        delay(100);
      }
      if (spacingQty>2 && down == 1 && abs(xpos - (spaceSlider3[0]+45+3)) < 8 && abs(ypos - (spaceSlider3[1]+spaceSlider3[2]/2+23+3)) < 8) {
        if (space3Qty>0){
          space3Qty-=1;
        }
        slideSpacingSliders(3);
        delay(100);
      }

      if (spacingQty>3 && down == 1 && abs(xpos - (spaceSlider4[0]+45+3)) < 8 && abs(ypos - (spaceSlider4[1]+spaceSlider4[2]/2-6+3)) < 8) {
        if (space4Qty<120){
          space4Qty+=1;
        }
        slideSpacingSliders(4);
        delay(100);
      }
      if (spacingQty>3 && down == 1 && abs(xpos - (spaceSlider4[0]+45+3)) < 8 && abs(ypos - (spaceSlider4[1]+spaceSlider4[2]/2+23+3)) < 8) {
        if (space4Qty>0){
          space4Qty-=1;
        }
        slideSpacingSliders(4);
        delay(100);
      }


      break;

    case 2:
      if (motorsInit == false) {
        motorsDisplay();
      }
      if (down == 1 && abs(xpos - homeSetupImg[4]) < homeSetupImg[2] / 2 && abs(ypos - homeSetupImg[5]) < homeSetupImg[3] / 2) {
        homeInit = false;
        pressedButtonsPress = 0;
        pressedButtons();
        screens = 1;
      }
      if (down == 1 && abs(xpos - homeSteppersImg[4]) < homeSteppersImg[2] / 2 && abs(ypos - homeSteppersImg[5]) < homeSteppersImg[3] / 2) {
        homeInit = false;
        pressedButtonsPress = 2;
        pressedButtons();
        screens = 2;
      }
      if (down == 1 && abs(xpos - homeStartImg[4]) < homeStartImg[2] / 2 && abs(ypos - homeStartImg[5]) < homeStartImg[3] / 2) {
        //homeInit=false;
        pressedButtonsPress = 3;
        pressedButtons();
      }
      if (down == 1 && abs(xpos - homeToolBoxImg[4]) < homeToolBoxImg[2] / 2 && abs(ypos - homeToolBoxImg[5]) < homeToolBoxImg[3] / 2) {
        pressedButtonsPress = 1;
        pressedButtons();
      }
      if (down == 1 && abs(xpos - setupHomeImg[4]) < setupHomeImg[2] / 2 && abs(ypos - setupHomeImg[5]) < setupHomeImg[3] / 2) {
        motorsInit = false;
        pressedButtonsPress = 4;
        pressedButtons();
        screens = 0;
      }
      break;
  } 
}
