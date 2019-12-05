// processControl.cpp
//
// Copyright (C) 2019 Grow

#include "processControl.h"

/***   processControl   ***/

processControl::processControl(uint8_t type=250)
  {
    state = 0;
    lastState = 0;
    actualTime = millis();
    parameter = 0;
    __Type = type;
  }

void processControl::printAction(String act)
  { Serial.print(F("debug,"));
    if(__Type==0){Serial.print(F("IPC"));}
    else if(__Type==1){Serial.print(F("MPC"));}
    else if(__Type==2){Serial.print(F("CC"));}
    else{Serial.print(F("Control (Undefined)"));}
    Serial.print(F(" : ")); 
    Serial.println(act);
  }

void processControl::setState(uint8_t st, float par = 0)
  { lastState = state;
    state = st;
    parameter = par;
    actualTime = millis();
    printAction(String(state));
  }
