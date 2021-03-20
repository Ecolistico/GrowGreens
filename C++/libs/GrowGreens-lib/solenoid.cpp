// solenoid.cpp
//
// Copyright (C) 2019 Grow

#include "solenoid.h"

/***   solenoid   ***/
// Statics variables definitions
uint8_t solenoid::_numberSolenoid = 0;

solenoid::solenoid(uint8_t num, uint8_t floor, bool reg, unsigned long timeOn) // Constructor
   { _State = LOW; // Off
     _Enable = false; // Enable
     _Floor = floor;
     _Region = reg;
     _Number = num;
     _TimeOn = timeOn;
     setTime();
     _H2OVolume = 0;
     _numberSolenoid++;
   }

void solenoid::setTime()
  { if(_State == HIGH) resetTime();
    else _ActualTime = millis() - _TimeOn;
  }

void solenoid::resetTime()
  {  _ActualTime = millis(); }

void solenoid::changeOrder(uint8_t new_number)
  { _Number = new_number; }

void solenoid::addConsumptionH2O(float newVolume)
  { if(newVolume>0) _H2OVolume += newVolume; }

void solenoid::solenoidPrint(String act, uint8_t level=0)
  { if(level==0){ Serial.print(F("debug,")); } // Debug
    else if(level==1){ Serial.print(F("info,")); } // Info
    else if(level==2){ Serial.print(F("warning,")); } // Warning
    else if(level==3){ Serial.print(F("error,")); } // Error
    else if(level==4){ Serial.print(F("critical,")); } // Error
    Serial.print(F("Solenoid Valve "));
    Serial.print(_Floor+1);
    if (_Region == 0) Serial.print(F("A"));
    else Serial.print(F("B"));
    Serial.print(_Number+1);
    Serial.print(F(": "));
    Serial.println(act);
  }

void solenoid::solenoidPrint(String act1, String act2, String act3, uint8_t level=0)
  { if(level==0){ Serial.print(F("debug,")); } // Debug
    else if(level==1){ Serial.print(F("info,")); } // Info
    else if(level==2){ Serial.print(F("warning,")); } // Warning
    else if(level==3){ Serial.print(F("error,")); } // Error
    else if(level==4){ Serial.print(F("critical,")); } // Error
    Serial.print(F("Solenoid Valve "));
    Serial.print(_Floor+1);
    if (_Region == 0) Serial.print(F("A"));
    else Serial.print(F("B"));
    Serial.print(_Number+1);
    Serial.print(F(": "));
    Serial.print(act1);
    Serial.print(act2);
    Serial.println(act3);
  }

unsigned long solenoid::getTime()
  { return (millis() - _ActualTime); }

void solenoid::setTimeOn(unsigned long t_on)
  { float seconds = float(t_on)/1000;
    _TimeOn = t_on;
    solenoidPrint(F("Time On changed to "), String(seconds), F(" seconds"), 0);
  }

unsigned long solenoid::getTimeOn()
  { return _TimeOn; }

bool solenoid::getState()
  {  return (_State); }

uint8_t solenoid::getNumber()
  {  return _Number; }

void solenoid::turnOn(bool rst_time)
  { _State = HIGH;
    solenoidPrint(F("Turn On"), 0);
    if(rst_time){setTime(); }
  }

void solenoid::turnOff(bool rst_time)
  { _State = LOW;
    solenoidPrint(F("Turn Off"), 0);
    if(rst_time){setTime(); }
  }

void solenoid::enable(bool en)
  { if(_Enable!=en){
      _Enable = en;
      if(_Enable){ solenoidPrint(F("Enabled"), 0); }
      else{ solenoidPrint(F("Disabled"), 0); }
    }
  }

bool solenoid::isEnable()
  {  return (_Enable); }

void solenoid::restartH2O(bool print)
  { if(print){ solenoidPrint(F("Water Volume Restarted"), 0); }
    _H2OVolume = 0;
  }

float solenoid::getH2O()
  { return _H2OVolume; }

void solenoid::printH2O()
  { solenoidPrint(F("Water Volume = "), String(_H2OVolume), F(" liters"), 0); }

/***   floorValves   ***/
// Statics variables definitions
uint8_t floorValves::_floorNumber = 0;

floorValves::floorValves(uint8_t floor, uint8_t valvesPerRegion) // Constructor
   { if(valvesPerRegion<=MAX_VALVES_PER_REGION){

       _valvesPerRegion = valvesPerRegion;
       _Floor = floor;
       _H2O_regA = 0;
       _H2O_regB = 0;

       for (int i = 0; i<valvesPerRegion; i++){
         _regA[i] = new solenoid(solenoid::_numberSolenoid, floor, 0, DEFAULT_TIME_ON);
         _regB[i] = new solenoid(solenoid::_numberSolenoid, floor, 1, DEFAULT_TIME_ON);
       }
       _floorNumber++;
     }
     else Serial.println(F("critical, Floor Valves ERROR: Solenoid exceed max number"));

   }

