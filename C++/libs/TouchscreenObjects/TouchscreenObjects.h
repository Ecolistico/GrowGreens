#ifndef TouchscreenObjects_h
#define TouchscreenObjects_h

#include <stdlib.h>
#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#include <wiring.h>
#endif

#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>
#include <Adafruit_GFX.h>
#include <bitmaps.h>

#define SOLIDSALTS 8
#define LIQUIDSALTS 2

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define GRAY    222222

#define ECOLISTICO  1
#define SMAKER      2
#define SETTINGS    3
#define SALT1       4
#define SALT2       5
#define SALT3       6
#define SALT4       7
#define SALT5       8
#define SALT6       9
#define SALT7       10
#define SALT8       11
#define SALTSALL    12
#define LIQUID1     13
#define LIQUID2     14
#define LIQUIDALL   15

#define MICROS      20
#define ACID        21
#define ACIDCAL     22
#define MICROSCAL   23
#define EEPROMscr   24
#define NONE        25
#define SCALEscr    26
#define MIXscr      27
#define CALIBsensors 28
#define pHCALIB     29
#define ECCALIB     30

#define TEXT        80
#define CIRCLE      81
#define IMAGE       82
#define LINE        83
#define RECTANGLE   84
#define TRIANGLE    85

#define GROWGREENS  90

#define MINPRESSURE 100
#define MAXPRESSURE 1000

#define XP 22
#define XM A2
#define YP A3
#define YM 23

//Pins for Testing screen as shield
/*#define XP 8
#define XM A2
#define YP A3
#define YM 9*/

#define TS_LEFT 100
#define TS_RT 933
#define TS_TOP 56
#define TS_BOT 899
#define Orientation 1

struct objects{
  uint8_t screen;
  uint8_t  type;
  int x;
  int y;
  uint8_t dim;
  int color;
  char* txt;
  int Width;
  int Height;
  uint8_t img;
};

struct variablesSTR{
  uint8_t screen;
  int x;
  int y;
  uint8_t dim;
  int color;
  String txt;
};
struct variablesFL{
  uint8_t screen;
  int x;
  int y;
  uint8_t dim;
  int color;
  float txt;
};
struct variablesINT{
  uint8_t screen;
  int x;
  int y;
  uint8_t dim;
  int color;
  int txt;
};
struct variablesUINT{
  uint8_t screen;
  int x;
  int y;
  uint8_t dim;
  int color;
  uint8_t txt;
};

