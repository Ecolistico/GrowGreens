// recirculationController.cpp
//
// Copyright (C) 2019 Grow

#include "recirculationController.h"
// Statics variables definitions
volatile long recirculationController::__NumPulses = 0;

recirculationController::recirculationController() // Constructor
  { /*** Default States ***/
    __InPump = LOW; __OutPump = LOW; __SolPump = LOW;
    for(int i=0; i<MAX_RECIRCULATION_TANK; i++){
      __InValve[i] = LOW;
      __OutValve[i] = LOW;
    }
    __ReleaseValve = LOW;
    for(int i=0; i<MAX_RECIRCULATION_DESTINATIONS; i++){
      __Go[i] = LOW;
    }
    __Fh2o = LOW; __FSol = LOW;
    __Rh2o = LOW; __RSol = LOW;

    /*** Default Aux Variables ***/
    __In = 0; __Out = 0;
    __VolKnut = 0;
    __VolKh2o = 0;
    __OutLiters = 0;
    __ActualLiters = 0;
    __FillLiters = 0;
    // Flow meter
    __K = 6.781; // flowSensor constant
    __H2OVol = 0;

    // Set null the UltrSonic pointers until begin function
    for(int i=0; i<MAX_NUMBER_US_SENSOR; i++){
      __Level[i] = NULL;
    }
  }

void recirculationController::countPulses()
  {  __NumPulses++; }

void recirculationController::getVolume()
  { if(__ActualTime-millis()>1000){
      __ActualTime = millis();
      noInterrupts(); // Disable Interrupts
      float newVolume = __NumPulses/(60*__K);
      __H2OVol += newVolume;
      __NumPulses = 0;
      interrupts(); // Enable Interrupts
    }
  }

void recirculationController::printAction(float volume, String from, String to)
  { Serial.print(F("Recirculation: Moving "));
    Serial.print(volume);
    Serial.print(F("liters from"));
    Serial.print(from);
    Serial.print(F(" to "));
    Serial.println(to);
  }

  void recirculationController::printAction(String act)
    { Serial.print(F("Recirculation: "));
      Serial.println(act);
    }

void recirculationController::begin(
  UltraSonic &level0,
  UltraSonic &level1,
  UltraSonic &level2,
  UltraSonic &level3,
  UltraSonic &level4,
  UltraSonic &level5,
  UltraSonic &level6
  ){
    __Level[0] = &level0;
    __Level[1] = &level1;
    __Level[2] = &level2;
    __Level[3] = &level3;
    __Level[4] = &level4;
    __Level[5] = &level5;
    __Level[6] = &level6;
    __ActualTime = millis();
  }

void recirculationController::flowSensorBegin()
  { pinMode(flowSensor1,INPUT);
    attachInterrupt(digitalPinToInterrupt(flowSensor1), countPulses, RISING);
  }

bool recirculationController::getInPump()
  { return __InPump; }

bool recirculationController::getOutPump()
  { return __OutPump; }

bool recirculationController::getSolPump()
  { return __SolPump; }

bool recirculationController::getInValve(uint8_t valve)
  { return __InValve[valve]; }

bool recirculationController::getOutValve(uint8_t valve)
  { return __OutValve[valve]; }

bool recirculationController::getReleaseValve()
  { return __ReleaseValve; }

bool recirculationController::getGoValve(uint8_t valve)
  { return __Go[valve]; }

bool recirculationController::getFH2OValve()
  { return __Fh2o; }

bool recirculationController::getFSolValve()
  { return __FSol; }

bool recirculationController::getRH2OValve()
  { return __Rh2o; }

bool recirculationController::getRSolValve()
  { return __RSol; }

void recirculationController::releaseKegs(bool nut)
  { if(nut){ __RSol = HIGH; }
    else{ __Rh2o = HIGH; }
  }
