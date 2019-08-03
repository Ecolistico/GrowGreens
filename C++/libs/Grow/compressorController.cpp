// compressorController.cpp
//
// Copyright (C) 2019 Grow

#include "compressorController.h"

/***   compressorController   ***/
// Statics variables definitions
compressorController::compressorController( // Constructor
  bool nutLogic,
  bool tankLogic,
  bool h2oLogic )
  {  __State = LOW;
     __Vnut = LOW;
     __Vtank = LOW;
     __Vh20 = LOW;
     __Fnut = LOW;
     __Fh2o = LOW;
     __nutInvertedLogic = nutLogic;
     __tankInvertedLogic = tankLogic;
     __h20InvertedLogic = h2oLogic;
     __KeepConnected = false;
     __Mode = 0;
 }

void compressorController::turnOn()
  { __State = HIGH; }

void compressorController::turnOff()
  { __State = LOW; }

void compressorController::doNothing()
  { turnOff();
    if(__nutInvertedLogic){__Vnut = HIGH;}
    else{__Vnut = LOW;}
    if(__tankInvertedLogic){__Vtank = HIGH;}
    else{__Vtank = LOW;}
    if(__h20InvertedLogic){__Vh20 = HIGH;}
    else{__Vh20 = LOW;}
  }

void compressorController::turnOffAll()
  { turnOff();
    if(__KeepConnected){ __Vnut = HIGH; }
    else{ __Vnut = LOW; }
    __Vtank = LOW;
    __Vh20 = LOW;
  }

void compressorController::openTank()
  { turnOff();
    if(__KeepConnected){ __Vnut = HIGH; }
    else{ __Vnut = LOW; }
    __Vtank = HIGH;
    __Vh20 = LOW;
  }

void compressorController::openNut()
  { turnOff();
    __Vtank = HIGH;
    __Vnut = HIGH;
    __Vh20 = LOW;
  }

void compressorController::openH2O()
  { turnOff();
    if(__KeepConnected){ __Vnut = HIGH; }
    else{ __Vnut = LOW; }
    __Vtank = LOW;
    __Vh20 = HIGH;
  }

void compressorController::openAll()
  { turnOff();
    __Vtank = HIGH;
    __Vnut = HIGH;
    __Vh20 = HIGH;
  }

void compressorController::fillTank()
  { turnOn();
    if(__KeepConnected){ __Vnut = HIGH; }
    else{ __Vnut = LOW; }
    __Vtank = HIGH;
    __Vh20 = LOW;
  }

void compressorController::fillNut()
  { turnOn();
    __Vtank = HIGH;
    __Vnut = HIGH;
    __Vh20 = LOW;
  }

void compressorController::fillH2O()
  { turnOn();
    if(__KeepConnected){ __Vnut = HIGH; }
    else{ __Vnut = LOW; }
    __Vtank = LOW;
    __Vh20 = HIGH;
  }

void compressorController::fillAll()
  { turnOn();
    __Vtank = HIGH;
    __Vnut = HIGH;
    __Vh20 = HIGH;
  }

void compressorController::printAct(String act)
  { Serial.print(F("Compressor: "));
    Serial.println(act);
  }

bool compressorController::getState() // Constructor
  { return __State; }

bool compressorController::getValveTank()
  { if(__tankInvertedLogic){return !__Vtank;}
    else{return __Vtank;}
  }

bool compressorController::getValveNut()
  { if(__nutInvertedLogic){return !__Vnut;}
    else{return __Vnut;}
  }

bool compressorController::getValveH2O()
  { if(__h20InvertedLogic){return !__Vh20;}
    else{return __Vh20;}
  }

bool compressorController::getFreeValveNut()
  { return __Fnut; }

bool compressorController::getFValveH2O()
  { return __Fh2o; }

void compressorController::openFreeNut()
  { __Fnut = HIGH; }

void compressorController::closeFreeNut()
  { __Fnut = LOW; }

void compressorController::openFreeH2O()
  { __Fh2o = HIGH; }

void compressorController::closeFreeH2O()
  { __Fh2o = LOW; }

void compressorController::keepConnected(bool con)
  { __KeepConnected = con;
    if(__KeepConnected && !__Vnut){
      __Vnut = HIGH;
    }
  }

uint8_t compressorController::getMode()
  { return __Mode; }

bool compressorController::setMode(uint8_t mode)
  { if(__Mode<=9){
      __Mode = mode;
      switch(mode){
        case 0: // Do nothing
          doNothing();
          printAct(F("Do nothing"));
          break;
        case 1: // Turn off all
          turnOffAll();
          printAct(F("Turn Off All"));
          break;
        case 2: // Pressure of the tank = compressor
          openTank();
          printAct(F("Equalizing pressure with air tank"));
          break;
        case 3: // Pressure of the nutrition kegs = compressor
          openNut();
          printAct(F("Equalizing pressure with nutrition kegs"));
          break;
        case 4: // Pressure of the H2O kegs = compressor
          openH2O();
          printAct(F("Equalizing pressure with water kegs"));
          break;
        case 5: // Equal all pressures
          openAll();
          printAct(F("Equalizing all pressures"));
          break;
        case 6: // Fill the tank
          fillTank();
          printAct(F("Compressing air tank"));
          break;
        case 7: // Fill the nutrition kegs
          fillNut();
          printAct(F("Compressing nutrition kegs"));
          break;
        case 8: // Fill the H2O kegs
          fillH2O();
          printAct(F("Compressing water kegs"));
          break;
        case 9: // Fill everything
          fillAll();
          printAct(F("Compressing all"));
          break;
        default: // Do nothing
          doNothing();
          printAct(F("Mode did not match. Do Nothing Instead"));
          break;
        }
      return true;
    }
    return false;
  }
