// actuator.cpp
//
// Copyright (C) 2019 Grow

#include "actuator.h"

/***   Actuator   ***/
// Statics variables definitions
uint8_t Actuator::__TotalActuators = 0;
Actuator *Actuator::ptr[MAX_ACTUATORS];

Actuator::Actuator( // Constructor
    uint8_t type,
    uint8_t floor,
    unsigned long t_on,
    unsigned long t_cycle
  )
   { // Just the first time init pointers
     if(__TotalActuators<1){
       for (int i=0; i < MAX_ACTUATORS; i++) {
         ptr[i] = NULL; // All Pointers NULL
       }
     }

     __Type = type;
     __Floor = floor;

     // Default parameters
     __State = LOW;
     __Enable = false;

     if(t_on<t_cycle){
       __TimeOn = t_on*1000UL;
       __CycleTime = t_cycle*1000UL;
     }
     else{
       __TimeOn = DEFAULT_TIME_ON;
       __CycleTime = DEFAULT_CYCLE_TIME;
     }

     ptr[__TotalActuators] = this; // Set Static pointer to object
     __TotalActuators++; // Add the new actuator to the total
   }

void Actuator::setTime()
  { if(__State == HIGH)
    { resetTime(); }
    else{ __Actual_time = millis() - __TimeOn; }
  }

void Actuator::resetTime()
  {  __Actual_time = millis(); }

void Actuator::printAction(String act)
  { Serial.print(F("Actuator ("));
    switch(__Type){
      case 0:
        Serial.print(F("Input Fan-"));
        break;
      case 1:
        Serial.print(F("Output Fan-"));
        break;
      case 2:
        Serial.print(F("Vent Fan-"));
        break;
      case 3:
        Serial.print(F("Humidity Valve-"));
        break;
      default:
        break;
    }
    Serial.print(__Floor+1);
    Serial.print(F("): "));
    Serial.println(act);
  }

void Actuator::turnOn()
  { __State = HIGH;
    setTime();
  }

void Actuator::turnOff()
  { __State = LOW;
    setTime();
  }

bool Actuator::setTimeOn(unsigned long t_on)
  { if(t_on*1000UL < __CycleTime){
     __TimeOn = t_on*1000UL;
     printAction("TimeOn changed to " + String(t_on) + " seconds");
     return true;
   }
   else{
     printAction("Cannot change TimeOn. Parameter has to be smaller than " +
     String(float(__CycleTime)/1000) + " seconds");
     return false;
   }
  }

bool Actuator::setCycleTime(unsigned long t_cycle)
  { if(__TimeOn < t_cycle*1000UL){
     __CycleTime = t_cycle*1000UL;
     printAction("TimeOff changed to " + String(t_cycle) + " seconds");
     return true;
   }
   else{
     printAction("Cannot change TimeOff. Parameter has to be bigger than " +
     String(float(__TimeOn)/1000) + " seconds");
     return false;
   }
  }

bool Actuator::setTime(unsigned long t_on, unsigned long t_cycle)
  { if(t_on < t_cycle){
     __TimeOn = t_on*1000UL;
     __CycleTime = t_cycle*1000UL;
     printAction("TimeOn changed to " + String(t_on) + " seconds");
     printAction("TimeOff changed to " + String(t_cycle) + " seconds");
     return true;
   }
   else{
     printAction(F("Cannot change Times. Parameter t_on has to be bigger than t_cycle"));
     return false;
   }
  }

unsigned long Actuator::getTimeOn()
  { return __TimeOn/1000; }

unsigned long Actuator::getCycleTime()
  { return __CycleTime/1000; }

unsigned long Actuator::getTime()
  { return (millis()-__Actual_time)/1000; }

void Actuator::enable(bool en)
  { if(__Enable!=en){
      __Enable = en;
      if(__Enable){ printAction(F("Enable")); }
      else{ printAction(F("Disable")); }
    }
  }

bool Actuator::isEnable()
  { return (__Enable);   }

bool Actuator::isType(uint8_t type)
  { if(__Type == type){return true;}
    else{ return false;}
  }

bool Actuator::isFloor(uint8_t floor)
  { if(__Floor == floor){return true;}
    else{ return false;}
  }

void Actuator::begin()
  { __Enable = true;
    resetTime();
  }

void Actuator::run()
  {  if(__Enable){
       if( millis()-__Actual_time<__TimeOn && __State==LOW){
         __State = HIGH;
       }
       else if(millis()-__Actual_time>=__TimeOn && millis()-__Actual_time<__CycleTime && __State==HIGH){
         __State = LOW;
       }
       else if(millis()-__Actual_time>=__CycleTime){
         resetTime();
       }
     }
  }