bool recirculationController::setIn(uint8_t solution)
  { if(solution>=0 && solution<MAX_RECIRCULATION_TANK){
      __In = solution;
      return true;
    }
    else{ return false; }
  }

bool recirculationController::setOut(uint8_t solution)
  { if(solution>=0 && solution<MAX_RECIRCULATION_TANK){
      __Out = solution;
      return true;
    }
    else{ return false; }
  }

uint8_t recirculationController::getIn()
  { return __In; }

uint8_t recirculationController::getOut()
  { return __Out; }

bool recirculationController::addVolKnut(float liters)
  { if(liters>=0){
      __VolKnut += liters;
      return true;
    }
    else{return false;}
  }

bool recirculationController::addVolKh2o(float liters)
  { if(liters>=0){
      __VolKh2o += liters;
      return true;
    }
    else{return false;}
  }

float recirculationController::getVolKnut()
  { return __VolKnut;}

float recirculationController::getVolKh2o()
  { return __VolKh2o;}

void recirculationController::fillH2O(float liters)
  { if(!__FSol){
      if(!__Fh2o){
        if(liters>0){
          __FillLiters = liters;
          __Fh2o = HIGH;
        }
        else{ printAction("Cannot execute fillH2O, parameter liters incorrect"); }
      }
      else{ printAction(F("Water kegs is already filling")); }
    }
    else{ printAction(F("Cannot fill water kegs because we are filling solution Maker"));}
  }

void recirculationController::fillSol(float liters)
  { if(!__Fh2o){
      if(!__FSol){
        if(liters>0){
          __FillLiters = liters;
          __FSol = HIGH;
        }
        else{ printAction("Cannot execute fillSol, parameter liters incorrect"); }
      }
      else{ printAction(F("Solution Maker is already filling")); }
    }
    else{ printAction(F("Cannot fill solution Maker because we are filling water kegs"));}
  }

void recirculationController::moveIn()
  { if(!__InPump){ // Pump has to be off
      float recirculationVol = __Level[0]->getVolume();
      float tankVol = __Level[__In+1]->getVolume();
      float maxVolume = __Level[__In+1]->getMaxVolume();

      if(recirculationVol+tankVol<maxVolume){ // Transfering water
        __InPump = HIGH;
        __InValve[__In] = HIGH;
        printAction(recirculationVol, "recirculation tank", "solution "+String(__In));
      }
      else{ // Releasing water
        __InPump = HIGH;
        __ReleaseValve = HIGH;
        printAction("Releasing water outside the system");
      }
    }
    else{ printAction("Cannot execute moveIn, another solution is moving in"); }
  }

bool recirculationController::moveOut(float liters, uint8_t to_Where)
  { if(!__OutPump){
      if(to_Where>=0 && to_Where<MAX_RECIRCULATION_DESTINATIONS){
        __ActualLiters = __Level[__Out+1]->getVolume();
        if(__ActualLiters>liters){ // If there are enough solution
          __OutLiters = liters;
          __OutPump = HIGH;
          __OutValve[__Out] = HIGH;
          __Go[to_Where] = HIGH;
          String toWhere;
          if(to_Where==0){toWhere = "nutrition kegs";}
          else if(to_Where==1){toWhere = "water kegs";}
          else if(to_Where==2){toWhere = "solution maker";}
          printAction(__OutLiters, "solution"+String(__Out+1), toWhere);
          return true;
        }
        else{
          printAction(F("There are not enough solution"));

          if(to_Where==0 || to_Where==2){ // Nutrition Kegs || Solution Maker
            // Move to Solution Maker
            __OutLiters = __ActualLiters;
            __OutPump = HIGH;
            __OutValve[__Out] = HIGH;
            __Go[2] = HIGH;
            printAction(__OutLiters, "solution"+String(__Out+1), "solution maker");
            fillSol(liters-__ActualLiters); // Fill solution maker with the rest
            printAction(liters-__ActualLiters, "water line", "solution maker");
          }
          else if(to_Where==1){ // Water Kegs
            // Move to water kegs
            __OutLiters = __ActualLiters;
            __OutPump = HIGH;
            __OutValve[__Out] = HIGH;
            __Go[1] = HIGH;
            printAction(__OutLiters, "solution"+String(__Out+1), "water kegs");
            fillH2O(liters-__ActualLiters); // Fill water kegs with the rest
            printAction(liters-__ActualLiters, "water line", "water kegs");
          }
          return false;
        }
      }
      else{
        printAction("Cannot execute moveOut, parameter toWhere incorrect");
        return false;
      }
    }
    else{
      printAction("Cannot execute moveOut, another solution is moving out");
      return false;
    }
  }

