void solenoid_setup(){
  uint8_t reg = 0;
  uint8_t fl = 0;
  Serial.println(F("Solenoid valves: Starting with Default Parameters"));
  for( int i = 0 ; i<solenoidValve::__TotalActuators; i++ ){
    solenoidValve::ptr[i]->begin(fl, reg);
    reg++;
    if(reg>=MAX_IRRIGATION_REGIONS){
      reg = 0;
      fl++;
    }
  }
  
  int count = 0;
  for(int i = 0 ; i<solenoidValve::__TotalActuators*5+1; i++ ){if(EEPROM.read(i)==0){count++;}}
  if(count>2){
    Serial.println(F("Solenoid valves: Creating routine by default"));
    if(solenoidValve::reOrderAll(HIGH, HIGH)){
    Serial.println(F("Routine created correctly"));
    }else{Serial.println(F("Routine cannot be configured correctly"));}
  }
  // Delay to give time to pass all serial parameters
  delay(1000);
}

void sensors_setup(){
  Serial.println(F("Setting up sensors:"));

  // Pressure Sensors
  analogSensor::beginAll(); // Starts pressure sensors
  // Charge Parameters for all pressure sensors
  for(int i=0; i<analogSensor::__TotalSensors; i++){chargeAnalogParameters(i);}
  Serial.println(F("Pressure Sensors started"));

  // Level Sensors
  UltraSonic::begin(); // Starts all the ultrasonic sensors
  // Charge Parameters for all ultrasonic sensors
  for(int i=0; i<UltraSonic::__TotalSensors; i++){chargeUltrasonicParameters(i);}
  Serial.println(F("UltraSonic Sensors started"));

  // Air/Water Sensors
  waterSensor::beginAll(); // Starts water sensors
  Serial.println(F("Water Sensors started"));
  // Delay to give time to pass all serial parameters
  delay(1000);
}

void enableSolenoid(int fl, int reg){
  for(int i=0; i<solenoidValve::__TotalActuators; i++){
    if(solenoidValve::ptr[i]->getFloor()==fl){
      // All the valves disable
      if(reg==0){ solenoidValve::ptr[i]->enable(false); }
      // Just germination on
      else if(reg<=2){
        if(solenoidValve::ptr[i]->getRegion()!=0 && solenoidValve::ptr[i]->getRegion()!=4){ solenoidValve::ptr[i]->enable(false); }
        else{ solenoidValve::ptr[i]->enable(true); }
      }
      // Germination and Stage 1 on
      else if(reg<=4){
        if(solenoidValve::ptr[i]->getRegion()!=0 && solenoidValve::ptr[i]->getRegion()!=4 && 
           solenoidValve::ptr[i]->getRegion()!=1 && solenoidValve::ptr[i]->getRegion()!=5){ solenoidValve::ptr[i]->enable(false); }
        else{ solenoidValve::ptr[i]->enable(true); } 
      }
      else if(reg<=6){ // Germination, Stage 1 and 2 on
        if(solenoidValve::ptr[i]->getRegion()!=0 && solenoidValve::ptr[i]->getRegion()!=4 && 
           solenoidValve::ptr[i]->getRegion()!=1 && solenoidValve::ptr[i]->getRegion()!=5 &&
           solenoidValve::ptr[i]->getRegion()!=2 && solenoidValve::ptr[i]->getRegion()!=6 ){ solenoidValve::ptr[i]->enable(false); }
        else{ solenoidValve::ptr[i]->enable(true); }
      }
      else{ solenoidValve::ptr[i]->enable(true); }
    }
  }
}

void enableLED(int fl, int reg){
  if(reg>=2){LED_Mod::enable(fl, int(reg/2));}
  else{LED_Mod::enable(fl, 0);}
}