bool Actuator::getState()
  { return __State; }

void Actuator::setTimeOnAll(
    uint8_t type,
    uint8_t floor,
    unsigned long timeOn
  ){
    for(int i=0; i < __TotalActuators; i++){
      if(ptr[i]->isType(type) && ptr[i]->isFloor(floor)){
        ptr[i]->setTimeOn(timeOn);
        break;
      }
    }
  }

void Actuator::setCycleTimeAll(
    uint8_t type,
    uint8_t floor,
    unsigned long cycleTime
  ){
    for(int i=0; i < __TotalActuators; i++){
      if(ptr[i]->isType(type) && ptr[i]->isFloor(floor)){
        ptr[i]->setCycleTime(cycleTime);
        break;
      }
    }
  }

void Actuator::setTimeAll(
    uint8_t type,
    uint8_t floor,
    unsigned long timeOn,
    unsigned long cycleTime
  ){
    for(int i=0; i < __TotalActuators; i++){
      if(ptr[i]->isType(type) && ptr[i]->isFloor(floor)){
        ptr[i]->setTimeOn(timeOn);
        ptr[i]->setCycleTime(cycleTime);
        break;
      }
    }
  }

unsigned long Actuator::getTimeOnAll(uint8_t type, uint8_t floor)
  { for(int i=0; i < __TotalActuators; i++){
      if(ptr[i]->isType(type) && ptr[i]->isFloor(floor)){
        unsigned long timeOn = ptr[i]->getTimeOn();
        ptr[i]->printAction("Time On " + String(timeOn) + " seconds");
        return timeOn;
      }
    }
    return 0;
  }

unsigned long Actuator::getCycleTimeAll(uint8_t type, uint8_t floor)
  { for(int i=0; i < __TotalActuators; i++){
      if(ptr[i]->isType(type) && ptr[i]->isFloor(floor)){
        unsigned long cycleTime = ptr[i]->getCycleTime();
        ptr[i]->printAction("Cycle time " + String(cycleTime) + " seconds");
        return cycleTime;
      }
    }
    return 0;
  }

unsigned long Actuator::getTimeAll(uint8_t type, uint8_t floor)
  { for(int i=0; i < __TotalActuators; i++){
      if(ptr[i]->isType(type) && ptr[i]->isFloor(floor)){
        unsigned long acTime = ptr[i]->getTime();
        ptr[i]->printAction("Time since cycle started " + String(acTime) + " seconds");
        return acTime;
      }
    }
    return 0;
  }

void Actuator::enableAll(uint8_t type, uint8_t floor, bool en){
    for(int i=0; i < __TotalActuators; i++){
      if(ptr[i]->isType(type) && ptr[i]->isFloor(floor)){
        ptr[i]->enable(en);
        break;
      }
    }
  }

void Actuator::beginAll()
  { for (int i = 0; i < __TotalActuators; i++){
      (ptr[i]->begin());
    }
  }

void Actuator::runAll()
  { for (int i = 0; i < __TotalActuators; i++){
      (ptr[i]->run());
    }
  }

/***   solenoidValve   ***/
// Statics variables definitions
bool solenoidValve::__EnableGroup = LOW;
String solenoidValve::__Group = "Irrigation";
uint8_t solenoidValve::__ActualNumber = 0;
unsigned long solenoidValve::__CycleTime = 600000;
unsigned long solenoidValve::__ActionTime = 0;
float solenoidValve::__K = 6.781; // flowSensor constant
volatile long solenoidValve::__NumPulses = 0;
float solenoidValve::__H2Ow = 0;
uint8_t solenoidValve::__TotalActuators = 0;
solenoidValve *solenoidValve::ptr[MAX_ACTUATORS];

void solenoidValve::countPulses()
  {  __NumPulses++; }

void solenoidValve::flowSensorBegin()
  { pinMode(flowSensor,INPUT);
    attachInterrupt(digitalPinToInterrupt(flowSensor), countPulses, RISING);
  }

solenoidValve::solenoidValve(String name) // Constructor
   {  // Just the first time init arrays
      if(__TotalActuators<1){
        __EnableGroup = false;
        for (int i=0; i < MAX_ACTUATORS; i++) { ptr[i] = NULL; } // All Pointers NULL
      }
     __State = LOW; // Off
     __Enable = true; // Enable
     __Floor = 0; // Default floor
     __Region = 0; // Default region
     __TimeOn = 10; // Default time
     setTime();
     __Name = name;
     __H2OVolume = 0;

     ptr[__TotalActuators] = this; // Set Static pointer to object
     __Number = __TotalActuators++; // Add the new actuator to the total
   }

