// recirculationController.cpp
//
// Copyright (C) 2019 Grow

#include "recirculationController.h"

recirculationController::recirculationController() // Constructor
  { __InPump = LOW; __OutPump = LOW;
    __In = 0; __Out = 0; // Start with water
    __In1 = LOW; __In2 = LOW; __In3 = LOW;
    __In4 = LOW; __In5 = LOW;
    __Out1 = LOW; __Out2 = LOW; __Out3 = LOW;
    __Out4 = LOW; __Out5 = LOW;
    __In_LogInv = LOW;
    __Out_LogInv = LOW;
    __Switch_Pin = 0;
    __Switch_State = LOW;
    __Switch_Counter = 0;
    // Set null the UltrSonic pointers until begin function
    for(int i=0; i<MAX_NUMBER_US_SENSOR; i++){
      __Level[i] = NULL;
    }
    __ActualTime = millis();
  }

recirculationController::recirculationController(bool InInverted, bool OutInverted) // Constructor
  { __InPump = LOW; __OutPump = LOW;
    __In = 5; __Out = 5; // Start with water
    __In1 = LOW; __In2 = LOW; __In3 = LOW;
    __In4 = LOW; __In5 = LOW;
    __Out1 = LOW; __Out2 = LOW; __Out3 = LOW;
    __Out4 = LOW; __Out5 = LOW;
    __In_LogInv = InInverted;
    __Out_LogInv = OutInverted;
    __Switch_Pin = 0;
    __Switch_State = LOW;
    __Switch_Counter = 0;
    // Set null the UltrSonic pointers until begin function
    for(int i=0; i<MAX_NUMBER_US_SENSOR; i++){
      __Level[i] = NULL;
    }
    __ActualTime = millis();
  }

void recirculationController::turnOn(bool &state, bool inverted_logic = LOW)
  { if(inverted_logic){state = LOW;}
    else{state = HIGH;}
  }

void recirculationController::turnOff(bool &state, bool inverted_logic = LOW)
  { if(inverted_logic){state = HIGH;}
    else{state = LOW;}
  }

bool recirculationController::getState_In1()
  { return __In1; }

bool recirculationController::getState_In2()
  { return __In2; }

bool recirculationController::getState_In3()
  { return __In3; }

bool recirculationController::getState_In4()
  { return __In4; }

bool recirculationController::getState_In5()
  { return __In5; }

bool recirculationController::getState_Out1()
  { return __Out1; }

bool recirculationController::getState_Out2()
  { return __Out2; }

bool recirculationController::getState_Out3()
  { return __Out3; }

bool recirculationController::getState_Out4()
  { return __Out4; }

bool recirculationController::getState_Out5()
  { return __Out5; }

bool recirculationController::getState_InPump()
  { return __InPump; }

bool recirculationController::getState_OutPump()
  { return __OutPump; }

/* Correct equation for recirculation tank */
float recirculationController::getLiters()
  { float height = __Level[0]->getValue();
    float liters = (88-(height/10))*(WIDTH_TANK*LENGTH_TANK);
    return liters;
  }

float recirculationController::getLiters1()
  { float height = __Level[1]->getValue();
    float liters = (88-(height/10))*PI*(RADIO_TANK*RADIO_TANK);
    return liters;
  }

float recirculationController::getLiters2()
  { float height = __Level[2]->getValue();
    float liters = (88-(height/10))*PI*(RADIO_TANK*RADIO_TANK);
    return liters;
  }

float recirculationController::getLiters3()
  { float height = __Level[3]->getValue();
    float liters = (88-(height/10))*PI*(RADIO_TANK*RADIO_TANK);
    return liters;
  }

float recirculationController::getLiters4()
  { float height = __Level[4]->getValue();
    float liters = (88-(height/10))*PI*(RADIO_TANK*RADIO_TANK);
    return liters;
  }

float recirculationController::getLiters5()
  { float height = __Level[5]->getValue();
    float liters = (88-(height/10))*PI*(RADIO_TANK*RADIO_TANK);
    return liters;
  }



