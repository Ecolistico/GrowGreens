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

variablesSTR variableObjectsSTR[14] = {
        {SETTINGS,    10,    160,  1,  WHITE,  "EMPTY"}, //0
        {SETTINGS,    89,    160,  1,  WHITE,  "EMPTY"}, //1
        {SETTINGS,    168,   160,  1,  WHITE,  "EMPTY"}, //2
        {SETTINGS,    247,   160,  1,  WHITE,  "EMPTY"}, //3
        {SETTINGS,    10,    210,  1,  WHITE,  "EMPTY"}, //4
        {SETTINGS,    89,    210,  1,  WHITE,  "EMPTY"}, //5
        {SETTINGS,    168,   210,  1,  WHITE,  "EMPTY"}, //6
        {SETTINGS,    247,   210,  1,  WHITE,  "EMPTY"}, //7
        {MIXscr,      70,    140,  3,  WHITE,  "ON"},    //8
        {MIXscr,      230,   140,  3,  WHITE,  "ON"},    //9
        {pHCALIB,     20,    80,   2,  WHITE,  ""},      //10
        {ECCALIB,     20,    80,   2,  WHITE,  ""},      //11
        {SMAKER,      120,   110,  2,  GREEN,  "START"}, //12
        {MIXscr,      130,   200,  2,  RED,    "SHOW"},  //13
    };

variablesFL variableObjectsFL[22]={
        {SMAKER,      80,   190,  2,  WHITE,  0.00},    //0
        {SMAKER,      230,  190,  2,  WHITE,  0.00},    //1
        {SMAKER,      80,   215,  2,  WHITE,  0.00},    //2
        {SETTINGS,    150,  70,   2,  WHITE,  0.00},    //3
        {SETTINGS,    10,   170,  1,  WHITE,  0.00},    //4
        {SETTINGS,    89,   170,  1,  WHITE,  0.00},    //5
        {SETTINGS,    168,  170,  1,  WHITE,  0.00},    //6
        {SETTINGS,    247,  170,  1,  WHITE,  0.00},    //7
        {SETTINGS,    10,   220,  1,  WHITE,  0.00},    //8
        {SETTINGS,    89,   220,  1,  WHITE,  0.00},    //9
        {SETTINGS,    168,  220,  1,  WHITE,  0.00},    //10
        {SETTINGS,    247,  220,  1,  WHITE,  0.00},    //11
        {SALTSALL,    180,  135,  3,  WHITE,  0.00},    //12
        {SALTSALL,    238,  175,  1,  WHITE,  0.00},    //13
        {SMAKER,      220,  140,  2,  WHITE,  0.00},    //14
        {SMAKER,      40,   140,  2,  WHITE,  0.00},    //15
        {LIQUIDALL,   170,  90,   2,  WHITE,  0.00},    //16
        {LIQUIDALL,   170,  170,  2,  WHITE,  0.00},    //17
        {SCALEscr,    100,   90,  3,  WHITE,  0.00},    //18
        {SCALEscr,    240,   145, 1,  WHITE,  0.00},    //19
        {SCALEscr,    55,    170, 2,  WHITE,  0.00},    //20
        {SALTSALL,    120,   200, 2,  WHITE,  0.00},    //21
    };

variablesINT variableObjectsINT[5]{
        {SMAKER,      230,  215,  2,  WHITE,  0},   //0
        {SETTINGS,    150,  95,   2,  WHITE,  0},   //1
        {MIXscr,      70,   65,   2,  WHITE,  0},   //2
        {MIXscr,      230,  65,   2,  WHITE,  0},   //3
        {SETTINGS,    150,  120,  2,  WHITE,  0},   //4
    };

variablesUINT variableObjectsUINT[2] = {
        {SALTSALL,    280,  40,   3,  GREEN,  0},   //0
        {LIQUIDALL,   280,  40,   3,  GREEN,  0},   //1
    };

variablesSTR moveableObjects[1] = {
    {SALTSALL,    122,  6+0*16,    1,  WHITE,  "X"},    
    };

