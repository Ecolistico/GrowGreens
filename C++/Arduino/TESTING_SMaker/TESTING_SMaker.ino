#include "sMakerCunEEPROM.h"

solutionMaker sMaker;

// Serial comunication
String inputstring = "";
bool input_string_complete = false;

// Serial Communication
void serialEvent();

float mediciones=millis();
int data=1;

void setup() {
  Serial.begin(115200);
  sMaker.read_EEPROM(HIGH); // Charge calibration parameters
  sMaker.begin();
  
  sMaker.readRequest();
  EEPROM.put(4000,sMaker.__pH);
  EEPROM.put(3996,sMaker.__eC);
  
}

void loop() {
  sMaker.run();
  /*if (millis()-mediciones>1800000 && data<337){
    sMaker.readRequest();
    EEPROM.put(4000-8*data,sMaker.__pH);
    EEPROM.put(3996-8*data,sMaker.__eC);
    data++;
  }else{data=0;}*/
     
}
