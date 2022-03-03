#include "sMakerCunEEPROM.h"

solutionMaker sMaker;

// Serial comunication
String inputstring = "";
bool input_string_complete = false;

// Serial Communication
void serialEvent();

float mediciones=millis();
int data=1;

// Debug
int phPin = A3;    // select the input pin for the potentiometer
float phValue = 0;  // variable to store the value coming from the sensor
unsigned long timer = 0;

float exponential_filter(float alpha, float t, float t_1)
  { if(isnan(alpha) || isnan(t) || isnan(t_1)){
      return t;
    }
    else{
      return (t*alpha+(1-alpha)*t_1);
    }
  }

float overSample(uint8_t readings = 10){
  float val = 0;
  for(int i = 0; i<readings; i++) val = val + analogRead(phPin);
  return float(val)/float(readings);
}

void setup() {
  Serial.begin(115200);
  pinMode(phPin, INPUT);
  //sMaker.read_EEPROM(HIGH); // Charge calibration parameters
  sMaker.begin();
  
  sMaker.readRequest();
  /*EEPROM.put(4000,sMaker.__pH);
  EEPROM.put(3996,sMaker.__eC);*/
  phValue = overSample();
  timer = millis();
}

void loop() {
  sMaker.run();

  if(millis()-timer > 1000){
    timer = millis();
    phValue = exponential_filter(0.75, overSample(), phValue);
    Serial.println(0.0116*phValue+0.5146);  
  }
  
  /*if (millis()-mediciones>1800000 && data<337){
    sMaker.readRequest();
    EEPROM.put(4000-8*data,sMaker.__pH);
    EEPROM.put(3996-8*data,sMaker.__eC);
    data++;
  }else{data=0;}*/
     
}
