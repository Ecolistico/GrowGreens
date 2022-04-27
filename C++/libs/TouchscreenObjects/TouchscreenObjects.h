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
#define MICROS      9
#define ACID        10
#define ACIDCAL     11
#define MICROSCAL   12
#define EEPROMscr   13
#define NONE        14
#define SCALEscr    15
#define MIXscr      16

#define TEXT        20
#define CIRCLE      21
#define IMAGE       22
#define LINE        23
#define RECTANGLE   24

#define GROWGREENS  30

#define MINPRESSURE 100
#define MAXPRESSURE 1000

#define XP 22
#define XM A2
#define YP A3
#define YM 23
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

struct variables{
  uint8_t screen;
  int x;
  int y;
  uint8_t dim;
  int color;
  String txt;
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
  {SMAKER,      TEXT,       100,  140,  2,  WHITE,  "   g  of",               0,  0,  0},
  {SMAKER,      CIRCLE,     50,   100,  10, RED,    "",                       0,  0,  0},
  {SMAKER,      CIRCLE,     80,   100,  10, GREEN,  "",                       0,  0,  0},
  {SMAKER,      CIRCLE,     110,  100,  10, RED,    "",                       0,  0,  0},
  {SMAKER,      CIRCLE,     140,  100,  10, GREEN,  "",                       0,  0,  0},
  {SMAKER,      CIRCLE,     170,  100,  10, RED,    "",                       0,  0,  0},
  {SMAKER,      CIRCLE,     200,  100,  10, GREEN,  "",                       0,  0,  0},
  {SMAKER,      CIRCLE,     230,  100,  10, RED,    "",                       0,  0,  0},
  {SMAKER,      CIRCLE,     260,  100,  10, GREEN,  "",                       0,  0,  0},
  {SMAKER,      LINE,       0,    80,   0,  YELLOW, "",                       320,80, 0},
  {SETTINGS,    TEXT,       90,   40,   3,  GREEN,  "SETTINGS",               0,  0,  0},
  {SETTINGS,    TEXT,       20,   80,   2,  WHITE,  "pH target: ",            0,  0,  0},
  {SETTINGS,    TEXT,       250,  76,   3,  WHITE,  "+ -",                    0,  0,  0},
  {SETTINGS,    TEXT,       20,   110,  2,  WHITE,  "EC target: ",            0,  0,  0},
  {SETTINGS,    TEXT,       250,  106,  3,  WHITE,  "+ -",                    0,  0,  0},
  {SETTINGS,    TEXT,       250,  10,   2,  GRAY,   "BACK",                   0,  0,  0},
  {SETTINGS,    TEXT,       20,   10,   2,  RED,    "SAVE",                   0,  0,  0},
  {SETTINGS,    TEXT,       100,  10,   2,  CYAN,   "Memory",                 0,  0,  0},
  {SETTINGS,    TEXT,       133,  135,  2,  GREEN,  "SALTS",                  0,  0,  0},
  {SETTINGS,    TEXT,       30,   160,  2,  WHITE,  "S1",                     0,  0,  0},
  {SETTINGS,    TEXT,       90,   160,  2,  WHITE,  "S2",                     0,  0,  0},
  {SETTINGS,    TEXT,       150,  160,  2,  WHITE,  "S3",                     0,  0,  0},
  {SETTINGS,    TEXT,       210,  160,  2,  WHITE,  "S4",                     0,  0,  0},
  {SETTINGS,    TEXT,       270,  160,  2,  WHITE,  "S5",                     0,  0,  0},
  {SETTINGS,    TEXT,       47,   210,  2,  WHITE,  "Micros",                 0,  0,  0},
  {SETTINGS,    TEXT,       203,  210,  2,  WHITE,  " Acid",                  0,  0,  0},
  {SETTINGS,    RECTANGLE,  10,   156,  0,  GRAY,  "",                        58, 46, 0},
  {SETTINGS,    RECTANGLE,  70,   156,  0,  GRAY,  "",                        58, 46, 0},
  {SETTINGS,    RECTANGLE,  130,  156,  0,  GRAY,  "",                        58, 46, 0},
  {SETTINGS,    RECTANGLE,  190,  156,  0,  GRAY,  "",                        58, 46, 0},
  {SETTINGS,    RECTANGLE,  250,  156,  0,  GRAY,  "",                        58, 46, 0},
  {SETTINGS,    RECTANGLE,  44,   207,  0,  GRAY,  "",                        78, 22, 0},
  {SETTINGS,    RECTANGLE,  200,  207,  0,  GRAY,  "",                        78, 22, 0},
  {SALT1,       TEXT,       250,  10,   2,  GRAY,   "BACK",                   0,  0,  0},
  {SALT1,       TEXT,       110,  40,   3,  GREEN,  "SALT 1",                 0,  0,  0},
  {SALT1,       TEXT,       40,   80,   2,  WHITE,  "KH2PO4",                 0,  0,  0},
  {SALT1,       TEXT,       40,   105,  2,  WHITE,  "NH4NO3",                 0,  0,  0},
  {SALT1,       TEXT,       40,   130,  2,  WHITE,  "Ca(NO3)2",               0,  0,  0},
  {SALT1,       TEXT,       40,   155,  2,  WHITE,  "KNO3",                   0,  0,  0},
  {SALT1,       TEXT,       40,   180,  2,  WHITE,  "MGSO4",                  0,  0,  0},
  {SALT1,       RECTANGLE,  150,  80,   0,  WHITE,  "",                       16, 16, 0},
  {SALT1,       RECTANGLE,  150,  105,  0,  WHITE,  "",                       16, 16, 0},
  {SALT1,       RECTANGLE,  150,  130,  0,  WHITE,  "",                       16, 16, 0},
  {SALT1,       RECTANGLE,  150,  155,  0,  WHITE,  "",                       16, 16, 0},
  {SALT1,       RECTANGLE,  150,  180,  0,  WHITE,  "",                       16, 16, 0},
  {SALT1,       TEXT,       200,  105,  2,  WHITE,  "mg/1000L",               0,  0,  0},
  {SALT1,       TEXT,       210,  165,  3,  WHITE,  "+   -",                  0,  0,  0},
  {SALT1,       TEXT,       238,  190,  1,  WHITE,  "+   -",                  0,  0,  0},
  {SALT2,       TEXT,       250,  10,   2,  GRAY,   "BACK",                   0,  0,  0},
  {SALT2,       TEXT,       110,  40,   3,  GREEN,  "SALT 2",                 0,  0,  0},
  {SALT2,       TEXT,       40,   80,   2,  WHITE,  "KH2PO4",                 0,  0,  0},
  {SALT2,       TEXT,       40,   105,  2,  WHITE,  "NH4NO3",                 0,  0,  0},
  {SALT2,       TEXT,       40,   130,  2,  WHITE,  "Ca(NO3)2",               0,  0,  0},
  {SALT2,       TEXT,       40,   155,  2,  WHITE,  "KNO3",                   0,  0,  0},
  {SALT2,       TEXT,       40,   180,  2,  WHITE,  "MGSO4",                  0,  0,  0},
  {SALT2,       RECTANGLE,  150,  80,   0,  WHITE,  "",                       16, 16, 0},
  {SALT2,       RECTANGLE,  150,  105,  0,  WHITE,  "",                       16, 16, 0},
  {SALT2,       RECTANGLE,  150,  130,  0,  WHITE,  "",                       16, 16, 0},
  {SALT2,       RECTANGLE,  150,  155,  0,  WHITE,  "",                       16, 16, 0},
  {SALT2,       RECTANGLE,  150,  180,  0,  WHITE,  "",                       16, 16, 0},
  {SALT2,       TEXT,       200,  105,  2,  WHITE,  "mg/1000L",               0,  0,  0},
  {SALT2,       TEXT,       210,  165,  3,  WHITE,  "+   -",                  0,  0,  0},
  {SALT2,       TEXT,       238,  190,  1,  WHITE,  "+   -",                  0,  0,  0},
  {SALT3,       TEXT,       250,  10,   2,  GRAY,   "BACK",                   0,  0,  0},
  {SALT3,       TEXT,       110,  40,   3,  GREEN,  "SALT 3",                 0,  0,  0},
  {SALT3,       TEXT,       40,   80,   2,  WHITE,  "KH2PO4",                 0,  0,  0},
  {SALT3,       TEXT,       40,   105,  2,  WHITE,  "NH4NO3",                 0,  0,  0},
  {SALT3,       TEXT,       40,   130,  2,  WHITE,  "Ca(NO3)2",               0,  0,  0},
  {SALT3,       TEXT,       40,   155,  2,  WHITE,  "KNO3",                   0,  0,  0},
  {SALT3,       TEXT,       40,   180,  2,  WHITE,  "MGSO4",                  0,  0,  0},
  {SALT3,       RECTANGLE,  150,  80,   0,  WHITE,  "",                       16, 16, 0},
  {SALT3,       RECTANGLE,  150,  105,  0,  WHITE,  "",                       16, 16, 0},
  {SALT3,       RECTANGLE,  150,  130,  0,  WHITE,  "",                       16, 16, 0},
  {SALT3,       RECTANGLE,  150,  155,  0,  WHITE,  "",                       16, 16, 0},
  {SALT3,       RECTANGLE,  150,  180,  0,  WHITE,  "",                       16, 16, 0},
  {SALT3,       TEXT,       200,  105,  2,  WHITE,  "mg/1000L",               0,  0,  0},
  {SALT3,       TEXT,       210,  165,  3,  WHITE,  "+   -",                  0,  0,  0},
  {SALT3,       TEXT,       238,  190,  1,  WHITE,  "+   -",                  0,  0,  0},
  {SALT4,       TEXT,       250,  10,   2,  GRAY,   "BACK",                   0,  0,  0},
  {SALT4,       TEXT,       110,  40,   3,  GREEN,  "SALT 4",                 0,  0,  0},
  {SALT4,       TEXT,       40,   80,   2,  WHITE,  "KH2PO4",                 0,  0,  0},
  {SALT4,       TEXT,       40,   105,  2,  WHITE,  "NH4NO3",                 0,  0,  0},
  {SALT4,       TEXT,       40,   130,  2,  WHITE,  "Ca(NO3)2",               0,  0,  0},
  {SALT4,       TEXT,       40,   155,  2,  WHITE,  "KNO3",                   0,  0,  0},
  {SALT4,       TEXT,       40,   180,  2,  WHITE,  "MGSO4",                  0,  0,  0},
  {SALT4,       RECTANGLE,  150,  80,   0,  WHITE,  "",                       16, 16, 0},
  {SALT4,       RECTANGLE,  150,  105,  0,  WHITE,  "",                       16, 16, 0},
  {SALT4,       RECTANGLE,  150,  130,  0,  WHITE,  "",                       16, 16, 0},
  {SALT4,       RECTANGLE,  150,  155,  0,  WHITE,  "",                       16, 16, 0},
  {SALT4,       RECTANGLE,  150,  180,  0,  WHITE,  "",                       16, 16, 0},
  {SALT4,       TEXT,       200,  105,  2,  WHITE,  "mg/1000L",               0,  0,  0},
  {SALT4,       TEXT,       210,  165,  3,  WHITE,  "+   -",                  0,  0,  0},
  {SALT4,       TEXT,       238,  190,  1,  WHITE,  "+   -",                  0,  0,  0},
  {SALT5,       TEXT,       250,  10,   2,  GRAY,   "BACK",                   0,  0,  0},
  {SALT5,       TEXT,       110,  40,   3,  GREEN,  "SALT 5",                 0,  0,  0},
  {SALT5,       TEXT,       40,   80,   2,  WHITE,  "KH2PO4",                 0,  0,  0},
  {SALT5,       TEXT,       40,   105,  2,  WHITE,  "NH4NO3",                 0,  0,  0},
  {SALT5,       TEXT,       40,   130,  2,  WHITE,  "Ca(NO3)2",               0,  0,  0},
  {SALT5,       TEXT,       40,   155,  2,  WHITE,  "KNO3",                   0,  0,  0},
  {SALT5,       TEXT,       40,   180,  2,  WHITE,  "MGSO4",                  0,  0,  0},
  {SALT5,       RECTANGLE,  150,  80,   0,  WHITE,  "",                       16, 16, 0},
  {SALT5,       RECTANGLE,  150,  105,  0,  WHITE,  "",                       16, 16, 0},
  {SALT5,       RECTANGLE,  150,  130,  0,  WHITE,  "",                       16, 16, 0},
  {SALT5,       RECTANGLE,  150,  155,  0,  WHITE,  "",                       16, 16, 0},
  {SALT5,       RECTANGLE,  150,  180,  0,  WHITE,  "",                       16, 16, 0},
  {SALT5,       TEXT,       200,  105,  2,  WHITE,  "mg/1000L",               0,  0,  0},
  {SALT5,       TEXT,       210,  165,  3,  WHITE,  "+   -",                  0,  0,  0},
  {SALT5,       TEXT,       238,  190,  1,  WHITE,  "+   -",                  0,  0,  0},
  {MICROS,      TEXT,       250,  10,   2,  GRAY,   "BACK",                   0,  0,  0},
  {MICROS,      TEXT,       160,  40,   3,  GREEN,  "MICROS",                 0,  0,  0},
  {MICROS,      TEXT,       100,  210,  2,  WHITE,  "Calibrate",              0,  0,  0},
  {MICROS,      RECTANGLE,  97,   207,  0,  GRAY,  "",                        115,22, 0},
  {MICROS,      TEXT,       30,   90,   2,  WHITE,  "ml in 1000L",            0,  0,  0},
  {MICROS,      TEXT,       250,  90,   3,  WHITE,  "+ -",                    0,  0,  0},
  {MICROS,      TEXT,       30,   140,  2,  WHITE,  "ml in calibration",      0,  0,  0},
  {MICROS,      TEXT,       250,  170,  3,  WHITE,  "+ -",                    0,  0,  0},
  {MICROS,      TEXT,       20,   40,   2,  WHITE,  "Pump1",                  0,  0,  0},
  {MICROS,      TEXT,       20,   60,   2,  WHITE,  "Pump2",                  0,  0,  0},
  {MICROS,      RECTANGLE,  90,   40,   0,  WHITE,  "",                       16, 16, 0},
  {MICROS,      RECTANGLE,  90,   60,   0,   WHITE,  "",                       16, 16, 0},
  {ACID,        TEXT,       250,  10,   2,  GRAY,   "BACK",                   0,  0,  0},
  {ACID,        TEXT,       160,  40,   3,  GREEN,  " ACID",                  0,  0,  0},
  {ACID,        TEXT,       100,  210,  2,  WHITE,  "Calibrate",              0,  0,  0},
  {ACID,        RECTANGLE,  97,   207,  0,  GRAY,  "",                        115,22, 0},
  {ACID,        TEXT,       30,   90,   2,  WHITE,  "pH in ACID",             0,  0,  0},
  {ACID,        TEXT,       250,  90,   3,  WHITE,  "+ -",                    0,  0,  0},
  {ACID,        TEXT,       30,   140,  2,  WHITE,  "ml in calibration",      0,  0,  0},
  {ACID,        TEXT,       250,  170,  3,  WHITE,  "+ -",                    0,  0,  0},
  {ACID,        TEXT,       20,   40,   2,  WHITE,  "Pump1",                  0,  0,  0},
  {ACID,        TEXT,       20,   60,   2,  WHITE,  "Pump2",                  0,  0,  0},
  {ACID,        RECTANGLE,  90,   40,   0,  WHITE,  "",                       16, 16, 0},
  {ACID,        RECTANGLE,  90,   60,  0,   WHITE,  "",                       16, 16, 0},
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
};

