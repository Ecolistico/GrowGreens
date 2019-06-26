void solenoid_setup(){
  int count = 0;
  for( int i = 0 ; i<MAX_ACTUATORS; i++ ){if(EEPROM.read(i)!=0){count++;}}
  if(count==0){
    byte reg = 0;
    byte fl = 0;
    byte solenoidTime = 3;
    unsigned long cycleTime = 600;
    Serial.println(F("Solenoid valves: Setting Default Parameters"));
    
    // solenoidValve.begin(floor, pos, t_On, t_Off);
    for( int i = 0 ; i<solenoidValve::__TotalActuators; i++ ){
      solenoidValve::ptr[i]->begin(fl, reg, solenoidTime, cycleTime);
      reg++;
      solenoidTime++;
      if(reg>=MAX_IRRIGATION_REGIONS){
        reg = 0;
        fl++;
      }
      if(reg<=4 && reg%(MAX_IRRIGATION_REGIONS/2)==0){
        solenoidTime = 3;
      }
      else if(reg>4 && reg%MAX_IRRIGATION_REGIONS==0){
        solenoidTime = 3;
      }
    }
    
    Serial.println(F("Solenoid valves: Creating routine by default"));
    if(solenoidValve::ptr[0]->reOrderAll(HIGH, HIGH)){
    Serial.println(F("Routine created correctly"));
    }else{Serial.println(F("Routine cannot be configured correctly"));}
  }

  else{
    byte reg = 0;
    byte fl = 0;
    byte solenoidTime = 0;
    unsigned long cycleTime = EEPROM.read(MAX_ACTUATORS+1)*(60*0.25);   
    Serial.println(F("Solenoid valves: Setting Parameters from EEPROM"));
    
    for( int i = 0 ; i<solenoidValve::__TotalActuators; i++ ){
      if(EEPROM.read(i)<128){
        solenoidTime = EEPROM.read(i);
        solenoidValve::ptr[i]->begin(fl, reg, solenoidTime, cycleTime);
        reg++;
      }
      else{
        solenoidTime = EEPROM.read(i)-128;
        solenoidValve::ptr[i]->begin(fl, reg, solenoidTime, cycleTime);
        solenoidValve::ptr[i]->enable(LOW);
        reg++;
      }
      if(reg+1>=MAX_IRRIGATION_REGIONS){
        reg = 0;
        fl++;
      }
    }
    Serial.println(F("Solenoid valves: Creating routine by default"));
    solenoidValve::ptr[0]->defaultOrder(MAX_FLOOR);
  }
}

void LED_setup()
  { int count = 0;
    int gap = MAX_ACTUATORS+1;
    for( int i = gap ; i<gap+MAX_LEDS; i++ ){if(EEPROM.read(i)!=0){count++;}}

    if(count!=0){
      Serial.println(F("LEDs: Setting up with EEPROM parameters"));
      for( int i = gap ; i<gap+LED::__TotalLeds; i++ ){
        if(EEPROM.read(i)==0){LED::ptr[i-gap]->enable(HIGH);}
        else{LED::ptr[i-gap]->enable(LOW);}
      }
    }
  }

/***** Pendiente de aquí para abajo *****/
/*
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 */
void MultiDay_setup(){
  
}

void sensors_setup(){
  Serial.println(F("Setting up sensors..."));

  pressureSensorNutrition.begin();
  pressureSensorWater.begin();
  pressureSensorCompressor.begin();
  
  pressureSensorNutrition.defaultFilter(); // Default Kalman Filter
  pressureSensorWater.defaultFilter(); // Default Kalman Filter
  pressureSensorCompressor.defaultFilter(); // Default Kalman Filter
  
  if(pressureSensorNutrition.setModel(1, -22.969, 0.2155) && pressureSensorWater.setModel(1, -22.969, 0.2155) && pressureSensorCompressor.setModel(1, -22.969, 0.2155)){
    Serial.println(F("Pressure Sensors started correctly"));
     pressureSensorNutrition.printModel();
     pressureSensorNutrition.printFilter();
     pressureSensorWater.printModel();
     pressureSensorWater.printFilter();
     pressureSensorCompressor.printModel();
     pressureSensorCompressor.printFilter();
  }
  else{Serial.println(F("Pressure Sensors started incorrectly"));}

  US0.beginAll();
  Serial.println(F("UltraSonic Sensors started correctly"));
}
