// dynamicMemory.cpp
//
// Copyright (C) 2019 Grow

#include "dynamicMemory.h"

/***   dynamicMemory   ***/

dynamicMem::dynamicMem() {
  myMem = new ExternalEEPROM;
}

bool dynamicMem::begin() {
  Wire.begin();
  if (myMem->begin() == false){
    Serial.println(F("error,EEPROM: No memory detected. Freezing."));
    while (1);
  }
  Serial.println(F("info,EEPROM: Memory detected!"));

  if (check_basicConfig()) {
    Serial.println(F("info,EEPROM: Basic configuration is complete"));
    if(check_pressureConfig()){
      if (check_sensorConfig()){
        Serial.println(F("info,EEPROM: Sensor configuration is complete"));
        if(check_logicConfig()){
          if(check_dynamicMem()) {
            Serial.println(F("info,EEPROM: Dynamic configuration is complete enough"));
            return true;
          } else Serial.println(F("error:EEPROM: Please provide dynamic config to continue"));
        } else Serial.println(F("error:EEPROM: Please provide logic config to continue"));
      } else Serial.println(F("error:EEPROM: Please provide sensor config to continue"));
    } else Serial.println(F("error:EEPROM: Please provide pressure config to continue"));
  } else Serial.println(F("error:EEPROM: Please provide basic config to continue"));

  return false;
}

void dynamicMem::clean(bool truncate /*= true*/) {
  int sizeLength = 0;
  if(truncate) sizeLength = 1000;
  else sizeLength = myMem->getMemorySize();
  Serial.println(F("warning,EEPROM: Deleting Memory..."));
  for(int i = 0; i<sizeLength; i++){
    write(i, 0);
  }
  Serial.println(F("warning,EEPROM: Memory Deleted"));
}

void dynamicMem::print(bool truncate /*= true*/) {
  int sizeLength = 0;
  if(truncate) sizeLength = 1000;
  else sizeLength = myMem->getMemorySize();
  Serial.println(F("info,EEPROM: Printing Memory Info"));
  for(int i = 0; i<sizeLength; i++){
    Serial.print(i); Serial.print(F(": ")); Serial.println(myMem->read(i));
  }
}

void dynamicMem::config_basic(basicConfig config){
  write(0, config.floors); // Floors number in pos 0
  write(1, config.solenoids); // Solenoids number in pos 1
  write(2, config.regions); // Regions number in pos 2
  write(3, config.cycleTime); // cycleTime in pos 3
  Serial.println(F("info,EEPROM: Basic configuration was successfully written!"));
}

basicConfig dynamicMem::getConfig_basic(){
  basicConfig bconfig;

  bconfig.floors = read_byte(0);
  bconfig.solenoids = read_byte(1);
  bconfig.regions = read_byte(2);
  bconfig.cycleTime = read_byte(3);

  return bconfig;
}

void dynamicMem::config_pressure(pressureConfig config){
  if(config.free_pressure<config.critical_pressure && config.critical_pressure<config.min_pressure && config.min_pressure<config.max_pressure){
    write(4, config.max_pressure);      // Max Pressure Allow in System
    write(5, config.min_pressure);      // Min Pressure Allow in System
    write(6, config.critical_pressure); // Critical Pressure Below Min_Pressure
    write(7, config.free_pressure);     // Free pressure to turn on water pump
    Serial.println(F("info,EEPROM: Pressure configuration was successfully written!"));
  }
  else Serial.println(F("error,EEPROM: Incorrect pressure incorrect parameters in config_pressure(pressureConfig config)"));
}

pressureConfig dynamicMem::getConfig_pressure(){
  pressureConfig pconfig;

  pconfig.max_pressure = read_byte(4);
  pconfig.min_pressure = read_byte(5);
  pconfig.critical_pressure = read_byte(6);
  pconfig.free_pressure = read_byte(7);

  return pconfig;
}

void dynamicMem::config_sensors(sensorConfig config){
  write(10, config.analogs); // analog number in pos 10
  write(11, config.flowmeters); // flowmeters number in pos 11
  write(12, config.scales); // scales number in pos 12
  write(13, config.switches); // switches number in pos 13
  write(14, config.ultrasonics); // ultrasonics number in pos 14
  Serial.println(F("info,EEPROM: Sensor configuration was successfully written!"));
}