extern variables variableObjects[];
extern variables variableObjects2[];
extern variables moveableObjects[];
extern String salts[];
extern uint8_t saltsOrder[];

//PROGMEM reading
template <typename T> void PROGMEM_readAnything (const T * sce, T& dest);
template <typename T> T PROGMEM_getAnything (const T * sce);


class SMakerTouchScreen{
    private:
        
        String variablePrev[32];
        String variablePrev2[7];
        int moveablePrevX[7];
        int moveablePrevY[7];

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
        float _acidPH;
        float _acidQTY;
        float _microsQTY;
        int _microsML;
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
        unsigned int _standbyTime;
        
        TouchScreen *ts;
        MCUFRIEND_kbv *tft;


        bool Touch_getXY();
        void TFTwrite(int x = 5, int y = 5, uint8_t sizeTxt = 3, int color = WHITE, String txt = "");
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
            float pHtarget= 5.72,
            int ECtarget= 1625,
            float S1mgl= 400.0,
            float S2mgl= 400.0,
            float S3mgl= 400.0,
            float S4mgl= 400.0,
            float S5mgl= 400.0,
            float acidPH= 5.0,
            float acidQTY= 6.16,
            float microsML= 10,
            float microsQTY= 6.16,
            uint8_t microsPump = 0,
            uint8_t acidPump = 1
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
        int __ECtarget;
        float _S1mgl;
        float _S2mgl;
        float _S3mgl;
        float _S4mgl;
        float _S5mgl;
        float _CalibWeight;
        uint8_t pumpToCalib;
        uint8_t _pumpsOrder[2];
        String __mixerSTATEtxt, __pumpSTATEtxt;
        bool __mixerState, __pumpState, __mixerStop, __pumpStop;
        int _pumpMinutes, _mixerMinutes;
};

#endif
