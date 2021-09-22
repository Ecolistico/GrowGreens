// fan.cpp
//
// Copyright (C) 2019 Grow

#include "fan.h"

/***   Fan   ***/
// Statics variables definitions
uint8_t fan::_numberFan = 0;

fan::fan( // Constructor
    uint8_t floor,
    unsigned long t_cycle,
    uint8_t t_on
  )
   {
     // Default parameters
     _State = LOW;
     _Enable = true;
     _Floor = floor;

     _CycleTime = t_cycle*60UL*1000UL;
     _TimeOn = t_on;

     resetTime();

     _numberFan++; // Add the new fan to the total
   }

void fan::setTime()
  { if(_State == HIGH) resetTime();
    else _ActualTime = millis() - (_CycleTime*_TimeOn/100);
  }

void fan::resetTime()
  {  _ActualTime = millis(); }

void fan::fanPrint(String act, uint8_t level /*= 0*/)
  { if(level==0){ Serial.print(F("debug,")); } // Debug
    else if(level==1){ Serial.print(F("info,")); } // Info
    else if(level==2){ Serial.print(F("warning,")); } // Warning
    else if(level==3){ Serial.print(F("error,")); } // Error
    else if(level==4){ Serial.print(F("critical,")); } // Error
    Serial.print(F("Fan "));
    Serial.print(_Floor+1);
    Serial.print(F(": "));
    Serial.println(act);
  }

void fan::fanPrint(String act1, String act2, String act3, uint8_t level /*= 0*/)
  { if(level==0){ Serial.print(F("debug,")); } // Debug
    else if(level==1){ Serial.print(F("info,")); } // Info
    else if(level==2){ Serial.print(F("warning,")); } // Warning
    else if(level==3){ Serial.print(F("error,")); } // Error
    else if(level==4){ Serial.print(F("critical,")); } // Error
    Serial.print(F("Fan "));
    Serial.print(_Floor+1);
    Serial.print(F(": "));
    Serial.print(act1);
    Serial.print(act2);
    Serial.println(act3);
  }

unsigned long fan::getTime()
  { return (millis() - _ActualTime); }

void fan::setTimeOn(uint8_t t_on)
  { if (t_on>=0 && t_on<=100) {
      _TimeOn = t_on;
      fanPrint(F("Time on changed to "), String(t_on), F(" %"), 0);
    }
    else if(t_on==0) enable(false);
    else fanPrint(F("Cannot change time on. Parameter has to be express in percentage (0-100)"), 3);
  }

void fan::setCycleTime(unsigned long t_cycle)
  { if(t_cycle!=0) {
      _CycleTime = t_cycle*60UL*1000UL;
      fanPrint(F("Cycle time changed to "), String(t_cycle), F(" minutes"), 0);
    }
    else fanPrint(F("Cycle time has to be greter than 0 minutes "), 3);
  }

uint8_t fan::getTimeOn()
  { return _TimeOn; }

unsigned long fan::getCycleTime()
  { return _CycleTime/(60UL*1000UL); }

uint8_t fan::getFloor()
  { return _Floor; }

void fan::turnOn(bool rst_time)
  { _State = HIGH;
    setTime();
    if(rst_time){setTime(); }
  }

void fan::turnOff(bool rst_time)
  { _State = LOW;
    setTime();
    if(rst_time){setTime(); }
  }

void fan::enable(bool en)
  { if(_Enable!=en){
      _Enable = en;
      if(_Enable){ fanPrint(F("Enable"), 0); }
      else{ fanPrint(F("Disable"), 0); }
    }
  }

bool fan::isEnable()
  { return _Enable;   }

bool fan::getState()
  { return _State; }

void fan::run()
  {  if(_Enable){
       if( millis()-_ActualTime<(_CycleTime*_TimeOn/100) && _State==LOW){
         _State = HIGH;
         fanPrint(F("Turn On"), 0);
       }
       else if(millis()-_ActualTime>=(_CycleTime*_TimeOn/100) && millis()-_ActualTime<_CycleTime && _State==HIGH){
         _State = LOW;
         fanPrint(F("Turn Off"), 0);
       }
       else if(millis()-_ActualTime>=_CycleTime){
         resetTime();
       }
     }
     else if (_State) _State = LOW;
  }

/***   systemFan   ***/
systemFan::systemFan(uint8_t floorNum, dynamicMem & myMem) // Constructor
   { if(floorNum<=MAX_FLOOR_NUMBER){
       _floorNumber = floorNum;
       for (int i = 0; i<floorNum; i++) {
         fan_memory fanParam = myMem.read_fan(i); // Get fan config
         _fan[i] = new fan(i, fanParam.cycleTime, fanParam.timeOn);
       }
     }
     else Serial.println(F("critical,System Fan ERROR: Floors exceed max number"));
   }

void systemFan::systemPrint(String act1, String act2, String act3, uint8_t level=0)
 { if(level==0){ Serial.print(F("debug,")); } // Debug
   else if(level==1){ Serial.print(F("info,")); } // Info
   else if(level==2){ Serial.print(F("warning,")); } // Warning
   else if(level==3){ Serial.print(F("error,")); } // Error
   else if(level==4){ Serial.print(F("critical,")); } // Error
   Serial.print(F("Fan System"));
   Serial.print(F(": "));
   Serial.print(act1);
   Serial.print(act2);
   Serial.println(act3);
 }

void systemFan::enable(bool en)
  { for (int i = 0; i<_floorNumber; i++) _fan[i]->enable(en); }

void systemFan::run()
  { for (int i = 0; i<_floorNumber; i++) _fan[i]->run(); }
