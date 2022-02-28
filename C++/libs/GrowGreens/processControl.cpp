// processControl.cpp
//
// Copyright (C) 2019 Grow

#include "processControl.h"

/***   processControl   ***/

processControl::processControl(uint8_t type /* = 250*/ )
  {
    _state = 0;
    _lastState = 0;
    _actualTime = millis();
    _parameter = 0;
    _type = type;
  }

void processControl::printAction(String act)
  { Serial.print(F("debug,"));
    Serial.print(F("Control Process ("));
    Serial.print(_type);
    Serial.print(F("): "));
    Serial.println(act);
  }

void processControl::setState(uint8_t st, float par /* = 0*/ )
  { _lastState = _state;
    _state = st;
    _parameter = par;
    _actualTime = millis();
    printAction(String(_state));
  }
