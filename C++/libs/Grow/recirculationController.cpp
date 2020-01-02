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
    __FPump = LOW;
    __Rh2o = LOW; __RSol = LOW;
    
    /*** Default Aux Variables ***/
    __In = 0; __Out = 0;
    __LastOut = 0;
    __VolKnut = 0;
    __VolKh2o = 0;
    __OutLiters = 0;
    __ActualLiters = 0;
    __FillLiters = 0;
    __SolLiters = 0;
    __Wait4Fill = 0;
    __WaitLiters = 0;
    // Flow meter
    //__K = 13.562; // flowSensor constant
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

void recirculationController::printAction(float volume, String from, String to, uint8_t level=0)
  { if(level==0){ Serial.print(F("debug,")); } // Debug
    else if(level==1){ Serial.print(F("info,")); } // Info
    else if(level==2){ Serial.print(F("warning,")); } // Warning
    else if(level==3){ Serial.print(F("error,")); } // Error
    else if(level==4){ Serial.print(F("critical,")); } // Error
    Serial.print(F("Recirculation: Moving "));
    Serial.print(volume);
    Serial.print(F(" liters from "));
    Serial.print(from);
    Serial.print(F(" to "));
    Serial.println(to);
  }

void recirculationController::printAction(String act, uint8_t level=0)
  { if(level==0){ Serial.print(F("debug,")); } // Debug
    else if(level==1){ Serial.print(F("info,")); } // Info
    else if(level==2){ Serial.print(F("warning,")); } // Warning
    else if(level==3){ Serial.print(F("error,")); } // Error
    else if(level==4){ Serial.print(F("critical,")); } // Error
    Serial.print(F("Recirculation: "));
    Serial.println(act);
  }