void floorValves::regionPrint(String act1, String act2, String act3, bool reg, uint8_t level=0)
  { if(level==0){ Serial.print(F("debug,")); } // Debug
    else if(level==1){ Serial.print(F("info,")); } // Info
    else if(level==2){ Serial.print(F("warning,")); } // Warning
    else if(level==3){ Serial.print(F("error,")); } // Error
    else if(level==4){ Serial.print(F("critical,")); } // Error
    Serial.print(F("Solenoid Region "));
    Serial.print(_Floor+1);
    if (reg == 0) Serial.print(F("A"));
    else Serial.print(F("B"));
    Serial.print(F(": "));
    Serial.print(act1);
    Serial.print(act2);
    Serial.println(act3);
  }

void floorValves::floorPrint(String act1, String act2, String act3, uint8_t level=0)
  { if(level==0){ Serial.print(F("debug,")); } // Debug
    else if(level==1){ Serial.print(F("info,")); } // Info
    else if(level==2){ Serial.print(F("warning,")); } // Warning
    else if(level==3){ Serial.print(F("error,")); } // Error
    else if(level==4){ Serial.print(F("critical,")); } // Error
    Serial.print(F("Solenoid Floor "));
    Serial.print(_Floor+1);
    Serial.print(F(": "));
    Serial.print(act1);
    Serial.print(act2);
    Serial.println(act3);
  }

void floorValves::updateH2O()
  { restartH2O_floor(false);
    for(int i = 0; i<_valvesPerRegion*2; i++){
      if(i<_valvesPerRegion) _H2O_regA += _regA[i]->getH2O();
      else _H2O_regB += _regB[i-_valvesPerRegion]->getH2O();
    }
  }

void floorValves::restartH2O(bool print)
  { for(int i = 0; i<_valvesPerRegion*2; i++) {
      if(i<_valvesPerRegion) _regA[i]->restartH2O(print);
      else _regB[i-_valvesPerRegion]->restartH2O(print);
    }
  }

void floorValves::printH2O()
  { for(int i = 0; i<_valvesPerRegion*2; i++) {
      if(i<_valvesPerRegion) _regA[i]->printH2O();
      else _regB[i-_valvesPerRegion]->printH2O();
    }
  }

void floorValves::restartH2O_floor(bool print)
  { if(print){
      regionPrint(F("Water Volume Restarted"), F(""), F(""), 0);
      regionPrint(F("Water Volume Restarted"), F(""), F(""), 1);
    }
    _H2O_regA = 0;
    _H2O_regB = 0;
  }

void floorValves::printH2O_floor()
  { regionPrint(F("Water Consumption - "), String(_H2O_regA), F(" liters"), 0);
    regionPrint(F("Water Consumption - "), String(_H2O_regB), F(" liters"), 1);
    floorPrint(F("Water Consumption - "), String(_H2O_regA+_H2O_regB), F(" liters"));
  }

  /***   systemValves   ***/
  systemValves::systemValves(uint8_t floorNum, uint8_t valvesPerRegion, uint8_t cycleTime = DEFAULT_CYCLE_TIME) // Constructor
     { if(floorNum<=MAX_FLOOR_NUMBER && valvesPerRegion<=MAX_VALVES_PER_REGION){
         _Enable = false;
         _CycleTime = cycleTime*60UL*1000UL;
         _floorNumber = floorNum;
         _valvesPerRegion = valvesPerRegion;
         _actualNumber = 0;
         _H2O_Consumption = 0;
         _H2O_Waste = 0;
         resetTime();
         for (int i = 0; i<floorNum; i++) _floor[i] = new floorValves(i, valvesPerRegion);
       }
       else Serial.println(F("critical,Systema Valves ERROR: Floors exceed max number"));
     }

 void systemValves::systemPrint(String act1, String act2, String act3, uint8_t level=0)
   { if(level==0){ Serial.print(F("debug,")); } // Debug
     else if(level==1){ Serial.print(F("info,")); } // Info
     else if(level==2){ Serial.print(F("warning,")); } // Warning
     else if(level==3){ Serial.print(F("error,")); } // Error
     else if(level==4){ Serial.print(F("critical,")); } // Error
     Serial.print(F("Solenoid System"));
     Serial.print(F(": "));
     Serial.print(act1);
     Serial.print(act2);
     Serial.println(act3);
   }

void systemValves::updateH2O()
   { restartH2O_system(false);
     for(int i = 0; i<_floorNumber; i++) {
       _floor[i]->updateH2O();
       _H2O_Consumption += (_floor[i]->_H2O_regA + _floor[i]->_H2O_regB);
     }
   }

 void systemValves::restartH2O(bool print)
   { for(int i = 0; i<_floorNumber; i++) _floor[i]->restartH2O(print); }

 void systemValves::printH2O()
   { for(int i = 0; i<_floorNumber; i++) {
       _floor[i]->printH2O();
     }
   }

 void systemValves::restartH2O_system(bool print)
   { if(print) systemPrint(F("Water Volume Restarted"), F(""), F(""));
     _H2O_Consumption = 0;
     _H2O_Waste = 0;
   }

 void systemValves::printH2O_system()
   { systemPrint(F("Water Consumption - "), String(_H2O_Consumption), F(" liters"), 0);
     systemPrint(F("Water Waste - "), String(_H2O_Waste), F(" liters"), 1);
   }

 void systemValves::addWasteH2O(float newVolume)
   { if(newVolume>0) _H2O_Waste += newVolume; }

