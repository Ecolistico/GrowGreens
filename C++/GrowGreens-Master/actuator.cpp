// GrowGreens.cpp
//
// Copyright (C) 2019 GrowGreens

#include "actuator.h"


/***   Actuator   ***/
// Statics variables definitions
byte Actuator::__TotalActuators = 0;
Actuator *Actuator::ptr[MAX_ACTUATORS];

Actuator::Actuator( ) // Constructor
   { // Just the first time init pointers
     if(__TotalActuators<1){
       for (int i=0; i < MAX_ACTUATORS; i++) {
         ptr[i] = NULL; // All Pointers NULL
       }
     }
     __State = LOW ;
     __TimeOn = 10 ;
     __TimeOff = 60000 ;
     set_time();
     __Enable = LOW ;

     ptr[__TotalActuators] = this; // Set Static pointer to object
     __TotalActuators++; // Add the new actuator to the total
   }

Actuator::Actuator( unsigned long t_on, unsigned long t_off ) // Constructor
   { // Just the first time init pointers
     if(__TotalActuators<1){
       for (int i=0; i < MAX_ACTUATORS; i++) {
         ptr[i] = NULL; // All Pointers NULL
       }
     }
     __State = LOW ;
     __TimeOn = t_on*1000UL ;
     __TimeOff = t_off*1000UL ;
     set_time();
     __Enable = LOW ;

     ptr[__TotalActuators] = this; // Set Static pointer to object
     __TotalActuators++; // Add the new actuator to the total
   }

Actuator::Actuator( bool _state, bool en, unsigned long t_on, unsigned long t_off ) // Constructor
   {// Just the first time init pointers
    if(__TotalActuators<1){
      for (int i=0; i < MAX_ACTUATORS; i++) {
        ptr[i] = NULL; // All Pointers NULL
      }
    }

    __State = _state ;
    __TimeOn = t_on*1000UL ;
    __TimeOff = t_off*1000UL ;
    set_time();
    __Enable = en ;

    ptr[__TotalActuators] = this; // Set Static pointer to object
    __TotalActuators++; // Add the new actuator to the total
   }

void Actuator::set_time( )
  { if(__State == HIGH)
    { resetTime(); }
    else
    { __Actual_time = millis() - __TimeOn; }
  }

void Actuator::resetTime()
{  __Actual_time = millis() ; }

bool Actuator::getState()
{ return __State; }

