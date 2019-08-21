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

    if(24%cyclesPerDay == 0 && percentageLight>=0 && percentageLight<=100 && initialHour<24 && initialHour>=0 ){ // If paremeters are wrong then...
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
       // Cycles are not a divisor of 24 hrs
       if(24%cyclesPerDay != 0){ Serial.println(F("Parameter cyclesPerDay incorrect")); }
       // percentageLight out of range [0-100]
       if(percentageLight<0 || percentageLight>100){ Serial.println(F("Parameter percentageLight incorrect")); }
       // initialHourout of Range [0-24) hrs
       if(initialHour>=24 || initialHour<0){ Serial.println(F("Parameter initialHour incorrect")); }
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
      if(24%cyclesPerDay != 0){ Serial.println(F("Parameter cyclesPerDay incorrect")); }
      // percentageLight out of range [0-100]
      if(percentageLight<0 || percentageLight>100){ Serial.println(F("Parameter percentageLight incorrect")); }
      // initialHourout of Range [0-24) hrs
      if(initialHour>=24 || initialHour<0){ Serial.println(F("Parameter initialHour incorrect")); }
    }
  }

bool MultiDay::isDay(uint8_t HOUR, uint8_t MINUTE)
  { float actualHour = HOUR + MINUTE/60;
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

LED_Mod::LED_Mod(String name, uint8_t floor, uint8_t region) // Constructor
  { // Just the first
     if(__TotalLeds<1){
       for (int i=0; i < MAX_LEDS; i++) {
         ptr[i] = NULL; // All Pointers NULL
       }
     }
    __State = LOW;
    __Enable = HIGH;
    __Name = name;
    __Floor = floor;
    __Region = region;
    ptr[__TotalLeds] = this; // Set Static pointer to object
    __TotalLeds++; // Add the new led object to the total
  }

void LED_Mod::printAction(String act)
  { Serial.print(F("LED Section "));
    Serial.print(__Name);
    Serial.print(F(": "));
    Serial.println(act);
  }

void LED_Mod::setState(bool state)
  { if(__Enable){
      if(__State!=state){
        __State = state;
        if(__State) printAction(F("Turn On"));
        else printAction(F("Turn Off"));
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
      if(__Enable){printAction(F("Enable"));}
      else{printAction(F("Disable"));}
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