void solenoidValve::setTime()
  { if(__State == HIGH)
      { resetTime(); }
    else
      { __Actual_time = millis() - __TimeOn; }
  }

void solenoidValve::resetTime()
  {  __Actual_time = millis(); }

bool solenoidValve::changeOrderNumber(uint8_t new_number)
  { if(new_number<__TotalActuators){
      __Number = new_number;
      return true; // Succesful
    }
    else {return false;} // Not enough space
  }

void solenoidValve::getWasteH2O(bool print = true)
  { noInterrupts(); // Disable Interrupts
    float newVolume = __NumPulses/(60*__K);
    __H2Ow = newVolume + __H2Ow;
    __NumPulses = 0;
    interrupts();   // Enable Interrupts
    printAllH2O(print);
  }

void solenoidValve::getConsumptionH2O()
  { noInterrupts(); // Disable Interrupts
    float newVolume = __NumPulses/(60*__K);
    if(newVolume>0){
      __H2OVolume= newVolume +__H2OVolume;
      solenoidPrint("Water Consumption = " + String(newVolume) + " liters");
    }
    __NumPulses = 0;
    interrupts();   // Enable Interrupts
  }

void solenoidValve::solenoidPrint(String act)
  { Serial.print(F("Solenoid Valve "));
    Serial.print(__Name);
    Serial.print(F(": "));
    Serial.println(act);
  }

void solenoidValve::groupPrint(String act)
  { Serial.print(F("Solenoid Group "));
    Serial.print(__Group);
    Serial.print(F(": "));
    Serial.println(act);
  }

unsigned long solenoidValve::getTime()
  { return (millis() - __Actual_time); }

bool solenoidValve::setTimeOn(unsigned long t_on)
  { if(__ActionTime - __TimeOn + t_on < __CycleTime){
      float seconds = float(t_on)/1000;
      __ActionTime -= __TimeOn;
      __TimeOn = t_on;
      __ActionTime += __TimeOn;
      solenoidPrint("Time On changed to " + String(seconds) + " seconds");
      return true; // Succesful
    }
    else{ // Not enough time
      float seconds = float(__CycleTime)/1000;
      solenoidPrint("Time On cannot be changed. Total solenoids turn-on time has to be smaller than " +
      String(seconds) + " seconds");
      return false;
    }
  }

unsigned long solenoidValve::getTimeOn()
  { return __TimeOn; }

bool solenoidValve::getState()
  {  return (__State); }

uint8_t solenoidValve::getOrder()
  {  return __Number; }

void solenoidValve::turnOn(bool rst_time)
  { __State = HIGH;
    solenoidPrint(F("Turn On"));
    if(rst_time){setTime(); }
  }

void solenoidValve::turnOff(bool rst_time)
  { __State = LOW;
    solenoidPrint(F("Turn Off"));
    if(rst_time){setTime(); }
  }

bool solenoidValve::reOrder(uint8_t number)
  { bool finished;

    if(number==__Number){ finished = true; }
    else if(number<__TotalActuators){
      for(int i = 0; i<__TotalActuators; i++){ // Redefine group loop
        if(ptr[i]->getOrder()==number){
          if( ptr[i]->changeOrderNumber(__Number) ){
            __Number = number;
            finished = true;
          }
          else{ finished = false; } // Imposible to change the order of the group
        }
      }
    }
    else{finished = false; } // Incorrect number proposition

    if(finished){ solenoidPrint("Reorder Succesful. New Number = " + String(__Number)); }
    else{ solenoidPrint("Reorder Failed. Number order has to be smaller than " + String(__TotalActuators)); }

    return finished;
  }

void solenoidValve::enable(bool en)
  { if(__Enable!=en){
      __Enable = en;
      if(__Enable){ solenoidPrint(F("Enabled")); }
      else{ solenoidPrint(F("Disabled")); }
    }
  }

bool solenoidValve::isEnable()
  {  return (__Enable); }

uint8_t solenoidValve::getFloor()
  { return __Floor; }

uint8_t solenoidValve::getRegion()
  { return __Region; }

String solenoidValve::getName()
  { return __Name; }