unsigned long systemValves::getActionTime()
  { unsigned long totalTime = 0;
    for (int i = 0; i<_floorNumber; i++){
      for (int j = 0; j<_valvesPerRegion; j++){
        totalTime += _floor[i]->_regA[j]->getTimeOn();
        totalTime += _floor[i]->_regB[j]->getTimeOn();
      }
    }
    return totalTime;
  }

unsigned long systemValves::getCycleTime()
  { return _CycleTime;}

bool systemValves::setCycleTime(unsigned long t_cycle)
  { unsigned long mSec = t_cycle*60UL*1000UL;
    if ((mSec*3UL/4UL)>getActionTime()){
      _CycleTime = mSec;
      return true;
    }
    return false;
  }

void systemValves::resetTime()
  {  _ActualTime = millis(); }

void systemValves::getTime()
  { return (millis() - _ActualTime); }

uint8_t systemValves::getActualNumber()
 { return _actualNumber; }

void systemValves::enable(bool en)
  { if(_Enable!=en){
     _Enable = en;
     if(_Enable){ systemPrint(F("Enabled"), F(""), F(""), 0); }
     else{ systemPrint(F("Disabled"), F(""), F(""), 0); }
   }
  }

bool systemValves::isEnable()
  {  return (_Enable); }

bool systemValves::run()
  { if(_Enable==true){ // If system is enable
      // Look for solenoid that is next
      bool finished = false;
      uint8_t fl = 0;
      bol reg = 0;
      uint8_t num = 0;

      for(int i = 0; i<_floorNumber; i++){
        if(finished) break;
        for(int j = 0; j<_valvesPerRegion; j++){
          if(finished) break;
          if(_actualNumber == _floor[i]->_regA[j]->getNumber()){
            fl = i;
            reg = 0;
            num = j;
            finished = true;
          }
          else if(_actualNumber == _floor[i]->_regB[j]->getNumber()){
            fl = i;
            reg = 1;
            num = j;
            finished = true;
          }
        }
      }

      // Check conditions
      if(finished){
        if(reg==0){
          if(_floor[fl]->_regA[num]->isEnable()) { // Solenoid Enable
            if(_floor[fl]->_regA[num]->getState()==LOW){ // Solenoid Off
              if(_actualNumber==0){ // First Solenoid
                // getWasteWater(); /* Requires flowMeter*/
                // restartAllH2O(); /* check if we want to print before*/
              }
              _floor[fl]->_regA[num]->turnOn(true);
            }
            else if(_floor[fl]->_regA[num]->getState()==HIGH && _floor[fl]->_regA[num]->getTime()>=_floor[fl]->_regA[num]->getTimeOn()) {
              _floor[fl]->_regA[num]->turnOff(false);
              //getConsumptionH2O(); /* Requires flowMeter*/
              _actualNumber++;
            }
          }
          else{ /* Check what happen when solenoid is disable */
            if(_floor[fl]->_regA[num]->getState()) _floor[fl]->_regA[num]->turnOff(false); // If solenoid in action turnOff
            if(){
              setTime()
            }

            // Else restartCycle
          }
        }
        else{
          if(_floor[fl]->_regB[num]->isEnable()) { // Solenoid Enable
            if(_floor[fl]->_regB[num]->getState()==LOW){ // Solenoid Off
              if(_actualNumber==0){ // First Solenoid
                // getWasteWater(); /* Requires flowMeter*/
                // restartAllH2O(); /* check if we want to print before*/
              }
              _floor[fl]->_regB[num]->turnOn(true);
            }
            else if(_floor[fl]->_regB[num]->getState()==HIGH && _floor[fl]->_regB[num]->getTime()>=_floor[fl]->_regB[num]->getTimeOn()) {
              _floor[fl]->_regB[num]->turnOff(false);
              //getConsumptionH2O(); /* Requires flowMeter*/
              _actualNumber++;
            }
          }
          else{ /* Check what happen when solenoid is disable */
            if(_floor[fl]->_regB[num]->getState()) _floor[fl]->_regB[num]->turnOff(false); // If solenoid in action turnOff
            if(){
              setTime()
            }

            // Else restartCycle
          }
        }
      }
      else if(_actualNumber>=solenoid::_numberSolenoid && getTime()>=_CycleTime){
        _actualNumber = 0;
        systemPrint(F("Restarting cycle"), F(""), F(""), 0);
      }
    }
    else return false; // If system disable
  }