void recirculationController::printAction(String act1, String act2, String act3, String act4, uint8_t level=0)
  { if(level==0){ Serial.print(F("debug,")); } // Debug
    else if(level==1){ Serial.print(F("info,")); } // Info
    else if(level==2){ Serial.print(F("warning,")); } // Warning
    else if(level==3){ Serial.print(F("error,")); } // Error
    else if(level==4){ Serial.print(F("critical,")); } // Error
    Serial.print(F("Recirculation: "));
    Serial.print(act1);
    Serial.print(act2);
    Serial.print(act3);
    Serial.println(act4);
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

bool recirculationController::getFPump()
  { return __FPump; }
  
uint8_t recirculationController::getWait4Fill()
  { return __Wait4Fill; }
  
bool recirculationController::getRH2OValve()
  { return __Rh2o; }

bool recirculationController::getRSolValve()
  { return __RSol; }

void recirculationController::releaseKegs(bool nut)
  { if(nut){ __RSol = HIGH; }
    else{ __Rh2o = HIGH; }
  }
  
void recirculationController::finishRelease(bool nut)
  { if(nut){ __RSol = LOW; }
    else{ __Rh2o = LOW; }
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
  { if(__VolKnut + liters>=0){
      __VolKnut += liters;
      return true;
    }
    else{
      __VolKnut = 0;
      return false;
    }
  }

bool recirculationController::addVolKh2o(float liters)
  { if(__VolKh2o + liters>=0){
      __VolKh2o += liters;
      return true;
    }
    else{
      __VolKh2o = 0;
      return false;
    }
  }

float recirculationController::getVolKnut()
  { return __VolKnut;}

float recirculationController::getVolKh2o()
  { return __VolKh2o;}

void recirculationController::resetVolKnut()
  { __VolKnut = 0; }

void recirculationController::resetVolKh2o()
  { __VolKh2o = 0; }

float recirculationController::getMissingLiters()
  { if(!__OutPump && !__Fh2o && !__FSol){ return 0; }
    float pumpLiters = __OutLiters-(__ActualLiters-__Level[__LastOut+1]->getVolume());
    if(pumpLiters<0){ pumpLiters=0; }
    float outsideLiters = __FillLiters-__H2OVol;
    if(outsideLiters<0){ outsideLiters=0; }
    float totalMissingLiters = pumpLiters + outsideLiters;
    return totalMissingLiters;
  }
  
void recirculationController::fillH2O(float liters)
  { if(!__FSol){
      if(!__Fh2o){
        if(liters>0){
          __FillLiters = liters;
          __Fh2o = HIGH;
          __FPump = HIGH;
        }
        else{ printAction(F("Cannot execute fillH2O... parameter liters incorrect"), 3); }
      }
      else{ printAction(F("Water kegs is already filling"), 2); }
    }
    else{ printAction(F("Cannot fill water kegs because we are filling solution Maker"), 3);}
  }

void recirculationController::fillSol(float liters)
  { if(!__Fh2o){
      if(!__FSol){
        if(liters>0){
          __FillLiters = liters;
          __FSol = HIGH;
          __FPump = HIGH;
        }
        else{ printAction(F("Cannot execute fillSol... parameter liters incorrect"), 3); }
      }
      else{ printAction(F("Solution Maker is already filling"), 2); }
    }
    else{ printAction(F("Cannot fill solution Maker because we are filling water kegs"), 3);}
  }

bool recirculationController::moveIn()
  { if(!__InPump){ // Pump has to be off
      float recirculationVol = __Level[0]->getVolume()-__Level[0]->getMinVolume();
      float tankVol = __Level[__In+1]->getVolume();
      float maxVolume = __Level[__In+1]->getMaxVolume();

      if(recirculationVol <= 0){
        printAction(F("Level is too low to turn on the pump"), 2);
      }
      else if(recirculationVol+tankVol<maxVolume){ // Transfering water
        __InPump = HIGH;
        __InValve[__In] = HIGH;
        printAction(recirculationVol, F("recirculation tank"), "solution "+String(__In+1), 0);
      }
      else{ // Releasing water
        // There is not release valve installed yet, not use rigth now
        // __InPump = HIGH;
        // __ReleaseValve = HIGH;
        printAction(F("Releasing water outside the system"), 3);
      }
      return true;
    }
    printAction(F("Cannot execute moveIn... pump already working"), 2);
    return false;
  }

uint8_t recirculationController::moveOut(float liters, uint8_t to_Where)
  { if(!__OutPump){
      if(to_Where>=0 && to_Where<MAX_RECIRCULATION_DESTINATIONS){
        __ActualLiters = __Level[__Out+1]->getVolume()-__Level[__Out+1]->getMinVolume();
        __LastOut = __Out;
  
        if(__ActualLiters>liters){ // If there are enough solution
          __OutLiters = liters;
          __OutPump = HIGH;
          __OutValve[__Out] = HIGH;
          __Go[to_Where] = HIGH;
          String toWhere;
          if(to_Where==NUTRITION_KEGS){
            toWhere = "nutrition kegs";
            addVolKnut(__OutLiters);
          }
          else if(to_Where==WATER_KEGS){
            toWhere = "water kegs";
            addVolKh2o(__OutLiters);
          }
          else if(to_Where==SOLUTION_MAKER){toWhere = "solution maker";}
          printAction(__OutLiters, "solution "+String(__Out+1), toWhere, 0);
          return 1;
        }
        else{
          printAction(F("There are not enough solution"), 0);
          liters = liters*1.15; // Add or prepare 15% extra water
          
          if(to_Where==NUTRITION_KEGS || to_Where==SOLUTION_MAKER){ // Nutrition Kegs || Solution Maker
            // Move to Solution Maker
            __OutLiters = __ActualLiters;
            __OutPump = HIGH;
            __OutValve[__Out] = HIGH;
            __Go[2] = HIGH;
            printAction(__OutLiters, "solution"+String(__Out+1), F("solution maker"), 0);
            __Wait4Fill = 1; // Wait for fill sMaker
            __WaitLiters = liters-__OutLiters; // Liters to move when await finished
          }
          else if(to_Where==WATER_KEGS){ // Water Kegs
            // Move to water kegs
            __OutLiters = __ActualLiters;
            __OutPump = HIGH;
            __OutValve[__Out] = HIGH;
            __Go[1] = HIGH;
            addVolKh2o(__OutLiters);
            printAction(__OutLiters, "solution"+String(__Out+1), F("water kegs"), 0);
            __Wait4Fill = 2; // Wait for fill kegs_h20
            __WaitLiters = liters-__OutLiters;  // Liters to move when await finished
          }
          return 2;
        }
      }
      else{
        printAction(F("Cannot execute moveOut... parameter toWhere incorrect"), 3);
        return 0;
      }
    }
    else{
      printAction(F("Cannot execute moveOut... pump already working"), 2);
      return 0;
    }
  }

bool recirculationController::moveSol()
  { // If there is enough level in sMaker
    if(__Level[6]->getState()!=1){
      __SolLiters = __Level[6]->getVolume();
      __SolPump = HIGH;
      printAction(F("Emptying solution Maker"), 1);
    }
    else{ printAction(F("There is nothing to move in solution Maker"), 2);}
    return true;
  }

void recirculationController::run(bool check, bool sensorState)
  { // Move In when level in recirculation tank is High and Input Pump is off
    if(__Level[0]->getState()==2 && !__InPump){ moveIn(); }

    // Stop Move In when level in recirculation tank is low
    // and InPump is ON
    if(__Level[0]->getState()==1 && __InPump){
      for(int i=0; i<MAX_RECIRCULATION_TANK; i++){
        if(__InValve[i]){ __InValve[i] = LOW; }
      }
      __InPump = LOW;
      printAction(F("Move In finished"), 0);
    }

    // Stop Move Out when level in tank reach the volume require
    // or when tank level is low and OutPump is ON
    if( (__ActualLiters-__Level[__LastOut+1]->getVolume()>=__OutLiters ||
        __Level[__LastOut+1]->getState()==1 ) && __OutPump){
      String toWhere;
      __OutPump = LOW;
      for(int i=0; i<MAX_RECIRCULATION_TANK; i++){
        if(__OutValve[i]){ __OutValve[i] = LOW; }
      }
      for(int i=0; i<MAX_RECIRCULATION_DESTINATIONS; i++){
        if(__Go[i]){
          __Go[i] = LOW;
          if(i==NUTRITION_KEGS){toWhere = "nutrition kegs";}
          else if(i==WATER_KEGS){toWhere = "water kegs";}
          else if(i==SOLUTION_MAKER){toWhere = "solution Maker";}
        }
      }
      printAction(F("Move Out finished. "), String(__OutLiters), 
      F(" liters were move to "), toWhere, 0);
      //__ActualLiters = 0;
      __OutLiters = 0;
      
      if(__Wait4Fill==1){ // Now fill sMaker
        fillSol(__WaitLiters); // Fill solution maker with the rest
        printAction(__WaitLiters, F("water line"), F("solution maker"), 0);
        __Wait4Fill = 0;
        __WaitLiters = 0;
      }
      else if(__Wait4Fill==2){ // Now fill kegs_h2o
        fillH2O(__WaitLiters); // Fill water kegs with the rest
        printAction(__WaitLiters, F("water line"), F("water kegs"), 0);
        __Wait4Fill = 0;
        __WaitLiters = 0;
      }
    }

    // Stop moving from solutionMaker to nutrition kegs when there is nothing
    if(__SolPump && __Level[6]->getState()==1){
      __SolPump = LOW;
      addVolKnut(__SolLiters-__Level[6]->getVolume());
      printAction(F("Solution Maker emptied"), 1);
    }

    // Stop filling with water when
    if(__Fh2o || __FSol){
      getVolume();
      if(__Fh2o && __H2OVol>=__FillLiters){
        __Fh2o = LOW;
        __FPump = LOW;
        printAction(F("Fill water kegs finished. "), String(__FillLiters),
        F(" liters were move to water kegs"), F(""), 0);
        addVolKh2o(__H2OVol);
        __FillLiters = 0;
        __H2OVol = 0;
      }
      else if(__FSol && (__H2OVol>=__FillLiters || __Level[6]->getVolume()-__Level[6]->getMinVolume()>=__FillLiters+__OutLiters)){
        __FSol = LOW;
        __FPump = LOW;
        printAction(F("Fill solution Maker finished. "), String(__FillLiters),
        F(" liters were move to solution maker"), F(""), 0);
        __FillLiters = 0;
        __H2OVol = 0;
      }
    }

    if(check){
      // Stop release
      if(__Rh2o && sensorState){
        __Rh2o = LOW;
        __VolKh2o = 0;
        printAction(F("Water kegs was emptied"), 1);
      }
      if(__RSol && sensorState){
        __RSol = LOW;
        __VolKnut = 0;
        printAction(F("Nutrition kegs was emptied"), 1);
      }
    }

  }