String salts[11]={
        "EMPTY",
        "NH4NO3",
        "KNO3+MgO",
        "KNO3+K2SO4",
        "5Ca(NO3)2",
        "Ca(NO3)2",
        "Mg(NO3)2",
        "NH4H2PO4",
        "KH2PO4",
        "MgSO4-7H2O",
        "Micros",
    };

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
    __ECmax = 1800;
    _calib=true;
    _increment = 1.00;
    _incrementW = 0.1;
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

    BOOT = true;
    scrLoaded = false;  

    __pumpSTATEtxt = "ON";
    __mixerSTATEtxt = "ON";
    __pumpState = false;
    __mixerState = false;
    __pumpStop = false;
    __mixerStop = false;
    _pumpMinutes = 1;
    _mixerMinutes = 1;
    _pHSteps = "";
    _ECSteps = "";
    _pHCalibTimer = 0;
    _ECCalibTimer = 0;
    systemRunning = false;
    systemRunningText = "START";
    show=false;

    __solidSaltCurrentIndex=0;
    for (int i = 0; i<SOLIDSALTS; i++){
        __SaltmgL[i]=1700.00;
        __Sflow[i] = 0.5;
        saltsOrder[i] = 0;
    }
    __liquidConst[0]=100.00;
    __liquidCal[0]=30.00;
    __liquidConst[1]=80.00;
    __liquidCal[1]=30.00;
    
    
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

void SMakerTouchScreen::TFTwriteSTR(int x, int y, uint8_t sizeTxt, int color, String txt){
    tft->setTextSize(sizeTxt);
    tft->setTextColor(color);
    tft->setCursor(x,y);
    tft->print(txt);
    }
void SMakerTouchScreen::TFTwriteFL(int x, int y, uint8_t sizeTxt, int color, float txt){
    tft->setTextSize(sizeTxt);
    tft->setTextColor(color);
    tft->setCursor(x,y);
    tft->print(txt);
    }
void SMakerTouchScreen::TFTwriteINT(int x, int y, uint8_t sizeTxt, int color, int txt){
    tft->setTextSize(sizeTxt);
    tft->setTextColor(color);
    tft->setCursor(x,y);
    tft->print(txt);
    }
void SMakerTouchScreen::TFTwriteUINT(int x, int y, uint8_t sizeTxt, int color, uint8_t txt){
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
                TFTwriteSTR(thisItem.x, thisItem.y, thisItem.dim, thisItem.color, thisItem.txt); 
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
            if (thisItem.type == TRIANGLE){
                tft->fillTriangle(thisItem.x, thisItem.y, thisItem.x+thisItem.Width, thisItem.y, thisItem.x+thisItem.Width/2, thisItem.y+thisItem.Height, thisItem.color); 
            }
        }   
    }
    scrLoaded = true;
    }

void SMakerTouchScreen::fillVariables(uint8_t screen){//Fills screen with txt variables
    for (int i=0; i<sizeof(variableObjectsSTR)/sizeof(variableObjectsSTR[0]);i++){
        if (variableObjectsSTR[i].screen == screen){
            if (variablePrevSTR[i]!=variableObjectsSTR[i].txt)TFTwriteSTR(variableObjectsSTR[i].x, variableObjectsSTR[i].y, variableObjectsSTR[i].dim, BLACK, variablePrevSTR[i]);       
            TFTwriteSTR(variableObjectsSTR[i].x, variableObjectsSTR[i].y, variableObjectsSTR[i].dim, variableObjectsSTR[i].color, variableObjectsSTR[i].txt); 
        } 
    }
    for (int i=0; i<sizeof(variableObjectsFL)/sizeof(variableObjectsFL[0]);i++){
        if (variableObjectsFL[i].screen == screen){
            if (variablePrevFL[i]!=variableObjectsFL[i].txt)TFTwriteFL(variableObjectsFL[i].x, variableObjectsFL[i].y, variableObjectsFL[i].dim, BLACK, variablePrevFL[i]);       
            TFTwriteFL(variableObjectsFL[i].x, variableObjectsFL[i].y, variableObjectsFL[i].dim, variableObjectsFL[i].color, variableObjectsFL[i].txt); 
        } 
    }
    for (int i=0; i<sizeof(variableObjectsINT)/sizeof(variableObjectsINT[0]);i++){
        if (variableObjectsINT[i].screen == screen){
            if (variablePrevINT[i]!=variableObjectsINT[i].txt)TFTwriteINT(variableObjectsINT[i].x, variableObjectsINT[i].y, variableObjectsINT[i].dim, BLACK, variablePrevINT[i]);       
            TFTwriteINT(variableObjectsINT[i].x, variableObjectsINT[i].y, variableObjectsINT[i].dim, variableObjectsINT[i].color, variableObjectsINT[i].txt); 
        } 
    }
    for (int i=0; i<sizeof(variableObjectsUINT)/sizeof(variableObjectsUINT[0]);i++){
        if (variableObjectsUINT[i].screen == screen){
            if (variablePrevUINT[i]!=variableObjectsUINT[i].txt)TFTwriteUINT(variableObjectsUINT[i].x, variableObjectsUINT[i].y, variableObjectsUINT[i].dim, BLACK, variablePrevUINT[i]);       
            TFTwriteUINT(variableObjectsUINT[i].x, variableObjectsUINT[i].y, variableObjectsUINT[i].dim, variableObjectsUINT[i].color, variableObjectsUINT[i].txt); 
        } 
    }
    for (int i=0; i<sizeof(moveableObjects)/sizeof(moveableObjects[0]);i++){
        if (moveableObjects[i].screen == screen){
            if (moveablePrevX[i]!=moveableObjects[i].x || moveablePrevY[i]!=moveableObjects[i].y)TFTwriteSTR(moveablePrevX[i], moveablePrevY[i], moveableObjects[i].dim, BLACK, moveableObjects[i].txt);       
            if(moveableObjects[i].y>0 && moveableObjects[i].y<240 && moveableObjects[i].y>0 && moveableObjects[i].x<320)TFTwriteSTR(moveableObjects[i].x, moveableObjects[i].y, moveableObjects[i].dim, moveableObjects[i].color, moveableObjects[i].txt); 
        } 
    }
    }

