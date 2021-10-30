// sensor.cpp
//
// Copyright (C) 2019 Grow

#include "sensor.h"

/***   analogSens   ***/
analogSens::analogSens(uint8_t pin, uint8_t num) // Constructor
  {  _Pin = pin;
    _number = num;

     _FirstRead = false;
     _Degree = 0;
     _A = 0; _B = 0; _C=0;
     _Value = 0;
     _PreValue = _Value;
     _Filter = 0;
     _Alpha = 0; _Noise = 0; _Err = 0;
  }

float analogSens::model(float val)
  { switch(_Degree){
      case 0:
        return val;
        break;
      case 1:
        return (_A+val);
        break;
      case 2:
        return (_A+_B*val);
        break;
      case 3:
        return (_A+_B*val+_C*val*val);
        break;
      default:
        return val;
        break;
    }
  }

float analogSens::filter(float val)
  {  switch(_Filter){
      case 0:
        return val;
        break;
      case 1:
        return exponential_filter(_Alpha, val, _PreValue);
        break;
      case 2:
        return kalman_filter(val, _PreValue);
        break;
      default:
        return val;
        break;
    }
  }

float analogSens::exponential_filter(float alpha, float t, float t_1)
  { if(isnan(alpha) || isnan(t) || isnan(t_1)){
      return t;
    }
    else{
      return (t*alpha+(1-alpha)*t_1);
    }
  }

float analogSens::kalman_filter(float t, float t_1)
  { if(_Err==0 || isnan(t) || isnan(t_1) ){
      if(_Err==0){
        _Err = 0.1;
      }
      return t;
    }
    else{
      float kalman_gain = _Err/(_Err+_Noise);
      _Err = (1-kalman_gain)*_Err;
      return (t_1+kalman_gain*(t-t_1));
    }
  }

void analogSens::printModel()
  { Serial.print(F("info,Sensor: Analog number "));
    Serial.print(_number);
    if(_Degree == 0 ){
      Serial.println(F(" is not using any model yet..."));
    }
    else{
      Serial.print(F(" Y=")); Serial.print(_A);
      Serial.print(F("+")); Serial.print(_B);
      Serial.print(F("*x+")); Serial.print(_C); Serial.println(F("*x^2"));
    }
  }

void analogSens::printFilter()
  { switch(_Filter){
      case 0:
        Serial.print(F("info,Sensor: Analog number "));
        Serial.print(_number);
        Serial.println(F(" is not using any filter..."));
        break;
      case 1:
        Serial.print(F("info,Sensor: Analog number "));
        Serial.print(_number);
        Serial.print(F(" is using an exponencial filter with Alpha="));
        Serial.println(_Alpha);
        break;
      case 2:
        Serial.print(F("info,Sensor: Analog number "));
        Serial.print(_number);
        Serial.print(F(" is using a Kalman filter with Noise="));
        Serial.print(_Noise); Serial.print(F(" and Actual Error="));
        Serial.println(_Err);
        break;
      default:
        Serial.print(F("error,Sensor: Analog number "));
        Serial.print(_number);
        Serial.println(F(" filters parameters unknowns"));
      }
    }

void analogSens::setModel(float a /*= 0*/, float b /*= 0*/, float c /*= 0*/)
{ uint8_t degree;
  if(a==0 && b==0 && c==0) degree = 0;
  else if(a!=0 && b==0 && c==0) degree = 1;
  else if(b!=0 && c==0) degree = 2;
  else if(c!=0) degree = 3;

  if(degree==0){
    _Degree = degree;
    _A = 0;
    _B = 0;
    _C = 0;
  } else {
      int count = 0;
      if(a!=0) count++;
      if(b!=0) count++;
      if(c!=0) count++;
      if(degree<=MAX_EQUATION_DEGREE && count>0 && count-1<=degree){
        _Degree = degree;
        _A = a;
        _B = b;
        _C = c;
        printModel();
      } else Serial.println(F("error,Sensor: analogSens::setModel( uint8_t degree, float a = 0, float b = 0, float c = 0) parameters provided are wrong"));
    }
  }

