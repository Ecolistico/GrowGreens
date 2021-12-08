#include <sMaker.h>
#include <EEPROM.h>

sMaker my_sMaker(
          48, // Dir1
          49, // Step1
          52, // Dir2
          53, // Step2
          A11, // Dir3
          A10, // Step3
          A7, // Dir4
          A6, // Step4
          45, // En1
          //43, // En2
          //37, // En3
          //31, // En4
          10, // Motor1
          11, // Motor2
          //5, // Motor1En
          //6, // Motor2En
          //46, // LedS1
          //48, // LedS2
          //50, // LedS3
          //52, // LedS4
          //44, // LedA1
          //42, // LedA2
          //38, // LedReady
          //40, // LedWorking
          17, // Temperature sensor
          24, // LCD button
          27 // Relay (motor or actuator to get the solution disolve)
        );

// Serial comunication
String inputstring = "";
bool input_string_complete = false;

// Aux Variables
bool show = true;

/*** Name Functions ***/
// EEPROM
void clean_EEPROM();
void print_EEPROM();
void read_EEPROM(bool pr);
void write_EEPROM(unsigned int pos, byte val);
// Serial Communication
void serialEvent();

void setup() {
  Serial.begin(115200);
  my_sMaker.begin();
  read_EEPROM(HIGH); // Charge calibration parameters
}

void loop() {
  my_sMaker.run();  
}