void recirculationController::turnOn_OutPump(byte valve)
  { switch(valve){
      case 0:
        turnOff(__OutPump, LOW);
        turnOff(__Out1, __Out_LogInv);
        turnOff(__Out2, __Out_LogInv);
        turnOff(__Out3, __Out_LogInv);
        turnOff(__Out4, __Out_LogInv);
        turnOff(__Out5, __Out_LogInv);
        break;
      case 1:
        turnOn(__Out1, __Out_LogInv);
        turnOn(__OutPump, LOW);
        break;
      case 2:
        turnOn(__Out2, __Out_LogInv);
        turnOn(__OutPump, LOW);
        break;
      case 3:
        turnOn(__Out3, __Out_LogInv);
        turnOn(__OutPump, LOW);
        break;
      case 4:
        turnOn(__Out4, __Out_LogInv);
        turnOn(__OutPump, LOW);
        break;
      case 5:
        turnOn(__Out5, __Out_LogInv);
        turnOn(__OutPump, LOW);
        break;
      default:
        turnOff(__OutPump, LOW);
        turnOff(__Out1, __Out_LogInv);
        turnOff(__Out2, __Out_LogInv);
        turnOff(__Out3, __Out_LogInv);
        turnOff(__Out4, __Out_LogInv);
        turnOff(__Out5, __Out_LogInv);
        break;
    }
  }

void recirculationController::turnOn_InPump(byte valve)
  { switch(valve){
      case 0:
        turnOff(__InPump, LOW);
        turnOff(__In1, __In_LogInv);
        turnOff(__In2, __In_LogInv);
        turnOff(__In3, __In_LogInv);
        turnOff(__In4, __In_LogInv);
        turnOff(__In5, __In_LogInv);
        break;
      case 1:
        turnOn(__In1, __In_LogInv);
        turnOn(__InPump, LOW);
        break;
      case 2:
        turnOn(__In2, __In_LogInv);
        turnOn(__InPump, LOW);
        break;
      case 3:
        turnOn(__In3, __In_LogInv);
        turnOn(__InPump, LOW);
        break;
      case 4:
        turnOn(__In4, __In_LogInv);
        turnOn(__InPump, LOW);
        break;
      case 5:
        turnOn(__In5, __In_LogInv);
        turnOn(__InPump, LOW);
        break;
      default:
        turnOff(__InPump, LOW);
        turnOff(__In1, __In_LogInv);
        turnOff(__In2, __In_LogInv);
        turnOff(__In3, __In_LogInv);
        turnOff(__In4, __In_LogInv);
        turnOff(__In5, __In_LogInv);
        break;
    }
  }

void recirculationController::read_LevelSwitch()
  { if(millis()-__ActualTime>100){
      __ActualTime = millis();
      bool val = digitalRead(__Switch_Pin);

      if(val != __Switch_State){
        if(__Switch_Counter >=10){
          __Switch_State = !__Switch_State;
          __Switch_Counter = 0;
        }
        __Switch_Counter++;
      }
      else{__Switch_Counter = 0;}
    }
  }

void recirculationController::begin(byte pin, UltraSonic &level0, UltraSonic &level1, UltraSonic &level2,
  UltraSonic &level3, UltraSonic &level4, UltraSonic &level5)
  { __Switch_Pin = pin;
    pinMode(__Switch_Pin,INPUT);
    __Level[0] = &level0;
    __Level[1] = &level1;
    __Level[2] = &level2;
    __Level[3] = &level3;
    __Level[4] = &level4;
    __Level[5] = &level5;
  }

void recirculationController::run()
  { read_LevelSwitch(); // Get the switch level state every 100ms

    // If level too high in recirculation tank
    if(__Level[0]->getState() == 2){
       if(__Level[__In]->getState() != 2){ // Check that in nutrient tank level is not high
         turnOn_InPump(__In); // Send recirculate to  tank
       }
       else{
         // Desechar recirculado
       }
    }

    //
  }