void analogSens::notFilter()
  { _Filter = 0;
    _Alpha = 0;
    _Noise = 0;
    _Err = 0;
    printFilter();
  }

void analogSens::defaultFilter()
  { /*
    // Default Kalman
    _Filter = 2;
    _Noise = 0.5;
    _Err = 1;
    */
    // Default exponential
    _Filter = 1;
    _Alpha = 0.25;
    printFilter();
  }

void analogSens::setExponentialFilter(float alpha /*= 0.5*/)
  { if(alpha>0 && alpha<1){
      _Filter = 1;
      _Alpha = alpha;
      printFilter();
    } else Serial.println(F("error,Sensor: analogSens::setExponentialFilter(float alpha = 0.5) parameters provided are wrong"));
  }

void analogSens::setKalmanFilter(float noise)
  { if(noise>0){
      _Filter = 2;
      _Noise = noise; // Enviromental Noise
      _Err = 1; // Initial Error
      printFilter();
    } else Serial.println(F("error,Sensor: analogSens::setKalmanFilter(float noise) parameters provided are wrong"));
  }

void analogSens::setFilter(uint8_t filter, float param)
  { if(filter==0) notFilter();
    else if(filter==1) setExponentialFilter(param);
    else if(filter==2) setKalmanFilter(param);
    else Serial.println(F("error,Sensor: analogSens::setFilter(uint8_t filter, float param) parameters provided are wrong"));
  }

float analogSens::getValue()
  { return _Value; }

float analogSens::getPreValue()
  { return _PreValue; }

void analogSens::begin()
  { pinMode(_Pin, INPUT); }

void analogSens::read()
  { float val = analogRead(_Pin);
    if(_FirstRead){
      _PreValue = _Value;
      _Value = filter(model(val));
    }
    else{
      _FirstRead = true;
      _PreValue = model(val);
      _Value = _PreValue;
      Serial.print(F("info,Sensor: Analog number "));
      Serial.print(_number);
      Serial.print(F(" initial value="));
      Serial.println(_Value);
    }
  }

void analogSens::printRead()
  { Serial.print(F("info,Sensor: Analog number "));
    Serial.print(_number);
    Serial.print(F(" value="));
    Serial.println(_Value);
  }

/*** Flowmeter    ***/
volatile long Flowmeter::_numPulses1 = 0;
volatile long Flowmeter::_numPulses2 = 0;

void Flowmeter::countPulses1()
  { _numPulses1++; }

void Flowmeter::countPulses2()
  { _numPulses2++; }

// Constructor
Flowmeter::Flowmeter(uint8_t pin, uint8_t num, float k)
  { _pin = pin;
    _number = num;
    _K = k;
    _water = 0;
  }

void Flowmeter::begin()
  { bool succeed = true;
    pinMode(_pin, INPUT);
    if(_number==0) attachInterrupt(digitalPinToInterrupt(_pin), countPulses1, RISING);
    else if(_number==1) attachInterrupt(digitalPinToInterrupt(_pin), countPulses2, RISING);
    else if (_number>=MAX_FLOWMETER_SENSOR) succeed = false;
    if(succeed){
      Serial.print(F("info,Sensor: Flometer number "));
      Serial.print(_number);
      Serial.println(F(" started correctly"));
    }
    else Serial.println(F("error,Sensor: Incorrect flowmeter number. It is allowed to control a maximum of 2 sensors."));
  }

void Flowmeter::restartWater()
  { _water = 0; }

float Flowmeter::getWater()
  { return _water; }

void Flowmeter::read()
  { noInterrupts(); // Disable Interrupts
    float newVolume = 0;
    if(_number==0){
      newVolume = _numPulses1/(60*_K);
      _numPulses1 = 0;
    }
    else if(_number==1){
      newVolume = _numPulses2/(60*_K);
      _numPulses2 = 0;
    }
    _water += newVolume;
    interrupts();   // Enable Interrupts
  }