sensorConfig dynamicMem::getConfig_sensors(){
  sensorConfig sconfig;

  sconfig.analogs = read_byte(10);
  sconfig.flowmeters = read_byte(11);
  sconfig.scales = read_byte(12);
  sconfig.switches = read_byte(13);
  sconfig.ultrasonics = read_byte(14);

  return sconfig;
}

void dynamicMem::config_logic(logicConfig config){
  write(20, config.Vair_Logic); // analog number in pos 10
  write(21, config.Vconnected_Logic); // flowmeters number in pos 11
  write(22, config.VFree_Logic); // scales number in pos 12
  Serial.println(F("info,EEPROM: Logic configuration was successfully written!"));
}

logicConfig dynamicMem::getConfig_logic(){
  logicConfig lconfig;

  lconfig.Vair_Logic = read_byte(20);
  lconfig.Vconnected_Logic = read_byte(21);
  lconfig.VFree_Logic = read_byte(22);

  return lconfig;
}

bool dynamicMem::check_float(float fl){
  if(isnan(fl)) {
    Serial.println(F("warning,EEPROM: check_float(float fl) found NAN"));
    return false;
  }
  else if(fl>=4294967040 || fl<=-4294967040) {
    Serial.println(F("warning,EEPROM: check_float(float fl) found OVF"));
    return false;
  }
  return true;
}

bool dynamicMem::check_basicConfig(){
  basicConfig bconfig = getConfig_basic();
  uint8_t counter = 0;

  if (bconfig.floors!=0 && bconfig.floors!=255) counter++;
  else Serial.println(F("warning,EEPROM: Floors (basic) parameter not found"));
  if (bconfig.solenoids!=0 && bconfig.solenoids!=255) counter++;
  else Serial.println(F("warning,EEPROM: Solenoids (basic) parameter not found"));
  if (bconfig.regions!=0 && bconfig.regions!=255) counter++;
  else Serial.println(F("warning,EEPROM: Regions (basic) parameter not found"));
  if (bconfig.cycleTime!=0 && bconfig.cycleTime!=255) counter++;
  else Serial.println(F("warning,EEPROM: CycleTime (basic) parameter not found"));

  if (counter==4) return true;
  else return false;
}

bool dynamicMem::check_pressureConfig(){
  pressureConfig pconfig = getConfig_pressure();
  uint8_t counter = 0;

  if (pconfig.max_pressure!=0 && pconfig.max_pressure!=255) counter++;
  else Serial.println(F("warning,EEPROM: Max_pressure (pressure) parameter not found"));
  if (pconfig.min_pressure!=0 && pconfig.min_pressure!=255) counter++;
  else Serial.println(F("warning,EEPROM: Min_pressure (pressure) parameter not found"));
  if (pconfig.critical_pressure!=0 && pconfig.critical_pressure!=255) counter++;
  else Serial.println(F("warning,EEPROM: Critical_pressure (pressure) parameter not found"));
  if (pconfig.free_pressure!=0 && pconfig.free_pressure!=255) counter++;
  else Serial.println(F("warning,EEPROM: Free_pressure (pressure) parameter not found"));

  if (counter==4) return true;
  else return false;
}

bool dynamicMem::check_sensorConfig(){
  sensorConfig sconfig = getConfig_sensors();
  uint8_t counter = 0;

  if (sconfig.analogs!=0 && sconfig.analogs!=255) counter++;
  else Serial.println(F("warning,EEPROM: Analogs (sensor) parameter not found"));
  if (sconfig.flowmeters!=0 && sconfig.flowmeters!=255) counter++;
  else Serial.println(F("warning,EEPROM: Flowmeters (sensor) parameter not found"));
  if (sconfig.scales!=0 && sconfig.scales!=255) counter++;
  else Serial.println(F("warning,EEPROM: Scales (sensor) parameter not found"));
  if (sconfig.switches!=0 && sconfig.switches!=255) counter++;
  else Serial.println(F("warning,EEPROM: Switches (sensor) parameter not found"));
  if (sconfig.ultrasonics!=0 && sconfig.ultrasonics!=255) counter++;
  else Serial.println(F("warning,EEPROM: Ultrasonics (sensor) parameter not found"));

  if (counter==5) return true;
  else return false;
}

