#include <growerStepper.h>
#include <Wire.h>
#include <External_EEPROM.h> // Click here to get the library: http://librarymanager/All#SparkFun_External_EEPROM

/*
You need to define the buffer lengths in SparkFun_External_EEPROM.h for 
generic arduino mega 2560 board. For more details ask Eleazar Dominguez(eleazardg@sippys.com.mx).
*/
/* Pendiente:
 *  2- Agregar funciones para Magnetic Switch
 *  2.1- Cuando cambie el estado imprimir en pantalla el cambio de estado.
 *  
 */

ExternalEEPROM myEEPROM;

/*** Define some Pins ***/
// Grower 1
#define gr1_XStep1 35
#define gr1_XDir1  36
#define gr1_XStep2 30
#define gr1_XDir2  31 
#define gr1_YStep  28
#define gr1_YDir   29 
#define gr1_En     23
#define gr1_MS     22

#define gr1_XHome1 32
#define gr1_XHome2 33
#define gr1_YHome  34

// Grower 2
#define gr2_XStep1 47
#define gr2_XDir1  48
#define gr2_XStep2 42
#define gr2_XDir2  43 
#define gr2_YStep  40
#define gr2_YDir   41 
#define gr2_En     53
#define gr2_MS     52

#define gr2_XHome1 44
#define gr2_XHome2 45
#define gr2_YHome  46

// Grower 3
#define gr3_XStep1 24
#define gr3_XDir1  25
#define gr3_XStep2 9
#define gr3_XDir2  8
#define gr3_YStep  11
#define gr3_YDir   10 
#define gr3_En     13
#define gr3_MS     12

#define gr3_XHome1 26
#define gr3_XHome2 27
#define gr3_YHome  37

// Grower 4
#define gr4_XStep1 51
#define gr4_XDir1  50

#define gr4_XHome1 38
#define gr4_XHome2 39
#define gr4_YHome  49
  
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) // For Arduino Mega 2560
  // Grower 4
  #define gr4_XStep2 A5
  #define gr4_XDir2  A4
  #define gr4_YStep  A3
  #define gr4_YDir   A2
  #define gr4_En     A0
  #define gr4_MS     A1
#else // For Arduino Due
  // Grower 4
  #define gr4_XStep2 59
  #define gr4_XDir2  58
  #define gr4_YStep  57
  #define gr4_YDir   56 
  #define gr4_En     54
  #define gr4_MS     55
#endif

/*** Define Aux Variables ***/
// Grower 1
long MAX_X1 = 0;
long MAX_Y1 = 0;
byte stepsPerRev1 = 0;
byte microstep1 = 0;
byte pulleyTeeth1 = 0;
byte xTooth1 = 0;
byte yTooth1 = 0;

// Grower 2
long MAX_X2 = 0;
long MAX_Y2 = 0;
byte stepsPerRev2 = 0;
byte microstep2 = 0;
byte pulleyTeeth2 = 0;
byte xTooth2 = 0;
byte yTooth2 = 0;

// Grower 3
long MAX_X3 = 0;
long MAX_Y3 = 0;
byte stepsPerRev3 = 0;
byte microstep3 = 0;
byte pulleyTeeth3 = 0;
byte xTooth3 = 0;
byte yTooth3 = 0;

// Grower 4
long MAX_X4 = 0;
long MAX_Y4 = 0;
byte stepsPerRev4 = 0;
byte microstep4 = 0;
byte pulleyTeeth4 = 0;
byte xTooth4 = 0;
byte yTooth4 = 0;

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
  gr1_En,
  gr1_MS
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
  gr2_En,
  gr2_MS
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
  gr3_En,
  gr3_MS
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
  gr4_En,
  gr4_MS
  );

// Serial comunication
String inputstring = "";
bool input_string_complete = false;

void setup() {
  Serial.begin(115200);
  Serial.println(gr4_XStep2);
  Serial.println(gr4_MS);
  begin_EEPROM();
  Serial.println(F("Setting up growers..."));
  getConfig();
  // Going Home
  grower1.begin(HIGH, stepsPerRev1, microstep1, pulleyTeeth1, xTooth1, yTooth1);
  grower2.begin(HIGH, stepsPerRev2, microstep2, pulleyTeeth2, xTooth2, yTooth2);
  grower3.begin(HIGH, stepsPerRev3, microstep3, pulleyTeeth3, xTooth3, yTooth3);
  grower4.begin(HIGH, stepsPerRev4, microstep4, pulleyTeeth4, xTooth4, yTooth4);
  // Withouth Going Home
  /*
  grower1.begin(LOW, stepsPerRev1, microstep1, pulleyTeeth1, xTooth1, yTooth1);
  grower2.begin(LOW, stepsPerRev2, microstep2, pulleyTeeth2, xTooth2, yTooth2);
  grower3.begin(LOW, stepsPerRev3, microstep3, pulleyTeeth3, xTooth3, yTooth3);
  grower4.begin(LOW, stepsPerRev4, microstep4, pulleyTeeth4, xTooth4, yTooth4);
  */
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
