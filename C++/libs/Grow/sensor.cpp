// sensor.cpp
//
// Copyright (C) 2019 Grow

#include "sensor.h"

/***   analogSensor   ***/
// Statics variables definitions
//

analogSensor::analogSensor( byte pin, String name ) // Constructor
  {  __Pin = pin ;
     __Name = name ;
    // Default parameters
     __Degree = 0 ;
     __A = 0 ; __B = 0 ; __C=0 ;
     __Value = 0 ;
     __PreValue = __Value ;
     __Filter = 0 ;
     __Alpha = 0 ; __Noise = 0 ; __Err = 0 ;
  }

float analogSensor::model(float val)
  { switch(__Degree){
      case 0:
        return val;
        break;
      case 1:
        return (__A+__B*val);
        break;
      case 2:
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
  { if( __Err==0 || isnan(t) || isnan(t_1) ){
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

void analogSensor::begin()
  { pinMode( __Pin, INPUT); }

bool analogSensor::setModel( byte degree, float a = 0, float b = 0, float c = 0)
  { if(degree==0){
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
        return true;
      }
      else{return false;}
    }
  }

void analogSensor::notFilter()
  { __Filter = 0 ; __Alpha = 0; __Noise = 0; __Err = 0; }

void analogSensor::defaultFilter()
  { __Filter = 2 ; __Noise = 0.5 ; __Err = 1 ;}

bool analogSensor::setExponentialFilter(float alpha = 0.5)
  { if(alpha>0 && alpha<1){
      __Filter = 1; __Alpha = alpha;
      return true;
    }
    else{return false;}
  }

bool analogSensor::setKalmanFilter(float noise)
  { if(noise!=0){
      __Filter = 2;
      __Noise = noise; // Enviromental Noise
      __Err = 1; // Initial Error
      return true;
    }
    else{return false;}
  }

float analogSensor::getValue()
  { return __Value; }

float analogSensor::getPreValue()
  { return __PreValue; }

void analogSensor::printModel()
  { Serial.print( __Name); Serial.print(F(" Sensor: "));
    if(__Degree == 0 ){
      Serial.println(F("You are not using any model yet..."));
    }
    else{
      Serial.print(F("Y = ")); Serial.print(__A);
      Serial.print(F(" + ")); Serial.print(__B);
      Serial.print(F("*x + ")); Serial.print(__C); Serial.println(F("*x^2"));
    }
  }

void analogSensor::printFilter()
  { Serial.print( __Name); Serial.print(F(" Sensor: "));
    switch(__Filter){
      case 0:
      Serial.println(F("You are not using any filter..."));
      break;
    case 1:
      Serial.print(F("You are using an exponencial filter with Alpha = "));
      Serial.println(__Alpha);
      break;
    case 2:
      Serial.print(F("You are using a Kalman filter with Noise = "));
      Serial.print(__Noise); Serial.print(F(" and Actual Error = "));
      Serial.println(__Err);
      break;
    default:
      Serial.println(F("Filters parameters unknowns"));
    }
  }

float analogSensor::read()
  { float val = analogRead(__Pin) ;
    __PreValue = __Value;
    __Value = filter(model(val));
    return __Value ;
  }


/***   UltraSonic   ***/
// Statics variables definitions
byte UltraSonic::__TotalActuators = 0;
UltraSonic *UltraSonic::ptr[MAX_ULTRASONIC];
unsigned long UltraSonic::__ActualTime = 0;

UltraSonic::UltraSonic( byte pin, String name, int minDist = MIN_SECUTIRY_DISTANCE, int maxDist = MAX_SECUTIRY_DISTANCE ) // Constructor
  {  // Just the first time init pointers
     if(__TotalActuators<1){
       for (int i=0; i < MAX_ULTRASONIC; i++) {
         ptr[i] = NULL; // All Pointers NULL
       }
     }

     __Pin = pin ;
     __State = 0 ;
     __Name = name ;
     __Sonar = new NewPing(__Pin, __Pin, MAX_DISTANCE);

     if(minDist<maxDist){
       __minDist = minDist;
       __maxDist = maxDist;
     }
     else{
       __minDist = MIN_SECUTIRY_DISTANCE;
       __maxDist = MAX_SECUTIRY_DISTANCE;
     }

     // Default parameters
     __Value = 0 ;
     __PreValue = __Value ;
     __Filter = 0 ;
     __Alpha = 0 ; __Noise = 0 ; __Err = 0 ;
     __countState = 0;

     __ActualTime = millis();

     ptr[__TotalActuators] = this; // Set Static pointer to object
     __TotalActuators++; // Add the new actuator to the total

  }

float UltraSonic::filter(float val)
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
  { if( __Err==0 || isnan(t) || isnan(t_1) ){
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

void UltraSonic::begin()
  { defaultFilter() ;}

void UltraSonic::beginAll()
  { for(int i = 0; i<__TotalActuators; i++){
      ptr[i]->begin();
    }
  }

void UltraSonic::notFilter()
  { __Filter = 0 ; __Alpha = 0; __Noise = 0; __Err = 0; }

void UltraSonic::defaultFilter()
  { __Filter = 1 ; __Alpha = 0.2; }

bool UltraSonic::setExponentialFilter(float alpha = 0.2)
  { if(alpha>0 && alpha<1){
      __Filter = 1; __Alpha = alpha;
      return true;
    }
    else{return false;}
  }

bool UltraSonic::setKalmanFilter(float noise)
  { if(noise!=0){
      __Filter = 2;
      __Noise = noise; // Enviromental Noise
      __Err = 1; // Initial Error
      return true;
    }
    else{return false;}
  }

float UltraSonic::getValue()
  { return __Value; }

float UltraSonic::getPreValue()
  { return __PreValue; }

byte UltraSonic::getState()
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

void UltraSonic::printFilter()
  { Serial.print( __Name); Serial.print(F(" Sensor: "));
    switch(__Filter){
      case 0:
      Serial.println(F("You are not using any filter..."));
      break;
    case 1:
      Serial.print(F("You are using an exponencial filter with Alpha = "));
      Serial.println(__Alpha);
      break;
    case 2:
      Serial.print(F("You are using a Kalman filter with Noise = "));
      Serial.print(__Noise); Serial.print(F(" and Actual Error = "));
      Serial.println(__Err);
      break;
    default:
      Serial.println(F("Filters parameters unknowns"));
    }
  }

void UltraSonic::readAndPrint()
  { for(int i = 0; i<__TotalActuators; i++){
      byte state = ptr[i]->run();
      Serial.print( ptr[i]->__Name); Serial.print(F(" Sensor: "));
      switch(state){
        case 0:
          Serial.println(ptr[i]->__Value);
          break;
        case 1:
          Serial.println(ptr[i]->__Value);
          break;
        case 2:
          Serial.println(ptr[i]->__Value);
          break;
      }
    }
  }

float UltraSonic::read()
  { __PreValue = __Value;
    __Value = filter(__Sonar->ping_cm());
    return __Value ;
  }

byte UltraSonic::run()
  { float val = read();

    if(val < __minDist){
      if(__countState<-10){
          __State = 2; // Water level too high
      }
      else{__countState--;} // Water level getting high
    }

    else if(val > __maxDist){
      if(__countState>10){
        __State = 1;  // Water level too low
      }
      else{__countState++;} // Water level getting low
    }

    else{
      __countState = 0;
      __State =  0; // Water in range}
    }
    return __State;
  }

void UltraSonic::runAll()
  { if(millis()-__ActualTime>100){
      __ActualTime = millis();
      for(int i = 0; i<__TotalActuators; i++){
          byte state = ptr[i]->run();
      }
    }
  }