void Actuator::changeState()
{ __State = !__State;
  set_time();
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
 { if(t_on*1000UL < __TimeOff){
     __TimeOn = t_on*1000UL;
     return true;
   }
   else{ return false; }
 }

bool Actuator::setTimeOff(unsigned long t_off)
 { if(__TimeOn < t_off*1000UL){
     __TimeOff = t_off*1000UL;
     return true;
   }
   else{ return false; }
 }

bool Actuator::setTime(unsigned long t_on, unsigned long t_off)
 { if(t_on < t_off){
     __TimeOn = t_on*1000UL;
     __TimeOff = t_off*1000UL;
     return true;
   }
   else{return false; }
 }

unsigned long Actuator::getTimeOn()
{ return __TimeOn ; }

unsigned long Actuator::getTimeOff()
{ return __TimeOff ; }

unsigned long Actuator::getTime()
{  return (millis() - __Actual_time) ; }

void Actuator::enable (bool en)
{  __Enable = en;    }

bool Actuator::isEnable()
{  return (__Enable) ;   }

bool Actuator::begin()
{ __Enable = HIGH;
  resetTime();
}

bool Actuator::run()
{  if(__Enable == true){
     if( millis() - __Actual_time < __TimeOn && __State == LOW){
       __State = HIGH;
     }
     else if( millis() - __Actual_time >= __TimeOn && millis() - __Actual_time < __TimeOff && __State == HIGH){
       __State = LOW;
     }
     else if( millis() - __Actual_time >= __TimeOff ){
       resetTime();
     }

     return true;
   }
   else{ return false; }
}

/***   solenoidValve   ***/
// Statics variables definitions
byte solenoidValve::__TotalActuators = 0;
byte solenoidValve::__ActualNumber[MAX_ACTUATORS_GROUPS];
byte solenoidValve::__MaxNumber[MAX_ACTUATORS_GROUPS];
bool solenoidValve::__Enable[MAX_ACTUATORS_GROUPS];
unsigned long solenoidValve::__CycleTime[MAX_ACTUATORS_GROUPS];
unsigned long solenoidValve::__ActionTime[MAX_ACTUATORS_GROUPS];
float solenoidValve::__K = 6.781; // flowSensor constant
solenoidValve *solenoidValve::ptr[MAX_ACTUATORS];
volatile long solenoidValve::__NumPulses = 0;
static float solenoidValve::__H2Ow = 0;

void solenoidValve::countPulses()
  {  __NumPulses++; }

void solenoidValve::flowSensorBegin()
  { pinMode(flowSensor,INPUT);
    attachInterrupt(digitalPinToInterrupt(flowSensor), countPulses, RISING);
  }

solenoidValve::solenoidValve(String name) // Constructor
   {  // Just the first time init arrays
      if(__TotalActuators<1){
        for (int i=0; i < MAX_ACTUATORS_GROUPS; i++) {
         __ActualNumber[i] = 0 ; // All zeros
         __MaxNumber[i] = 0 ; // All zeros
         __Enable[i] = LOW ; // All enable LOW
         __CycleTime[i] = 60000 ; // All Cycles 1 minute
         __ActionTime[i] = 0 ; // All zeros
       }
        for (int i=0; i < MAX_ACTUATORS; i++) {
          ptr[i] = NULL; // All Pointers NULL
        }
      }

     __State = LOW ; // Off
     __Group = 0 ; // Default group
     __Number = 0 ; // Default number
     __Floor = 0 ; // Default floor
     __Region = 0; // Default region
     __TimeOn = 10 ; // Default time
     set_time() ;
     __Name = name ;
     __H2OVolume = 0;

     ptr[__TotalActuators] = this; // Set Static pointer to object
     __TotalActuators++; // Add the new actuator to the total
   }

void solenoidValve::set_time( )
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
    if(__ActionTime[__Group] - __TimeOn + t_on < __CycleTime[__Group]){
      float seconds = float(t_on)/1000;
      __ActionTime[__Group] -= __TimeOn ;
      __TimeOn = t_on ;
      __ActionTime[__Group] += __TimeOn ;
      Serial.print(F("Time On changed to "));
      Serial.print(seconds); Serial.println(F(" seconds"));
      return true ; // Succesful
    }
    else{ // Not enough time
      float seconds = float(__CycleTime[__Group])/1000;
      Serial.print(F("Time On cannot be changed. Total solenoids turn-on time has to be smaller than "));
      Serial.print(seconds); Serial.println(F(" seconds"));
      return false ;
    }
  }

bool solenoidValve::setCycleTime ( unsigned long t_cycle)
  { Serial.print(F("Solenoid Group ")); Serial.print(__Group); Serial.print(F(": ")) ;
    if( __ActionTime[__Group] < t_cycle ){
      float minute = float(t_cycle)/60000;
      __CycleTime[__Group] = t_cycle ;
      Serial.print(F("Cycle Time changed to "));
      Serial.print(minute); Serial.println(F(" minutes"));
      return true ; // Succesful
    }
    else{ // Not enough time
      float minute = float(__ActionTime[__Group])/60000;
      Serial.print(F("Cycle Time cannot be changed, it has to be greater than "));
      Serial.print(minute); Serial.println(F(" minutes"));
      return false ;
    }
  }

bool solenoidValve::getState()
  {  return (__State) ;    }

void solenoidValve::changeState()
  { __State = !__State; }

void solenoidValve::turnOn( )
  { __State = HIGH; }

void solenoidValve::turnOff()
  { __State = LOW; }

