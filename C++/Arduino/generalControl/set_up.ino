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
}

void sensors_setup(){
  Serial.println(F("Setting up sensors:"));

  // DHT (Temp/Hum)
  dht22_ext.begin();
  Serial.println(F("DHT sensor started correctly"));

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
}
