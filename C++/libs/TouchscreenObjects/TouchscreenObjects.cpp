#include "TouchscreenObjects.h"

//PROGMEM reading
template <typename T> void PROGMEM_readAnything (const T * sce, T& dest){
  memcpy_P (&dest, sce, sizeof (T));
  }

template <typename T> T PROGMEM_getAnything (const T * sce){
  static T temp;
  memcpy_P (&temp, sce, sizeof (T));
  return temp;
  }

variables variableObjects[32] = {
        {SMAKER,      80,   190,  2,  WHITE,  ""},
        {SMAKER,      230,  190,  2,  WHITE,  ""},
        {SMAKER,      80,   215,  2,  WHITE,  ""},
        {SMAKER,      230,  215,  2,  WHITE,  ""},
        {SETTINGS,    150,  80,   2,  WHITE,  ""},
        {SETTINGS,    150,  110,  2,  WHITE,  ""},
        {SETTINGS,    15,   180,  1,  WHITE,  ""},
        {SETTINGS,    75,   180,  1,  WHITE,  ""},
        {SETTINGS,    135,  180,  1,  WHITE,  ""},
        {SETTINGS,    195,  180,  1,  WHITE,  ""},
        {SETTINGS,    255,  180,  1,  WHITE,  ""},
        {SETTINGS,    15,   190,  1,  WHITE,  ""},
        {SETTINGS,    75,   190,  1,  WHITE,  ""},
        {SETTINGS,    135,  190,  1,  WHITE,  ""},
        {SETTINGS,    195,  190,  1,  WHITE,  ""},
        {SETTINGS,    255,  190,  1,  WHITE,  ""},
        {SALT1,       200,  135,  3,  WHITE,  ""},
        {SALT2,       200,  135,  3,  WHITE,  ""},
        {SALT3,       200,  135,  3,  WHITE,  ""},
        {SALT4,       200,  135,  3,  WHITE,  ""},
        {SALT5,       200,  135,  3,  WHITE,  ""},
        {SALT1,       238,  175,  1,  WHITE,  ""},
        {SALT2,       238,  175,  1,  WHITE,  ""},
        {SALT3,       238,  175,  1,  WHITE,  ""},
        {SALT4,       238,  175,  1,  WHITE,  ""},
        {SALT5,       238,  175,  1,  WHITE,  ""},
        {SMAKER,      220,  140,  2,  WHITE,  ""},
        {SMAKER,      40,   140,  2,  WHITE,  ""},
        {ACID,        180,  90,   2,  WHITE,  ""},
        {ACID,        170,  170,  2,  WHITE,  ""},
        {MICROS,      180,  90,   2,  WHITE,  ""},
        {MICROS,      170,  170,  2,  WHITE,  ""},
    };

variables variableObjects2[7] = {
        {SCALEscr,    100,   90,  3,  WHITE,  ""},
        {SCALEscr,    240,   145, 1,  WHITE,  ""},
        {SCALEscr,    55,    170, 2,  WHITE,  ""},
        {MIXscr,      70,    140, 3,  WHITE,  "ON"},
        {MIXscr,      230,   140, 3,  WHITE,  "ON"},
        {MIXscr,      70,    65,  2,  WHITE,  ""},
        {MIXscr,      230,   65,  2,  WHITE,  ""},
    };

variables moveableObjects[7] = {
    {SALT1,       153,  81+0*25,   2,  WHITE,  "X"},
    {SALT2,       153,  81+1*25,   2,  WHITE,  "X"},
    {SALT3,       153,  81+2*25,   2,  WHITE,  "X"},
    {SALT4,       153,  81+3*25,   2,  WHITE,  "X"},
    {SALT5,       153,  81+4*25,   2,  WHITE,  "X"},
    {MICROS,      93,   41+0*20,   2,  WHITE,  "X"},
    {ACID,        93,   41+1*20,   2,  WHITE,  "X"},
    };

String salts[5]={
        "KH2PO4",
        "NH4NO3",
        "Ca(NO3)2",
        "KNO3",
        "MgSO4"
    };
uint8_t  saltsOrder[5]={0,1,2,3,4};

SMakerTouchScreen::SMakerTouchScreen(){
    screens=ECOLISTICO;
    tft = new MCUFRIEND_kbv;
    ts = new TouchScreen(XP, YP, XM, YM, 500);
    _temp = 25.40;
    _hum = 60.10;
    _pH = 5.60;
    _EC = 1259;
    __pHtarget = 5.72;
    __ECtarget = 1400;
    _calib=true;
    _S1mgl = 400.0;
    _S2mgl = 400.0;
    _S3mgl = 400.0;
    _S4mgl = 400.0;
    _S5mgl = 400.0;
    _increment = 1.38;
    _incrementW = 0.1;
    _acidPH = 5.0;
    _acidQTY = 6.16;//10 seg
    _microsQTY = 6.16;
    _microsML = 10;
    _targetGrams = 215.13;
    _scale = 0.01;
    _standbyTime=5000;
    eepromPrinted=false;
    EEPROMpage=1;
    CleanEEPROM=false;
    DefaultEEPROM=false;
    SaveEEPROM = false;
    tareSCALE = false;
    calibSCALE = false;
    calibPUMP = false;
    _CalibWeight = 200;
    _pumpsOrder[0]=0;
    _pumpsOrder[1]=1;

    BOOT = true;
    scrLoaded = false;  

    String variablePrev[32];
    String variablePrev2[7];
    int moveablePrevX[7];
    int moveablePrevY[7];

    __pumpSTATEtxt = "ON";
    __mixerSTATEtxt = "ON";
    __pumpState = false;
    __mixerState = false;
    __pumpStop = false;
    __mixerStop = false;
    _pumpMinutes = 1;
    _mixerMinutes = 1;
}