bool solenoidValve::reOrder_byFloor(uint8_t fl)
  { int count = 0;
    for(int i = 0; i<__TotalActuators; i++){
      if(ptr[i]->getFloor()==fl){
        for(int j = 0; j<MAX_IRRIGATION_REGIONS; j++){
            float order = ptr[i]->getOrder();
            int newOrder = order-int(order/MAX_IRRIGATION_REGIONS)*MAX_IRRIGATION_REGIONS;
            if(j==newOrder){
              if(ptr[i]->reOrder( (MAX_FLOOR-1)*MAX_IRRIGATION_REGIONS+j) ){
                count++;
                break;
              }
            }
        }
      }
    }
    if(count==MAX_IRRIGATION_REGIONS){
      groupPrint(F("re-order by Floor succesful"));
      return true;
    }
    else{
      groupPrint(F("re-order by Floor unsuccesful"));
      return false;}
  }

void solenoidValve::restartH2O(bool print)
  { if(print){ solenoidPrint(F("Water Volume Restarted")); }
    __H2OVolume = 0;
  }

void solenoidValve::restartAllH2O(bool print)
  { Serial.println(F("Restarting all water parameters saved"));
    __H2Ow = 0;
    if(print){ Serial.println(F("Waste Water Volume Restarted")); }
    for(int i = 0; i<__TotalActuators; i++){ ptr[i]->restartH2O(print); }
  }

float solenoidValve::getH2O()
  { return __H2OVolume; }

void solenoidValve::printH2O()
  { solenoidPrint("Water Volume = " + String(__H2OVolume) + " liters"); }

void solenoidValve::printAllH2O(bool printAll = true)
  { if(__H2Ow>0){ groupPrint("Wasted Water Volume = " + String(__H2Ow) + " liters"); }
    if(printAll){ for(int i = 0; i<__TotalActuators; i++){ptr[i]->printH2O();} }
  }

uint8_t solenoidValve::begin(uint8_t fl, uint8_t reg)
  { if(fl<MAX_FLOOR){
      if(reg<MAX_IRRIGATION_REGIONS){
        __State = LOW; // Off
        __Floor = fl; // Select floor
        __Region = reg; // Select region
        __TimeOn = SOLENOID_TIME_ON*1000UL;
        setTime();

        __ActualNumber = 0; // Restart count with each init
        __Enable = HIGH; // Enable HIGH
        __CycleTime = SOLENOID_CYCLE_TIME*60000UL;
        __ActionTime += __TimeOn; // Add action time to each group

        solenoidPrint(F("Started correctly"));
        return 0; // Succesful
      }
      else{
        solenoidPrint(F("Region value incorrectly configured"));
        return 1; // Region outside of range [0-MAX_IRRIGATION_REGIONS-1]
      }
    }
    else{
      solenoidPrint(F("Floor value incorrectly configured"));
      return 2; // Floor outside of range [0-MAX_FLOOR-1]
    }
  }

bool solenoidValve::run()
  { if(__EnableGroup==true){ // If the Group is enable
      if(__Enable==true){ // If the solenoid is enable
        if(__ActualNumber==__Number){ // If the solenoid is in action
          if(__State==LOW){
            if(__Number==0){ // If the cycle is starting
              getWasteH2O(false);
              restartAllH2O(false);
            }
            // If valve is off and have not started water irrigation
            if(__Number!=MAX_IRRIGATION_REGIONS*(MAX_FLOOR-1)){
              turnOn(true);
            }
            // If valve is off and has to start water irrigation
            else{
              unsigned long controlTime = 0;
              for(int i=0; i<__Number; i++){ controlTime+=ptr[i]->getTimeOn(); }
              /* Condition to start:
               * First counter>time that already happen (controlTime)+(3/4 of the total time_off)
               */
              if(ptr[0]->getTime()>controlTime+float(__CycleTime-__ActionTime)*0.75){
                getWasteH2O(false);
                turnOn(true);
              }
            }
          }
          // Turn off the solenoid when it is time
          else if( (millis()-__Actual_time)>=__TimeOn && __State==HIGH){
            turnOff(false);
            getConsumptionH2O();
            __ActualNumber++;
            // If it is turn of the water irrigation disable the Group
            if(__ActualNumber==MAX_IRRIGATION_REGIONS*(MAX_FLOOR-1)){ enableGroup(false); }
            // The Group has to be enable when preconditions for water irrigation get ready
            // If the last solenoid was turned off then disable the Group
            else if(__ActualNumber==__TotalActuators){enableGroup(false); }
            // The Group has to be enable when the preconditions get ready
          }
        }
        // Restart cycle
        else if(__ActualNumber>=__TotalActuators && (millis()-__Actual_time)>=__CycleTime){
          __ActualNumber = 0;
          groupPrint(F("Restarting cycle"));
        }
        return true;
     }
     else{ // If the solenoid is disable
       if(__ActualNumber == __Number){ // If the solenoid is in action turn it off
           if(__State==HIGH){turnOff(false);}
           if(__Number == 0){
             getWasteH2O(false);
             restartAllH2O(false);
           }
           setTime();
           __ActualNumber++;
           // If it is turn of the water irrigation disable the Group
           if(__ActualNumber==MAX_IRRIGATION_REGIONS*(MAX_FLOOR-1)){ enableGroup(false); }
           // The Group has to be enable when preconditions for water irrigation get ready
           // If the last solenoid was turned off then disable the Group
           else if(__ActualNumber==__TotalActuators){enableGroup(false); }
           // The Group has to be enable when the preconditions get ready
       }
       else if(__ActualNumber>=__TotalActuators && (millis()-__Actual_time)>=__CycleTime){
         __ActualNumber = 0;
         groupPrint(F("Restarting cycle"));
       }
       return false;
      }
    }
    else{ // If the Group is disable
      // Testing if(__State==HIGH){turnOff(false);}
      return false;
    }
  }

