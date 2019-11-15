// sensor.cpp
//
// Copyright (C) 2019 Grow

#include "sensor.h"

/***   analogSensor   ***/
// Statics variables definitions
uint8_t analogSensor::__TotalSensors = 0;
analogSensor *analogSensor::ptr[MAX_ANALOG_SENSOR];
unsigned long analogSensor::__ActualTime = 0;

analogSensor::analogSensor(uint8_t pin, String name) // Constructor
  { // Just the first time init pointers
    if(__TotalSensors<1){
      for (int i=0; i < MAX_ANALOG_SENSOR; i++) {
        ptr[i] = NULL; // All Pointers NULL
      }
    }

     __Pin = pin;
     __Name = name;
     // Default parameters
     __FirstRead = false;
     __Degree = 0;
     __A = 0; __B = 0; __C=0;
     __Value = 0;
     __PreValue = __Value;
     __Filter = 0;
     __Alpha = 0; __Noise = 0; __Err = 0;

     ptr[__TotalSensors] = this; // Set Static pointer to object
     __TotalSensors++; // Add the new sensor to the total
  }

float analogSensor::model(float val)
  { switch(__Degree){
      case 0:
        return val;
        break;
      case 1:
        return (__A+val);
        break;
      case 2:
        return (__A+__B*val);
        break;
      case 3:
        return (__A+__B*val+__C*val*val);
        break;
      default:
        return val;
        break;
    }
  }

float analogSensor::filter(float val)
  {  switch(__Filter){
      case 0:
        return val;
        break;
      case 1:
        return exponential_filter(__Alpha, val, __PreValue);
        break;
      case 2:
        return kalman_filter(val, __PreValue);
        break;
      default:
        return val;
        break;
    }
  }

float analogSensor::exponential_filter(float alpha, float t, float t_1)
  { if(isnan(alpha) || isnan(t) || isnan(t_1)){
      return t;
    }
    else{
      return (t*alpha+(1-alpha)*t_1);
    }
  }

float analogSensor::kalman_filter(float t, float t_1)
  { if(__Err==0 || isnan(t) || isnan(t_1) ){
      if(__Err==0){
        __Err = 0.1;
      }
      return t;
    }
    else{
      float kalman_gain = __Err/(__Err+__Noise);
      __Err = (1-kalman_gain)*__Err;
      return (t_1+kalman_gain*(t-t_1));
    }
  }

void analogSensor::printModel()
  { Serial.print(__Name); Serial.print(F(" Sensor: "));
    if(__Degree == 0 ){
      Serial.println(F("You are not using any model yet..."));
    }
    else{
      Serial.print(F("Y=")); Serial.print(__A);
      Serial.print(F("+")); Serial.print(__B);
      Serial.print(F("*x+")); Serial.print(__C); Serial.println(F("*x^2"));
    }
  }

void analogSensor::printFilter()
  { Serial.print(__Name); Serial.print(F(" Sensor: "));
    switch(__Filter){
      case 0:
      Serial.println(F("You are not using any filter..."));
      break;
    case 1:
      Serial.print(F("You are using an exponencial filter with Alpha="));
      Serial.println(__Alpha);
      break;
    case 2:
      Serial.print(F("You are using a Kalman filter with Noise="));
      Serial.print(__Noise); Serial.print(F(" and Actual Error="));
      Serial.println(__Err);
      break;
    default:
      Serial.println(F("Filters parameters unknowns"));
    }
  }

void analogSensor::begin()
  { pinMode(__Pin, INPUT); }

void analogSensor::read()
  { float val = analogRead(__Pin);
    if(__FirstRead){
      __PreValue = __Value;
      __Value = filter(model(val));
    }
    else{
      __FirstRead = true;
      __PreValue = model(val);
      __Value = __PreValue;
      Serial.print(String(__Name));
      Serial.print(F("Sensor: Initial value="));
      Serial.println(__Value);
    }
  }