void Flowmeter::printRead()
  { Serial.print(F("info,Sensor: Flowmeter number "));
    Serial.print(_number);
    Serial.print(F(" water= "));
    Serial.print(_water);
    Serial.println(F(" liters"));
  }

/*** ScaleSens    ***/
// Constructor
ScaleSens::ScaleSens(uint8_t pin1, uint8_t pin2, uint8_t num)
  { _pin1 = pin1;
    _pin2 = pin2;
    _number = num;
    _weight = 0;
    _minWeight = 0;
    _maxWeight = 0;
    _en = false;
    _sc = new HX711;
  }

void ScaleSens::begin(long offset, float scale, float min_weight, float max_weight)
  { _sc->begin(_pin1, _pin2); // Data, Clock
    _sc->set_offset(offset); // Set Offset
    _sc->set_scale(scale); // Set Scale
    _minWeight = min_weight; // Set minimun weight to control purposes
    _maxWeight = max_weight; // Set maximun weight to control purposes
    _printTimer = millis();

    if (_sc->is_ready()) {
      Serial.print(F("info,Sensor: Scale number "));
      Serial.print(_number);
      Serial.println(F(" started correctly"));
      _en = true;
    }
    else {
      Serial.print(F("error,Sensor: Scale number "));
      Serial.print(_number);
      Serial.println(F(" not found"));
    }
  }

void ScaleSens::read()
  { if(_en) _weight = _sc->get_units(10);
    else if(millis()-_printTimer>10000){  // Each 10 seconds print a warning
      _printTimer = millis();
      Serial.print(F("error,Sensor: Scale number "));
      Serial.print(_number);
      Serial.println(F(" not found"));
    }
  }

float ScaleSens::getWeight()
  { return _weight; }

float ScaleSens::getMinWeight()
  { float resp = _minWeight + (_sc->get_offset()/_sc->get_scale());
    return resp;
  }

float ScaleSens::getMaxWeight()
  { float resp = _maxWeight + (_sc->get_offset()/_sc->get_scale());
    return resp;
  }

void ScaleSens::setMinWeight(float weight)
  { _minWeight = weight; }

void ScaleSens::setMaxWeight(float weight)
  { _maxWeight = weight; }

void ScaleSens::printVal(String value)
  { Serial.print(F("info,Sensor: Scale number "));
    Serial.print(_number);
    Serial.print(F(" value = "));
    Serial.println(value);
  }

void ScaleSens::printRead()
  { Serial.print(F("info,Sensor: Scale number "));
    Serial.print(_number);
    Serial.print(F(" weight= "));
    Serial.print(_weight);
    Serial.println(F(" kg"));
  }

void ScaleSens::printRead_notScale()
  { double value;
    if(_en) value = _sc->get_value(10); // Get the average of 10 readings
    else value = 0;
    Serial.print(F("info,Sensor: Scale number "));
    Serial.print(_number);
    Serial.print(F(" value (scale@1) = "));
    Serial.println(value);
  }

void ScaleSens::printRead_notOffset()
  { long value;
    if(_en) value = _sc->read_average(10); // Get the average of 10 readings
    else value = 0;
    Serial.print(F("info,Sensor: Scale number "));
    Serial.print(_number);
    Serial.print(F(" value (offset@0) = "));
    Serial.println(value);
  }

/*** SwitchSens    ***/
// Constructor
SwitchSens::SwitchSens(uint8_t pin, uint8_t num, bool logic /* = true */)
  { _pin = pin;
    _number = num;
    _counter = 0;
    _logicInverted = logic;
    _state = LOW;
    _readState = LOW;
  }

void SwitchSens::begin()
  { pinMode(_pin, INPUT_PULLUP); }

void SwitchSens::setLogic(bool logic)
  { _logicInverted = logic; }

bool SwitchSens::getState()
  { if(_logicInverted) return _state; // If logic inverted
    else return !_state;
  }