unsigned long solenoidValve::getActionTime()
  { return __ActionTime; }

unsigned long solenoidValve::getCycleTime()
  { return __CycleTime; }

bool solenoidValve::setCycleTime ( unsigned long t_cycle)
  { if( __ActionTime < t_cycle ){
      float minute = float(t_cycle)/60000;
      __CycleTime = t_cycle;
      ptr[0]->groupPrint("Cycle Time changed to " + String(minute) + " minutes");
      return true; // Succesful
    }
    else{ // Not enough time
      float minute = float(__ActionTime)/60000;
      ptr[0]->groupPrint("Cycle Time cannot be changed, it has to be greater than " +
      String(minute) + " minutes");
      return false;
    }
  }

uint8_t solenoidValve::getActualNumber()
  { return (__ActualNumber); }

bool solenoidValve::reOrderAll(bool sequence, bool order)
  { int count = 0;

    if(sequence==LOW){
      if(order==LOW){
        for(int k = 0; k<MAX_FLOOR; k++){
          for(int j = 0; j<MAX_IRRIGATION_REGIONS; j++){
            for(int i = 0; i<__TotalActuators; i++){
              if(ptr[i]->getFloor()==k && ptr[i]->getRegion()==j){
                if(ptr[i]->reOrder(k*MAX_IRRIGATION_REGIONS+j)){
                  count++;
                  break;
                }
              }
            }
          }
        }
      }
      else{
        for(int k = 0; k<MAX_FLOOR; k++){
          for(int j = MAX_IRRIGATION_REGIONS-1; j>=0; j--){
            for(int i = 0; i<__TotalActuators; i++){
              if(ptr[i]->getFloor()==k && ptr[i]->getRegion()==j){
                if(ptr[i]->reOrder(k*MAX_IRRIGATION_REGIONS+7-j)){
                  count++;
                  break;
                }
              }
            }
          }
        }
      }
    }
    else{
      if(order==LOW){
        for(int k = 0; k<MAX_FLOOR; k++){
          for(int j = MAX_IRRIGATION_REGIONS-1; j>=0; j--){
            for(int i = 0; i<__TotalActuators; i++){
              if(j<MAX_IRRIGATION_REGIONS/2){
                if(ptr[i]->getFloor()==k && ptr[i]->getRegion()==j){
                  if(ptr[i]->reOrder(k*MAX_IRRIGATION_REGIONS+j*2)){
                    count++;
                    break;
                  }
                }
              }
              if(j>=MAX_IRRIGATION_REGIONS/2){
                if(ptr[i]->getFloor()==k && ptr[i]->getRegion()==j){
                  if(ptr[i]->reOrder(k*MAX_IRRIGATION_REGIONS+(j-MAX_IRRIGATION_REGIONS/2)*2+1)){
                    count++;
                    break;
                  }
                }
              }
            }
          }
        }
      }
      else{
        for(int k = 0; k<MAX_FLOOR; k++){
          for(int j = MAX_IRRIGATION_REGIONS-1; j>=0; j--){
            for(int i = 0; i<__TotalActuators; i++){
              if(j<MAX_IRRIGATION_REGIONS/2){
                if(ptr[i]->getFloor()==k && ptr[i]->getRegion()==j){
                  if(ptr[i]->reOrder(k*MAX_IRRIGATION_REGIONS+7-j*2)){
                    count++;
                    break;
                  }
                }
              }
              if(j>=MAX_IRRIGATION_REGIONS/2){
                if(ptr[i]->getFloor()==k && ptr[i]->getRegion()==j){
                  if(ptr[i]->reOrder(k*MAX_IRRIGATION_REGIONS+6-2*(j-MAX_IRRIGATION_REGIONS/2))){
                    count++;
                    break;
                  }
                }
              }
            }
          }
        }
      }
    }

    if(count==__TotalActuators){return true;}
    else{return false;} // Something is wrong
  }