bool SMakerTouchScreen::Touch_getXY(){ //Function to get XY Touch
    TSPoint tp = ts->getPoint();
    pinMode(YP, OUTPUT);      //restore shared pins
    pinMode(XM, OUTPUT);
    digitalWrite(YP, HIGH);   //because TFT control pins
    digitalWrite(XM, HIGH);
    bool pressed = (tp.z > MINPRESSURE && tp.z < MAXPRESSURE);
    if (pressed) {
        switch (Orientation) {
        case 0:
            xpos = map(tp.x, TS_LEFT, TS_RT, 0, tft->width());
            ypos = map(tp.y, TS_TOP, TS_BOT, 0, tft->height());
            break;
        case 1:
            xpos = map(tp.y, TS_TOP, TS_BOT, 0, tft->width());
            ypos = map(tp.x, TS_RT, TS_LEFT, 0, tft->height());
            break;
        case 2:
            xpos = map(tp.x, TS_RT, TS_LEFT, 0, tft->width());
            ypos = map(tp.y, TS_BOT, TS_TOP, 0, tft->height());
            break;
        case 3:
            xpos = map(tp.y, TS_BOT, TS_TOP, 0, tft->width());
            ypos = map(tp.x, TS_LEFT, TS_RT, 0, tft->height());
            break;
        }
    }
    return pressed;
    }

void SMakerTouchScreen::TFTwrite(int x, int y, uint8_t sizeTxt, int color, String txt){
    tft->setTextSize(sizeTxt);
    tft->setTextColor(color);
    tft->setCursor(x,y);
    tft->print(txt);
    }

void SMakerTouchScreen::TFTfullCircle (int x = 5, int y = 5, uint8_t sizeD = 2, int color = WHITE){
    tft->drawCircle(x, y, sizeD, color);
    tft->fillCircle(x, y, sizeD, color);
    }

void SMakerTouchScreen::fillScreens(uint8_t screen){//Erases screen and fills with fixed objects
    tft->fillScreen(BLACK);
    for (int i=0; i<sizeof(fixedObjects)/sizeof(fixedObjects[0]);i++){
        objects thisItem;
        PROGMEM_readAnything (&fixedObjects[i], thisItem);
        if (thisItem.screen == screen){
        if (thisItem.type == TEXT){
            TFTwrite(thisItem.x, thisItem.y, thisItem.dim, thisItem.color, thisItem.txt); 
        }
        if (thisItem.type == CIRCLE){
            TFTfullCircle(thisItem.x, thisItem.y, thisItem.dim, thisItem.color); 
        }
        if (thisItem.type == IMAGE){
            if (thisItem.img==GROWGREENS){
            tft->drawBitmap(thisItem.x,thisItem.y, growGreens, thisItem.Width, thisItem.Height, BLACK, thisItem.color); 
            }
        }
        if (thisItem.type == LINE){
            tft->drawLine(thisItem.x, thisItem.y, thisItem.Width, thisItem.Height, thisItem.color); 
        }
        if (thisItem.type == RECTANGLE){
            tft->drawRect(thisItem.x, thisItem.y, thisItem.Width, thisItem.Height, thisItem.color); 
        }
        }   
    }
    scrLoaded = true;
    }

void SMakerTouchScreen::fillVariables(uint8_t screen){//Fills screen with txt variables
    for (int i=0; i<sizeof(variableObjects)/sizeof(variableObjects[0]);i++){
        if (variableObjects[i].screen == screen){
            if (variablePrev[i]!=variableObjects[i].txt)TFTwrite(variableObjects[i].x, variableObjects[i].y, variableObjects[i].dim, BLACK, variablePrev[i]);       
            TFTwrite(variableObjects[i].x, variableObjects[i].y, variableObjects[i].dim, variableObjects[i].color, variableObjects[i].txt); 
        } 
    }
    for (int i=0; i<sizeof(variableObjects2)/sizeof(variableObjects2[0]);i++){
        if (variableObjects2[i].screen == screen){
            if (variablePrev2[i]!=variableObjects2[i].txt)TFTwrite(variableObjects2[i].x, variableObjects2[i].y, variableObjects2[i].dim, BLACK, variablePrev2[i]);       
            TFTwrite(variableObjects2[i].x, variableObjects2[i].y, variableObjects2[i].dim, variableObjects2[i].color, variableObjects2[i].txt); 
        } 
    }
    for (int i=0; i<sizeof(moveableObjects)/sizeof(moveableObjects[0]);i++){
        if (moveableObjects[i].screen == screen){
            if (moveablePrevX[i]!=moveableObjects[i].x || moveablePrevY[i]!=moveableObjects[i].y)TFTwrite(moveablePrevX[i], moveablePrevY[i], moveableObjects[i].dim, BLACK, moveableObjects[i].txt);       
            TFTwrite(moveableObjects[i].x, moveableObjects[i].y, moveableObjects[i].dim, moveableObjects[i].color, moveableObjects[i].txt); 
        } 
    }
    }