bool isDayInThatSolenoid(uint8_t solenoid){
  uint8_t fl = solenoidValve::ptr[solenoid]->getFloor();
  switch(fl){ // Check in that floor if is day or night
    case 0:
      return day1.getState();
      break;
    case 1:
      return day2.getState();
      break;
    case 2:
      return day3.getState();
      break;
    case 3:
      return day4.getState();
      break;
    default:
      return false;
      break;
  }
}

uint8_t inWhatFloorIsNight(){ // This function cannot be executed until the first hourUpdate
  if(!day1.getState()) { return 1; }
  else if(!day2.getState()) { return 2; }
  else if(!day3.getState()) { return 3; }
  else if(!day4.getState()) { return 4; }
  else{ Serial.println(F("inWhatFloorIsNight(): Error time information not updated yet")); return 0; }
}

void updateDay(){ 
  if(day1.getState()!=day1.isDay(dateHour, dateMinute)){
    if(day1.getState()){
      Serial.println(F("Day Started in Floor 1"));
      LED_Mod::turnOn(0);
      }
    else{
      Serial.println(F("Night Started in Floor 1"));
      LED_Mod::turnOff(0);
      }
  }
  if(day2.getState()!=day2.isDay(dateHour, dateMinute)){
    if(day2.getState()){
      Serial.println(F("Day Started in Floor 2"));
      LED_Mod::turnOn(1);
      }
    else{
      Serial.println(F("Night Started in Floor 2"));
      LED_Mod::turnOff(1);
      }
  }
  if(day3.getState()!=day3.isDay(dateHour, dateMinute)){
    if(
      day3.getState()){Serial.println(F("Day Started in Floor 3"));
      LED_Mod::turnOn(2);
      }
    else{
      Serial.println(F("Night Started in Floor 3"));
      LED_Mod::turnOn(2);
      }
  }
  if(day4.getState()!=day4.isDay(dateHour, dateMinute)){
    if(day4.getState()){
      Serial.println(F("Day Started in Floor 4"));
      LED_Mod::turnOn(3);
      }
    else{
      Serial.println(F("Night Started in Floor 4"));
      LED_Mod::turnOn(3);
      }
  }
}

void substractSolutionConsumption(bool updateConsumption = false){
  uint8_t nite = inWhatFloorIsNight()-1;
  float consumption = 0; // Water consumption in the last stage
  solenoidValve *pointer;
  
  // Get the water consumption in day floors
  for(int i=0; i<MAX_FLOOR; i++){
    if(i!=nite){ consumption += solenoidValve::getWaterByFloor(i); }
  }

  Recirculation.addVolKnut(-consumption); // Substract water consumption from the kegs
  
  // Erase the water consumption in that valves
  for(int i=0; i<solenoidValve::__TotalActuators; i++){
    pointer = solenoidValve::ptr[i];
    if(pointer->getFloor()!=nite){ pointer->restartH2O(false); }
  }

  if(updateConsumption){ // Update solutionConsumption parameter
    if(consumption>solutionConsumption){ solutionConsumption = consumption; }
    else{ solutionConsumption = (solutionConsumption+consumption)/2; }
  }
}

void substractWaterConsumption(bool updateConsumption = false){
  uint8_t nite = inWhatFloorIsNight()-1;
  float consumption = 0; // Water consumption in the last stage
  solenoidValve *pointer;
  
  // Get the water consumption in night floor
  consumption = solenoidValve::getWaterByFloor(nite);
  
  Recirculation.addVolKh2o(-consumption); // Substract water consumption from the kegs
  
  // Erase the water consumption in that valves
  for(int i=0; i<solenoidValve::__TotalActuators; i++){
    pointer = solenoidValve::ptr[i];
    if(pointer->getFloor()==nite){ pointer->restartH2O(false); }
  }

  if(updateConsumption){ // Update h2oConsumption parameter
    if(consumption>h2oConsumption){ h2oConsumption = consumption; }
    else{ h2oConsumption = (h2oConsumption+consumption)/2; }
  }
}