bool solenoidValve::defaultOrder(uint8_t night_floor)
  { int count = 0;

    for(int i=0; i<MAX_IRRIGATION_REGIONS; i++){
      for(int j = MAX_FLOOR-1; j>=0; j--){
        for(int k = 0; k<__TotalActuators; k++){
          if(j==night_floor){break;}
          if(ptr[k]->getFloor()==j && ptr[k]->getRegion()==i){
            if(i<MAX_IRRIGATION_REGIONS/2){
              if(ptr[k]->reOrder(count+(MAX_FLOOR-1)*(i+1))){ // Dudas sobre esta línea
                count++;
                break;
              }
            }
            else{
              if(ptr[k]->reOrder(count-((MAX_FLOOR-1)*MAX_IRRIGATION_REGIONS/2)+(MAX_FLOOR-1)*(i-4))){
                count++;
                break;
              }
            }
          }
        }
      }
    }

    for(int i=0; i<MAX_IRRIGATION_REGIONS; i++){
      for(int j = 0; j<__TotalActuators; j++){
        if(ptr[j]->getFloor()==night_floor && ptr[j]->getRegion()==i) {
          if(i<MAX_IRRIGATION_REGIONS/2){
            if(ptr[j]->reOrder(count+i+1)){
              count++;
              break;
            }
          }
          else{
            if(ptr[j]->reOrder(count-MAX_IRRIGATION_REGIONS/2+(i-4))){
              count++;
              break;
            }
          }
        }
      }
    }

    if(count==__TotalActuators){
      ptr[0]->groupPrint(F("default re-order succesful"));
      return true;
    }
    else{
      ptr[0]->groupPrint(F("default re-order unsuccesful"));
      return false;
    }
  }

void solenoidValve::enableGroup(bool en)
  { __EnableGroup = en;
    if(__EnableGroup){
      ptr[__ActualNumber]->groupPrint(F("Enabled"));
      ptr[__ActualNumber]->getWasteH2O(false);
    }
    else{
      ptr[__ActualNumber]->groupPrint(F("Disabled"));
      ptr[__ActualNumber]->getConsumptionH2O();
      }
  }

bool solenoidValve::isEnableGroup()
  { return (__EnableGroup); }

float solenoidValve::getWaterAll()
  { float h2o = 0;
    for(int i=0; i<__TotalActuators; i++){
      h2o += ptr[i]->getH2O();
    }
    h2o += __H2Ow;
    return h2o;
  }

float solenoidValve::getWaterByFloor(uint8_t fl)
  { float h2o = 0;
    for(int i=0; i<__TotalActuators; i++){
      if(ptr[i]->getFloor()==fl){h2o += ptr[i]->getH2O();}
    }
    return h2o;
  }
void solenoidValve::runAll()
  { for (int i = 0; i < __TotalActuators; i++){
      (ptr[i]->run());
    }
  }

/***   asyncActuator   ***/
// Constructor
asyncActuator::asyncActuator(String name)
   { __State = LOW;
     __Enable = true;
     __Name = name;
   }

void asyncActuator::printAction(String act)
  { Serial.print(F("Actuator ("));
    Serial.print(__Name);
    Serial.print(F("): "));
    Serial.println(act);
  }

void asyncActuator::turnOn()
  { if(!__State){
      __State = HIGH;
      printAction(F("Turn On"));
    }
  }

void asyncActuator::turnOff()
  { if(__State){
      __State = LOW;
      printAction(F("Turn Off"));
    }
  }

void asyncActuator::enable(bool en)
  { if(__Enable!=en){
      __Enable = en;
      if(__Enable){ printAction(F("Enable")); }
      else{ printAction(F("Disable")); }
    }
  }

bool asyncActuator::isEnable()
  { return (__Enable); }

bool asyncActuator::getState()
  { return __State; }