bool analogSensor::setModel(
  uint8_t degree,
  float a = 0,
  float b = 0,
  float c = 0
){ if(degree==0){
      __Degree = degree; __A = 0; __B = 0; __C = 0;
      return true;
    }
    else{
      int count = 0;
      if(a!=0){count++;}
      if(b!=0){count++;}
      if(c!=0){count++;}
      if(degree<=MAX_EQUATION_DEGREE && count>0 && count-1<=degree){
        __Degree = degree; __A = a; __B = b; __C = c;
        printModel();
        return true;
      }
      else{return false;}
    }
  }

void analogSensor::notFilter()
  { __Filter = 0;
    __Alpha = 0;
    __Noise = 0;
    __Err = 0;
    printFilter();
  }

void analogSensor::defaultFilter()
  { /*
    // Default Kalman
    __Filter = 2;
    __Noise = 0.5;
    __Err = 1;
    */
    // Default exponential
    __Filter = 1;
    __Alpha = 0.25;
    printFilter();
  }

bool analogSensor::setExponentialFilter(float alpha = 0.5)
  { if(alpha>0 && alpha<1){
      __Filter = 1; __Alpha = alpha;
      printFilter();
      return true;
    }
    else{return false;}
  }

bool analogSensor::setKalmanFilter(float noise)
  { if(noise>0){
      __Filter = 2;
      __Noise = noise; // Enviromental Noise
      __Err = 1; // Initial Error
      printFilter();
      return true;
    }
    else{return false;}
  }

float analogSensor::getValue()
  { return __Value; }

float analogSensor::getPreValue()
  { return __PreValue; }

void analogSensor::beginAll()
  { for(int i = 0; i<__TotalSensors; i++){
      ptr[i]->begin();
    }
    __ActualTime = millis();
  }

void analogSensor::readAll()
  { if(millis()-__ActualTime>100){
      __ActualTime = millis();
      for(int i = 0; i<__TotalSensors; i++){
        ptr[i]->read();
      }
    }
  }

/***   UltraSonic   ***/
// Statics variables definitions
uint8_t UltraSonic::__TotalSensors = 0;
UltraSonic *UltraSonic::ptr[MAX_ULTRASONIC];
unsigned long UltraSonic::__ActualTime = 0;

UltraSonic::UltraSonic( // Constructor
  uint8_t pin1,
  uint8_t pin2,
  String name,
  int minDist = MIN_SECUTIRY_DISTANCE,
  int maxDist = MAX_SECUTIRY_DISTANCE
){  // Just the first time init pointers
     if(__TotalSensors<1){
       for (int i=0; i < MAX_ULTRASONIC; i++) {
         ptr[i] = NULL; // All Pointers NULL
       }
     }

     __Pin1 = pin1;
     __Pin2 = pin2;
     __Name = name;
     __Sonar = new NewPing(__Pin1, __Pin2, MAX_DISTANCE);

     if(minDist<maxDist){
       __minDist = minDist;
       __maxDist = maxDist;
     }
     else{
       __minDist = MIN_SECUTIRY_DISTANCE;
       __maxDist = MAX_SECUTIRY_DISTANCE;
     }

     // Default parameters
     __State = 0;
     __FirstRead = false;
     __Distance = 0;
     __PreDistance = 0;
     __Model = 0;
     __Param = 0;
     __Height = 0;
     __Filter = 0;
     __Alpha = 0; __Noise = 0; __Err = 0;
     __countState = 0;

     ptr[__TotalSensors] = this; // Set Static pointer to object
     __TotalSensors++; // Add the new sensor to the total

  }

float UltraSonic::model(float val)
  { switch(__Model){
      case 0: // Not Model
        return val;
        break;
      case 1: // Cilinder
        return cilinder(val);
        break;
      case 2: // Resctangular Prism
        return rectangularPrism(val);
        break;
      default:
        return val;
        break;
    }
  }

float UltraSonic::cilinder(float val)
  { float vol = (PI*__Param*__Param)*(val)/1000; // Volumen in liters
    if(vol<0) {return vol;}
    return vol;
  }

