// processControl.cpp
//
// Copyright (C) 2019 Grow

#include "processControl.h"

/***   processControl   ***/

processControl::processControl(String name)
  {
    state = 0;
    lastState = 0;
    actualTime = millis();
    parameter = 0;
    __Name = name;
  }

void processControl::printAction(String act)
  {
    Serial.print(__Name); Serial.print(" : "); Serial.println(act);
  }

void processControl::setState(uint8_t st, float par = 0)
  { lastState = state;
    state = st;
    parameter = par;
    actualTime = millis();
    printAction(String(state));
  }
