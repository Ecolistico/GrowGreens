// processControl.cpp
//
// Copyright (C) 2019 Grow

#include "processControl.h"

/***   processControl   ***/

processControl::processControl()
  {
    state = 0;
    lastState = 0;
    actualTime = millis();
    parameter = 0;
  }

processControl::setState(uint8_t st, float par = 0)
  { lastState = state;
    state = st;
    parameter = par;
    actualTime = millis();
  }
