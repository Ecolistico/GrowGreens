// sensor.cpp
//
// Copyright (C) 2019 GrowGreens

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

float analogSensor::test(float val)
  { __PreValue = __Value;
    __Value = filter(model(val));
    return __Value ;
  }

float analogSensor::read()
  { float val = analogRead(__Pin) ;
    __PreValue = __Value;
    __Value = filter(model(val));
    return __Value ;
  }