float UltraSonic::rectangularPrism(float val)
  { float vol = (__Param)*(val)/1000; // Volumen in liters
    if(vol<0) {return vol;}
    return vol;
  }

float UltraSonic::filter(float val)
  {  switch(__Filter){
      case 0:
        return val;
        break;
      case 1:
        return exponential_filter(__Alpha, val, __PreDistance);
        break;
      case 2:
        return kalman_filter(val, __PreDistance);
        break;
      default:
        return val;
        break;
    }
  }

float UltraSonic::exponential_filter(float alpha, float t, float t_1)
  { if(isnan(alpha) || isnan(t) || isnan(t_1)){
      return t;
    }
    else{
      return (t*alpha+(1-alpha)*t_1);
    }
  }

float UltraSonic::kalman_filter(float t, float t_1)
  { if(__Err==0 || isnan(t) || isnan(t_1) ){
      if(__Err==0){
        __Err = 0.1;
      }
      return t;
    }
    else{
      float kalman_gain = __Err/(__Err+__Noise);
      __Err = (1-kalman_gain)*__Err;
      return (t_1+kalman_gain*(t-t_1));
    }
  }

float UltraSonic::read()
  { if(__FirstRead){
      __PreDistance = __Distance;
      __Distance = filter(__Sonar->ping_cm());
    }
    else{
      __PreDistance = __Sonar->ping_cm();
      __Distance = __PreDistance;
    }
    return __Distance;
  }

void UltraSonic::printModel()
  { Serial.print(__Name); Serial.print(F(" Sensor: "));
    if(__Model == 0){
      Serial.println(F("You are not using any model yet..."));
    }
    else if(__Model == 1){
      Serial.print(F("You are using a cilinder with radio="));
      Serial.print(__Param);
      Serial.print(F("cm and height="));
      Serial.print(__Height);
      Serial.println(F("cm"));
    }
    else if(__Model == 2){
      Serial.print(F("You are using a rectangular prism with area="));
      Serial.print(__Param);
      Serial.println(F("cm2 and height="));
      Serial.print(__Height);
      Serial.println(F("cm"));
    }
  }

void UltraSonic::printFilter()
  { Serial.print(__Name); Serial.print(F(" Sensor: "));
    switch(__Filter){
      case 0:
      Serial.println(F("You are not using any filter..."));
      break;
    case 1:
      Serial.print(F("You are using an exponencial filter with Alpha="));
      Serial.println(__Alpha);
      break;
    case 2:
      Serial.print(F("You are using a Kalman filter with Noise="));
      Serial.print(__Noise); Serial.print(F(" and Actual Error="));
      Serial.println(__Err);
      break;
    default:
      Serial.println(F("Filters parameters unknowns"));
    }
  }

uint8_t UltraSonic::updateState()
  { float val = read();

    if(val < __minDist){
      if(__countState<-10 || !__FirstRead){
          __State = 2; // Water level too high
      }
      else{__countState--; } // Water level getting high
    }

    else if(val > __maxDist){
      if(__countState>10 || !__FirstRead){
        __State = 1;  // Water level too low
      }
      else{__countState++; } // Water level getting low
    }

    else{
      __countState = 0;
      __State =  0; // Water in range}
    }

    if(!__FirstRead){
      __FirstRead = true;
      Serial.print(String(__Name));
      Serial.print(F("Sensor: Initial value="));
      Serial.print(model(__Height-__Distance));
      Serial.println(F("liters"));
    }
    return __State;
  }

bool UltraSonic::setModel(uint8_t model, float param, float height)
  { if(model>=0 && model<=2 && param>0 && height>=__maxDist){
      __Model = model;
      __Param = param;
      __Height = height;
      printModel();
      return true;
    }
    else{return false;}
  }

void UltraSonic::notFilter()
  { __Filter = 0;
    __Alpha = 0;
    __Noise = 0;
    __Err = 0;
    printFilter();
  }

void UltraSonic::defaultFilter()
  { __Filter = 1;
    __Alpha = 0.25;
    printFilter();
  }