void SMakerTouchScreen::updateVariables()
    {    
        for (int i= 0; i<sizeof(variablePrevSTR)/sizeof(variablePrevSTR[0]);i++){
            variablePrevSTR[i] = variableObjectsSTR[i].txt;
        }

        for (int i= 0; i<sizeof(variablePrevFL)/sizeof(variablePrevFL[0]);i++){
            variablePrevFL[i] = variableObjectsFL[i].txt;
        }

        for (int i= 0; i<sizeof(variablePrevINT)/sizeof(variablePrevINT[0]);i++){
            variablePrevINT[i] = variableObjectsINT[i].txt;
        }

        for (int i= 0; i<sizeof(variablePrevUINT)/sizeof(variablePrevUINT[0]);i++){
            variablePrevUINT[i] = variableObjectsUINT[i].txt;
        }

        for (int i= 0; i<sizeof(moveablePrevX)/sizeof(moveablePrevX[0]);i++){
            moveablePrevX[i] = moveableObjects[i].x;
            moveablePrevY[i] = moveableObjects[i].y;
        }

        variableObjectsFL[0].txt = _temp;                   //fl
        variableObjectsFL[1].txt = _hum;                    //fl
        variableObjectsFL[2].txt = _pH;                     //fl
        variableObjectsFL[3].txt = __pHtarget;              //fl
        variableObjectsFL[4].txt = __SaltmgL[0];            //fl
        variableObjectsFL[5].txt = __SaltmgL[1];            //fl
        variableObjectsFL[6].txt = __SaltmgL[2];            //fl
        variableObjectsFL[7].txt = __SaltmgL[3];            //fl
        variableObjectsFL[8].txt = __SaltmgL[4];            //fl
        variableObjectsFL[9].txt = __SaltmgL[5];            //fl
        variableObjectsFL[10].txt = __SaltmgL[6];           //fl
        variableObjectsFL[11].txt = __SaltmgL[7];           //fl
        variableObjectsFL[12].txt = __SaltmgL[__solidSaltCurrentIndex];         //fl
        variableObjectsFL[13].txt = _increment;             //fl
        variableObjectsFL[14].txt = _targetGrams;           //fl
        variableObjectsFL[15].txt = _scale;                 //fl
        variableObjectsFL[16].txt = __liquidConst[__liquidSaltCurrentIndex];    //fl
        variableObjectsFL[17].txt = __liquidCal[__liquidSaltCurrentIndex];      //fl
        variableObjectsFL[18].txt = _scale;                 //fl
        variableObjectsFL[19].txt = _incrementW;            //fl
        variableObjectsFL[20].txt = _CalibWeight;           //fl
        variableObjectsFL[21].txt = __Sflow[__solidSaltCurrentIndex];           //fl

        variableObjectsSTR[0].txt =salts[saltsOrder[0]];    //str
        variableObjectsSTR[1].txt =salts[saltsOrder[1]];    //str
        variableObjectsSTR[2].txt =salts[saltsOrder[2]];    //str
        variableObjectsSTR[3].txt =salts[saltsOrder[3]];    //str
        variableObjectsSTR[4].txt =salts[saltsOrder[4]];    //str
        variableObjectsSTR[5].txt =salts[saltsOrder[5]];    //str
        variableObjectsSTR[6].txt =salts[saltsOrder[6]];    //str
        variableObjectsSTR[7].txt =salts[saltsOrder[7]];    //str
        variableObjectsSTR[8].txt = __pumpSTATEtxt;         //str
        variableObjectsSTR[9].txt = __mixerSTATEtxt;        //str
        variableObjectsSTR[10].txt = _pHSteps;              //str
        variableObjectsSTR[11].txt = _ECSteps;              //str
        variableObjectsSTR[12].txt = systemRunningText;     //str


        variableObjectsINT[0].txt = _EC;                    //int
        variableObjectsINT[1].txt = __ECtarget;             //int
        variableObjectsINT[2].txt = _pumpMinutes;           //int
        variableObjectsINT[3].txt = _mixerMinutes;          //int
        variableObjectsINT[4].txt = __ECmax;                //int
        
        variableObjectsUINT[0].txt =__solidSaltCurrentIndex+1;  //uint
        variableObjectsUINT[1].txt =__liquidSaltCurrentIndex+1; //uint
        

        if (systemRunning){
            variableObjectsSTR[12].color = RED;
        }else{
            variableObjectsSTR[12].color = GREEN;
        }

        if (show){
            variableObjectsSTR[13].color = GREEN;
        }else{
            variableObjectsSTR[13].color = RED;
        }

        moveableObjects[0].y =6+saltsOrder[__solidSaltCurrentIndex]*16;
    }

