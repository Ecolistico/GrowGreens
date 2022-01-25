#include "sMakerCun.h"
//#include <EEPROM.h>

solutionMaker sMaker;

// Serial comunication
String inputstring = "";
bool input_string_complete = false;

// Aux Variables
bool show = true;

/*** Name Functions ***/
// EEPROM
/*void clean_EEPROM();  
void print_EEPROM();
void read_EEPROM(bool pr);
void write_EEPROM(unsigned int pos, byte val);*/
// Serial Communication
void serialEvent();

void setup() {
  Serial.begin(115200);
  sMaker.begin();
  //read_EEPROM(HIGH); // Charge calibration parameters
}

void loop() {
  sMaker.run();   
}
