/* Import libraries */
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

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

/* Objects */
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

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

/* Functions */
// I2C
void receive();
void request();
void i2c_debug();
// TFT
void tft_debug();

void setup() {
  Serial.begin(115200);
  Serial.println("Initialize");
  Wire.begin(77);
  Wire.onReceive(receive);
  Wire.onRequest(request);
  tft.begin();
  tft.fillScreen(GRAY);
  yield();
  //tft_debug();
}
 
 
void loop() {
  i2c_debug();
}