bool dynamicMem::check_logicConfig(){
  logicConfig lconfig = getConfig_logic();
  uint8_t counter = 0;

  if (lconfig.Vair_Logic==0 || lconfig.Vair_Logic==1) counter++;
  else Serial.println(F("warning,EEPROM: Vair_Logic (logic) parameter not found"));
  if (lconfig.Vconnected_Logic==0 || lconfig.Vconnected_Logic==1) counter++;
  else Serial.println(F("warning,EEPROM: Vconnected_Logic (logic) parameter not found"));
  if (lconfig.VFree_Logic==0 || lconfig.VFree_Logic==1) counter++;
  else Serial.println(F("warning,EEPROM: VFree_Logic (logic) parameter not found"));

  if (counter==3) return true;
  else return false;

}

bool dynamicMem::check_dynamicMem(){
  sensorConfig sconfig;
  analogSensor analog;
  flowmeter flow;
  scale sc;
  switchSensor sw;
  ultrasonicSensor ultra;

  sconfig.analogs = read_byte(10);
  if(sconfig.analogs == 254 ) sconfig.analogs = 0;
  sconfig.flowmeters = read_byte(11);
  if(sconfig.flowmeters == 254 ) sconfig.flowmeters = 0;
  sconfig.scales = read_byte(12);
  if(sconfig.scales == 254 ) sconfig.scales = 0;
  sconfig.switches = read_byte(13);
  if(sconfig.switches == 254 ) sconfig.switches = 0;
  sconfig.ultrasonics = read_byte(14);
  if(sconfig.ultrasonics == 254 ) sconfig.ultrasonics = 0;

  for(int i = 0; i<sconfig.analogs; i++) {
    analog = read_analog(i);
    if(analog.pin == 0 || analog.pin == 255) {
      Serial.print(F("error,EEPROM: Analog sensor number "));
      Serial.print(i+1);
      Serial.println(F(" is not configured yet"));
      return false;
    }
    if((analog.A == 0 || !check_float(analog.A)) && (analog.B == 0 || !check_float(analog.B)) && (analog.C == 0 || !check_float(analog.C))) {
      Serial.print(F("warning,EEPROM: Analog sensor number "));
      Serial.print(i+1);
      Serial.println(F(" has not calibration parameters"));
    }
  }

  for(int i = 0; i<sconfig.flowmeters; i++) {
    flow = read_flowmeter(i);
    if(flow.pin == 0 || flow.pin == 255) {
      Serial.print(F("error,EEPROM: Flowmeter sensor number "));
      Serial.print(i+1);
      Serial.println(F(" is not configured yet"));
      return false;
    }
    if(flow.K == 0 || !check_float(flow.K)){
      Serial.print(F("warning,EEPROM: Flowmeter sensor number "));
      Serial.print(i+1);
      Serial.println(F(" has not calibration parameter"));
    }
  }

  for(int i = 0; i<sconfig.scales; i++) {
    sc = read_scale(i);
    if(sc.pin1 == 0 || sc.pin1 == 255 || sc.pin2 == 0 || sc.pin2 == 255) {
      Serial.print(F("error,EEPROM: Scale sensor number "));
      Serial.print(i+1);
      Serial.println(F(" is not configured yet"));
      return false;
    }
    if((sc.offset == 0 || sc.offset == -1) && (sc.scale == 0 || !check_float(sc.scale))){
      Serial.print(F("warning,EEPROM: Scale sensor number "));
      Serial.print(i+1);
      Serial.println(F(" has not calibration parameters"));
    }
  }

  for(int i = 0; i<sconfig.switches; i++) {
    sw = read_switch(i);
    if(sw.pin == 0 || sw.pin == 255) {
      Serial.print(F("error,EEPROM: Switch sensor number "));
      Serial.print(i+1);
      Serial.println(F(" is not configured yet"));
      return false;
    }
    if(sw.logic!=0 && sw.logic!=1){
      Serial.print(F("warning,EEPROM: Switch sensor number "));
      Serial.print(i+1);
      Serial.println(F(" bool parameter is wrong"));
    }
  }

  for(int i = 0; i<sconfig.ultrasonics; i++) {
    ultra = read_ultrasonic(i);
    if(ultra.pin1 == 0 || ultra.pin1 == 255 || ultra.pin2 == 0 || ultra.pin2 == 255) {
      Serial.print(F("error,EEPROM: Ultrasonic sensor number "));
      Serial.print(i+1);
      Serial.println(F(" is not configured yet"));
      return false;
    }
    if((ultra.param == 0 || !check_float(ultra.param)) && (ultra.height == 0 || !check_float(ultra.height))){
      Serial.print(F("warning,EEPROM: Ultrasonic sensor number "));
      Serial.print(i+1);
      Serial.println(F(" has not calibration parameters"));
    }
  }

  return true;
}