void recirculationController::moveSol()
  { if(__Go[2]){ // If solution Maker Valve Open
      if(__Level[6]->getState()!=1){
        __SolPump = HIGH;
        printAction("Emptying solution Maker");
      }
      else{ printAction("There is nothing to move in solution Maker");}
    }
    else{ printAction("Cannot start moving solution because move out is not finished");}
  }

void recirculationController::run(bool releaseState)
  { // Move In when level in recirculation tank is High
    if(__Level[0]->getState()==2){ moveIn(); }

    // Stop Move In when level in recirculation tank is low
    // and InPump is ON
    if(__Level[0]->getState()==1 && __InPump){
      for(int i=0; i<MAX_RECIRCULATION_TANK; i++){
        if(__InValve[i]){ __InValve[i] = LOW; }
      }
      __InPump = LOW;
      printAction(F("Move In finished"));
    }

    // Stop Move Out when level in tank reach the volume require
    // or when tank level is low adn OutPump is ON
    if( (__ActualLiters-__Level[__Out+1]->getVolume()>=__OutLiters ||
        __Level[__Out+1]->getState()==1 ) && __OutPump){
      String toWhere;
      __OutPump = LOW;
      for(int i=0; i<MAX_RECIRCULATION_TANK; i++){
        if(__OutValve[i]){ __OutValve[i] = LOW; }
      }
      for(int i=0; i<MAX_RECIRCULATION_DESTINATIONS; i++){
        if(__Go[i]){
          __Go[i] = LOW;
          if(i==0){toWhere = "nutrition kegs";}
          else if(i==1){toWhere = "water kegs";}
          else if(i==2){toWhere = "solution Maker";}
        }
      }
      printAction("Move Out finished. " + String(__OutLiters) +
      " liters were move to " + toWhere);
      if(__Out==0){addVolKnut(__ActualLiters-__Level[__Out+1]->getVolume());}
      else if(__Out==1){addVolKh2o(__ActualLiters-__Level[__Out+1]->getVolume());}
      __ActualLiters = 0;
      __OutLiters = 0;
    }

    // Stop filling with water when
    if(__Fh2o || __FSol){
      getVolume();
      if(__H2OVol>=__FillLiters){
        if(__Fh2o){
          __Fh2o = LOW;
          printAction("Fill water kegs finished. " + String(__FillLiters) +
          " liters were move to water kegs");
        }
        else if(__FSol){
          __FSol = LOW;
          printAction("Fill solution Maker finished. " + String(__FillLiters) +
          " liters were move to solution maker");
        }
        __FillLiters = 0;
        __H2OVol = 0;
      }
    }

    // Stop moving from solutionMaker to nutrition kegs when there is nothing
    if(__SolPump && __Level[6]->getState()==1){
      __SolPump = LOW;
      printAction("Solution Maker emptied");
    }

    // Stop release
    if(__Rh2o && releaseState){
      __Rh2o = LOW;
      __VolKh2o = 0;
      printAction("Water kegs was emptied");
    }
    if(__RSol && releaseState){
      __RSol = LOW;
      __VolKnut = 0;
      printAction("Nutrition kegs was emptied");
    }
  }
