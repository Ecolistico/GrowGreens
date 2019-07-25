// actuator.cpp
//
// Copyright (C) 2019 Grow

#include "actuator.h"

/***   Actuator   ***/
// Statics variables definitions
byte Actuator::__TotalActuators = 0;
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

void Actuator::set_time()
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
    set_time();
  }

void Actuator::turnOff()
  { __State = LOW;
    set_time();
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
  { if(__CycleTime < t_cycle*1000UL){
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
     printAction("Cannot change Times. Parameter t_on has to be bigger than t_cycle");
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
  { __Enable = en;
    if(__Enable){ printAction("Enable"); }
    else{ printAction("Disable"); }
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

unsigned long Actuator::getTimeOnAll(
    uint8_t type,
    uint8_t floor
  ){
    for(int i=0; i < __TotalActuators; i++){
      if(ptr[i]->isType(type) && ptr[i]->isFloor(floor)){
        return ptr[i]->getTimeOn();
      }
    }
  }

unsigned long Actuator::getCycleTimeAll(
    uint8_t type,
    uint8_t floor
  ){
    for(int i=0; i < __TotalActuators; i++){
      if(ptr[i]->isType(type) && ptr[i]->isFloor(floor)){
        return ptr[i]->getCycleTime();
      }
    }
  }

unsigned long Actuator::getTimeAll(
    uint8_t type,
    uint8_t floor
  ){
    for(int i=0; i < __TotalActuators; i++){
      if(ptr[i]->isType(type) && ptr[i]->isFloor(floor)){
        return ptr[i]->getTime();
      }
    }
  }

void Actuator::enableAll(
    uint8_t type,
    uint8_t floor,
    bool en
  ){
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
bool solenoidValve::__Enable_group = LOW;
String solenoidValve::__Group = "Irrigation";
byte solenoidValve::__TotalActuators = 0;
byte solenoidValve::__ActualNumber = 0;
unsigned long solenoidValve::__CycleTime = 600000;
unsigned long solenoidValve::__ActionTime = 0;
float solenoidValve::__K = 6.781; // flowSensor constant
solenoidValve *solenoidValve::ptr[MAX_ACTUATORS];
volatile long solenoidValve::__NumPulses = 0;
float solenoidValve::__H2Ow = 0;

void solenoidValve::countPulses()
  {  __NumPulses++; }

void solenoidValve::flowSensorBegin()
  { pinMode(flowSensor,INPUT);
    attachInterrupt(digitalPinToInterrupt(flowSensor), countPulses, RISING);
  }

solenoidValve::solenoidValve(String name) // Constructor
   {  // Just the first time init arrays
      if(__TotalActuators<1){
        __Enable_group = HIGH;
        for (int i=0; i < MAX_ACTUATORS; i++) { ptr[i] = NULL; } // All Pointers NULL
      }
     __State = LOW ; // Off
     __Enable = HIGH ;; // Enable
     __Floor = 0 ; // Default floor
     __Region = 0; // Default region
     __TimeOn = 10 ; // Default time
     set_time() ;
     __Name = name ;
     __H2OVolume = 0;

     ptr[__TotalActuators] = this; // Set Static pointer to object
     __Number = __TotalActuators++; // Add the new actuator to the total
   }

void solenoidValve::set_time()
  { if(__State == HIGH)
      { resetTime(); }
    else
      { __Actual_time = millis() - __TimeOn; }
  }

void solenoidValve::resetTime()
  {  __Actual_time = millis(); }

bool solenoidValve::changeOrderNumber(byte new_number)
  { if(new_number<MAX_ACTUATORS){
      __Number = new_number;
      return true ; // Succesful
    }
    else {return false;} // Not enough space
  }

unsigned long solenoidValve::getTime()
  { return (millis() - __Actual_time) ; }

bool solenoidValve::setTimeOn ( unsigned long t_on)
  { Serial.print(F("Solenoid Valve ")); Serial.print(__Name); Serial.print(F(": ")) ;
    if(__ActionTime - __TimeOn + t_on < __CycleTime){
      float seconds = float(t_on)/1000;
      __ActionTime -= __TimeOn ;
      __TimeOn = t_on ;
      __ActionTime += __TimeOn ;
      Serial.print(F("Time On changed to "));
      Serial.print(seconds); Serial.println(F(" seconds"));
      return true ; // Succesful
    }
    else{ // Not enough time
      float seconds = float(__CycleTime)/1000;
      Serial.print(F("Time On cannot be changed. Total solenoids turn-on time has to be smaller than "));
      Serial.print(seconds); Serial.println(F(" seconds"));
      return false ;
    }
  }

bool solenoidValve::setCycleTime ( unsigned long t_cycle)
  { Serial.print(F("Solenoid Group ")); Serial.print(__Group); Serial.print(F(": ")) ;
    if( __ActionTime < t_cycle ){
      float minute = float(t_cycle)/60000;
      __CycleTime = t_cycle ;
      Serial.print(F("Cycle Time changed to "));
      Serial.print(minute); Serial.println(F(" minutes"));
      return true ; // Succesful
    }
    else{ // Not enough time
      float minute = float(__ActionTime)/60000;
      Serial.print(F("Cycle Time cannot be changed, it has to be greater than "));
      Serial.print(minute); Serial.println(F(" minutes"));
      return false ;
    }
  }

unsigned long solenoidValve::getTimeOn()
  { return __TimeOn; }

unsigned long solenoidValve::getCycleTime()
  { return __CycleTime; }

bool solenoidValve::getState()
  {  return (__State) ;    }

void solenoidValve::changeState()
  { __State = !__State;
    if( __State == HIGH ){Serial.print(F("Solenoid Valve ")); Serial.print(__Name); Serial.println(F(": Turn On")) ;}
    else{  Serial.print(F("Solenoid Valve ")); Serial.print(__Name); Serial.println(F(": Turn Off")) ;}
  }

void solenoidValve::turnOn( )
  { __State = HIGH;
    Serial.print(F("Solenoid Valve ")); Serial.print(__Name); Serial.println(F(": Turn On")) ;
  }

void solenoidValve::turnOff()
  { __State = LOW;
    Serial.print(F("Solenoid Valve ")); Serial.print(__Name); Serial.println(F(": Turn Off")) ;
  }

void solenoidValve::changeState(bool rst_time)
  { __State = !__State;
    if( __State == HIGH ){Serial.print(F("Solenoid Valve ")); Serial.print(__Name); Serial.println(F(": Turn On")) ;}
    else{  Serial.print(F("Solenoid Valve ")); Serial.print(__Name); Serial.println(F(": Turn Off")) ;}
    if(rst_time){set_time();}
  }

void solenoidValve::turnOn(bool rst_time)
  { __State = HIGH;
    Serial.print(F("Solenoid Valve ")); Serial.print(__Name); Serial.println(F(": Turn On")) ;
    if(rst_time){set_time(); }
  }

void solenoidValve::turnOff(bool rst_time)
  { __State = LOW;
    Serial.print(F("Solenoid Valve ")); Serial.print(__Name); Serial.println(F(": Turn Off")) ;
    if(rst_time){set_time(); }
  }

bool solenoidValve::reOrder(byte number)
  { bool finished;
    Serial.print(F("Solenoid Valve ")); Serial.print(__Name); Serial.print(F(": ")) ;
    if(number==__Number){ finished = true; }

    else if(number<__TotalActuators){
      for(int i = 0; i<__TotalActuators; i++){ // Redefine group loop
        if(ptr[i]->getOrder()==number){
          if( ptr[i]->changeOrderNumber(__Number) ){
            __Number = number ;
            finished = true ;
          }
          else{ finished = false ; } // Imposible to change the order of the group
        }
      }

    }
    else{finished = false; } // Incorrect number proposition

    if(finished){ Serial.print(F("Reorder Succesful. New Number = ")); Serial.println(__Number);}
    else{
      Serial.print(F("Reorder Failed. Number order has to be smaller than "));
      Serial.println(__TotalActuators);
    }
    return finished;
  }

byte solenoidValve::getOrder()
  {  return __Number ; }

byte solenoidValve::getActualNumber()
  { return (__ActualNumber) ; }

void solenoidValve::enable (bool en)
  { __Enable = en ;
    Serial.print(F("Solenoid Valve ")); Serial.print(__Name); Serial.print(F(": ")) ;
    if(__Enable){Serial.println(F("Enabled"));}
    else{Serial.println(F("Disabled"));}
  }

bool solenoidValve::isEnable()
  {  return (__Enable) ; }

void solenoidValve::enableGroup (bool en)
  { __Enable_group = en ;
    Serial.print(F("Solenoid Group: "));
    if(__Enable_group){
      Serial.println(F("Enabled"));
      ptr[__ActualNumber]->getWasteH2O(false);
    }
    else{
      Serial.println(F("Disabled"));
      ptr[__ActualNumber]->getConsumptionH2O();
      }
  }

bool solenoidValve::isEnableGroup()
  {  return (__Enable_group) ; }

byte solenoidValve::getFloor()
  { return __Floor ; }

byte solenoidValve::getRegion()
  { return __Region ; }

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

bool solenoidValve::reOrder_byFloor(byte fl)
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
    Serial.print(F("Solenoid Group ")); Serial.print(__Group);
    if(count==MAX_IRRIGATION_REGIONS){
      Serial.println(F(": re-order by Floor succesful")) ;
      return true;
    }
    else{
      Serial.println(F(": re-order by Floor unsuccesful")) ;
      return false;}
  }

bool solenoidValve::defaultOrder(byte night_floor)
  { int count = 0;

    for(int i=0; i<MAX_IRRIGATION_REGIONS; i++){
      for(int j = MAX_FLOOR-1; j>=0; j--){
        for(int k = 0; k<__TotalActuators; k++){
          if(j==night_floor){break;}
          if(ptr[k]->getFloor()==j && ptr[k]->getRegion()==i){
            if(i<MAX_IRRIGATION_REGIONS/2){
              if(ptr[k]->reOrder(count+(MAX_FLOOR-1)*(i+1))){
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
      Serial.print(F("Solenoid Group "));
      Serial.print(__Group); Serial.println(F(": default re-order succesful")) ;
      return true;
    }
    else{
      Serial.print(F("Solenoid Group "));
      Serial.print(__Group); Serial.println(F(": default re-order unsuccesful")) ;
      return false;
    }
  }

void solenoidValve::getWasteH2O(bool print = HIGH)
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
    __H2OVolume= newVolume +__H2OVolume;
    __NumPulses = 0;
    __ActualNumber++;
    interrupts();   // Enable Interrupts
    Serial.print(F("Solenoid Valve ")); Serial.print(__Name); Serial.print(F(": Water Consumption = ")) ;
    Serial.print(newVolume);  Serial.println(F(" liters")) ;
  }

void solenoidValve::restartH2O()
  { Serial.print(F("Solenoid Valve ")); Serial.print(__Name);
    Serial.println(F(": Water Volume Restarted")) ;
    __H2OVolume = 0;
  }

void solenoidValve:: restartAllH2O()
  { Serial.println(F("Restarting all water parameters saved"));
    __H2Ow = 0;
    Serial.println(F("Waste Water Volume Restarted"));
    for(int i = 0; i<__TotalActuators; i++){ptr[i]->restartH2O();}
  }

float solenoidValve::getH2O()
  { return __H2OVolume; }

void solenoidValve::printH2O()
  { Serial.print(F("Solenoid Valve ")); Serial.print(__Name);
    Serial.print(F(": Water Volume = ")); Serial.print(__H2OVolume);
    Serial.println(F(" liters"));
  }

void solenoidValve::printAllH2O(bool printAll = HIGH)
  { Serial.print(F("Solenoid Group ")); Serial.print(__Group);
    Serial.print(F(": Wasted Water Volume = ")); Serial.print(__H2Ow);
    Serial.println(F(" liters"));
    if(printAll){for(int i = 0; i<__TotalActuators; i++){ptr[i]->printH2O();}}
  }

String solenoidValve::getName()
  { return __Name; }

byte solenoidValve::begin( byte fl, byte reg, unsigned long t_on, unsigned long cycleTime )
  { Serial.print(F("Solenoid Valve ")); Serial.print(__Name); Serial.print(F(": ")) ;
    if( __ActionTime + t_on*1000UL < cycleTime*1000UL){
      if(fl<MAX_FLOOR){
        if(reg<MAX_IRRIGATION_REGIONS){
          __State = LOW ; // Off
          __Floor = fl ; // Select floor
          __Region = reg ; // Select region
          __TimeOn = t_on*1000UL ;
          set_time() ;

          __ActualNumber = 0 ; // Restart count with each init
          __Enable = HIGH ; // Enable HIGH
          __CycleTime = cycleTime*1000UL;
          __ActionTime += __TimeOn ; // Add action time to each group

          Serial.println(F("Started correctly"));
          return 0; // Succesful
        }
        else{
          Serial.println(F("Region value incorrectly configured"));
          return 1; // Region outside of range [0-MAX_IRRIGATION_REGIONS-1]
        }
      }
      else{
        Serial.println(F("Floor value incorrectly configured"));
        return 2; // Floor outside of range [0-MAX_FLOOR-1]
      }
    }
    else{
      Serial.println(F("ON/OFF times incorrectly configured"));
      return 3; // Not enough time
    }

  }

bool solenoidValve::run()
  { if(__Enable_group == true){
      if(__Enable == true){
        if(__ActualNumber == __Number){
          if(__State == LOW){
            if(__Number == 0){getWasteH2O();}
            turnOn(true);
          }
          else if( (millis()-__Actual_time)>=__TimeOn && __State==HIGH){
            turnOff();
            getConsumptionH2O();
          }
        }
        else if(__ActualNumber>=__TotalActuators && (millis()-__Actual_time)>=__CycleTime){
          __ActualNumber = 0 ;
          Serial.print(F("Solenoid Group ")); Serial.print(__Group); Serial.println(F(": Restarting cycle")) ;
        }
        return true;
     }
     else{
       if(__ActualNumber == __Number){
           if(__Number == 0){getWasteH2O();}
           if(__State==HIGH){turnOff();}
           set_time();
           __ActualNumber++;
       }
       else if(__ActualNumber>=__TotalActuators && (millis()-__Actual_time)>=__CycleTime){
         __ActualNumber = 0 ;
         Serial.print(F("Solenoid Group ")); Serial.print(__Group); Serial.println(F(": Restarting cycle")) ;
       }
       __State = LOW;
       return false;
     }
    }
    else{
      if(__State==HIGH){turnOff();}
      return false;
    }

  }

  void solenoidValve::runAll()
    {
      for (int i = 0; i < __TotalActuators; i++){
        (ptr[i]->run());
      }
    }