void dynamicMem::write(int pos, uint8_t val){
  uint8_t auxVal = myMem->read(pos);
  if(val!=auxVal){
    Serial.print(F("debug,Saving in EEPROM on position("));
    Serial.print(pos);
    Serial.print(F(") Value="));
    Serial.println(val);
    myMem->write(pos, val);
  }
  else{
    Serial.print(F("warning,EEPROM on position("));
    Serial.print(pos);
    Serial.print(F(") Value="));
    Serial.print(val);
    Serial.println(F(" already saved"));
  }
}

uint8_t dynamicMem::read_byte(int pos){
  uint8_t auxVal = myMem->read(pos);
  Serial.print(F("debug,EEPROM: pos("));
  Serial.print(pos); Serial.print(F(")= ")); Serial.println(auxVal);
  return auxVal;
}

void dynamicMem::write(int pos, int val){
  int auxVal = 0;
  myMem->get(pos, auxVal);
  if(val!=auxVal){
    Serial.print(F("info,Saving in EEPROM on position("));
    Serial.print(pos);
    Serial.print(F(") Value="));
    Serial.println(val);
    myMem->put(pos, val);
  }
  else{
    Serial.print(F("warning,EEPROM on position("));
    Serial.print(pos);
    Serial.print(F(") Value="));
    Serial.print(val);
    Serial.println(F(" already saved"));
  }
}

int dynamicMem::read_int(int pos){
  int auxVal = 0;
  myMem->get(pos, auxVal);
  Serial.print(F("debug,EEPROM: pos("));
  Serial.print(pos); Serial.print(F(")= ")); Serial.println(auxVal);
  return auxVal;
}

void dynamicMem::write(int pos, float val){
  float auxVal = 0;
  myMem->get(pos, auxVal);
  if(val!=auxVal){
    Serial.print(F("debug,Saving in EEPROM on position("));
    Serial.print(pos);
    Serial.print(F(") Value="));
    Serial.println(val);
    myMem->put(pos, val);
  }
  else{
    Serial.print(F("warning,EEPROM on position("));
    Serial.print(pos);
    Serial.print(F(") Value="));
    Serial.print(val);
    Serial.println(F(" already saved"));
  }
}

float dynamicMem::read_float(int pos){
  float auxVal = 0;
  myMem->get(pos, auxVal);
  Serial.print(F("debug,EEPROM: pos("));
  Serial.print(pos); Serial.print(F(")= ")); Serial.println(auxVal);
  return auxVal;
}

void dynamicMem::write(int pos, long val){
  long auxVal = 0;
  myMem->get(pos, auxVal);
  if(val!=auxVal){
    Serial.print(F("info,Saving in EEPROM on position("));
    Serial.print(pos);
    Serial.print(F(") Value="));
    Serial.println(val);
    myMem->put(pos, val);
  }
  else{
    Serial.print(F("warning,EEPROM on position("));
    Serial.print(pos);
    Serial.print(F(") Value="));
    Serial.print(val);
    Serial.println(F(" already saved"));
  }
}