void SMakerTouchScreen::updateVariables()
    {    
        for (int i= 0; i<sizeof(variablePrev)/sizeof(variablePrev[0]);i++){
            variablePrev[i] = variableObjects[i].txt;
        }
        for (int i= 0; i<sizeof(variablePrev2)/sizeof(variablePrev2[0]);i++){
            variablePrev2[i] = variableObjects2[i].txt;
        }
        for (int i= 0; i<sizeof(moveablePrevX)/sizeof(moveablePrevX[0]);i++){
            moveablePrevX[i] = moveableObjects[i].x;
            moveablePrevY[i] = moveableObjects[i].y;
        }
        variableObjects[0].txt = String(_temp);
        variableObjects[1].txt = String(_hum);
        variableObjects[2].txt = String(_pH);
        variableObjects[3].txt = String(_EC);
        variableObjects[4].txt = String(__pHtarget);
        variableObjects[5].txt = String(__ECtarget);
        variableObjects[6].txt =salts[saltsOrder[0]];
        variableObjects[7].txt =salts[saltsOrder[1]];
        variableObjects[8].txt =salts[saltsOrder[2]];
        variableObjects[9].txt =salts[saltsOrder[3]];
        variableObjects[10].txt =salts[saltsOrder[4]];
        variableObjects[11].txt = String(_S1mgl);
        variableObjects[12].txt = String(_S2mgl);
        variableObjects[13].txt = String(_S3mgl);
        variableObjects[14].txt = String(_S4mgl);
        variableObjects[15].txt = String(_S5mgl);
        variableObjects[16].txt = String(_S1mgl);
        variableObjects[17].txt = String(_S2mgl);
        variableObjects[18].txt = String(_S3mgl);
        variableObjects[19].txt = String(_S4mgl);
        variableObjects[20].txt = String(_S5mgl);
        variableObjects[21].txt = String(_increment);
        variableObjects[22].txt = String(_increment);
        variableObjects[23].txt = String(_increment);
        variableObjects[24].txt = String(_increment);
        variableObjects[25].txt = String(_increment);
        variableObjects[26].txt = String(_targetGrams);
        variableObjects[27].txt = String(_scale);
        variableObjects[28].txt = String(_acidPH);
        variableObjects[29].txt = String(_acidQTY);
        variableObjects[30].txt = String(_microsML);
        variableObjects[31].txt = String(_microsQTY);
        
        variableObjects2[0].txt = String(_scale);
        variableObjects2[1].txt = String(_incrementW);
        variableObjects2[2].txt = String(_CalibWeight);
        variableObjects2[3].txt = String(__pumpSTATEtxt);
        variableObjects2[4].txt = String(__mixerSTATEtxt);
        variableObjects2[5].txt = String(_pumpMinutes);
        variableObjects2[6].txt = String(_mixerMinutes);

        moveableObjects[0].y = 81+saltsOrder[0]*25;
        moveableObjects[1].y = 81+saltsOrder[1]*25;
        moveableObjects[2].y = 81+saltsOrder[2]*25;
        moveableObjects[3].y = 81+saltsOrder[3]*25;
        moveableObjects[4].y = 81+saltsOrder[4]*25;
        moveableObjects[5].y = 41+_pumpsOrder[0]*20;
        moveableObjects[6].y = 41+_pumpsOrder[1]*20;
    }

void SMakerTouchScreen::printEEPROM(uint8_t eepr, int i){
        if (screens == EEPROMscr && scrLoaded == true){
            TFTwrite(125,10,2,WHITE,String(EEPROMpage));
            i = i-(EEPROMpage-1)*90;
            if(i>=0  && i<10)TFTwrite(40,i*18+60,1,WHITE,String(eepr));
            if(i>=10 && i<20)TFTwrite(70,(i-10)*18+60,1,WHITE,String(eepr));
            if(i>=20 && i<30)TFTwrite(100,(i-20)*18+60,1,WHITE,String(eepr));
            if(i>=30 && i<40)TFTwrite(130,(i-30)*18+60,1,WHITE,String(eepr));
            if(i>=40 && i<50)TFTwrite(160,(i-40)*18+60,1,WHITE,String(eepr));
            if(i>=50 && i<60)TFTwrite(190,(i-50)*18+60,1,WHITE,String(eepr));
            if(i>=60 && i<70)TFTwrite(220,(i-60)*18+60,1,WHITE,String(eepr));
            if(i>=70 && i<80)TFTwrite(250,(i-70)*18+60,1,WHITE,String(eepr));
            if(i>=80 && i<90)TFTwrite(280,(i-80)*18+60,1,WHITE,String(eepr));
        }
    }

void SMakerTouchScreen::begin(){
    uint16_t ID = tft->readID();
    if (ID == 0xD3D3) ID = 0x9486; // write-only shield
    tft->begin(ID);
    tft->setRotation(Orientation);            //PORTRAIT
    screensTimer=millis();
    printTimer=millis();
    buttonTimer=millis();
    scaleTimer=millis();
    calibrateTimer=60000;
    }

void SMakerTouchScreen::run(){
    bool down = Touch_getXY();
    if (!scrLoaded)fillScreens(screens);
    fillVariables(screens);
    updateVariables();
    screensCheck(down);
    }

void SMakerTouchScreen::importData(
    float temp,
    float hum,
    float pH,
    int EC,
    float targetGrams,
    float scale)
    {
        _temp = temp;
        _hum = hum;
        _pH = pH;
        _EC = EC;
        _targetGrams = targetGrams;
        _scale = scale;
    }

