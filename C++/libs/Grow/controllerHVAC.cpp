// controllerHVAC.cpp
//
// Copyright (C) 2019 Grow

#include "controllerHVAC.h"

/***   controllerHVAC   ***/
// Statics variables definitions

void controllerHVAC::resetTime()
 {  __Actual_time = millis() ; }

controllerHVAC::controllerHVAC( ) // Constructor
  { __W = LOW;
    __Y = LOW;
    __G = LOW;
    __O = LOW;
    __B = LOW;
    __Warn = LOW;
    __Work = LOW;
    __Fan = LOW;
    __Mode = 0;
    __PrevMode = 0;
    __Actual_time = millis();
  }

controllerHVAC::controllerHVAC( byte Mode ) // Constructor
  { __W = LOW;
    __Y = LOW;
    __G = LOW;
    __O = LOW;
    __B = LOW;
    __Warn = LOW;
    __Work = LOW;
    __Fan = LOW;
    __Mode = Mode;
    __PrevMode = 0;
    __Actual_time = millis();
  }

controllerHVAC::controllerHVAC( byte Mode, bool fan ) // Constructor
  { __W = LOW;
    __Y = LOW;
    __G = LOW;
    __O = LOW;
    __B = LOW;
    __Warn = LOW;
    __Work = LOW;
    __Fan = fan;
    __Mode = Mode;
    __PrevMode = 0;
    __Actual_time = millis();
  }

bool controllerHVAC::getHR_State()
  { return __W; }

bool controllerHVAC::getCC_State()
  { return __Y; }

bool controllerHVAC::getFR_State()
  { return __G; }

bool controllerHVAC::getCVC_State()
  { return __O; }

bool controllerHVAC::getCVH_State()
  { return __B; }

bool controllerHVAC::getWarn_State()
  { return __Warn; }

bool controllerHVAC::getWork_State()
  { return __Work; }

bool controllerHVAC::changeMode(byte Mode)
  { if(Mode>=0 && Mode<3){
      __PrevMode = __Mode;
      __Mode = Mode;
      switch(__Mode){
        case OFF_MODE:
          Serial.println(F("HVAC Controller: Changing Mode - OFF"));
          break;
        case COOL_MODE:
          Serial.println(F("HVAC Controller: Changing Mode - COOL"));
          break;
        case HEAT_MODE:
          Serial.println(F("HVAC Controller: Changing Mode - HEAT"));
          break;
      }
      return true;
    }
    return false;
  }

void controllerHVAC::changeFan(bool fan)
  { __Fan = fan;
    switch(__Fan){
      case AUTO_FAN:
        Serial.println(F("HVAC Controller: Fan Mode - AUTO"));
        break;
      case ON_FAN:
        Serial.println(F("HVAC Controller: Fan Mode - ON"));
        break;
    }
  }

void controllerHVAC::run()
  {
    // Mode 0: Turn Off
    if(__Mode == 0){
      // All off
      __Work = LOW;
      __Warn = LOW;
      __W = LOW; // Heat Relay Off
      __Y = LOW; // Compressor Contactor Off
      __O = LOW; // Changeover Valve Cooling Off
      __B = LOW; // Changeover Valve Heating Off
    }

    // Mode 1: Cooling
    else if(__Mode == 1){
      __O = HIGH; // Changeover Valve Cooling On
      __B = LOW;  // Changeover Valve Heating Off
      __W = LOW;  // Heat Relay Off

      if(__PrevMode != __Mode){
        resetTime();
        __PrevMode = __Mode;
      }
      if(millis()-__Actual_time<=WAIT_TIME){
        __Work = LOW;
        __Warn = HIGH;
        __Y = LOW; // Compressor Contactor  Off
      }
      else if(millis()-__Actual_time>WAIT_TIME){
        __Work = HIGH;
        __Warn = LOW;
        __Y = HIGH; // Compressor Contactor Cooling On
      }
    }

    // Mode 2: Heating
    else if(__Mode == 2){
      __B = HIGH;  // Changeover Valve Heating On
      __O = LOW; // Changeover Valve Cooling Off
      __Y = LOW;  // Compressor Contactor Off

      if(__PrevMode != __Mode){
        resetTime();
        __PrevMode = __Mode;
      }
      if(millis()-__Actual_time<=WAIT_TIME){
        __Work = LOW;
        __Warn = HIGH;
        __W = LOW; // Heat Relay  Off
      }
      else if(millis()-__Actual_time>WAIT_TIME){
        __Work = HIGH;
        __Warn = LOW;
        __W = HIGH; // Heat Relay Cooling On
      }
    }

    // Fan Relay Condition. Always ON or just ON when W o Y ON
    if(__Fan){
      __G = HIGH; // Fan Relay On
    }
    else{
      if(__W == HIGH || __Y == HIGH){ __G = HIGH; } // Fan Relay On
      else{ __G = LOW; } // Fan Relay Off
    }

  }