long dynamicMem::read_long(int pos){
  long auxVal = 0;
  myMem->get(pos, auxVal);
  Serial.print(F("debug,EEPROM: pos("));
  Serial.print(pos); Serial.print(F(")= ")); Serial.println(auxVal);
  return auxVal;
}

void dynamicMem::write(int pos, unsigned long val){
  unsigned long auxVal = 0;
  myMem->get(pos, auxVal);
  if(val!=auxVal){
    Serial.print(F("info,Saving in EEPROM on position("));
    Serial.print(pos);
    Serial.print(F(") Value="));
    Serial.println(val);
    myMem->put(pos, val);
  }
  else{
    Serial.print(F("warning,EEPROM on position("));
    Serial.print(pos);
    Serial.print(F(") Value="));
    Serial.print(val);
    Serial.println(F(" already saved"));
  }
}

unsigned long dynamicMem::read_ulong(int pos){
  unsigned long auxVal = 0;
  myMem->get(pos, auxVal);
  Serial.print(F("debug,EEPROM: pos("));
  Serial.print(pos); Serial.print(F(")= ")); Serial.println(auxVal);
  return auxVal;
}

void dynamicMem::save_irrigationParameters(uint8_t floor, uint8_t region, uint8_t number, uint8_t time){
  if(floor<MAX_FLOOR_NUMBER && number<MAX_VALVES_PER_REGION){
    basicConfig bconfig;

    bconfig.floors = read_byte(0);
    bconfig.solenoids = read_byte(1);
    bconfig.regions = read_byte(2);
    bconfig.cycleTime = read_byte(3);

    int minPos = DYNAMIC_START;
    int maxPos = MaxIrrigationPos();
    int posWrite = minPos + number + region*bconfig.solenoids + floor*bconfig.solenoids*bconfig.regions;

    if(posWrite>=minPos && posWrite<maxPos) myMem->write(posWrite, time);
    else Serial.println(F("error,EEPROM: save_irrigationparemters(uint8_t floor, bool region, uint8_t number, uint8_t time) dynamic memory is poorly distributed"));
  } else Serial.println(F("error,EEPROM: save_irrigationparemters(uint8_t floor, bool region, uint8_t number, uint8_t time) wrong values provided"));
}

void dynamicMem::save_fanParameters(uint8_t floor, fan_memory fanM){
  if(floor<MAX_FLOOR_NUMBER && floor<=read_byte(0)){
    bool error = false;

    int minPos = MaxIrrigationPos();
    int maxPos = MaxFanPos();
    int currentPos = MaxIrrigationPos() + floor*sizeof(fan_memory);

    if (currentPos>=minPos && currentPos<maxPos && !error){
      write(currentPos, fanM.timeOn);
      currentPos += sizeof(fanM.timeOn);
    } else if(!error){
      Serial.println(F("error,EEPROM: save_fanParameters(int floor, fan_memory fanM) wrong values provided"));
      error = true;
    }

    if (currentPos>=minPos && currentPos<maxPos && !error){
      write(currentPos, fanM.cycleTime);
      currentPos += sizeof(fanM.cycleTime);
    } else if(!error){
      Serial.println(F("error,EEPROM: save_fanParameters(int floor, fan_memory fanM) wrong values provided"));
      error = true;
    }

    if (!error) Serial.println(F("info,EEPROM: Fan info saved correctly"));

  } else Serial.println(F("error,EEPROM: save_fanParameters(int floor, fan_memory fanM) wrong values provided"));
}

