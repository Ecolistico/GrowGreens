/* Import libraries */
#include <Wire.h>
#include <SPI.h>
#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>
#include <Adafruit_GFX.h>

/* Definitions */
// Pins
#define TFT_DC 9
#define TFT_CS 10
// Colors
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define GRAY    222222
// Touch screen pressure
#define MINPRESSURE 200
#define MAXPRESSURE 1000

// Useful variables
// ALL Touch panels and wiring is DIFFERENT
// copy-paste results from TouchScreen_Calibr_native.ino
const int XP = 8, XM = A2, YP = A3, YM = 9; //240x320 ID=0x9341
const int TS_LEFT = 100, TS_RT = 933, TS_TOP = 56, TS_BOT = 899;
const int Orientation = 3; // 0, 2 are portrait and 1, 3 landscape

/* Objects */
MCUFRIEND_kbv tft;
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
Adafruit_GFX_Button on_btn, off_btn;

/* Structures */ 
typedef struct {
  uint8_t minute_begin;
  uint8_t hour_begin;
  uint8_t minute_end;
  uint8_t hour_end;
} led_config;

union datetime
{ unsigned long epoch;
  byte longBytes[4];
};

union floatNumber
{ float number;
  byte longBytes[4];
};

/* Variables */
// LED´s setting
led_config f1;
led_config f2;
led_config f3;
led_config f4;
led_config f5;
led_config f6;
led_config f7;
led_config f8;

// Sensor
floatNumber temp1;
floatNumber hum1;
floatNumber temp2;
floatNumber hum2;
floatNumber temp3;
floatNumber hum3;
floatNumber temp4;
floatNumber hum4;
floatNumber temp5;
floatNumber hum5;
floatNumber temp6;
floatNumber hum6;

// Time
datetime myTime;

// I2C
bool flag = true;
char x;

// Touchscreen
int xpos, ypos;             // to get touch position
uint8_t screens = 0;        // Switch between screens
bool first_update = false;  // Control when update info dispay in screen
uint8_t minute = 0;         // Configuration start minute
uint8_t hour = 0;           // Configuration start hour
uint8_t minute1 = 0;        // Configuration end minute
uint8_t hour1 = 0;          // Configuration end hour

// Update parameters
uint8_t updatePin = 50;
uint8_t updateFlag = false;
unsigned long updateTimer;

/* Functions */
// I2C
void receive();
void request();
void i2c_debug();
// TFT
void beginTFT();
bool Touch_getXY(void);
void splashscreen();
void home_screen();
void ConfigFloor();
void hourDisplay1();
void minuteDisplay1();
void hourDisplay2();
void minutoDisplay2();
void updateTFT();

void setup() {
  Serial.begin(115200);
  Serial.println("Initialize");
  Wire.begin(77);
  Wire.onReceive(receive);
  Wire.onRequest(request);
  beginTFT();
  yield();
}
 
 
void loop() {
  i2c_debug();
  updateTFT();
}
