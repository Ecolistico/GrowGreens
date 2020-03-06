// day.cpp
//
// Copyright (C) 2019 Grow

#include "day.h"

/***   multiDay   ***/
MultiDay::MultiDay() // Constructor
  { __isItDay = LOW;
    __daysPerDay = 1;
    __hoursDay = 16;
    __hoursNight = 8;
    __beginHour[0] = 6;
   }

MultiDay::MultiDay(uint8_t cyclesPerDay, float percentageLight, float initialHour) // Constructor
  { __isItDay = LOW;

    if(24%cyclesPerDay == 0 && percentageLight>=0 && percentageLight<=100 && initialHour<24 && initialHour>=0 ){ // If paremeters are fine then...
      int hoursPerDay = 24/cyclesPerDay;
      __daysPerDay = cyclesPerDay;
      __hoursDay = (hoursPerDay*percentageLight)/100;
      __hoursNight = hoursPerDay - __hoursDay;

      __beginHour[0] = initialHour-hoursPerDay*int(initialHour/hoursPerDay);

      if(__daysPerDay>1){
        for(int i=1; i<__daysPerDay; i++){
          __beginHour[i] = __beginHour[i-1] + ((24/__daysPerDay));
        }
      }
    }

    else{ // If parameters are wrong set default cycle.
       __daysPerDay = 1;
       __hoursDay = 16;
       __hoursNight = 8;
       __beginHour[0] = 6;
    }
  }

bool MultiDay::getState()
  { return __isItDay; }

void MultiDay::redefine(uint8_t cyclesPerDay, float percentageLight, float initialHour )
  { if(24%cyclesPerDay == 0 && percentageLight>=0 && percentageLight<=100 && initialHour<24 && initialHour>=0 ){ // If paremeters are wrong then...
      int hoursPerDay = 24/cyclesPerDay;
      __daysPerDay = cyclesPerDay;
      __hoursDay = (hoursPerDay*percentageLight)/100;
      __hoursNight = hoursPerDay - __hoursDay;

      __beginHour[0] = initialHour-hoursPerDay*int(initialHour/hoursPerDay);

      if(__daysPerDay>1){
        for(int i=1; i<__daysPerDay; i++){
          __beginHour[i] = __beginHour[i-1] + ((24/__daysPerDay));
        }
      }
    }

    else{ // If parameters are wrong
      // Cycles are not a divisor of 24 hrs
      if(24%cyclesPerDay != 0){ Serial.println(F("error,Parameter cyclesPerDay incorrect")); }
      // percentageLight out of range [0-100]
      if(percentageLight<0 || percentageLight>100){ Serial.println(F("error,Parameter percentageLight incorrect")); }
      // initialHourout of Range [0-24) hrs
      if(initialHour>=24 || initialHour<0){ Serial.println(F("error,Parameter initialHour incorrect")); }
    }
  }

bool MultiDay::isDay(uint8_t HOUR, uint8_t MINUTE)
  { float actualHour = float(HOUR) + float(MINUTE)/60;
    uint8_t control_Stage = 200;

    for(int i=__daysPerDay-1; i>=0; i--){
      if(__beginHour[i]<=actualHour){
        control_Stage = i;
        break;
      }
    }

    if(control_Stage == 200){
      control_Stage = __daysPerDay-1;
    }

    if(__daysPerDay-control_Stage==1 && actualHour-__beginHour[control_Stage]<0){
      if(actualHour >= __beginHour[control_Stage]-24 && actualHour < __beginHour[control_Stage]+__hoursDay-24){
        __isItDay = HIGH;
      }
      else if(actualHour >= __beginHour[control_Stage]+__hoursDay-24 && actualHour < __beginHour[control_Stage]+__hoursDay+__hoursNight-24){
        __isItDay = LOW;
      }
    }
    else{
      if(actualHour >= __beginHour[control_Stage] && actualHour < __beginHour[control_Stage]+__hoursDay){
        __isItDay = HIGH;
      }
      else if(actualHour >= __beginHour[control_Stage]+__hoursDay && actualHour < __beginHour[control_Stage]+__hoursDay+__hoursNight){
        __isItDay = LOW;
      }
    }
     return (__isItDay);
  }

/***   LED_Mod   ***/
// Statics variables definitions
uint8_t LED_Mod::__TotalLeds = 0;
LED_Mod *LED_Mod::ptr[MAX_LEDS];

LED_Mod::LED_Mod(uint8_t floor, uint8_t region) // Constructor
  { // Just the first
     if(__TotalLeds<1){
       for (int i=0; i < MAX_LEDS; i++) {
         ptr[i] = NULL; // All Pointers NULL
       }
     }
    __State = LOW;
    __Enable = HIGH;
    __Floor = floor;
    __Region = region;
    ptr[__TotalLeds] = this; // Set Static pointer to object
    __TotalLeds++; // Add the new led object to the total
  }

