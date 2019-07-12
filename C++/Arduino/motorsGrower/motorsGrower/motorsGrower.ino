/*
 * Falta Agregar:
 *  - Funciones EEPROM para codificar/guardar/descodificar parametros de calibración
 *  - Funciones seriales de comunicación
 *  - *Probar todas las funciones de la libraría*
 *  
 */

#include "growerStepper.h"
#include <EEPROM.h>

/*** Define some Pins ***/
// Grower 1
const byte gr1_XStep1 = 27;
const byte gr1_XDir1 = 29;
const byte gr1_XStep2 = 33;
const byte gr1_XDir2 = 35; 
const byte gr1_YStep = 39;
const byte gr1_YDir = 41; 
const byte gr1_En = 31;

const byte gr1_XHome1 = A13;
const byte gr1_XHome2 = A14;
const byte gr1_YHome = A15;

long MAX_X1 = 0;
long MAX_Y1 = 0;

// Grower 2
const byte gr2_XStep1 = 22;
const byte gr2_XDir1 = 24;
const byte gr2_XStep2 = 26;
const byte gr2_XDir2 = 28; 
const byte gr2_YStep = 30;
const byte gr2_YDir = 32; 
const byte gr2_En = 34;

const byte gr2_XHome1 = A12;
const byte gr2_XHome2 = A11;
const byte gr2_YHome = A10;

long MAX_X2 = 0;
long MAX_Y2 = 0;

// Grower 3
const byte gr3_XStep1 = 36;
const byte gr3_XDir1 = 38;
const byte gr3_XStep2 = 40;
const byte gr3_XDir2 = 42; 
const byte gr3_YStep = 44;
const byte gr3_YDir = 46; 
const byte gr3_En = 48;

const byte gr3_XHome1 = A9;
const byte gr3_XHome2 = A8;
const byte gr3_YHome = A7;

long MAX_X3 = 0;
long MAX_Y3 = 0;

// Grower 4
const byte gr4_XStep1 = 2;
const byte gr4_XDir1 = 3;
const byte gr4_XStep2 = 4;
const byte gr4_XDir2 = 5; 
const byte gr4_YStep = 6;
const byte gr4_YDir = 7; 
const byte gr4_En = 8;

const byte gr4_XHome1 = A6;
const byte gr4_XHome2 = A5;
const byte gr4_YHome = A4;

long MAX_X4 = 0;
long MAX_Y4 = 0;

unsigned long EEPROM_timer;

growerStepper grower1(
  1,
  gr1_XDir1, 
  gr1_XStep1, 
  gr1_XDir2, 
  gr1_XStep2, 
  gr1_YDir, 
  gr1_YStep, 
  gr1_XHome1, 
  gr1_XHome2, 
  gr1_YHome,
  gr1_En
  );

growerStepper grower2(
  2,
  gr2_XDir1, 
  gr2_XStep1, 
  gr2_XDir2, 
  gr2_XStep2, 
  gr2_YDir, 
  gr2_YStep, 
  gr2_XHome1, 
  gr2_XHome2, 
  gr2_YHome,
  gr2_En
  );

growerStepper grower3(
  3,
  gr3_XDir1, 
  gr3_XStep1, 
  gr3_XDir2, 
  gr3_XStep2, 
  gr3_YDir, 
  gr3_YStep, 
  gr3_XHome1, 
  gr3_XHome2, 
  gr3_YHome,
  gr3_En
  );

growerStepper grower4(
  4,
  gr4_XDir1, 
  gr4_XStep1, 
  gr4_XDir2, 
  gr4_XStep2, 
  gr4_YDir, 
  gr4_YStep, 
  gr4_XHome1, 
  gr4_XHome2, 
  gr4_YHome,
  gr4_En
  );

// Serial comunication
String inputstring = "";
bool input_string_complete = false;

void setup() {
  Serial.begin(9600);
  Serial.println(F("Setting up growers..."));
  grower1.begin(HIGH);
  grower2.begin(LOW); // Not send to home
  grower3.begin(LOW); // Not send to home
  grower4.begin(LOW); // Not send to home
  read_EEPROM(HIGH); // Charge calibration parameters for each Grower
  EEPROM_timer = millis();
}

void loop() {
  grower1.run();
  grower2.run();
  grower3.run();
  grower4.run();
  update_CalibrationParameters();
}
