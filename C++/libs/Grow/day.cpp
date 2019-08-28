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
      if(24%cyclesPerDay != 0){ Serial.println(F("Parameter cyclesPerDay incorrect")); }
      // percentageLight out of range [0-100]
      if(percentageLight<0 || percentageLight>100){ Serial.println(F("Parameter percentageLight incorrect")); }
      // initialHourout of Range [0-24) hrs
      if(initialHour>=24 || initialHour<0){ Serial.println(F("Parameter initialHour incorrect")); }
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

/***   irrigationController   ***/
irrigationController::irrigationController() // Default Constructor
  { __solution = 0; // Not solution define yet
    __actualCycle = 0;
    __cyclesPerDay = 1;
    __hoursPerCycle = 24;
    __beginHour[0] = 0;

    for(int i=0; i<MAX_SOLUTIONS_NUMBER; i++){
      __order[i] = i;
      __percentage[i] = 25;
    }
  }

// Constructor
irrigationController::irrigationController( uint8_t cyclesPerDay,
                                            uint8_t initialHour,
                                            uint8_t ord1,
                                            uint8_t ord2,
                                            uint8_t ord3,
                                            uint8_t ord4,
                                            uint8_t per1,
                                            uint8_t per2,
                                            uint8_t per3,
                                            uint8_t per4 ){
    __solution = 0; // Not solution define yet
    __actualCycle = 0;

    // If paremeters are fine then...
    if(24%cyclesPerDay==0 && initialHour<24 && initialHour>=0 &&
       ord1+ord2+ord3+ord4==6 &&
       ord1!=ord2 && ord1!=ord3 && ord1!=ord4 && ord2!=ord3 && ord2!=ord4 && ord3!=ord4 &&
       per1+per2+per3+per4==100){

      __cyclesPerDay = cyclesPerDay;
      __hoursPerCycle = 24/cyclesPerDay;

      __beginHour[0] = initialHour-__hoursPerCycle*int(initialHour/__hoursPerCycle);

      if(__cyclesPerDay>1){
        for(int i=1; i<__cyclesPerDay; i++){
          __beginHour[i] = __beginHour[i-1] + __hoursPerCycle;
        }
      }

      __order[0] = ord1; __order[1] = ord2; __order[2] = ord3; __order[3] = ord4;
      __percentage[0] = per1; __percentage[1] = per2;
      __percentage[2] = per3; __percentage[3] = per4;
    }

    else{ // If parameters are wrong set default cycle.
        __cyclesPerDay = 1;
        __hoursPerCycle = 24;
        __beginHour[0] = 0;

        for(int i=0; i<MAX_SOLUTIONS_NUMBER; i++){
          __order[i] = i;
          __percentage[i] = 25;
        }
    }
  }

uint8_t irrigationController::getSolution()
  { return __solution; }

uint8_t irrigationController::getCycle()
  { return __actualCycle; }

void irrigationController::redefine( uint8_t cyclesPerDay,
                                     uint8_t initialHour,
                                     uint8_t ord1,
                                     uint8_t ord2,
                                     uint8_t ord3,
                                     uint8_t ord4,
                                     uint8_t per1,
                                     uint8_t per2,
                                     uint8_t per3,
                                     uint8_t per4 ){
    __actualCycle = 0;

    // If paremeters are fine then...
    if(24%cyclesPerDay==0 && initialHour<24 && initialHour>=0 &&
       ord1+ord2+ord3+ord4==6 &&
       ord1!=ord2 && ord1!=ord3 && ord1!=ord4 && ord2!=ord3 && ord2!=ord4 && ord3!=ord4 &&
       per1+per2+per3+per4==100){

      __cyclesPerDay = cyclesPerDay;
      __hoursPerCycle = 24/cyclesPerDay;

      __beginHour[0] = initialHour-__hoursPerCycle*int(initialHour/__hoursPerCycle);

      if(__cyclesPerDay>1){
        for(int i=1; i<__cyclesPerDay; i++){
          __beginHour[i] = __beginHour[i-1] + __hoursPerCycle;
        }
      }

      __order[0] = ord1; __order[1] = ord2; __order[2] = ord3; __order[3] = ord4;
      __percentage[0] = per1; __percentage[1] = per2;
      __percentage[2] = per3; __percentage[3] = per4;
    }

    else{ // If parameters are wrong set default cycle.
        __cyclesPerDay = 1;
        __hoursPerCycle = 24;
        __beginHour[0] = 0;

        for(int i=0; i<MAX_SOLUTIONS_NUMBER; i++){
          __order[i] = i;
          __percentage[i] = 25;
        }
       // Cycles are not a divisor of 24 hrs
       if(24%cyclesPerDay != 0){ Serial.println(F("Parameter cyclesPerDay incorrect")); }
       // percentages not sum 100
       if(per1+per2+per3+per4!=100){ Serial.println(F("Percentages sum has to be equal to 100")); }
       // order parameters wrong
       if(ord1+ord2+ord3+ord4!=6 || ord1==ord2 || ord1==ord3 || ord1==ord4 ||
          ord2==ord3 || ord2==ord4 || ord3==ord4){ Serial.println(F("Order parameters incorrect")); }
    }
  }

uint8_t irrigationController::whatSolution(uint8_t HOUR, uint8_t MINUTE)
  { bool resp = false;
    uint8_t control_Stage = 200;
    float actualHour = float(HOUR) + float(MINUTE)/60;

    // Get the actual irrigation stage
    for(int i=__cyclesPerDay-1; i>=0; i--){
      if(__beginHour[i]<=actualHour){
        control_Stage = i;
        break;
      }
    }
    if(control_Stage == 200){ control_Stage = __cyclesPerDay-1; }

    // Get the time passed into the actual stage
    if(__cyclesPerDay-control_Stage==1 && actualHour-__beginHour[control_Stage]<0){
      actualHour += 24-__beginHour[control_Stage];
    }
    else{
      actualHour -= __beginHour[control_Stage];
    }

    // Get the percentage of the time that have already passed
    float percentageGap = float(actualHour)/__hoursPerCycle*100;
    float percentageSum = 0;
    if(!resp){
      for(int i=0; i<MAX_SOLUTIONS_NUMBER; i++){
        if(__order[i]==0){
          percentageSum += __percentage[i];
          __solution = i+1;
        }
      }
      if(percentageGap<=percentageSum){ resp = true; }
    }

    if(!resp){
      for(int i=0; i<MAX_SOLUTIONS_NUMBER; i++){
        if(__order[i]==1){
          percentageSum += __percentage[i];
          __solution = i+1;
        }
      }
      if(percentageGap<=percentageSum){ resp = true; }
    }

    if(!resp){
      for(int i=0; i<MAX_SOLUTIONS_NUMBER; i++){
        if(__order[i]==2){
          percentageSum += __percentage[i];
          __solution = i+1;
        }
      }
      if(percentageGap<=percentageSum){ resp = true; }
    }

    if(!resp){
      for(int i=0; i<MAX_SOLUTIONS_NUMBER; i++){
        if(__order[i]==3){ __solution = i+1; }
      }
    }

    return __solution;
  }