//screen,       type,   x,    y,    dim,color,  txt,                  width, height, img  **For lines width and height are X2 and Y2
const objects fixedObjects[] PROGMEM = {
  {ECOLISTICO,  TEXT,       30,   30,   2,  GREEN,  "SMaker",                 0,  0,  0},
  {ECOLISTICO,  TEXT,       20,   200,  2,  GREEN,  "Powered by ECOLISTICO",  0,  0,  0},
  {ECOLISTICO,  TEXT,       30,   70,   4,  WHITE,  "GROW GREENS",            0,  0,  0},
  {ECOLISTICO,  IMAGE,      130,  110,  0,  GREEN,  "SMaker",                 60, 54, GROWGREENS},
  {SMAKER,      TEXT,       100,  40,   3,  WHITE,  "S MAKER",                0,  0,  0},
  {SMAKER,      TEXT,       20,   190,  2,  WHITE,  "Temp: ",                 0,  0,  0},
  {SMAKER,      TEXT,       175,  190,  2,  WHITE,  "Hum: ",                  0,  0,  0},
  {SMAKER,      TEXT,       20,   215,  2,  WHITE,  "pH: ",                   0,  0,  0},
  {SMAKER,      TEXT,       175,  215,  2,  WHITE,  "EC: ",                   0,  0,  0},
  {SMAKER,      TEXT,       210,  10,   2,  GRAY,   "SETTINGS",               0,  0,  0},
  {SMAKER,      TEXT,       130,  10,   2,  GRAY,   "SCALE",                  0,  0,  0},
  {SMAKER,      TEXT,       10,   10,   2,  GRAY,   "MIX",                    0,  0,  0},
  {SMAKER,      TEXT,       60,   10,   2,  GRAY,   "SENS",                   0,  0,  0},
  {SMAKER,      TEXT,       100,  140,  2,  WHITE,  "   g  of",               0,  0,  0},
  {SMAKER,      CIRCLE,     50,   80,   10, RED,    "",                       0,  0,  0},
  {SMAKER,      CIRCLE,     80,   80,   10, GREEN,  "",                       0,  0,  0},
  {SMAKER,      CIRCLE,     110,  80,   10, RED,    "",                       0,  0,  0},
  {SMAKER,      CIRCLE,     140,  80,   10, GREEN,  "",                       0,  0,  0},
  {SMAKER,      CIRCLE,     170,  80,   10, RED,    "",                       0,  0,  0},
  {SMAKER,      CIRCLE,     200,  80,   10, GREEN,  "",                       0,  0,  0},
  {SMAKER,      CIRCLE,     230,  80,   10, RED,    "",                       0,  0,  0},
  {SMAKER,      CIRCLE,     260,  80,   10, GREEN,  "",                       0,  0,  0},
  {SMAKER,      LINE,       0,    65,   0,  YELLOW, "",                       320,65, 0},
  {SETTINGS,    TEXT,       90,   40,   3,  GREEN,  "SETTINGS",               0,  0,  0},
  {SETTINGS,    TEXT,       20,   70,   2,  WHITE,  "pH target: ",            0,  0,  0},
  {SETTINGS,    TEXT,       250,  66,   3,  WHITE,  "+ -",                    0,  0,  0},
  {SETTINGS,    TEXT,       20,   95,   2,  WHITE,  "EC target: ",            0,  0,  0},
  {SETTINGS,    TEXT,       250,  91,   3,  WHITE,  "+ -",                    0,  0,  0},
  {SETTINGS,    TEXT,       20,   120,  2,  WHITE,  "EC max: ",               0,  0,  0},
  {SETTINGS,    TEXT,       250,  116,  3,  WHITE,  "+ -",                    0,  0,  0},
  {SETTINGS,    TEXT,       250,  10,   2,  GRAY,   "BACK",                   0,  0,  0},
  {SETTINGS,    TEXT,       20,   10,   2,  RED,    "SAVE",                   0,  0,  0},
  {SETTINGS,    TEXT,       100,  10,   2,  CYAN,   "Memory",                 0,  0,  0},
  {SETTINGS,    TEXT,       30,   140,  2,  WHITE,  "S1",                     0,  0,  0},
  {SETTINGS,    TEXT,       109,  140,  2,  WHITE,  "S2",                     0,  0,  0},
  {SETTINGS,    TEXT,       188,  140,  2,  WHITE,  "S3",                     0,  0,  0},
  {SETTINGS,    TEXT,       269,  140,  2,  WHITE,  "S4",                     0,  0,  0},
  {SETTINGS,    TEXT,       30,   190,  2,  WHITE,  "S5",                     0,  0,  0},
  {SETTINGS,    TEXT,       109,  190,  2,  WHITE,  "S6",                     0,  0,  0},
  {SETTINGS,    TEXT,       188,  190,  2,  WHITE,  "S7",                     0,  0,  0},
  {SETTINGS,    TEXT,       269,  190,  2,  WHITE,  "S8",                     0,  0,  0},
  {SETTINGS,    RECTANGLE,  5,    136,  0,  GRAY,   "",                       70, 46, 0},
  {SETTINGS,    RECTANGLE,  84,   136,  0,  GRAY,   "",                       70, 46, 0},
  {SETTINGS,    RECTANGLE,  163,  136,  0,  GRAY,   "",                       70, 46, 0},
  {SETTINGS,    RECTANGLE,  242,  136,  0,  GRAY,   "",                       70, 46, 0},
  {SETTINGS,    RECTANGLE,  5,    186,  0,  GRAY,   "",                       70, 46, 0},
  {SETTINGS,    RECTANGLE,  84,   186,  0,  GRAY,   "",                       70, 46, 0},
  {SETTINGS,    RECTANGLE,  163,  186,  0,  GRAY,   "",                       70, 46, 0},
  {SETTINGS,    RECTANGLE,  242,  186,  0,  GRAY,   "",                       70, 46, 0},
  {EEPROMscr,   TEXT,       250,  10,   2,  GRAY,   "BACK",                   0,  0,  0},
  {EEPROMscr,   TEXT,       40,   40,   1,  RED,    "1",                      0,  0,  0},
  {EEPROMscr,   TEXT,       70,   40,   1,  RED,    "2",                      0,  0,  0},
  {EEPROMscr,   TEXT,       100,  40,   1,  RED,    "3",                      0,  0,  0},
  {EEPROMscr,   TEXT,       130,  40,   1,  RED,    "4",                      0,  0,  0},
  {EEPROMscr,   TEXT,       160,  40,   1,  RED,    "5",                      0,  0,  0},
  {EEPROMscr,   TEXT,       190,  40,   1,  RED,    "6",                      0,  0,  0},
  {EEPROMscr,   TEXT,       220,  40,   1,  RED,    "7",                      0,  0,  0},
  {EEPROMscr,   TEXT,       250,  40,   1,  RED,    "8",                      0,  0,  0},
  {EEPROMscr,   TEXT,       280,  40,   1,  RED,    "9",                      0,  0,  0},
  {EEPROMscr,   TEXT,       15,   60,   1,  RED,    "1",                      0,  0,  0},
  {EEPROMscr,   TEXT,       15,   78,   1,  RED,    "2",                      0,  0,  0},
  {EEPROMscr,   TEXT,       15,   96,   1,  RED,    "3",                      0,  0,  0},
  {EEPROMscr,   TEXT,       15,   114,  1,  RED,    "4",                      0,  0,  0},
  {EEPROMscr,   TEXT,       15,   132,  1,  RED,    "5",                      0,  0,  0},
  {EEPROMscr,   TEXT,       15,   150,  1,  RED,    "6",                      0,  0,  0},
  {EEPROMscr,   TEXT,       15,   168,  1,  RED,    "7",                      0,  0,  0},
  {EEPROMscr,   TEXT,       15,   186,  1,  RED,    "8",                      0,  0,  0},
  {EEPROMscr,   TEXT,       15,   204,  1,  RED,    "9",                      0,  0,  0},
  {EEPROMscr,   TEXT,       15,   222,  1,  RED,    "10",                     0,  0,  0},
  {EEPROMscr,   TEXT,       100,  10,   2,  CYAN,   "<   >",                  0,  0,  0},
  {EEPROMscr,   TEXT,       180,  10,   2,  RED,    "ERASE",                  0,  0,  0},
  {EEPROMscr,   TEXT,       10,   10,   2,  GREEN,  "DEFAULT",                0,  0,  0},
  {SCALEscr,    TEXT,       110,  40,   3,  GREEN,  "SCALE",                  0,  0,  0},
  {SCALEscr,    TEXT,       250,  10,   2,  GRAY,   "BACK",                   0,  0,  0},
  {SCALEscr,    TEXT,       230,  90,   3,  WHITE,  "g",                      0,  0,  0},
  {SCALEscr,    TEXT,       40,   130,  2,  WHITE,  "Calibration",            0,  0,  0},
  {SCALEscr,    TEXT,       40,   148,  2,  WHITE,  "  weight",               0,  0,  0},
  {SCALEscr,    TEXT,       47,   210,  2,  WHITE,  " Tare ",                 0,  0,  0},
  {SCALEscr,    TEXT,       200,  210,  2,  WHITE,  " Calib",                 0,  0,  0},
  {SCALEscr,    RECTANGLE,  44,   207,  0,  GRAY,   "",                       78, 22, 0},
  {SCALEscr,    RECTANGLE,  200,  207,  0,  GRAY,   "",                       78, 22, 0},
  {SCALEscr,    TEXT,       210,  140,  3,  WHITE,  "+   -",                  0,  0,  0},
  {SCALEscr,    TEXT,       238,  165,  1,  WHITE,  "+   -",                  0,  0,  0},
  {SCALEscr,    TEXT,       20,   10,   2,  RED,    "SAVE",                   0,  0,  0},
  {MIXscr,      TEXT,       250,  10,   2,  GRAY,   "BACK",                   0,  0,  0},
  {MIXscr,      TEXT,       50,   100,  3,  WHITE,  "PUMP",                   0,  0,  0},
  {MIXscr,      TEXT,       200,  100,  3,  WHITE,  "MIXER",                  0,  0,  0},
  {MIXscr,      RECTANGLE,  30,   90,   0,  GRAY,   "",                       110,80, 0},
  {MIXscr,      RECTANGLE,  190,  90,   0,  GRAY,   "",                       110,80, 0},
  {MIXscr,      TEXT,       40,   60,   3,  WHITE,  "+   -",                  0,  0,  0},
  {MIXscr,      TEXT,       200,  60,   3,  WHITE,  "+   -",                  0,  0,  0},
  {MIXscr,      TEXT,       110,  40,   3,  GREEN,  "  MIX",                  0,  0,  0},
  {CALIBsensors,TEXT,       250,  10,   2,  GRAY,   "BACK",                   0,  0,  0},
  {CALIBsensors,TEXT,       40,   40,   2,  GREEN,  "Sensors to calibrate",   0,  0,  0},
  {CALIBsensors,TEXT,       65,   120,  3,  WHITE,  "pH",                     0,  0,  0},
  {CALIBsensors,TEXT,       230,  120,  3,  WHITE,  "EC",                     0,  0,  0},
  {CALIBsensors,RECTANGLE,  30,   90,   0,  GRAY,   "",                       110,80, 0},
  {CALIBsensors,RECTANGLE,  190,  90,   0,  GRAY,   "",                       110,80, 0},
  {pHCALIB,     TEXT,       250,  10,   2,  GRAY,   "BACK",                   0,  0,  0},
  {pHCALIB,     TEXT,       40,   40,   3,  GREEN,  "Pasos cal pH",           0,  0,  0},
  {pHCALIB,     TEXT,       40,   120,  3,  WHITE,  "CALIB",                  0,  0,  0},
  {pHCALIB,     TEXT,       200,  120,  3,  WHITE,  "CALIB",                  0,  0,  0},
  {pHCALIB,     TEXT,       50,   160,  3,  WHITE,  " 4.0",                   0,  0,  0},
  {pHCALIB,     TEXT,       200,  160,  3,  WHITE,  " 7.0",                   0,  0,  0},  
  {pHCALIB,     RECTANGLE,  30,   110,  0,  GRAY,   "",                      110,80, 0},
  {pHCALIB,     RECTANGLE,  190,  110,  0,  GRAY,   "",                      110,80, 0},
  {ECCALIB,     TEXT,       250,  10,   2,  GRAY,   "BACK",                   0,  0,  0},
  {ECCALIB,     TEXT,       40,   40,   3,  GREEN,  "Pasos cal EC",           0,  0,  0},
  {ECCALIB,     TEXT,       40,   120,  3,  WHITE,  "CALIB",                  0,  0,  0},
  {ECCALIB,     TEXT,       200,  120,  3,  WHITE,  "CALIB",                  0,  0,  0},
  {ECCALIB,     TEXT,       50,   160,  3,  WHITE,  " DRY",                   0,  0,  0},
  {ECCALIB,     TEXT,       200,  160,  3,  WHITE,  "12880",                  0,  0,  0}, 
  {ECCALIB,     RECTANGLE,  30,   110,  0,  GRAY,   "",                       110,80, 0},
  {ECCALIB,     RECTANGLE,  190,  110,  0,  GRAY,   "",                       110,80, 0},
  {SALTSALL,    TEXT,       250,  10,   2,  GRAY,   "BACK",                   0,  0,  0},
  {SALTSALL,    TEXT,       160,  40,   3,  GREEN,  "S Salt",                 0,  0,  0},
  {SALTSALL,    TEXT,       40,   5,    1,  WHITE,  "EMPTY",                  0  ,0  ,0},
  {SALTSALL,    TEXT,       40,   21,   1,  WHITE,  "NH4NO3",                 0  ,0  ,0},
  {SALTSALL,    TEXT,       40,   37,   1,  WHITE,  "KNO3+MgO",               0  ,0  ,0},
  {SALTSALL,    TEXT,       40,   53,   1,  WHITE,  "KNO3+K2SO4",             0  ,0  ,0},
  {SALTSALL,    TEXT,       40,   69,   1,  WHITE,  "5Ca(NO3)2",              0  ,0  ,0},
  {SALTSALL,    TEXT,       40,   85,   1,  WHITE,  "Ca(NO3)2",               0  ,0  ,0},
  {SALTSALL,    TEXT,       40,   101,  1,  WHITE,  "Mg(NO3)2",               0  ,0  ,0},
  {SALTSALL,    TEXT,       40,   117,  1,  WHITE,  "NH4H2PO4",               0  ,0  ,0},
  {SALTSALL,    TEXT,       40,   133,  1,  WHITE,  "KH2PO4",                 0  ,0  ,0},
  {SALTSALL,    TEXT,       40,   149,  1,  WHITE,  "MgSO4-7H2O",             0  ,0  ,0},
  {SALTSALL,    TEXT,       40,   165,  1,  WHITE,  "Micros",                 0  ,0  ,0},
  {SALTSALL,    RECTANGLE,  120,  5,    0,  WHITE,  "",                       10, 10, 0},
  {SALTSALL,    RECTANGLE,  120,  21,   0,  WHITE,  "",                       10, 10, 0},
  {SALTSALL,    RECTANGLE,  120,  37,   0,  WHITE,  "",                       10, 10, 0},
  {SALTSALL,    RECTANGLE,  120,  53,   0,  WHITE,  "",                       10, 10, 0},
  {SALTSALL,    RECTANGLE,  120,  69,   0,  WHITE,  "",                       10, 10, 0},
  {SALTSALL,    RECTANGLE,  120,  85,   0,  WHITE,  "",                       10, 10, 0},
  {SALTSALL,    RECTANGLE,  120,  101,  0,  WHITE,  "",                       10, 10, 0},
  {SALTSALL,    RECTANGLE,  120,  117,  0,  WHITE,  "",                       10, 10, 0},
  {SALTSALL,    RECTANGLE,  120,  133,  0,  WHITE,  "",                       10, 10, 0},
  {SALTSALL,    RECTANGLE,  120,  149,  0,  WHITE,  "",                       10, 10, 0},
  {SALTSALL,    RECTANGLE,  120,  165,  0,  WHITE,  "",                       10, 10, 0},
  {SALTSALL,    TEXT,       200,  105,  2,  WHITE,  "ml OBJ",                 0,  0,  0},
  {SALTSALL,    TEXT,       210,  165,  3,  WHITE,  "+   -",                  0,  0,  0},
  {SALTSALL,    TEXT,       238,  190,  1,  WHITE,  "+   -",                  0,  0,  0},
  {SALTSALL,    TEXT,       40,   200,  2,  WHITE,  "mlRev",                  0,  0,  0},
  {SALTSALL,    TEXT,       120,  220,  1,  WHITE,  "+   -",                  0,  0,  0},
  {SALTSALL,    TEXT,       210,  220,  2,  RED,    "TEST",                   0,  0,  0},
  {SALTSALL,    RECTANGLE,  205,  215,  0,  RED,    "",                       56, 25, 0},
  {LIQUIDALL,   TEXT,       160,  40,   3,  GREEN,  "Liquid",                 0,  0,  0},
  {LIQUIDALL,   TEXT,       100,  210,  2,  WHITE,  "Calibrate",              0,  0,  0},
  {LIQUIDALL,   RECTANGLE,  97,   207,  0,  GRAY,  "",                        115,22, 0},
  {LIQUIDALL,   TEXT,       30,   90,   2,  WHITE,  "ml in 1000L",            0,  0,  0},
  {LIQUIDALL,   TEXT,       250,  90,   3,  WHITE,  "+ -",                    0,  0,  0},
  {LIQUIDALL,   TEXT,       30,   140,  2,  WHITE,  "ml in calibration",      0,  0,  0},
  {LIQUIDALL,   TEXT,       250,  170,  3,  WHITE,  "+ -",                    0,  0,  0},
  {LIQUIDALL,   TEXT,       250,  10,   2,  GRAY,   "BACK",                   0,  0,  0},
};

