// irrigationController.cpp
//
// Copyright (C) 2019 Grow

#include "irrigationController.h"

/***   irrigationController   ***/
// Constructor
irrigationController::irrigationController(logicConfig lconfig)
  {  _Pump = LOW;
     _VAir = LOW;
     _VConnected = LOW;
     _VFree = LOW;
     _VAir_InvertedLogic = lconfig.Vair_Logic;
     _VConnected_InvertedLogic = lconfig.Vconnected_Logic;
     _VFree_InvertedLogic = lconfig.VFree_Logic;
     _KeepConnected = false;
     _Mode = 0;
     _Enable = true;
 }

void irrigationController::turnOffAll()
  { // If KeepConnected Active let Valve ON
    if(_KeepConnected) _VConnected = HIGH;
    else _VConnected = LOW;
    _VAir = LOW;
    _VFree = LOW;
  }

void irrigationController::pressurize()
  { _VAir = HIGH;
    if(_KeepConnected) {
      _VConnected = HIGH;
      _VFree = LOW;
    }
    else _VConnected = LOW;
  }

void irrigationController::despressurize()
  { _VFree = HIGH;
    if (_KeepConnected) {
      _VAir = LOW;
      _VConnected = HIGH;
    }
    else _VConnected = LOW;
  }

void irrigationController::setMode(uint8_t mode)
  { _Mode = mode;
    switch(mode){
      case 0: // Turn off all notConnected
        _KeepConnected = false;
        turnOffAll();
        printAct(F("Compressor Controller: Turn Off All (disconnected)"), 0);
        break;
      case 1: // Turn off all connected
        _KeepConnected = true;
        turnOffAll();
        printAct(F("Compressor Controller: Turn Off All (connected)"), 0);
        break;
      case 2: // Pressurize all
        _KeepConnected = true;
        pressurize();
        printAct(F("Compressor Controller: Pressurize all"), 1);
        break;
      case 3: // Pressurize air tank
        _KeepConnected = false;
        _VFree = LOW;
        pressurize();
        printAct(F("Compressor Controller: Pressurize air tank"), 1);
        break;
      case 4: // Pressurize air tank and Despressurize water tank
        _KeepConnected = false;
        pressurize();
        despressurize();
        printAct(F("Compressor Controller: Pressurize air tank and Despressurize water tank"), 1);
        break;
      case 5: // Despressurize water tank
        _KeepConnected = false;
        _VAir = LOW;
        despressurize();
        printAct(F("Compressor Controller: Despressurize water tank"), 1);
        break;
      case 6: // Despressurize all
        _KeepConnected = true;
        despressurize();
        printAct(F("Compressor Controller: Despressurize all"), 1);
        break;
      default: // Do nothing
        printAct(F("Compressor Controller: Mode did not match. Do Nothing Instead"), 2);
        break;
    }
  }

void irrigationController::printAct(String act, uint8_t level /*= 0 */)
  { if(level==0){ Serial.print(F("debug,")); } // Debug
    else if(level==1){ Serial.print(F("info,")); } // Info
    else if(level==2){ Serial.print(F("warning,")); } // Warning
    else if(level==3){ Serial.print(F("error,")); } // Error
    else if(level==4){ Serial.print(F("critical,")); } // Error
    Serial.print(F("(Irrigation) "));
    Serial.println(act);
  }

bool irrigationController::getPump()
  { if(_Enable) return _Pump;
    else return false;
  }

bool irrigationController::getVAir()
  { bool state;
    if(_Enable) state = _VAir;
    else state = false;
    if(_VAir_InvertedLogic) return !state;
    else return state;
  }

bool irrigationController::getVconnected()
  { bool state;
    if(_Enable) state = _VConnected;
    else state = false;
    if(_VConnected_InvertedLogic) return !state;
    else return state;
  }

bool irrigationController::getVfree()
  { bool state;
    if(_Enable) state = _VFree;
    else state = false;
    if(_VFree_InvertedLogic) return !state;
    else return state;
  }

void irrigationController::Off(bool connected)
  { if(_Mode!=connected) setMode(connected);
    else printAct(F("Compressor Controller: It is already running function Off()"), 2);
  }

void irrigationController::pressurizeAll()
  { if(_Mode!=2) setMode(2);
    else printAct(F("Compressor Controller: It is already running function pressurizeAll()"), 2);
  }

void irrigationController::pressurizeAir()
  { if(_Mode!=3) setMode(3);
    else printAct(F("Compressor Controller: It is already running function pressurizeAir()"), 2);
  }

void irrigationController::pressurize_depressurize()
  { if(_Mode!=4) setMode(4);
    else printAct(F("Compressor Controller: It is already running function pressurize_depressurize()"), 2);
  }

void irrigationController::depressurizeWater()
  { if(_Mode!=5) setMode(5);
    else printAct(F("Compressor Controller: It is already running function depressurizeWater()"), 2);
  }

void irrigationController::depressurizeAll()
  { if(_Mode!=+6) setMode(6);
    else printAct(F("Compressor Controller: It is already running function depressurizeAll()"), 2);
  }

void irrigationController::keepConnected(bool con)
  { if(_KeepConnected!=con){
      _KeepConnected = con;
      if(_KeepConnected){
        printAct(F("Connecting air and water tank"), 0);
        if(_Mode == 0) setMode(1);
        else if (_Mode == 3) setMode(2);
        else if (_Mode == 5) setMode(6);
      }
      else if(!_KeepConnected){
        printAct(F("Disconnecting air and water tank"), 0);
        if(_Mode == 1) setMode(0);
        else if (_Mode == 2) setMode(3);
        else if (_Mode == 6) setMode(5);
      }
    }
  }

void irrigationController::turnOnPump()
  { _Pump = HIGH;
    printAct(F("Recirculation Controller: Pump was turn on"), 1);
  }

void irrigationController::turnOffPump()
  { _Pump = LOW;
    printAct(F("Recirculation Controller: Pump was turn off"), 1);
  }

void irrigationController::enable(bool en)
  { if(_Enable!=en){
      _Enable = en;
      if(_Enable) printAct(F("Irrigation Controller: Enabled"), 1);
      else printAct(F("Irrigation Controller: Disabled"), 1);
    }
  }

void irrigationController::update()
  { _PumpL = getPump();
    _VAirL = getVAir();
    _VConnectedL = getVconnected();
    _VFreeL = getVfree();
  }