void solenoidValve::changeState(bool rst_time)
  { __State = !__State;
    if(rst_time){set_time();}
  }

void solenoidValve::turnOn(bool rst_time)
  { __State = HIGH;
    if(rst_time){set_time(); }
  }

void solenoidValve::turnOff(bool rst_time)
  { __State = LOW;
    if(rst_time){set_time(); }
  }

byte solenoidValve::changeGroup(byte new_gr)
  { if(new_gr==__Group){return 0 ; } // Succesful
    else if(new_gr<MAX_ACTUATORS_GROUPS){
        if( __ActionTime[new_gr] + __TimeOn < __CycleTime[new_gr] ){
            bool finished = LOW;
            byte aux = 1;
            while(finished==LOW){
              for(int i = 0; i<__TotalActuators; i++){ // Redefine group loop
                if(ptr[i]->getGroup()==__Group && ptr[i]->getOrder()==__Number+aux){
                  if( ptr[i]->changeOrderNumber(ptr[i]->getOrder()-1) ){aux++;}
                  else{return 3;} // Imposible to change the order of the group
                }
              }
              if(__Number+aux >= __MaxNumber[__Group]){ // Readjust group condition
                finished = HIGH ;
                __MaxNumber[__Group]--;
                __ActionTime[__Group]-=__TimeOn;
              }
            }

          __Group = new_gr ; // New group
          __Number = __MaxNumber[__Group]++ ; // New number into the group
          __ActionTime[__Group] += __TimeOn ; // Add the action time
          return 0; // Succesful
        }
        else {return 1;} // Not enough time
    }
    else {return 2;} // Not enough space
  }

byte solenoidValve::getGroup()
  {  return __Group ; }