void SwitchSens::read()
  { _readState = digitalRead(_pin);
    if(_readState!=_state){
      _counter++;
      if(_counter>=2){
        _counter = 0;
        _state = _readState;
      }
    }
  }

void SwitchSens::printRead()
  { Serial.print(F("info,Sensor: Switch number "));
    Serial.print(_number);
    if((_logicInverted && _state) || (!_logicInverted && !_state)) Serial.println(F(" is ON"));
    else Serial.print(F(" is OFF"));
  }

/***   UltraSonic   ***/
// Constructor
UltraSonic::UltraSonic( uint8_t pin1, uint8_t pin2, uint8_t num, int minDist /*= MIN_SECURITY_DISTANCE*/, int maxDist /*= MAX_SECURITY_DISTANCE*/)
{  _Pin1 = pin1;
   _Pin2 = pin2;
   _number = num;
   _Sonar = new NewPing(_Pin1, _Pin2, MAX_DISTANCE);

   if(minDist<maxDist){
     _minDist = minDist;
     _maxDist = maxDist;
   } else{
     _minDist = MIN_SECURITY_DISTANCE;
     _maxDist = MAX_SECURITY_DISTANCE;
   }

   // Default parameters
   _State = 0;
   _FirstRead = false;
   _Distance = 0;
   _PreDistance = 0;
   _Model = 0;
   _Param = 0;
   _Height = 0;
   _Filter = 0;
   _Alpha = 0; _Noise = 0; _Err = 0;
   _countState = 0;

  }

