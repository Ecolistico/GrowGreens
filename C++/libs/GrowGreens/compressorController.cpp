// compressorController.cpp
//
// Copyright (C) 2019 Grow

#include "compressorController.h"

/***   compressorController   ***/
compressorController::compressorController( // Constructor
  bool Vair_Logic,
  bool Vconnected_Logic,
  bool VFree_Logic )
  {  _VAir = LOW;
     _VConnected = LOW;
     _VFree = LOW;
     _VAir_InvertedLogic = Vair_Logic;
     _VConnected_InvertedLogic = Vconnected_Logic;
     _VFree_InvertedLogic = VFree_Logic;
     _KeepConnected = false;
     _Mode = 0;
 }

void compressorController::turnOffAll()
  { // If KeepConnected Active let Valve ON
    if(_KeepConnected) _VConnected = HIGH;
    else _VConnected = LOW;
    _VAir = LOW;
    _VFree = LOW;
  }

void compressorController::pressurize()
  { _VAir = HIGH;
    if(_KeepConnected) {
      _VConnected = HIGH;
      _VFree = LOW;
    }
    else _VConnected = LOW;
  }

void compressorController::despressurize()
  { _VFree = HIGH;
    if (_KeepConnected) {
      _VAir = LOW;
      _VConnected = HIGH;
    }
    else _VConnected = LOW;
  }

void compressorController::setMode(uint8_t mode)
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

void compressorController::printAct(String act, uint8_t level=0)
  { if(level==0){ Serial.print(F("debug,")); } // Debug
    else if(level==1){ Serial.print(F("info,")); } // Info
    else if(level==2){ Serial.print(F("warning,")); } // Warning
    else if(level==3){ Serial.print(F("error,")); } // Error
    else if(level==4){ Serial.print(F("critical,")); } // Error
    Serial.print(F("Compressor: "));
    Serial.println(act);
  }

bool compressorController::getVAir()
  { if(_VAir_InvertedLogic) return !_VAir;
    else return _VAir;
  }

bool compressorController::getVconnected()
  { if(_VConnected_InvertedLogic) return !_VConnected;
    else return _VConnected;
  }

bool compressorController::getVfree()
  { if(_VFree_InvertedLogic) return !_VFree;
    else return _VFree;
  }

void compressorController::Off(bool connected)
  { if(_Mode!=connected) setMode(connected);
    else printAct(F("Compressor Controller: It is already running function Off()"), 2);
  }

void compressorController::pressurizeAll()
  { if(_Mode!=2) setMode(2);
    else printAct(F("Compressor Controller: It is already running function pressurizeAll()"), 2);
  }

void compressorController::pressurizeAir()
  { if(_Mode!=3) setMode(3);
    else printAct(F("Compressor Controller: It is already running function pressurizeAir()"), 2);
  }

void compressorController::pressurize_depressurize()
  { if(_Mode!=4) setMode(4);
    else printAct(F("Compressor Controller: It is already running function pressurize_depressurize()"), 2);
  }

void compressorController::depressurizeWater()
  { if(_Mode!=5) setMode(5);
    else printAct(F("Compressor Controller: It is already running function depressurizeWater()"), 2);
  }

void compressorController::depressurizeAll()
  { if(_Mode!=+6) setMode(6);
    else printAct(F("Compressor Controller: It is already running function depressurizeAll()"), 2);
  }

void compressorController::keepConnected(bool con)
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