bool solenoidValve::reOrder(byte number)
  { bool finished;
    Serial.print(F("Solenoid Valve ")); Serial.print(__Name); Serial.print(F(": ")) ;
    if(number==__Number){ finished = true; }

    else if(number<__MaxNumber[__Group]){
      for(int i = 0; i<__TotalActuators; i++){ // Redefine group loop
        if(ptr[i]->getGroup()==__Group && ptr[i]->getOrder()==number){
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
      Serial.println(__MaxNumber[__Group]);
    }
    return finished;
  }

byte solenoidValve::getOrder()
  {  return __Number ; }

byte solenoidValve::getActualNumber()
  { return (__ActualNumber[__Group]) ; }

void solenoidValve::enable (bool en)
  { __Enable[__Group] = en ;
    Serial.print(F("Solenoid Group ")); Serial.print(__Group); Serial.print(F(": ")) ;
    if(__Enable[__Group]){Serial.println(F("Enabled"));}
    else{Serial.println(F("Disabled"));}
  }

bool solenoidValve::isEnable()
  {  return (__Enable[__Group]) ; }

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
              if(ptr[i]->getGroup()==__Group && ptr[i]->getFloor()==k && ptr[i]->getRegion()==j ){
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
              if(ptr[i]->getGroup()==__Group && ptr[i]->getFloor()==k && ptr[i]->getRegion()==j ){
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
                if(ptr[i]->getGroup()==__Group && ptr[i]->getFloor()==k && ptr[i]->getRegion()==j ){
                  if(ptr[i]->reOrder(k*MAX_IRRIGATION_REGIONS+j*2)){
                    count++;
                    break;
                  }
                }
              }
              if(j>=MAX_IRRIGATION_REGIONS/2){
                if(ptr[i]->getGroup()==__Group && ptr[i]->getFloor()==k && ptr[i]->getRegion()==j ){
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
                if(ptr[i]->getGroup()==__Group && ptr[i]->getFloor()==k && ptr[i]->getRegion()==j ){
                  if(ptr[i]->reOrder(k*MAX_IRRIGATION_REGIONS+7-j*2)){
                    count++;
                    break;
                  }
                }
              }
              if(j>=MAX_IRRIGATION_REGIONS/2){
                if(ptr[i]->getGroup()==__Group && ptr[i]->getFloor()==k && ptr[i]->getRegion()==j ){
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

    if(count==__MaxNumber[__Group]){return true;}
    else{return false;} // Something is wrong
  }

bool solenoidValve::reOrder_byFloor(byte fl)
  { int count = 0;
    for(int i = 0; i<__TotalActuators; i++){
      if(ptr[i]->getGroup()==__Group && ptr[i]->getFloor()==fl){
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
    Serial.print(F(": Water Volume = ")); Serial.println(__H2OVolume);
  }

void solenoidValve::printAllH2O()
  { Serial.print(F("Solenoid Group ")); Serial.print(__Group);
    Serial.print(F(": Wasted Water Volume = ")); Serial.println(__H2Ow);
    for(int i = 0; i<__TotalActuators; i++){ptr[i]->printH2O();}
  }

String solenoidValve::getName()
  { return __Name; }

byte solenoidValve::begin( byte gr, byte fl, byte reg, unsigned long t_on, unsigned long cycleTime )
  { Serial.print(F("Solenoid Valve ")); Serial.print(__Name); Serial.print(F(": ")) ;
    if(gr<MAX_ACTUATORS_GROUPS){
      if( __ActionTime[gr] + t_on*1000UL < cycleTime*1000UL){
        if(fl<MAX_FLOOR){
          if(reg<MAX_IRRIGATION_REGIONS){
            __State = LOW ; // Off
            __Group = gr ; // Select group
            __Floor = fl ; // Select floor
            __Region = reg ; // Select region
            __Number = __MaxNumber[__Group]++ ; // Number into the group
            __TimeOn = t_on*1000UL ;
            set_time() ;

            __ActualNumber[__Group] = 0 ; // Restart count with each init
            __Enable[__Group] = HIGH ; // Enable HIGH
            __CycleTime[__Group] = cycleTime*1000UL;
            __ActionTime[__Group] += __TimeOn ; // Add action time to each group

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
    else{
      Serial.println(F("Number of solenoid valves exceeded the memory limit"));
      return 4; // Not enough space
    }
  }

void solenoidValve::run()
  { if(__Enable[__Group] == true){
        if(__ActualNumber[__Group] == __Number){
          if(__State == LOW){
            if(__Number == 0){
              // Get Water Consumption  when no solenoids are active
              noInterrupts(); // Disable Interrupts
              float newVolume = __NumPulses/(60*__K);
              __H2Ow = newVolume + __H2Ow;
              __NumPulses = 0;
              interrupts();   // Enable Interrupts
              printAllH2O();
            }
            turnOn(true);
            Serial.print(F("Solenoid Valve ")); Serial.print(__Name); Serial.println(F(": Turn On")) ;
          }
          else if( (millis()-__Actual_time)>=__TimeOn && __State==HIGH){
            turnOff();
            // Get Water Consumption to that Region
            noInterrupts(); // Disable Interrupts
            float newVolume = __NumPulses/(60*__K);
            __H2OVolume= newVolume +__H2OVolume;
            __NumPulses = 0;
            __ActualNumber[__Group]++;
            interrupts();   // Enable Interrupts
            Serial.print(F("Solenoid Valve ")); Serial.print(__Name); Serial.println(F(": Turn Off")) ;
            Serial.print(F("Solenoid Valve ")); Serial.print(__Name); Serial.print(F(": Water Consumption = ")) ;
            Serial.print(newVolume);  Serial.println(F(" liters")) ;
          }
        }
        else if(__ActualNumber[__Group]>=__MaxNumber[__Group] && (millis()-__Actual_time)>=__CycleTime[__Group]){
          __ActualNumber[__Group] = 0 ;
          Serial.print(F("Solenoid Group ")); Serial.print(__Group); Serial.println(F(": Restarting cycle")) ;
        }
        return true;
     }
     else{
       __State = LOW;
       return false;
     }
  }