float UltraSonic::model(float val)
  { switch(_Model){
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
  { float vol = (PI*_Param*_Param)*(val)/1000; // Volumen in liters
    if(vol<0) {return vol;}
    return vol;
  }

float UltraSonic::rectangularPrism(float val)
  { float vol = (_Param)*(val)/1000; // Volumen in liters
    if(vol<0) {return vol;}
    return vol;
  }

float UltraSonic::filter(float val)
  {  switch(_Filter){
      case 0:
        return val;
        break;
      case 1:
        return exponential_filter(_Alpha, val, _PreDistance);
        break;
      case 2:
        return kalman_filter(val, _PreDistance);
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
  { if(_Err==0 || isnan(t) || isnan(t_1) ){
      if(_Err==0){
        _Err = 0.1;
      }
      return t;
    }
    else{
      float kalman_gain = _Err/(_Err+_Noise);
      _Err = (1-kalman_gain)*_Err;
      return (t_1+kalman_gain*(t-t_1));
    }
  }

float UltraSonic::read()
  { if(_FirstRead){
      _PreDistance = _Distance;
      _Distance = filter(_Sonar->ping_cm());
    }
    else{
      _PreDistance = _Sonar->ping_cm();
      _Distance = _PreDistance;
    }
    return _Distance;
  }

void UltraSonic::printModel()
  { Serial.print(F("info,Sensor: Ultrasonic number "));
    Serial.print(_number);
    if(_Model == 0){
      Serial.print(F(" is not using any model yet..."));
    }
    else if(_Model == 1){
      Serial.print(F(" is using a cilinder with radio="));
      Serial.print(_Param);
      Serial.print(F("cm and height="));
      Serial.print(_Height);
      Serial.println(F("cm"));
    }
    else if(_Model == 2){
      Serial.print(F(" is using a rectangular prism with area="));
      Serial.print(_Param);
      Serial.print(F("cm2 and height="));
      Serial.print(_Height);
      Serial.println(F("cm"));
    }
  }

void UltraSonic::printFilter()
  { switch(_Filter){
      case 0:
      Serial.print(F("info,Sensor: Ultrasonic number "));
      Serial.print(_number);
      Serial.println(F(" is not using any filter..."));
      break;
    case 1:
      Serial.print(F("info,Sensor: Ultrasonic number "));
      Serial.print(_number);
      Serial.print(F(" is using an exponencial filter with Alpha="));
      Serial.println(_Alpha);
      break;
    case 2:
      Serial.print(F("info,Sensor: Ultrasonic number "));
      Serial.print(_number);
      Serial.print(F(" is using a Kalman filter with Noise="));
      Serial.print(_Noise); Serial.print(F(" and Actual Error="));
      Serial.println(_Err);
      break;
    default:
      Serial.print(F("error,Sensor: Ultrasonic number"));
      Serial.print(_number);
      Serial.println(F(" filters parameters unknowns"));
    }
  }

uint8_t UltraSonic::updateState()
  { float val = read();

    if(val < _minDist){
      if(_countState<-10 || !_FirstRead){
          _State = 2; // Water level too high
      }
      else{_countState--; } // Water level getting high
    }

    else if(val > _maxDist){
      if(_countState>10 || !_FirstRead){
        _State = 1;  // Water level too low
      }
      else{_countState++; } // Water level getting low
    }

    else{
      _countState = 0;
      _State =  0; // Water in range}
    }

    if(!_FirstRead){
      _FirstRead = true;
      Serial.print(F("info,Sensor: Ultrasonic number "));
      Serial.print(_number);
      Serial.print(F(" initial value="));
      Serial.print(model(_Height-_Distance));
      Serial.println(F(" liters"));

    }
    return _State;
  }

void UltraSonic::setModel(uint8_t model, float param, float height)
  { if(model>=0 && model<=2 && param>0 && height>=_maxDist){
      _Model = model;
      _Param = param;
      _Height = height;
      printModel();
    } else Serial.println(F("error,Sensor: UltraSonic::setModel(uint8_t model, float param, float height) parameters provided are wrong"));
  }

void UltraSonic::notFilter()
  { _Filter = 0;
    _Alpha = 0;
    _Noise = 0;
    _Err = 0;
    printFilter();
  }

void UltraSonic::defaultFilter()
  { _Filter = 1;
    _Alpha = 0.25;
    printFilter();
  }

void UltraSonic::setExponentialFilter(float alpha /* = 0.2 */)
  { if(alpha>0 && alpha<1){
      _Filter = 1; _Alpha = alpha;
      printFilter();
    } else Serial.println(F("error,Sensor: UltraSonic::setExponentialFilter(float alpha = 0.2) parameters provided are wrong"));
  }

void UltraSonic::setKalmanFilter(float noise)
  { if(noise>0){
      _Filter = 2;
      _Noise = noise; // Enviromental Noise
      _Err = 1; // Initial Error
      printFilter();
    } else Serial.println(F("error,Sensor: UltraSonic::setKalmanFilter(float noise) parameters provided are wrong"));
  }

int UltraSonic::getMinDist()
  { return _minDist;}

int UltraSonic::getMaxDist()
  { return _maxDist;}

float UltraSonic::getDistance()
  { return _Distance; }

float UltraSonic::getPreDistance()
  { return _PreDistance; }

float UltraSonic::getVolume()
  { return model(_Height-_Distance);}

float UltraSonic::getMinVolume()
  { return model(_Height-_maxDist);}

float UltraSonic::getMaxVolume()
  { return model(_Height-_minDist);}

uint8_t UltraSonic::getState()
  { return _State; }

bool UltraSonic::changeMinDist(int minDist)
  { if(minDist<_maxDist && minDist>=MIN_SECURITY_DISTANCE){
      _minDist = minDist;
      return true;
    } else return false;
  }

bool UltraSonic::changeMaxDist(int maxDist)
  { if(maxDist>_minDist && maxDist<=MAX_SECURITY_DISTANCE){
      _maxDist = maxDist;
      return true;
    } else return false;
  }

/***   sensorController   ***/
// Constructor
sensorController::sensorController(sensorConfig sconfig, dynamicMem & myMem)
  { // Save sensorConfig
    _sconfig = sconfig;

    // Initialize timers
    _sensorTimer = millis();

    // Initialize analog sensors
    int s_analogs = 0;
    if(_sconfig.analogs!=254) s_analogs = _sconfig.analogs;
    for (int i = 0; i<s_analogs; i++){
      analogSensor analogParameter = myMem.read_analog(i); // Get analog config
      _myAnalogs[i] = new analogSens(analogParameter.pin, i); // (pin, number)
      _myAnalogs[i]->setModel(analogParameter.A, analogParameter.B, analogParameter.C); // (A, B, C)
      _myAnalogs[i]->setFilter(analogParameter.filter, analogParameter.filterParam); // (filter, param)
      _myAnalogs[i]->begin();
    }
    // Initialize flowmeter sensors
    int s_flowmeters = 0;
    if(_sconfig.flowmeters!=254) s_flowmeters = _sconfig.flowmeters;
    for (int i = 0; i<s_flowmeters; i++){
      flowmeter flowParameter = myMem.read_flowmeter(i); // Get flowmeter config
      _myFlowmeters[i] = new Flowmeter(flowParameter.pin, i, flowParameter.K); // (pin, number, k_constant)
      _myFlowmeters[i]->begin();
    }
    // Initialize scale sensors
    int s_scales = 0;
    if(_sconfig.scales!=254) s_scales = _sconfig.scales;
    for (int i = 0; i<s_scales; i++){
      scale scaleParameter = myMem.read_scale(i); // Get scale config
      _myScales[i] = new ScaleSens(scaleParameter.pin1, scaleParameter.pin2, i); // (pin1, pin2, num)
      _myScales[i]->begin(scaleParameter.offset, scaleParameter.scale, scaleParameter.min_weight, scaleParameter.max_weight); // (offset, scale, min_weight, max_weight)
    }
    // Initialize switch sensors
    int s_switches = 0;
    if(_sconfig.switches!=254) s_switches = _sconfig.switches;
    for (int i = 0; i<s_switches; i++){
      switchSensor switchParameter = myMem.read_switch(i);
      _mySwitches[i] = new SwitchSens(switchParameter.pin, i, switchParameter.logic); // (pin, num, logic)
      _mySwitches[i]->begin();
    }
    // Initialize ultrasonic sensors
    int s_ultrasonics = 0;
    if(_sconfig.ultrasonics!=254) s_ultrasonics = _sconfig.ultrasonics;
    for (int i = 0; i<s_ultrasonics; i++){
      ultrasonicSensor ultrasonicParameter = myMem.read_ultrasonic(i);
      _myUltrasonics[i] = new UltraSonic(ultrasonicParameter.pin1, ultrasonicParameter.pin2, i); // (pin1, pin2, num)
      _myUltrasonics[i]->setModel(ultrasonicParameter.model, ultrasonicParameter.param, ultrasonicParameter.height); //  (model, param, height)
    }
  }

void sensorController::read()
  { if(millis()-_sensorTimer>READ_SENSOR_TIME){
      _sensorTimer = millis();
      // Read analog sensors
      int s_analogs = 0;
      if(_sconfig.analogs!=254) s_analogs = _sconfig.analogs;
      for (int i = 0; i<s_analogs; i++) _myAnalogs[i]->read();
      // Read flowmeter sensors
      int s_flowmeters = 0;
      if(_sconfig.flowmeters!=254) s_flowmeters = _sconfig.flowmeters;
      for (int i = 0; i<s_flowmeters; i++) _myFlowmeters[i]->read();
      // Read scale sensors
      int s_scales = 0;
      if(_sconfig.scales!=254) s_scales = _sconfig.scales;
      for (int i = 0; i<s_scales; i++) _myScales[i]->read();
      // Read switch sensors
      int s_switches = 0;
      if(_sconfig.switches!=254) s_switches = _sconfig.switches;
      for (int i = 0; i<s_switches; i++) _mySwitches[i]->read();
      // Read ultrasonic sensors
      int s_ultrasonics = 0;
      if(_sconfig.ultrasonics!=254) s_ultrasonics = _sconfig.ultrasonics;
      for (int i = 0; i<s_ultrasonics; i++) _myUltrasonics[i]->updateState();
    }
  }