void dynamicMem::save_analog(uint8_t num, analogSensor analog){
  bool error = false;

  int minPos = MaxFanPos();
  int maxPos = MaxAnalogPos();
  int currentPos = MaxFanPos() + num*sizeof(analogSensor);

  if (currentPos>=minPos && currentPos<maxPos && !error){
    write(currentPos, analog.pin);
    currentPos += sizeof(analog.pin);
  } else if(!error){
    Serial.println(F("error,EEPROM: save_analog(uint8_t num, analogSensor analog) wrong values provided"));
    error = true;
  }

  if (currentPos>=minPos && currentPos<maxPos && !error){
    write(currentPos, analog.A);
    currentPos += sizeof(analog.A);
  } else if(!error){
    Serial.println(F("error,EEPROM: save_analog(uint8_t num, analogSensor analog) wrong values provided"));
    error = true;
  }

  if (currentPos>=minPos && currentPos<maxPos && !error){
    write(currentPos, analog.B);
    currentPos += sizeof(analog.B);
  } else if(!error){
    Serial.println(F("error,EEPROM: save_analog(uint8_t num, analogSensor analog) wrong values provided"));
    error = true;
  }

  if (currentPos>=minPos && currentPos<maxPos && !error){
    write(currentPos, analog.C);
    currentPos += sizeof(analog.C);
  } else if(!error){
    Serial.println(F("error,EEPROM: save_analog(uint8_t num, analogSensor analog) wrong values provided"));
    error = true;
  }

  if (currentPos>=minPos && currentPos<maxPos && !error){
    write(currentPos, analog.filter);
    currentPos += sizeof(analog.filter);
  } else if(!error){
    Serial.println(F("error,EEPROM: save_analog(uint8_t num, analogSensor analog) wrong values provided"));
    error = true;
  }

  if (currentPos>=minPos && currentPos<maxPos && !error){
    write(currentPos, analog.filterParam);
    currentPos += sizeof(analog.filterParam);
  } else if(!error){
    Serial.println(F("error,EEPROM: save_analog(uint8_t num, analogSensor analog) wrong values provided"));
    error = true;
  }

  if (!error) Serial.println(F("info,EEPROM: Analog Sensor saved correctly"));
}

void dynamicMem::save_flowmeter(uint8_t num, flowmeter fm){
  bool error = false;

  int minPos = MaxAnalogPos();
  int maxPos = MaxFlowmeterPos();
  int currentPos = MaxAnalogPos() + num*sizeof(flowmeter);

  if (currentPos>=minPos && currentPos<maxPos && !error){
    write(currentPos, fm.pin);
    currentPos += sizeof(fm.pin);
  } else if(!error){
    Serial.println(F("error,EEPROM: save_flowmeter(uint8_t num, flowmeter fm) wrong values provided"));
    error = true;
  }

  if (currentPos>=minPos && currentPos<maxPos && !error){
    write(currentPos, fm.K);
    currentPos += sizeof(fm.K);
  } else if(!error){
    Serial.println(F("error,EEPROM: save_flowmeter(uint8_t num, flowmeter fm) wrong values provided"));
    error = true;
  }

  if (!error) Serial.println(F("info,EEPROM: Flowmeter Sensor saved correctly"));
}

void dynamicMem::save_scale(uint8_t num, scale sc){
  bool error = false;

  int minPos = MaxFlowmeterPos();
  int maxPos = MaxScalePos();
  int currentPos = MaxFlowmeterPos() + num*sizeof(scale);

  if (currentPos>=minPos && currentPos<maxPos && !error){
    write(currentPos, sc.pin1);
    currentPos += sizeof(sc.pin1);
  } else if(!error){
    Serial.println(F("error,EEPROM: save_scale(uint8_t num, scale sc) wrong values provided"));
    error = true;
  }

  if (currentPos>=minPos && currentPos<maxPos && !error){
    write(currentPos, sc.pin2);
    currentPos += sizeof(sc.pin2);
  } else if(!error){
    Serial.println(F("error,EEPROM: save_scale(uint8_t num, scale sc) wrong values provided"));
    error = true;
  }

  if (currentPos>=minPos && currentPos<maxPos && !error){
    write(currentPos, sc.offset);
    currentPos += sizeof(sc.offset);
  } else if(!error){
    Serial.println(F("error,EEPROM: save_scale(uint8_t num, scale sc) wrong values provided"));
    error = true;
  }

  if (currentPos>=minPos && currentPos<maxPos && !error){
    write(currentPos, sc.scale);
    currentPos += sizeof(sc.scale);
  } else if(!error){
    Serial.println(F("error,EEPROM: save_scale(uint8_t num, scale sc) wrong values provided"));
    error = true;
  }

  if (!error) Serial.println(F("info,EEPROM: Scale Sensor saved correctly"));
}