bool UltraSonic::setExponentialFilter(float alpha = 0.2)
  { if(alpha>0 && alpha<1){
      __Filter = 1; __Alpha = alpha;
      printFilter();
      return true;
    }
    else{return false;}
  }

bool UltraSonic::setKalmanFilter(float noise)
  { if(noise>0){
      __Filter = 2;
      __Noise = noise; // Enviromental Noise
      __Err = 1; // Initial Error
      printFilter();
      return true;
    }
    else{return false;}
  }

int UltraSonic::getMinDist()
  { return __minDist;}

int UltraSonic::getMaxDist()
  { return __maxDist;}

float UltraSonic::getDistance()
  { return __Distance; }

float UltraSonic::getPreDistance()
  { return __PreDistance; }

float UltraSonic::getVolume()
  { return model(__Height-__Distance);}

float UltraSonic::getMaxVolume()
  { return model(__Height);}

uint8_t UltraSonic::getState()
  { return __State; }

bool UltraSonic::changeMinDist(int minDist)
  { if(minDist<__maxDist && minDist>=MIN_SECUTIRY_DISTANCE){
      __minDist = minDist;
      return true;
    }
    else{return false;}
  }

bool UltraSonic::changeMaxDist(int maxDist)
  { if(maxDist>__minDist && maxDist<=MAX_SECUTIRY_DISTANCE){
      __maxDist = maxDist;
      return true;
    }
    else{return false;}
  }

void UltraSonic::readAndPrint()
  { for(int i = 0; i<__TotalSensors; i++){
      uint8_t state = ptr[i]->updateState();
      Serial.print(ptr[i]->__Name); Serial.print(F(" Sensor: "));
      Serial.println(ptr[i]->__Distance);
    }
  }

void UltraSonic::begin()
  { __ActualTime = millis(); }

void UltraSonic::readAll()
  { if(millis()-__ActualTime>100){
      __ActualTime = millis();
      for(int i = 0; i<__TotalSensors; i++){
          uint8_t state = ptr[i]->updateState();
      }
    }
  }

/***   waterSensor   ***/
// Statics variables definitions
uint8_t waterSensor::__TotalSensors = 0;
waterSensor *waterSensor::ptr[MAX_WATER_SENSOR];
unsigned long waterSensor::__ActualTime = 0;

waterSensor::waterSensor(uint8_t pin, String name) // Constructor
  { // Just the first time init pointers
    if(__TotalSensors<1){
      for (int i=0; i < MAX_WATER_SENSOR; i++) {
        ptr[i] = NULL; // All Pointers NULL
      }
    }
     __Pin = pin;
     __Name = name;
     // Default parameters
     __FirstRead = false;
     __State = LOW;
     __countState = 0;

     ptr[__TotalSensors] = this; // Set Static pointer to object
     __TotalSensors++; // Add the new sensor to the total
  }

void waterSensor::begin()
  { pinMode(__Pin, INPUT); }

void waterSensor::read()
  { bool state = digitalRead(__Pin);

    if(state=!__State && (__countState>=10 || !__FirstRead) ){
      __State = state;
      __countState = 0;
    }
    else if(state=!__State && __countState<10){ __countState++; }
    else{ __countState = 0; }

    if(!__FirstRead){
      __FirstRead = true;
      Serial.print(String(__Name));
      Serial.print(F("Sensor: Initial state = "));
      if(__State==AIR_STATE){Serial.println(F("air in line"));}
      else if(__State==WATER_STATE){Serial.println(F("water in line"));}
    }
  }

bool waterSensor::getState()
  { return __State; }

void waterSensor::beginAll()
  { for(int i = 0; i<__TotalSensors; i++){
      ptr[i]->begin();
    }
    __ActualTime = millis();
  }

void waterSensor::readAll()
  { if(millis()-__ActualTime>100){
      __ActualTime = millis();
      for(int i = 0; i<__TotalSensors; i++){
        ptr[i]->read();
      }
    }
  }
