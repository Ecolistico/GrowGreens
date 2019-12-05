// irrigationController.cpp
//
// Copyright (C) 2019 Grow

#include "irrigationController.h"

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
      __ec[i] = 1200;
      __ph[i] = 6.5;
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

      for(int i=0; i<MAX_SOLUTIONS_NUMBER; i++){
        __ec[i] = 1200;
        __ph[i] = 6.5;
      }
    }

    else{ // If parameters are wrong set default cycle.
        __cyclesPerDay = 1;
        __hoursPerCycle = 24;
        __beginHour[0] = 0;

        for(int i=0; i<MAX_SOLUTIONS_NUMBER; i++){
          __order[i] = i;
          __percentage[i] = 25;
          __ec[i] = 1200;
          __ph[i] = 6.5;
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
       if(24%cyclesPerDay != 0){ Serial.println(F("error,Parameter cyclesPerDay incorrect")); }
       // percentages not sum 100
       if(per1+per2+per3+per4!=100){ Serial.println(F("error,Percentages sum has to be equal to 100")); }
       // order parameters wrong
       if(ord1+ord2+ord3+ord4!=6 || ord1==ord2 || ord1==ord3 || ord1==ord4 ||
          ord2==ord3 || ord2==ord4 || ord3==ord4){ Serial.println(F("error,Order parameters incorrect")); }
    }
  }

void irrigationController::setEC(uint8_t sol, uint16_t ec)
  { if(sol>=0 && sol<MAX_SOLUTIONS_NUMBER){
      if(ec>0 && ec<5000){
        __ec[sol] = ec;
      }
      else{ Serial.println(F("error,set EC failed: ec out of range [0-5000]")); }
    }
    else{ Serial.println(F("error,set EC failed: solution out of range [0-3]")); }
  }

void irrigationController::setEC(uint16_t ec1, uint16_t ec2, uint16_t ec3, uint16_t ec4)
  { setEC(0, ec1);
    setEC(1, ec2);
    setEC(2, ec3);
    setEC(3, ec4);
  }

void irrigationController::setPH(uint8_t sol, float ph)
  { if(sol>=0 && sol<MAX_SOLUTIONS_NUMBER){
      if(ph>0 && ph<14){
        __ph[sol] = ph;
      }
      else{ Serial.println(F("error,set PH failed: ph out of range [0-5000]")); }
    }
    else{ Serial.println(F("error,set PH failed: solution out of range [0-3]")); }

  }

void irrigationController::setPH(float ph1, float ph2, float ph3, float ph4)
  { setPH(0, ph1);
    setPH(1, ph2);
    setPH(2, ph3);
    setPH(3, ph4);
  }

uint16_t irrigationController::getEC(uint8_t sol)
  { if(sol>=0 && sol<MAX_SOLUTIONS_NUMBER){ return __ec[sol]; }
    else { return 0; }
  }

float irrigationController::getPH(uint8_t sol)
  { if(sol>=0 && sol<MAX_SOLUTIONS_NUMBER){ return __ph[sol]; }
    else { return 0; }
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