void SMakerTouchScreen::printEEPROM(uint8_t eepr, int i){
        if (screens == EEPROMscr && scrLoaded == true){
            TFTwriteUINT(125,10,2,WHITE,EEPROMpage);
            i = i-(EEPROMpage-1)*90;
            if(i>=0  && i<10)TFTwriteUINT(40,i*18+60,1,WHITE,eepr);
            if(i>=10 && i<20)TFTwriteUINT(70,(i-10)*18+60,1,WHITE,eepr);
            if(i>=20 && i<30)TFTwriteUINT(100,(i-20)*18+60,1,WHITE,eepr);
            if(i>=30 && i<40)TFTwriteUINT(130,(i-30)*18+60,1,WHITE,eepr);
            if(i>=40 && i<50)TFTwriteUINT(160,(i-40)*18+60,1,WHITE,eepr);
            if(i>=50 && i<60)TFTwriteUINT(190,(i-50)*18+60,1,WHITE,eepr);
            if(i>=60 && i<70)TFTwriteUINT(220,(i-60)*18+60,1,WHITE,eepr);
            if(i>=70 && i<80)TFTwriteUINT(250,(i-70)*18+60,1,WHITE,eepr);
            if(i>=80 && i<90)TFTwriteUINT(280,(i-80)*18+60,1,WHITE,eepr);
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
    int ECmax,
    float Smgl[SOLIDSALTS],
    float Sflow[SOLIDSALTS],
    uint8_t SOrder[SOLIDSALTS])
    {
        __pHtarget = pHtarget;
        __ECtarget = ECtarget;
        __ECmax  = ECmax;
        for (int i = 0; i< SOLIDSALTS; i++){
            __SaltmgL[i] = Smgl[i];
            __Sflow[i] = Sflow[i];
            saltsOrder[i] = SOrder[i];
        }
        
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
            else if (down == 1 && xpos<120 && xpos>60 && ypos<20 && ypos>5 && millis()-buttonTimer>1000) {
                changeScreen(CALIBsensors);
                buttonTimer=millis();
            }  
            else if (down == 1 && xpos<200 && xpos>120 && ypos<130 && ypos>110 && millis()-buttonTimer>1000) {
                systemRunning=!systemRunning;
                if (systemRunning==false){
                    systemRunningText = "START";
                } else {
                    systemRunningText = "STOP";
                }
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
            else if (down == 1 && xpos<270 && xpos>250 && ypos<86 && ypos>66 && millis()-buttonTimer>50) { //pH target+
                if(__pHtarget<6.50)__pHtarget=__pHtarget + 0.03;
                buttonTimer=millis();
            }  
            else if (down == 1 && xpos<270 && xpos>250 && ypos<111 && ypos>91 && millis()-buttonTimer>50) { //EC target+
                if(__ECtarget<1800) __ECtarget=__ECtarget + 10;
                buttonTimer=millis();
            }
            else if (down == 1 && xpos<270 && xpos>250 && ypos<136 && ypos>116 && millis()-buttonTimer>50) { //EC target+
                if(__ECtarget<1800) __ECmax=__ECmax + 10;
                buttonTimer=millis();
            }    
            else if (down == 1 && xpos<300 && xpos>280 && ypos<86 && ypos>66 && millis()-buttonTimer>50) { //pH target-
                if(__pHtarget>5.00)__pHtarget=__pHtarget - 0.03;
                buttonTimer=millis();
            }  
            else if (down == 1 && xpos<300 && xpos>280 && ypos<111 && ypos>91 && millis()-buttonTimer>50) { //EC target-
                if(__ECtarget>1200)__ECtarget=__ECtarget - 10;
                buttonTimer=millis();
            }  
            else if (down == 1 && xpos<300 && xpos>280 && ypos<136 && ypos>116 && millis()-buttonTimer>50) { //EC target+
                if(__ECtarget<1800) __ECmax=__ECmax - 10;
                buttonTimer=millis();
            }    
            else if (down == 1 && xpos<75 && xpos>5 && ypos<181 && ypos>136 && millis()-buttonTimer>50) { //Select salt1
                __solidSaltCurrentIndex=0;
                changeScreen(SALTSALL);
            }
            else if (down == 1 && xpos<154 && xpos>84 && ypos<181 && ypos>136 && millis()-buttonTimer>50) { //Select salt2
                __solidSaltCurrentIndex=1;
                changeScreen(SALTSALL);
            }  
            else if (down == 1 && xpos<233 && xpos>163 && ypos<181 && ypos>136 && millis()-buttonTimer>50) { //Select salt3
                __solidSaltCurrentIndex=2;
                changeScreen(SALTSALL);
            }  
            else if (down == 1 && xpos<312 && xpos>242 && ypos<181 && ypos>136 && millis()-buttonTimer>50) { //Select salt4
                __solidSaltCurrentIndex=3;
                changeScreen(SALTSALL);
            }  
            else if (down == 1 && xpos<75 && xpos>5 && ypos<232 && ypos>186 && millis()-buttonTimer>50) { //Select salt5
                __solidSaltCurrentIndex=4;
                changeScreen(SALTSALL);
            }  
            else if (down == 1 && xpos<154 && xpos>84 && ypos<232 && ypos>186 && millis()-buttonTimer>50) { //Select salt4
                __solidSaltCurrentIndex=5;
                changeScreen(SALTSALL);
            }  
            else if (down == 1 && xpos<233 && xpos>163 && ypos<232 && ypos>186 && millis()-buttonTimer>50) { //Select salt5
                __solidSaltCurrentIndex=6;
                changeScreen(SALTSALL);
            }  
            else if (down == 1 && xpos<312 && xpos>242 && ypos<232 && ypos>186 && millis()-buttonTimer>50) { //Select salt5
                __solidSaltCurrentIndex=7;
                changeScreen(SALTSALL);
            }      
            /*else if (down == 1 && xpos<45 && xpos>5 && ypos<240 && ypos>156 && millis()-buttonTimer>50) { //Select salt1
                __liquidSaltCurrentIndex=0;
                changeScreen(LIQUIDALL);
            }
            else if (down == 1 && xpos<90 && xpos>50 && ypos<240 && ypos>156 && millis()-buttonTimer>50) { //Select salt2
                __liquidSaltCurrentIndex=1;
                changeScreen(LIQUIDALL);
            }*/      
            break;

        case SALTSALL:
            if (down == 1 && xpos<300 && xpos>240 && ypos<20 && ypos>5 && millis()-buttonTimer>1000) {
                changeScreen(SETTINGS);
            }
            else if (down == 1 && xpos<230 && xpos>210 && ypos<185 && ypos>165 && millis()-buttonTimer>50) { //Salt 1 +
                if(__SaltmgL[__solidSaltCurrentIndex]+_increment<3000)__SaltmgL[__solidSaltCurrentIndex] = __SaltmgL[__solidSaltCurrentIndex]+_increment;
                buttonTimer=millis();
            }  
            else if (down == 1 && xpos<298 && xpos>278 && ypos<185 && ypos>165 && millis()-buttonTimer>50) { //Salt 1 -
                if(__SaltmgL[__solidSaltCurrentIndex]-_increment>=0)__SaltmgL[__solidSaltCurrentIndex] = __SaltmgL[__solidSaltCurrentIndex]-_increment;
                buttonTimer=millis();
            }
            else if (down == 1 && xpos<245 && xpos>235 && ypos<200 && ypos>190 && millis()-buttonTimer>500) { //Salt 1 increment +
                if(_increment<100)_increment=_increment*10;
                buttonTimer=millis();
            }  
            else if (down == 1 && xpos<265 && xpos>255 && ypos<200 && ypos>190 && millis()-buttonTimer>500) { //Salt 1 increment -
                if(_increment>0.01)_increment=_increment/10;
                buttonTimer=millis();
            }
            else if (down == 1 && xpos<130 && xpos>120 && ypos<15 && ypos>5 && millis()-buttonTimer>500) { //Salt 1 increment -
                saltsOrder[__solidSaltCurrentIndex]=0;
                buttonTimer=millis();
            }
            else if (down == 1 && xpos<130 && xpos>120 && ypos<31 && ypos>21 && millis()-buttonTimer>500) { //Salt 1 increment -
                saltsOrder[__solidSaltCurrentIndex]=1;
                buttonTimer=millis();
            } 
            else if (down == 1 && xpos<130 && xpos>120 && ypos<47 && ypos>37 && millis()-buttonTimer>500) { //Salt 1 increment -
                saltsOrder[__solidSaltCurrentIndex]=2;
                buttonTimer=millis();
            }
            else if (down == 1 && xpos<130 && xpos>120 && ypos<63 && ypos>53 && millis()-buttonTimer>500) { //Salt 1 increment -
                saltsOrder[__solidSaltCurrentIndex]=3;
                buttonTimer=millis();
            } 
            else if (down == 1 && xpos<130 && xpos>120 && ypos<79 && ypos>69 && millis()-buttonTimer>500) { //Salt 1 increment -
                saltsOrder[__solidSaltCurrentIndex]=4;
                buttonTimer=millis();
            }
            else if (down == 1 && xpos<130 && xpos>120 && ypos<95 && ypos>85 && millis()-buttonTimer>500) { //Salt 1 increment -
                saltsOrder[__solidSaltCurrentIndex]=5;
                buttonTimer=millis();
            } 
            else if (down == 1 && xpos<130 && xpos>120 && ypos<111 && ypos>101 && millis()-buttonTimer>500) { //Salt 1 increment -
                saltsOrder[__solidSaltCurrentIndex]=6;
                buttonTimer=millis();
            }
            else if (down == 1 && xpos<130 && xpos>120 && ypos<127 && ypos>117 && millis()-buttonTimer>500) { //Salt 1 increment -
                saltsOrder[__solidSaltCurrentIndex]=7;
                buttonTimer=millis();
            } 
            else if (down == 1 && xpos<130 && xpos>120 && ypos<143 && ypos>133 && millis()-buttonTimer>500) { //Salt 1 increment -
                saltsOrder[__solidSaltCurrentIndex]=8;
                buttonTimer=millis();
            } 
            else if (down == 1 && xpos<130 && xpos>120 && ypos<159 && ypos>149 && millis()-buttonTimer>500) { //Salt 1 increment -
                saltsOrder[__solidSaltCurrentIndex]=9;
                buttonTimer=millis();
            }
            else if (down == 1 && xpos<130 && xpos>120 && ypos<175 && ypos>165 && millis()-buttonTimer>500) { //Salt 1 increment -
                saltsOrder[__solidSaltCurrentIndex]=10;
                buttonTimer=millis();
            }
            else if (down == 1 && xpos<130 && xpos>120 && ypos<230 && ypos>220 && millis()-buttonTimer>50) { //Salt 1 increment +
                if(__Sflow[__solidSaltCurrentIndex]<1.5)__Sflow[__solidSaltCurrentIndex]+=0.01;
                buttonTimer=millis();
            }  
            else if (down == 1 && xpos<150 && xpos>140 && ypos<230 && ypos>220 && millis()-buttonTimer>50) { //Salt 1 increment -
                if(__Sflow[__solidSaltCurrentIndex]>0)__Sflow[__solidSaltCurrentIndex]-=0.01;
                buttonTimer=millis();
            }
            else if (down == 1 && xpos<261 && xpos>205 && ypos<240 && ypos>215 && millis()-buttonTimer>3000) { //Salt 1 increment -
                calibPUMP=true;
                pumpToCalib=__solidSaltCurrentIndex;
                buttonTimer=millis();
            }
            break;

        case LIQUIDALL:
            if (down == 1 && xpos<300 && xpos>240 && ypos<20 && ypos>5 && millis()-buttonTimer>1000) {
                changeScreen(SETTINGS);
            }   
            else if (down == 1 && xpos<270 && xpos>250 && ypos<110 && ypos>90 && millis()-buttonTimer>50) { //pH acid +
                if(__liquidConst[__liquidSaltCurrentIndex]+1<=300)__liquidConst[__liquidSaltCurrentIndex]=__liquidConst[__liquidSaltCurrentIndex] + 1;
                buttonTimer=millis();
            }  
            else if (down == 1 && xpos<270 && xpos>250 && ypos<190 && ypos>170 && millis()-buttonTimer>50) { //acid cal+
                __liquidCal[__liquidSaltCurrentIndex]=__liquidCal[__liquidSaltCurrentIndex] + 1;
                buttonTimer=millis();
            }  
            else if (down == 1 && xpos<300 && xpos>280 && ypos<110 && ypos>90 && millis()-buttonTimer>50) { //pH acid -
                if(__liquidConst[__liquidSaltCurrentIndex]-1>=0)__liquidConst[__liquidSaltCurrentIndex]=__liquidConst[__liquidSaltCurrentIndex] - 1;
                buttonTimer=millis();
            }  
            else if (down == 1 && xpos<300 && xpos>280 && ypos<190 && ypos>170 && millis()-buttonTimer>50) { //acid cal-
                if(__liquidCal[__liquidSaltCurrentIndex]-1>=0)__liquidCal[__liquidSaltCurrentIndex]=__liquidCal[__liquidSaltCurrentIndex] - 1;
                buttonTimer=millis();
            }
            else if (down == 1 && xpos<200 && xpos>120 && ypos<230 && ypos>207 && millis()-buttonTimer>50) { //Select micros
                calibPUMP=true;
                pumpToCalib=__liquidSaltCurrentIndex;
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
                }
                if(!__pumpState){
                    __pumpSTATEtxt = "ON";
                    __pumpStop = true;
                }
            }
            else if (down == 1 && xpos<300 && xpos>190 && ypos<170 && ypos>90 && millis()-buttonTimer>1000) {
                buttonTimer=millis();
                __mixerState = !__mixerState;
                if(__mixerState){
                    __mixerSTATEtxt = "OFF";
                }
                if(!__mixerState){
                    __mixerSTATEtxt = "ON";
                    __mixerStop = true;
                }
            }
            else if (down == 1 && xpos<60 && xpos>40 && ypos<80 && ypos>60 && millis()-buttonTimer>50) { //Salt 1 +
                if(_pumpMinutes+1<31)_pumpMinutes = _pumpMinutes+1;
                buttonTimer=millis();
            }  
            else if (down == 1 && xpos<128 && xpos>108 && ypos<80 && ypos>60 && millis()-buttonTimer>50) { //Salt 1 -
                if(_pumpMinutes-1>0)_pumpMinutes = _pumpMinutes-1;
                buttonTimer=millis();
            } 
            else if (down == 1 && xpos<220 && xpos>200 && ypos<80 && ypos>60 && millis()-buttonTimer>50) { //Salt 1 +
                if(_mixerMinutes+1<31)_mixerMinutes = _mixerMinutes+1;
                buttonTimer=millis();
            }  
            else if (down == 1 && xpos<288 && xpos>268 && ypos<80 && ypos>60 && millis()-buttonTimer>50) { //Salt 1 -
                if(_mixerMinutes-1>0)_mixerMinutes = _mixerMinutes-1;
                buttonTimer=millis();
            } 
            else if (down == 1 && xpos<170 && xpos>130 && ypos<210 && ypos>200 && millis()-buttonTimer>500) { //Salt 1 -
                show = !show;
                buttonTimer=millis();
            } 
            break;
        case CALIBsensors:
            if (down == 1 && xpos<300 && xpos>240 && ypos<20 && ypos>5 && millis()-buttonTimer>1000) {
                changeScreen(SMAKER);
                buttonTimer=millis();
            }     
            else if (down == 1 && xpos<140 && xpos>30 && ypos<170 && ypos>90 && millis()-buttonTimer>1000) {
                changeScreen(pHCALIB);
                buttonTimer=millis();
                _pHStepsN = 0;
                _pHSlopeSteps = 0;
                _pHCalibTimer = millis();
            }
            else if (down == 1 && xpos<300 && xpos>190 && ypos<170 && ypos>90 && millis()-buttonTimer>1000) {
                changeScreen(ECCALIB);
                buttonTimer=millis();
                _ECStepsN = 0;
                _ECEZOsteps = 0;
                _ECCalibTimer = millis();
            }  
            break;
        case pHCALIB:
            if (millis()-_pHCalibTimer > 5000 && _pHStepsN == 0){
                _pHStepsN = 1;
                _pHCalibTimer = millis();
            }
            else if (millis()-_pHCalibTimer > 5000 && _pHStepsN == 1){
                _pHStepsN = 2;
            }
            else if (millis()-_pHCalibTimer > 60000 && _pHStepsN == 3){
                _pHSlopeSteps = 1;
                _pHStepsN = 4;
                _pHCalibTimer = millis();
            }
            else if (millis()-_pHCalibTimer > 5000 && _pHStepsN == 4){
                _pHStepsN = 5;
                _pHCalibTimer = millis();
            }
            else if (millis()-_pHCalibTimer > 5000 && _pHStepsN == 5){
                _pHStepsN = 6;
            }
            else if (millis()-_pHCalibTimer > 60000 && _pHStepsN == 7){
                _pHSlopeSteps = 2;
                _pHStepsN = 8;
                _pHCalibTimer = millis();
            }
            switch (_pHStepsN){
                case 0:
                    _pHSteps = "1 Enjuagar";
                    break;
                case 1:
                    _pHSteps = "2 Colocar en sol pH 4.0";
                    break;
                case 2:
                    _pHSteps = "3 Presionar Calib 4.0";
                    break;
                case 3:
                    _pHSteps = "4 Esperar";
                    break;
                case 4:
                    _pHSteps = "5 Enjuagar";
                    break;
                case 5:
                    _pHSteps = "6 Colocar en sol pH 7.0";
                    break;
                case 6:
                    _pHSteps = "7 Presionar Calib 7.0";
                    break;
                case 7:
                    _pHSteps = "8 Esperar";
                    break;
                case 8:
                    _pHSteps = "9 Terminar y guardar";
                    break;
            }
            if (down == 1 && xpos<300 && xpos>240 && ypos<20 && ypos>5 && millis()-buttonTimer>1000) {
                changeScreen(CALIBsensors);
                buttonTimer=millis();
            }     
            else if (down == 1 && xpos<140 && xpos>30 && ypos<190 && ypos>110 && millis()-buttonTimer>1000 && _pHStepsN == 2) {
                buttonTimer=millis();
                _pHStepsN = 3;
                _pHCalibTimer = millis();
            }
            else if (down == 1 && xpos<300 && xpos>190 && ypos<190 && ypos>110 && millis()-buttonTimer>1000 && _pHStepsN == 6) {
                if (_pHStepsN == 6){
                    buttonTimer=millis();
                    _pHStepsN = 7;
                    _pHCalibTimer = millis();
                }
            }
            break;
        case ECCALIB:
            if (millis()-_ECCalibTimer > 5000 && _ECStepsN == 0){
                _ECStepsN = 1;
            }
            else if (millis()-_ECCalibTimer > 60000 && _ECStepsN == 2){
                _ECEZOsteps = 1;
                _ECStepsN = 3;
                _ECCalibTimer = millis();
            }
            else if (millis()-_ECCalibTimer > 5000 && _ECStepsN == 3){
                _ECStepsN = 4;
            }
            else if (millis()-_ECCalibTimer > 60000 && _ECStepsN == 5){
                _ECEZOsteps = 2;
                _ECStepsN = 6;
                _ECCalibTimer = millis();
            }
            switch (_ECStepsN){
                case 0:
                    _ECSteps = "1 Enjuagar y secar";
                    break;
                case 1:
                    _ECSteps = "2 Presionar Calib Seco";
                    break;
                case 2:
                    _ECSteps = "3 Esperar";
                    break;
                case 3:
                    _ECSteps = "4 Colocar en sol 12880";
                    break;
                case 4:
                    _ECSteps = "5 Presionar Calib 12880";
                    break;
                case 5:
                    _ECSteps = "6 Esperar";
                    break;
                case 6:
                    _ECSteps = "7 Terminar para guardar";
                    break;
            }
            if (down == 1 && xpos<300 && xpos>240 && ypos<20 && ypos>5 && millis()-buttonTimer>1000) {
                changeScreen(CALIBsensors);
                buttonTimer=millis();
            }     
            else if (down == 1 && xpos<140 && xpos>30 && ypos<190 && ypos>110 && millis()-buttonTimer>1000) {
                buttonTimer=millis();
                _ECStepsN = 2;
                _ECCalibTimer = millis();
            }
            else if (down == 1 && xpos<300 && xpos>190 && ypos<190 && ypos>110 && millis()-buttonTimer>1000) {
                if (_ECStepsN == 4){
                    buttonTimer=millis();
                    _ECStepsN = 5;
                    _ECCalibTimer = millis();
                }
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