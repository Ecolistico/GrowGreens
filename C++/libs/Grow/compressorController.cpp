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

void compressorController::fillTankAndH2O()
  { turnOn();
    if(__KeepConnected){ __Vnut = HIGH; }
    else{ __Vnut = LOW; }
    __Vtank = HIGH;
    __Vh20 = HIGH;
  }

void compressorController::fillAll()
  { turnOn();
    __Vtank = HIGH;
    __Vnut = HIGH;
    __Vh20 = HIGH;
  }

void compressorController::setMode(uint8_t mode)
  { __Mode = mode;
    switch(mode){
      case 0: // Do nothing
        doNothing();
        printAct(F("Do nothing"));
        break;
      case 1: // Turn off all
        turnOffAll();
        printAct(F("Turn Off All"));
        break;
      case 2: // Fill the tank
        fillTank();
        printAct(F("Compressing air tank"));
        break;
      case 3: // Fill the nutrition kegs
        fillNut();
        printAct(F("Compressing nutrition kegs"));
        break;
      case 4: // Fill the H2O kegs
        fillH2O();
        printAct(F("Compressing water kegs"));
        break;
      case 5: // Fill the air Tank and H2O Kegs
        fillTankAndH2O();
        printAct(F("Compressing air tank and water kegs"));
        break;
      case 6: // Fill everything
        fillAll();
        printAct(F("Compressing all"));
        break;
      default: // Do nothing
        doNothing();
        printAct(F("Mode did not match. Do Nothing Instead"));
        break;
    }
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

bool compressorController::getFreeValveH2O()
  { return __Fh2o; }

void compressorController::openFreeNut()
  { if(!__Vnut){
      if(!__Fnut){
        __Fnut = HIGH;
        printAct(F("Free Nutrition Valve open"));
      }
      else{ printAct(F("Free Nutrition Valve already open")); }
    }
    else{ printAct(F("Cannot Free Pressure in nutrition kegs because nutrition valve is open")); }
  }

void compressorController::closeFreeNut()
  { if(__Fnut){
      __Fnut = LOW;
      printAct(F("Free Nutrition Valve close"));
    }
    else{ printAct(F("Free Nutrition Valve already close")); }
  }

void compressorController::openFreeH2O()
  { if(!__Vh20){
      if(!__Fh2o){
        __Fh2o = HIGH;
        printAct(F("Free Water Valve open"));
      }
      else{ printAct(F("Free Water Valve already open")); }
    }
    else{ printAct(F("Cannot Free Pressure in water kegs because water valve is open")); }
  }

void compressorController::closeFreeH2O()
  { if(__Fh2o){
      __Fh2o = LOW;
      printAct(F("Free Water Valve close"));
    }
    else{ printAct(F("Free Water Valve already close")); }
  }

void compressorController::compressTank()
  { if(!__Vnut){
      if(__Mode==4){
        printAct(F("Already filling water kegs, opening valves to fill also air tank"));
        setMode(5);
      }
      else if(__Mode!=2 && __Mode!=3 && __Mode!=5 && __Mode!=6){
        printAct(F("Getting the correct pressure in water kegs"));
        setMode(2);
      }
      else{printAct(F("Already compressing air tank"));}
    }
    else{printAct(F("Cannot Compress air tank because nutrition valve is open"));}
  }

void compressorController::compressNut()
  { if(!__Fnut){
      if(__Mode==4 || __Mode==5){
        printAct(F("Already filling water kegs, opening valves to fill nutrition kegs"));
        setMode(6);
      }
      else if(__Mode!=3 && __Mode!=6){
        printAct(F("Getting the correct pressure in nutrition kegs"));
        setMode(3);
      }
      else{printAct(F("Already compressing nutrition kegs"));}
    }
    else{printAct(F("Cannot Compress nutrition kegs because Free Nutrition Valve is open"));}
  }

void compressorController::compressH2O()
  { if(!__Fh2o){
      if(__Mode==2){
        printAct(F("Already filling air tank, opening valves to fill also water kegs"));
        setMode(5);
      }
      else if(__Mode==3){
        printAct(F("Already filling nutrition kegs, opening valves to fill also water kegs"));
        setMode(6);
      }
      else if(__Mode!=4 && __Mode!=5 && __Mode!=6){
        printAct(F("Getting the correct pressure in water kegs"));
        setMode(4);
      }
      else{printAct(F("Already compressing water kegs"));}
    }
    else{printAct(F("Cannot Compress water kegs because Free Water Valve is open"));}
  }

void compressorController::Off()
  { setMode(1); }

void compressorController::keepConnected(bool con)
  { if(__KeepConnected!=con){
      __KeepConnected = con;
      if(__KeepConnected){
        printAct(F("Keeping connected nutrition kegs and air tank"));
        if(!__Vnut){ __Vnut = HIGH; }
      }
      else if(!__KeepConnected){
        printAct(F("Disconnecting nutrition kegs and air tank"));
        if(__Vnut && __Mode!=3 && __Mode!=6){
          __Vnut = LOW;
        }
      }
    }
  }