extern variablesSTR variableObjectsSTR[];
extern variablesFL variableObjectsFL[];
extern variablesINT variableObjectsINT[];
extern variablesUINT variableObjectsUINT[];
extern variablesSTR moveableObjects[];
extern String salts[];

//PROGMEM reading
template <typename T> void PROGMEM_readAnything (const T * sce, T& dest);
template <typename T> T PROGMEM_getAnything (const T * sce);


class SMakerTouchScreen{
    private:
        
        String  variablePrevSTR[14];
        float   variablePrevFL[22];
        int     variablePrevINT[5];
        uint8_t variablePrevUINT[2];
        int moveablePrevX[1];
        int moveablePrevY[1];

        int xpos,ypos;
        uint8_t screens;
        uint8_t EEPROMpage;

        float _temp;
        float _hum;
        float _pH;
        int _EC;
        bool _calib;

        float _increment;
        float _incrementW;
        float _targetGrams;
        float _scale;
        bool BOOT;
        bool scrLoaded;
        bool eepromPrinted;

        unsigned long screensTimer;
        unsigned long printTimer;
        unsigned long buttonTimer;
        unsigned long calibrateTimer;
        unsigned long scaleTimer;
        unsigned long _standbyTime;
        unsigned long _pHCalibTimer;
        unsigned long _ECCalibTimer;
        