void SMakerTouchScreen::importParameters(
    float pHtarget,
    int ECtarget,
    float S1mgl,
    float S2mgl,
    float S3mgl,
    float S4mgl,
    float S5mgl,
    float acidPH,
    float acidQTY,
    float microsML,
    float microsQTY,
    uint8_t microsPump,
    uint8_t acidPump)
    {
        __pHtarget = pHtarget;
        __ECtarget = ECtarget;
        _S1mgl = S1mgl;
        _S2mgl = S2mgl;
        _S3mgl = S3mgl;
        _S4mgl = S4mgl;
        _S5mgl = S5mgl;
        _acidPH = acidPH;
        _acidQTY = acidQTY; //10 seg
        _microsML = microsML;
        _microsQTY = microsQTY;
        _pumpsOrder[0] = microsPump;
        _pumpsOrder[1] = acidPump;
    }

void SMakerTouchScreen::screensCheck(bool down = 0){  
    switch (screens){
        case ECOLISTICO:
            if (millis()-screensTimer>_standbyTime && BOOT){
                changeScreen(SMAKER);
            }
            break;
        case SMAKER:
            if (down == 1 && xpos<300 && xpos>210 && ypos<20 && ypos>5 && millis()-buttonTimer>1000) {
                changeScreen(SETTINGS);
                buttonTimer=millis();
            }     
            else if (down == 1 && xpos<190 && xpos>130 && ypos<20 && ypos>5 && millis()-buttonTimer>1000) {
                changeScreen(SCALEscr);
                buttonTimer=millis();
            }
            else if (down == 1 && xpos<50 && xpos>10 && ypos<20 && ypos>5 && millis()-buttonTimer>1000) {
                changeScreen(MIXscr);
                buttonTimer=millis();
            }  
            break;
        case SETTINGS:
            if (down == 1 && xpos<300 && xpos>240 && ypos<20 && ypos>5 && millis()-buttonTimer>1000) {
                changeScreen(SMAKER);
                buttonTimer=millis();
            }   
            else if (down == 1 && xpos<180 && xpos>100 && ypos<20 && ypos>5 && millis()-buttonTimer>1000) {
                changeScreen(EEPROMscr);
                buttonTimer=millis();
            } 
            else if (down == 1 && xpos<80 && xpos>10 && ypos<20 && ypos>5 && millis()-buttonTimer>1000) {
                changeScreen(SMAKER);
                SaveEEPROM = true;
                buttonTimer=millis();
            } 
            else if (down == 1 && xpos<270 && xpos>250 && ypos<96 && ypos>76 && millis()-buttonTimer>50) { //pH target+
                if(__pHtarget<6.50)__pHtarget=__pHtarget + 0.03;
                buttonTimer=millis();
            }  
            else if (down == 1 && xpos<270 && xpos>250 && ypos<126 && ypos>106 && millis()-buttonTimer>50) { //EC target+
            if(__ECtarget<1800) __ECtarget=__ECtarget + 10;
                buttonTimer=millis();
            }  
            else if (down == 1 && xpos<300 && xpos>280 && ypos<96 && ypos>76 && millis()-buttonTimer>50) { //pH target-
                if(__pHtarget>5.00)__pHtarget=__pHtarget - 0.03;
                buttonTimer=millis();
            }  
            else if (down == 1 && xpos<300 && xpos>280 && ypos<126 && ypos>106 && millis()-buttonTimer>50) { //EC target-
                if(__ECtarget>1200)__ECtarget=__ECtarget - 10;
                buttonTimer=millis();
            }  
            else if (down == 1 && xpos<65 && xpos>10 && ypos<202 && ypos>156 && millis()-buttonTimer>50) { //Select salt1
                changeScreen(SALT1);
            }
            else if (down == 1 && xpos<125 && xpos>70 && ypos<202 && ypos>156 && millis()-buttonTimer>50) { //Select salt2
                changeScreen(SALT2);
            }  
            else if (down == 1 && xpos<185 && xpos>130 && ypos<202 && ypos>156 && millis()-buttonTimer>50) { //Select salt3
                changeScreen(SALT3);
            }  
            else if (down == 1 && xpos<245 && xpos>190 && ypos<202 && ypos>156 && millis()-buttonTimer>50) { //Select salt4
                changeScreen(SALT4);
            }  
            else if (down == 1 && xpos<305 && xpos>250 && ypos<202 && ypos>156 && millis()-buttonTimer>50) { //Select salt5
                changeScreen(SALT5);
            }    
            else if (down == 1 && xpos<122 && xpos>44 && ypos<230 && ypos>207 && millis()-buttonTimer>50) { //Select micros
                changeScreen(MICROS);
            }     
            else if (down == 1 && xpos<278 && xpos>200 && ypos<230 && ypos>207 && millis()-buttonTimer>50) { //Select micros
                changeScreen(ACID);
            }     
            break;

        case SALT1:
            if (down == 1 && xpos<300 && xpos>240 && ypos<20 && ypos>5 && millis()-buttonTimer>1000) {
                changeScreen(SETTINGS);
            }
            else if (down == 1 && xpos<230 && xpos>210 && ypos<185 && ypos>165 && millis()-buttonTimer>50) { //Salt 1 +
                if(_S1mgl+_increment<1000)_S1mgl = _S1mgl+_increment;
                buttonTimer=millis();
            }  
            else if (down == 1 && xpos<298 && xpos>278 && ypos<185 && ypos>165 && millis()-buttonTimer>50) { //Salt 1 -
                if(_S1mgl-_increment>=0)_S1mgl = _S1mgl-_increment;
                buttonTimer=millis();
            }
            else if (down == 1 && xpos<245 && xpos>235 && ypos<200 && ypos>190 && millis()-buttonTimer>500) { //Salt 1 increment +
                if(_increment<138)_increment=_increment*10;
                buttonTimer=millis();
            }  
            else if (down == 1 && xpos<265 && xpos>255 && ypos<200 && ypos>190 && millis()-buttonTimer>500) { //Salt 1 increment -
                if(_increment>1.38)_increment=_increment/10;
                buttonTimer=millis();
            }
            else if (down == 1 && xpos<166 && xpos>150 && ypos<96 && ypos>80 && millis()-buttonTimer>100) { //Salt 1 increment -
                saltsOrder[0]=0;
                buttonTimer=millis();
            }
            else if (down == 1 && xpos<166 && xpos>150 && ypos<121 && ypos>105 && millis()-buttonTimer>100) { //Salt 1 increment -
                saltsOrder[0]=1;
                buttonTimer=millis();
            } 
            else if (down == 1 && xpos<166 && xpos>150 && ypos<146 && ypos>130 && millis()-buttonTimer>100) { //Salt 1 increment -
                saltsOrder[0]=2;
                buttonTimer=millis();
            }
            else if (down == 1 && xpos<166 && xpos>150 && ypos<171 && ypos>155 && millis()-buttonTimer>100) { //Salt 1 increment -
                saltsOrder[0]=3;
                buttonTimer=millis();
            } 
            else if (down == 1 && xpos<166 && xpos>150 && ypos<196 && ypos>180 && millis()-buttonTimer>100) { //Salt 1 increment -
                saltsOrder[0]=4;
                buttonTimer=millis();
            }     
            break;

        case SALT2:
            if (down == 1 && xpos<300 && xpos>240 && ypos<20 && ypos>5 && millis()-buttonTimer>1000) {
                changeScreen(SETTINGS);
            }   
            else if (down == 1 && xpos<230 && xpos>210 && ypos<185 && ypos>165 && millis()-buttonTimer>50) { //Salt 1 +
                if(_S2mgl+_increment<1000)_S2mgl = _S2mgl+_increment;
                buttonTimer=millis();
            }  
            else if (down == 1 && xpos<298 && xpos>278 && ypos<185 && ypos>165 && millis()-buttonTimer>50) { //Salt 1 -
                if(_S2mgl-_increment>=0)_S2mgl = _S2mgl-_increment;
                buttonTimer=millis();
            }
            else if (down == 1 && xpos<245 && xpos>235 && ypos<200 && ypos>190 && millis()-buttonTimer>500) { //Salt 1 increment +
                if(_increment<138)_increment=_increment*10;
                buttonTimer=millis();
            }  
            else if (down == 1 && xpos<265 && xpos>255 && ypos<200 && ypos>190 && millis()-buttonTimer>500) { //Salt 1 increment -
                if(_increment>1.38)_increment=_increment/10;
                buttonTimer=millis();
            }         
            else if (down == 1 && xpos<166 && xpos>150 && ypos<96 && ypos>80 && millis()-buttonTimer>100) { //Salt 1 increment -
                saltsOrder[1]=0;
                buttonTimer=millis();
            }
            else if (down == 1 && xpos<166 && xpos>150 && ypos<121 && ypos>105 && millis()-buttonTimer>100) { //Salt 1 increment -
                saltsOrder[1]=1;
                buttonTimer=millis();
            } 
            else if (down == 1 && xpos<166 && xpos>150 && ypos<146 && ypos>130 && millis()-buttonTimer>100) { //Salt 1 increment -
                saltsOrder[1]=2;
                buttonTimer=millis();
            }
            else if (down == 1 && xpos<166 && xpos>150 && ypos<171 && ypos>155 && millis()-buttonTimer>100) { //Salt 1 increment -
                saltsOrder[1]=3;
                buttonTimer=millis();
            } 
            else if (down == 1 && xpos<166 && xpos>150 && ypos<196 && ypos>180 && millis()-buttonTimer>100) { //Salt 1 increment -
                saltsOrder[1]=4;
                buttonTimer=millis();
            }     
            break;

        case SALT3:
            if (down == 1 && xpos<300 && xpos>240 && ypos<20 && ypos>5 && millis()-buttonTimer>1000) {
                changeScreen(SETTINGS);
            }   
            else if (down == 1 && xpos<230 && xpos>210 && ypos<185 && ypos>165 && millis()-buttonTimer>50) { //Salt 1 +
                if(_S3mgl+_increment<1000)_S3mgl = _S3mgl+_increment;
                buttonTimer=millis();
            }  
            else if (down == 1 && xpos<298 && xpos>278 && ypos<185 && ypos>165 && millis()-buttonTimer>50) { //Salt 1 -
                if(_S3mgl-_increment>=0)_S3mgl = _S3mgl-_increment;
                buttonTimer=millis();
            }
            else if (down == 1 && xpos<245 && xpos>235 && ypos<200 && ypos>190 && millis()-buttonTimer>500) { //Salt 1 increment +
                if(_increment<138)_increment=_increment*10;
                buttonTimer=millis();
            }  
            else if (down == 1 && xpos<265 && xpos>255 && ypos<200 && ypos>190 && millis()-buttonTimer>500) { //Salt 1 increment -
                if(_increment>1.38)_increment=_increment/10;
                buttonTimer=millis();
            }         
            else if (down == 1 && xpos<166 && xpos>150 && ypos<96 && ypos>80 && millis()-buttonTimer>100) { //Salt 1 increment -
                saltsOrder[2]=0;
                buttonTimer=millis();
            }
            else if (down == 1 && xpos<166 && xpos>150 && ypos<121 && ypos>105 && millis()-buttonTimer>100) { //Salt 1 increment -
                saltsOrder[2]=1;
                buttonTimer=millis();
            } 
            else if (down == 1 && xpos<166 && xpos>150 && ypos<146 && ypos>130 && millis()-buttonTimer>100) { //Salt 1 increment -
                saltsOrder[2]=2;
                buttonTimer=millis();
            }
            else if (down == 1 && xpos<166 && xpos>150 && ypos<171 && ypos>155 && millis()-buttonTimer>100) { //Salt 1 increment -
                saltsOrder[2]=3;
                buttonTimer=millis();
            } 
            else if (down == 1 && xpos<166 && xpos>150 && ypos<196 && ypos>180 && millis()-buttonTimer>100) { //Salt 1 increment -
                saltsOrder[2]=4;
                buttonTimer=millis();
            }     
            break;

        case SALT4:
            if (down == 1 && xpos<300 && xpos>240 && ypos<20 && ypos>5 && millis()-buttonTimer>1000) {
                changeScreen(SETTINGS);
            }   
            else if (down == 1 && xpos<230 && xpos>210 && ypos<185 && ypos>165 && millis()-buttonTimer>50) { //Salt 1 +
                if(_S4mgl+_increment<1000)_S4mgl = _S4mgl+_increment;
                buttonTimer=millis();
            }  
            else if (down == 1 && xpos<298 && xpos>278 && ypos<185 && ypos>165 && millis()-buttonTimer>50) { //Salt 1 -
                if(_S4mgl-_increment>=0)_S4mgl = _S4mgl-_increment;
                buttonTimer=millis();
            }
            else if (down == 1 && xpos<245 && xpos>235 && ypos<200 && ypos>190 && millis()-buttonTimer>500) { //Salt 1 increment +
                if(_increment<138)_increment=_increment*10;
                buttonTimer=millis();
            }  
            else if (down == 1 && xpos<265 && xpos>255 && ypos<200 && ypos>190 && millis()-buttonTimer>500) { //Salt 1 increment -
                if(_increment>1.38)_increment=_increment/10;
                buttonTimer=millis();
            }         
            else if (down == 1 && xpos<166 && xpos>150 && ypos<96 && ypos>80 && millis()-buttonTimer>100) { //Salt 1 increment -
                saltsOrder[3]=0;
                buttonTimer=millis();
            }
            else if (down == 1 && xpos<166 && xpos>150 && ypos<121 && ypos>105 && millis()-buttonTimer>100) { //Salt 1 increment -
                saltsOrder[3]=1;
                buttonTimer=millis();
            } 
            else if (down == 1 && xpos<166 && xpos>150 && ypos<146 && ypos>130 && millis()-buttonTimer>100) { //Salt 1 increment -
                saltsOrder[3]=2;
                buttonTimer=millis();
            }
            else if (down == 1 && xpos<166 && xpos>150 && ypos<171 && ypos>155 && millis()-buttonTimer>100) { //Salt 1 increment -
                saltsOrder[3]=3;
                buttonTimer=millis();
            } 
            else if (down == 1 && xpos<166 && xpos>150 && ypos<196 && ypos>180 && millis()-buttonTimer>100) { //Salt 1 increment -
                saltsOrder[3]=4;
                buttonTimer=millis();
            }     
            break;

        case SALT5:
            if (down == 1 && xpos<300 && xpos>240 && ypos<20 && ypos>5 && millis()-buttonTimer>1000) {
                changeScreen(SETTINGS);
            }   
            else if (down == 1 && xpos<230 && xpos>210 && ypos<185 && ypos>165 && millis()-buttonTimer>50) { //Salt 1 +
                if(_S5mgl+_increment<1000)_S5mgl = _S5mgl+_increment;
                buttonTimer=millis();
            }  
            else if (down == 1 && xpos<298 && xpos>278 && ypos<185 && ypos>165 && millis()-buttonTimer>50) { //Salt 1 -
                if(_S5mgl-_increment>=0)_S5mgl = _S5mgl-_increment;
                buttonTimer=millis();
            }
            else if (down == 1 && xpos<245 && xpos>235 && ypos<200 && ypos>190 && millis()-buttonTimer>500) { //Salt 1 increment +
                if(_increment<138)_increment=_increment*10;
                buttonTimer=millis();
            }  
            else if (down == 1 && xpos<265 && xpos>255 && ypos<200 && ypos>190 && millis()-buttonTimer>500) { //Salt 1 increment -
                if(_increment>1.38)_increment=_increment/10;
                buttonTimer=millis();
            }
            else if (down == 1 && xpos<166 && xpos>150 && ypos<96 && ypos>80 && millis()-buttonTimer>100) { //Salt 1 increment -
                saltsOrder[4]=0;
                buttonTimer=millis();
            }
            else if (down == 1 && xpos<166 && xpos>150 && ypos<121 && ypos>105 && millis()-buttonTimer>100) { //Salt 1 increment -
                saltsOrder[4]=1;
                buttonTimer=millis();
            } 
            else if (down == 1 && xpos<166 && xpos>150 && ypos<146 && ypos>130 && millis()-buttonTimer>100) { //Salt 1 increment -
                saltsOrder[4]=2;
                buttonTimer=millis();
            }
            else if (down == 1 && xpos<166 && xpos>150 && ypos<171 && ypos>155 && millis()-buttonTimer>100) { //Salt 1 increment -
                saltsOrder[4]=3;
                buttonTimer=millis();
            } 
            else if (down == 1 && xpos<166 && xpos>150 && ypos<196 && ypos>180 && millis()-buttonTimer>100) { //Salt 1 increment -
                saltsOrder[4]=4;
                buttonTimer=millis();
            }              
            break;

        case MICROS:
            if (down == 1 && xpos<300 && xpos>240 && ypos<20 && ypos>5 && millis()-buttonTimer>1000) {
                changeScreen(SETTINGS);
            }   
            else if (down == 1 && xpos<270 && xpos>250 && ypos<110 && ypos>90 && millis()-buttonTimer>50) { //pH acid +
                if(_microsML+1<=100)_microsML=_microsML + 1;
                buttonTimer=millis();
            }  
            else if (down == 1 && xpos<270 && xpos>250 && ypos<190 && ypos>170 && millis()-buttonTimer>50) { //acid cal+
                _microsQTY=_microsQTY + 0.01;
                buttonTimer=millis();
            }  
            else if (down == 1 && xpos<300 && xpos>280 && ypos<110 && ypos>90 && millis()-buttonTimer>50) { //pH acid -
                if(_microsML-1>=0)_microsML=_microsML - 0.01;
                buttonTimer=millis();
            }  
            else if (down == 1 && xpos<300 && xpos>280 && ypos<190 && ypos>170 && millis()-buttonTimer>50) { //acid cal-
                if(_microsQTY-0.01>=0)_microsQTY=_microsQTY - 0.01;
                buttonTimer=millis();
            }
            else if (down == 1 && xpos<200 && xpos>120 && ypos<230 && ypos>207 && millis()-buttonTimer>50) { //Select micros
                calibPUMP=true;
                pumpToCalib=_pumpsOrder[0];
                buttonTimer=millis();
            }
            else if (down == 1 && xpos<106 && xpos>90 && ypos<56 && ypos>40 && millis()-buttonTimer>100) { //Salt 1 increment -
                _pumpsOrder[0]=0;
                _pumpsOrder[1]=1;
                buttonTimer=millis();
            }
            else if (down == 1 && xpos<106 && xpos>90 && ypos<66 && ypos>60 && millis()-buttonTimer>100) { //Salt 1 increment -
                _pumpsOrder[0]=1;
                _pumpsOrder[1]=0;
                buttonTimer=millis();
            }       
            break;
        case ACID:
            if (down == 1 && xpos<300 && xpos>240 && ypos<20 && ypos>5 && millis()-buttonTimer>1000) {
                changeScreen(SETTINGS);
            }
            else if (down == 1 && xpos<270 && xpos>250 && ypos<110 && ypos>90 && millis()-buttonTimer>50) { //pH acid +
                if(_acidPH+0.01<=7)_acidPH=_acidPH + 0.01;
                buttonTimer=millis();
            }  
            else if (down == 1 && xpos<270 && xpos>250 && ypos<190 && ypos>170 && millis()-buttonTimer>50) { //acid cal+
                _acidQTY=_acidQTY + 0.01;
                buttonTimer=millis();
            }  
            else if (down == 1 && xpos<300 && xpos>280 && ypos<110 && ypos>90 && millis()-buttonTimer>50) { //pH acid -
                if(_acidPH-0.01>=0)_acidPH=_acidPH - 0.01;
                buttonTimer=millis();
            }  
            else if (down == 1 && xpos<300 && xpos>280 && ypos<190 && ypos>170 && millis()-buttonTimer>50) { //acid cal-
                if(_acidQTY-0.01>=0)_acidQTY=_acidQTY - 0.01;
                buttonTimer=millis();
            }  
            else if (down == 1 && xpos<200 && xpos>120 && ypos<230 && ypos>207 && millis()-buttonTimer>50) { //Select micros
                calibPUMP=true;
                pumpToCalib=_pumpsOrder[1];
                buttonTimer=millis();
            }
            else if (down == 1 && xpos<106 && xpos>90 && ypos<56 && ypos>40 && millis()-buttonTimer>100) { //Salt 1 increment -
                _pumpsOrder[1]=0;
                _pumpsOrder[0]=1;
                buttonTimer=millis();
            }
            else if (down == 1 && xpos<106 && xpos>90 && ypos<66 && ypos>60 && millis()-buttonTimer>100) { //Salt 1 increment -
                _pumpsOrder[1]=1;
                _pumpsOrder[0]=0;
                buttonTimer=millis();
            }  
            break;

        case EEPROMscr:
            if (down == 1 && xpos<300 && xpos>240 && ypos<20 && ypos>5 && millis()-buttonTimer>1000) {
                changeScreen(SETTINGS);
                EEPROMpage = 1;
                buttonTimer=millis();
            } 
            else if (down == 1 && xpos<115 && xpos>100 && ypos<30 && ypos>10 && millis()-buttonTimer>1000) {
                if (EEPROMpage>1)EEPROMpage=EEPROMpage-1;
                screens = NONE;
                changeScreen(EEPROMscr);
                buttonTimer=millis();
            }  
            else if (down == 1 && xpos<165 && xpos>150 && ypos<30 && ypos>10 && millis()-buttonTimer>1000) {
                if (EEPROMpage<9)EEPROMpage=EEPROMpage+1;
                screens = NONE;
                changeScreen(EEPROMscr);
                buttonTimer=millis();
            }   
            else if (down == 1 && xpos<230 && xpos>180 && ypos<30 && ypos>10 && millis()-buttonTimer>2000) {
                CleanEEPROM=true;
                EEPROMpage = 1;
                screens = NONE;
                changeScreen(EEPROMscr);
                buttonTimer=millis();
            }else if (down == 1 && xpos<90 && xpos>10 && ypos<30 && ypos>10 && millis()-buttonTimer>2000) {
                DefaultEEPROM=true;
                EEPROMpage = 1;
                changeScreen(SMAKER);
                buttonTimer=millis();
            }
            break;
        case SCALEscr:
            if (down == 1 && xpos<300 && xpos>240 && ypos<20 && ypos>5 && millis()-buttonTimer>1000) {
                changeScreen(SMAKER);
                buttonTimer=millis();
            } 
            else if (down == 1 && xpos<230 && xpos>210 && ypos<160 && ypos>140 && millis()-buttonTimer>50) { //Salt 1 +
                if(_CalibWeight+_incrementW<1000)_CalibWeight = _CalibWeight+_incrementW;
                buttonTimer=millis();
            }  
            else if (down == 1 && xpos<298 && xpos>278 && ypos<160 && ypos>140 && millis()-buttonTimer>50) { //Salt 1 -
                if(_CalibWeight-_incrementW>=0)_CalibWeight = _CalibWeight-_incrementW;
                buttonTimer=millis();
            } 
            else if (down == 1 && xpos<245 && xpos>235 && ypos<180 && ypos>170 && millis()-buttonTimer>500) { //Salt 1 increment +
                if(_incrementW<10)_incrementW=_incrementW*10;
                buttonTimer=millis();
            }  
            else if (down == 1 && xpos<265 && xpos>255 && ypos<180 && ypos>170 && millis()-buttonTimer>500) { //Salt 1 increment -
                if(_incrementW>0.1)_incrementW=_incrementW/10;
                buttonTimer=millis();
            }     
            else if (down == 1 && xpos<122 && xpos>44 && ypos<230 && ypos>207 && millis()-buttonTimer>500) { //Select micros
                tareSCALE = true;
                buttonTimer=millis();
            }     
            else if (down == 1 && xpos<278 && xpos>200 && ypos<230 && ypos>207 && millis()-buttonTimer>500) { //Select micros
                calibSCALE = true;
                buttonTimer=millis();
            }
            else if (down == 1 && xpos<80 && xpos>10 && ypos<20 && ypos>5 && millis()-buttonTimer>1000) {
                changeScreen(SMAKER);
                SaveSCALE = true;
                buttonTimer=millis();
            } 
            break;
        case MIXscr:
            if (down == 1 && xpos<300 && xpos>240 && ypos<20 && ypos>5 && millis()-buttonTimer>1000) {
                changeScreen(SMAKER);
                buttonTimer=millis();
            } 
            else if (down == 1 && xpos<140 && xpos>30 && ypos<170 && ypos>90 && millis()-buttonTimer>1000) {
                buttonTimer=millis();
                __pumpState = !__pumpState;
                if(__pumpState){
                    __pumpSTATEtxt = "OFF";
                    Serial.println("Pump turned on");
                }
                if(!__pumpState){
                    __pumpSTATEtxt = "ON";
                    Serial.println("Pump turned off");
                    __pumpStop = true;
                }
            }
            else if (down == 1 && xpos<300 && xpos>190 && ypos<170 && ypos>90 && millis()-buttonTimer>1000) {
                buttonTimer=millis();
                __mixerState = !__mixerState;
                Serial.println(__mixerState);
                if(__mixerState){
                    __mixerSTATEtxt = "OFF";
                }
                if(!__mixerState){
                    __mixerSTATEtxt = "ON";
                    __mixerStop = true;
                }
            }
            else if (down == 1 && xpos<60 && xpos>40 && ypos<80 && ypos>60 && millis()-buttonTimer>1000) { //Salt 1 +
                if(_pumpMinutes+1<30)_pumpMinutes = _pumpMinutes+1;
                buttonTimer=millis();
            }  
            else if (down == 1 && xpos<128 && xpos>108 && ypos<80 && ypos>60 && millis()-buttonTimer>1000) { //Salt 1 -
                if(_pumpMinutes-1>0)_pumpMinutes = _pumpMinutes-1;
                buttonTimer=millis();
            } 
            else if (down == 1 && xpos<220 && xpos>200 && ypos<80 && ypos>60 && millis()-buttonTimer>1000) { //Salt 1 +
                if(_mixerMinutes+1<30)_mixerMinutes = _mixerMinutes+1;
                buttonTimer=millis();
            }  
            else if (down == 1 && xpos<288 && xpos>268 && ypos<80 && ypos>60 && millis()-buttonTimer>1000) { //Salt 1 -
                if(_mixerMinutes-1>0)_mixerMinutes = _mixerMinutes-1;
                buttonTimer=millis();
            } 
            break;
    }
}

void SMakerTouchScreen::changeScreen(uint8_t screen, unsigned int standbyTime=5000){
        if(screens!=screen){
            screens = screen;
            scrLoaded=false;
        }
        if (screen==ECOLISTICO)BOOT = true;
        screensTimer = millis();
        buttonTimer=millis();
        _standbyTime=standbyTime;
    }