void LED_Mod::printAction(String act, uint8_t level=0)
  { if(level==0){ Serial.print(F("debug,")); } // Debug
    else if(level==1){ Serial.print(F("info,")); } // Info
    else if(level==2){ Serial.print(F("warning,")); } // Warning
    else if(level==3){ Serial.print(F("error,")); } // Error
    else if(level==4){ Serial.print(F("critical,")); } // Error
    Serial.print(F("LED Section L"));
    Serial.print(__Floor+1);
    Serial.print(F("S"));
    Serial.print(__Region+1);
    Serial.print(F(": "));
    Serial.println(act);
  }

void LED_Mod::setState(bool state)
  { if(__Enable){
      if(__State!=state){
        __State = state;
        if(__State) printAction(F("Turn On"), 0);
        else printAction(F("Turn Off"), 0);
      }
    }
  }

bool LED_Mod::getState()
  { if(__Enable){ return __State; }
    else{ return LOW; }
  }

uint8_t LED_Mod::getFloor()
  { return __Floor; }

uint8_t LED_Mod:: getRegion()
  { return __Region; }

void LED_Mod::enable(bool en)
  { if(__Enable!=en){
      __Enable = en;
      if(__Enable){printAction(F("Enable"), 0);}
      else{printAction(F("Disable"), 0);}
    }
  }

bool LED_Mod::isEnable()
  { return __Enable; }

void LED_Mod::turnOn()
  { setState(HIGH); }

void LED_Mod::turnOff()
  { setState(LOW); }

void LED_Mod::turnOn(uint8_t floor)
  { for(int i = 0; i<__TotalLeds; i++){
      if(ptr[i]->getFloor()==floor){ ptr[i]->turnOn(); }
    }
  }

void LED_Mod::turnOff(uint8_t floor)
  { for(int i = 0; i<__TotalLeds; i++){
      if(ptr[i]->getFloor()==floor){ ptr[i]->turnOff(); }
    }
  }

void LED_Mod::enable(uint8_t floor, uint8_t region)
  { for(int i = 0; i<__TotalLeds; i++){
      if(ptr[i]->getFloor()==floor){
        if(ptr[i]->getRegion()<region){ ptr[i]->enable(true); }
        else{ ptr[i]->enable(false); }
      }
    }
  }

/***   fixDay   ***/
fixDay::fixDay() // Constructor
  { __Led1 = LOW;
    __Led2 = LOW;
    __Led3 = LOW;
    __Led4 = LOW;
   }
   
void fixDay::printAction(String act, uint8_t level=0)
  { if(level==0){ Serial.print(F("debug,")); } // Debug
    else if(level==1){ Serial.print(F("info,")); } // Info
    else if(level==2){ Serial.print(F("warning,")); } // Warning
    else if(level==3){ Serial.print(F("error,")); } // Error
    else if(level==4){ Serial.print(F("critical,")); } // Error
    Serial.println(act);
  }

bool fixDay::getLed1()
  { return __Led1; }
  
bool fixDay::getLed2()
  { return __Led2; }
  
bool fixDay::getLed3()
  { return __Led3; }
  
bool fixDay::getLed4()
  { return __Led4; }
  
void fixDay::updateLedState(uint8_t hour)
  { if(hour==0 || hour==1) {
      __Led1 = LOW;
      __Led2 = LOW;
      __Led3 = HIGH;
      __Led4 = HIGH;
    } 
    else if(hour==2 || hour==3){
      __Led1 = HIGH;
      __Led2 = LOW;
      __Led3 = LOW;
      __Led4 = HIGH;
    }
    else if(hour==4 || hour==5){
      __Led1 = HIGH;
      __Led2 = HIGH;
      __Led3 = LOW;
      __Led4 = LOW;
    }
    else if(hour==6 || hour==7){
      __Led1 = LOW;
      __Led2 = HIGH;
      __Led3 = HIGH;
      __Led4 = LOW;
    }
    else if(hour==8 || hour==9){
      __Led1 = LOW;
      __Led2 = LOW;
      __Led3 = HIGH;
      __Led4 = HIGH;
    }
    else if(hour==10 || hour==11){
      __Led1 = HIGH;
      __Led2 = LOW;
      __Led3 = LOW;
      __Led4 = HIGH;
    }
    else if(hour==12 || hour==13){
      __Led1 = HIGH;
      __Led2 = HIGH;
      __Led3 = LOW;
      __Led4 = LOW;
    }
    else if(hour==14 || hour==15){
      __Led1 = LOW;
      __Led2 = HIGH;
      __Led3 = HIGH;
      __Led4 = LOW;
    }
    else if(hour==16 || hour==17){
      __Led1 = LOW;
      __Led2 = LOW;
      __Led3 = HIGH;
      __Led4 = HIGH;
    }
    else if(hour==18 || hour==19){
      __Led1 = HIGH;
      __Led2 = LOW;
      __Led3 = LOW;
      __Led4 = HIGH;
    }
    else if(hour==20 || hour==21){
      __Led1 = HIGH;
      __Led2 = HIGH;
      __Led3 = LOW;
      __Led4 = LOW;
    }
    else if(hour==22 || hour==23){
      __Led1 = LOW;
      __Led2 = HIGH;
      __Led3 = HIGH;
      __Led4 = LOW;
    }
  }