        TouchScreen *ts;
        MCUFRIEND_kbv *tft;
        String _pHSteps, _ECSteps;

        bool Touch_getXY();
        void TFTwriteSTR(int x = 5, int y = 5, uint8_t sizeTxt = 3, int color = WHITE, String txt = "");
        void TFTwriteFL(int x = 5, int y = 5, uint8_t sizeTxt = 3, int color = WHITE, float txt = 0.00);
        void TFTwriteINT(int x = 5, int y = 5, uint8_t sizeTxt = 3, int color = WHITE, int txt = 0);
        void TFTwriteUINT(int x = 5, int y = 5, uint8_t sizeTxt = 3, int color = WHITE, uint8_t txt = 0);
        void TFTfullCircle(int x = 5, int y = 5, uint8_t sizeD = 2, int color = WHITE);
        void fillScreens(uint8_t screen);
        void fillVariables(uint8_t screen);
        void updateVariables();
        void screensCheck(bool down = 0);
        
    public:
        SMakerTouchScreen();
        void begin();
        void run();
        void importData(
            float temp= 25.40,
            float hum= 60.10,
            float pH= 5.60,
            int EC= 1259,
            float targetGrams= 215.13,
            float scale= 0.01
        );
        void printEEPROM(uint8_t eepr, int i);
        void importParameters(
            float pHtarget,
            int ECtarget,
            int ECmax,
            float Smgl[SOLIDSALTS],
            float Sflow[SOLIDSALTS],
            uint8_t SOrder[SOLIDSALTS]
        );
        void changeScreen(uint8_t screen, unsigned int standbyTime=5000);
        
        bool CleanEEPROM;
        bool DefaultEEPROM;
        bool SaveEEPROM;
        bool tareSCALE;
        bool calibSCALE;
        bool calibPUMP;
        bool SaveSCALE;
        float __pHtarget;
        int __ECtarget, __ECmax;
        float __SaltmgL[SOLIDSALTS], __Sflow[SOLIDSALTS] ;
        float   __liquidConst[LIQUIDSALTS],__liquidCal[LIQUIDSALTS];
        float _CalibWeight;
        uint8_t pumpToCalib;
        uint8_t __solidSaltCurrentIndex, __liquidSaltCurrentIndex;
        String __mixerSTATEtxt, __pumpSTATEtxt, systemRunningText;
        bool __mixerState, __pumpState, __mixerStop, __pumpStop;
        int _pumpMinutes, _mixerMinutes;
        uint8_t _pHStepsN, _ECStepsN, _ECEZOsteps, _pHSlopeSteps;
        bool systemRunning;
        bool show;
        uint8_t saltsOrder[SOLIDSALTS];
};

#endif