void dynamicMem::save_switch(uint8_t num, switchSensor sw){
  bool error = false;

  int minPos = MaxScalePos();
  int maxPos = MaxSwitchPos();
  int currentPos = MaxScalePos() + num*sizeof(switchSensor);

  if (currentPos>=minPos && currentPos<maxPos && !error){
    write(currentPos, sw.pin);
    currentPos += sizeof(sw.pin);
  } else if(!error){
    Serial.println(F("error,EEPROM: save_switch(uint8_t num, switchSensor sw) wrong values provided"));
    error = true;
  }

  if (currentPos>=minPos && currentPos<maxPos && !error){
    write(currentPos, sw.logic);
    currentPos += sizeof(sw.logic);
  } else if(!error){
    Serial.println(F("error,EEPROM: save_switch(uint8_t num, switchSensor sw) wrong values provided"));
    error = true;
  }

  if (!error) Serial.println(F("info,EEPROM: Switch Sensor saved correctly"));
}

void dynamicMem::save_ultrasonic(uint8_t num, ultrasonicSensor ultra){
  bool error = false;

  int minPos = MaxSwitchPos();
  int maxPos = MaxUltrasonicPos();
  int currentPos = MaxSwitchPos() + num*sizeof(ultrasonicSensor);

  if (currentPos>=minPos && currentPos<maxPos && !error){
    write(currentPos, ultra.pin1);
    currentPos += sizeof(ultra.pin1);
  } else if(!error){
    Serial.println(F("error,EEPROM: save_ultrasonic(uint8_t num, ultrasonicSensor ultra) wrong values provided"));
    error = true;
  }

  if (currentPos>=minPos && currentPos<maxPos && !error){
    write(currentPos, ultra.pin2);
    currentPos += sizeof(ultra.pin2);
  } else if(!error){
    Serial.println(F("error,EEPROM: save_ultrasonic(uint8_t num, ultrasonicSensor ultra) wrong values provided"));
    error = true;
  }

  if (currentPos>=minPos && currentPos<maxPos && !error){
    write(currentPos, ultra.model);
    currentPos += sizeof(ultra.model);
  } else if(!error){
    Serial.println(F("error,EEPROM: save_ultrasonic(uint8_t num, ultrasonicSensor ultra) wrong values provided"));
    error = true;
  }

  if (currentPos>=minPos && currentPos<maxPos && !error){
    write(currentPos, ultra.param);
    currentPos += sizeof(ultra.param);
  } else if(!error){
    Serial.println(F("error,EEPROM: save_ultrasonic(uint8_t num, ultrasonicSensor ultra) wrong values provided"));
    error = true;
  }

  if (currentPos>=minPos && currentPos<maxPos && !error){
    write(currentPos, ultra.height);
    currentPos += sizeof(ultra.height);
  } else if(!error){
    Serial.println(F("error,EEPROM: save_ultrasonic(uint8_t num, ultrasonicSensor ultra) wrong values provided"));
    error = true;
  }

  if (!error) Serial.println(F("info,EEPROM: Ultrasonic Sensor saved correctly"));
}

uint8_t dynamicMem::read_irrigationParameters(uint8_t floor, uint8_t region, uint8_t number){
  basicConfig bconfig;

  bconfig.floors = read_byte(0);
  bconfig.solenoids = read_byte(1);
  bconfig.regions = read_byte(2);
  bconfig.cycleTime = read_byte(3);

  int minPos = DYNAMIC_START;
  int maxPos = MaxIrrigationPos();
  int posWrite = minPos + number + region*bconfig.solenoids + floor*bconfig.solenoids*bconfig.regions;

  return read_byte(posWrite);
}

fan_memory dynamicMem::read_fan(uint8_t floor) {
  fan_memory fan;

  int pos = MaxIrrigationPos() + floor*sizeof(fan_memory);

  fan.timeOn = read_byte(pos);
  pos += sizeof(uint8_t);
  fan.cycleTime = read_ulong(pos);
  pos += sizeof(long);

  return fan;
}

analogSensor dynamicMem::read_analog(uint8_t num) {
  analogSensor analog;

  int pos = MaxFanPos() + num*sizeof(analogSensor);

  analog.pin = read_byte(pos);
  pos += sizeof(uint8_t);
  analog.A = read_float(pos);
  pos += sizeof(float);
  analog.B = read_float(pos);
  pos += sizeof(float);
  analog.C = read_float(pos);
  pos += sizeof(float);
  analog.filter = read_byte(pos);
  pos += sizeof(uint8_t);
  analog.filterParam = read_float(pos);

  return analog;
}

flowmeter dynamicMem::read_flowmeter(uint8_t num) {
  flowmeter flow;

  int pos = MaxAnalogPos() + num*sizeof(flowmeter);

  flow.pin = read_byte(pos);
  pos += sizeof(uint8_t);
  flow.K = read_float(pos);

  return flow;
}

scale dynamicMem::read_scale(uint8_t num) {
  scale sc;

  int pos = MaxFlowmeterPos() + num*sizeof(scale);

  sc.pin1 = read_byte(pos);
  pos += sizeof(uint8_t);
  sc.pin2 = read_byte(pos);
  pos += sizeof(uint8_t);
  sc.offset = read_long(pos);
  pos += sizeof(long);
  sc.scale = read_float(pos);

  return sc;
}

switchSensor dynamicMem::read_switch(uint8_t num) {
  switchSensor sw;

  int pos = MaxScalePos() + num*sizeof(switchSensor);

  sw.pin = read_byte(pos);
  pos += sizeof(uint8_t);
  sw.logic = read_byte(pos);

  return sw;
}

ultrasonicSensor dynamicMem::read_ultrasonic(uint8_t num) {
  ultrasonicSensor ultra;

  int pos = MaxSwitchPos() + num*sizeof(ultrasonicSensor);

  ultra.pin1 = read_byte(pos);
  pos += sizeof(uint8_t);
  ultra.pin2 = read_byte(pos);
  pos += sizeof(uint8_t);
  ultra.model = read_byte(pos);
  pos += sizeof(uint8_t);
  ultra.param = read_float(pos);
  pos += sizeof(float);
  ultra.height = read_float(pos);

  return ultra;
}

int dynamicMem::MaxIrrigationPos(){
  uint8_t floors = read_byte(0);
  uint8_t solenoids = read_byte(1);
  uint8_t regions = read_byte(2);

  int pos = DYNAMIC_START + (floors*solenoids*regions);

  return pos;
}

int dynamicMem::MaxFanPos(){
  uint8_t floors = read_byte(0);
  int pos = MaxIrrigationPos() + floors*sizeof(fan_memory);
  return pos;
}

int dynamicMem::MaxAnalogPos(){
  int analogs = read_byte(10);
  // 254 is a key for 0
  if (analogs == 254 ) analogs = 0;
  int pos = MaxFanPos() + analogs*sizeof(analogSensor);
  return pos;
}

int dynamicMem::MaxFlowmeterPos() {
  int flowmeters = read_byte(11);
  // 254 is a key for 0
  if (flowmeters == 254 ) flowmeters = 0;
  int pos = MaxAnalogPos() + flowmeters*sizeof(flowmeter);
  return pos;
}

int dynamicMem::MaxScalePos() {
  int scales = read_byte(12);
  // 254 is a key for 0
  if (scales == 254 ) scales = 0;
  int pos = MaxFlowmeterPos() + scales*sizeof(scale);
  return pos;
}

int dynamicMem::MaxSwitchPos() {
  int switches = read_byte(13);
  // 254 is a key for 0
  if (switches == 254 ) switches = 0;
  int pos = MaxScalePos() + switches*sizeof(switchSensor);
  return pos;
}

int dynamicMem::MaxUltrasonicPos() {
  int ultras = read_byte(14);
  // 254 is a key for 0
  if (ultras == 254 ) ultras = 0;
  int pos = MaxSwitchPos() + ultras*sizeof(ultrasonicSensor);
  return pos;
}
