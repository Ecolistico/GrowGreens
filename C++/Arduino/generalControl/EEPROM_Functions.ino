void clean_EEPROM(){
  Serial.println(F("EEPROM: Deleting Memory..."));
  for(int i=0; i<solenoidValve::__TotalActuators*5+17; i++){
    EEPROM.update(i, 0);
  }
  for(int i=255; i<sizeof(float)*3; i++){
    EEPROM.update(i, 0);
  }
  for(int i=511; i<662; i++){
    EEPROM.update(i, 0);
  }
  Serial.println(F("EEPROM: Memory Deleted"));
}

void print_EEPROM(){
  Serial.println(F("EEPROM: Printing Memory Info"));
  for(int i=0; i<solenoidValve::__TotalActuators*5+17; i++){
    Serial.print(i); Serial.print(F(": ")); Serial.println(EEPROM.read(i));
  }
  for(int i=255; i<sizeof(float)*3; i++){
    Serial.print(i); Serial.print(F(": ")); Serial.println(EEPROM.read(i));
  }
  for(int i=511; i<662; i++){
    Serial.print(i); Serial.print(F(": ")); Serial.println(EEPROM.read(i));
  }
}

void save_EEPROM(int pos, int val){
  int actualVal = EEPROM.read(pos);
  if(val!=actualVal){
    Serial.print(F("Saving in EEPROM on position("));
    Serial.print(pos);
    Serial.print(F(") Value="));
    Serial.println(val);
    EEPROM.update(pos, val);
  }
  else{ 
    Serial.print(F("EEPROM on position("));
    Serial.print(pos);
    Serial.print(F(") Value="));
    Serial.print(val);
    Serial.println(F(" already saved"));
  }
}

void save_EEPROM(int pos, float val){
  float actualVal = 0;
  EEPROM.get(pos, actualVal);
  if(val!=actualVal){
    Serial.print(F("Saving in EEPROM on position("));
    Serial.print(pos);
    Serial.print(F(") Value="));
    Serial.println(val);
    EEPROM.put(pos, val);
  }
  else{
    Serial.print(F("EEPROM on position("));
    Serial.print(pos);
    Serial.print(F(") Value="));
    Serial.print(val);
    Serial.println(F(" already saved"));
  }
}

void solenoidSaveTimeOn(int fl, int reg, int sol, int val){
  /* Definitions:
   *    Floor. It can be a whole number from 0 to MAX_FLOOR-1
   *    Region. It can be a whole nuber from 0 to MAX_FLOOR-1
   *    In alpha version each region is:
   *        0 = Germination A
   *        1 = Stage1 A
   *        2 = Stage2 A
   *        3 = Stage3 A
   *        4 = Germination B
   *        5 = Stage1 B
   *        6 = Stage2 B
   *        7 = Stage3 B: [0-3]
   *    Solution. It can be a whole number from 0 to MAX_SOLUTIONS_NUMBER
   *    In alpha version each solution is:
   *        0 = Night (Water)
   *        1 = KNO3
   *        2 = NH2H2PO4
   *        3 = Ca(NO3)2
   *        4 = MgSO4 + Micros
   */
  if(fl>=0 && fl<MAX_FLOOR){
    if(reg>=0 && reg<MAX_IRRIGATION_REGIONS){
      if(sol>=0 && sol<=MAX_SOLUTIONS_NUMBER){
        if(val>=0 && val<=254){
          int pos = fl*MAX_IRRIGATION_REGIONS + reg + sol*solenoidValve::__TotalActuators;
          save_EEPROM(pos, val);
        }
        else{ Serial.println(F("Solenoid Save Time On (): value parameter incorrect")); }
      }
      else{ Serial.println(F("Solenoid Save Time On (): solution parameter incorrect")); }
    }
    else{ Serial.println(F("Solenoid Save Time On (): region parameter incorrect")); }
  }
  else{ Serial.println(F("Solenoid Save Time On (): floor parameter incorrect")); }
}

void solenoidSaveCycleTime(int val){
  if(val>=0 && val<=254){
    int pos = solenoidValve::__TotalActuators*5;
    save_EEPROM(pos, val);
  }
  else{ Serial.println(F("Solenoid Save Cycle Time (): value parameter incorrect")); }
}

void chargeSolenoidParameters(int sol){
  int pos = 0;
  int val = 0;
  
  for(int i=0; i<solenoidValve::__TotalActuators; i++){
    if(isDayInThatSolenoid(i)){
      pos = solenoidValve::__TotalActuators*sol + i;
    }
    else{
      pos = i;
    }
    val = EEPROM.read(pos);
    if(val!=0 && long(val*1000)!=solenoidValve::ptr[i]->getTimeOn()){
      unsigned long timeOn_ms = long(val*1000);
      solenoidValve::ptr[i]->setTimeOn(timeOn_ms);  
    } 
  }
  
  val = EEPROM.read(solenoidValve::__TotalActuators*5);
  if(val!=0 && long(val*60000)!=solenoidValve::getCycleTime()){
    unsigned long cycleTime_ms = long(val*60000);
    solenoidValve::setCycleTime(cycleTime_ms); 
  }

  // Disable all the solenoids that has to be off
  pos = solenoidValve::__TotalActuators*5+12;
  int reg_1f = EEPROM.read(pos+1);
  int reg_2f = EEPROM.read(pos+2);
  int reg_3f = EEPROM.read(pos+3);
  int reg_4f = EEPROM.read(pos+4);

  for(int i=0; i<solenoidValve::__TotalActuators; i++){
    uint8_t fl = solenoidValve::ptr[i]->getFloor();
    uint8_t rg = solenoidValve::ptr[i]->getRegion(); 
    if(rg>=MAX_FLOOR){ rg -= MAX_FLOOR; } // Set rg in range [0-3]
    if(fl==0 && rg<reg_1f){ solenoidValve::ptr[i]->enable(true); } // Enable solenoid
    else if(fl==1 && rg<reg_2f){ solenoidValve::ptr[i]->enable(true); } // Enable solenoid
    else if(fl==2 && rg<reg_3f){ solenoidValve::ptr[i]->enable(true); } // Enable solenoid
    else if(fl==3 && rg<reg_4f){ solenoidValve::ptr[i]->enable(true); } // Enable solenoid
    else{ solenoidValve::ptr[i]->enable(false); } // Disable
  }
}

void multidaySave(int fl, int cyclesNumber, float lightPercentage, float initHour){
  int pos = solenoidValve::__TotalActuators*5;
  
  if(24%cyclesNumber==0){
    if(lightPercentage>=0 && lightPercentage<=100){
      if(initHour>=0 && initHour<24){
        int inHour = int(initHour*10);
        switch(fl){
          case 0:
            day1.redefine(cyclesNumber, lightPercentage, initHour);
            save_EEPROM(pos+1, cyclesNumber);
            save_EEPROM(pos+2, int(lightPercentage));
            save_EEPROM(pos+3, inHour);
            break;
          case 1:
            day2.redefine(cyclesNumber, lightPercentage, initHour);
            save_EEPROM(pos+4, cyclesNumber);
            save_EEPROM(pos+5, int(lightPercentage));
            save_EEPROM(pos+6, inHour);
            break;
          case 2:
            day3.redefine(cyclesNumber, lightPercentage, initHour);
            save_EEPROM(pos+7, cyclesNumber);
            save_EEPROM(pos+8, int(lightPercentage));
            save_EEPROM(pos+9, inHour);
            break;
          case 3:
            day4.redefine(cyclesNumber, lightPercentage, initHour);
            save_EEPROM(pos+10, cyclesNumber);
            save_EEPROM(pos+11, int(lightPercentage));
            save_EEPROM(pos+12, inHour);
            break;
          default:
            Serial.println(F("Multiday Save EEPROM Function: parameter floor incorrect"));
            break;
        }
      }
      else{ Serial.println(F("Multiday Save EEPROM Function: parameter initial hour incorrect")); }
    }
    else{ Serial.println(F("Multiday Save EEPROM Function: parameter light% incorrect")); }
  }
  else{ Serial.println(F("Multiday Save EEPROM Function: parameter cycles incorrect")); }
}

void chargeMultidayParameters(){
  Serial.println(F("Charging Multiday Parameters from EEPROM"));
  int pos = solenoidValve::__TotalActuators*5;
  // Redefine day 1
  int cycles = EEPROM.read(pos+1);
  int light = EEPROM.read(pos+2);
  float initHour = float(EEPROM.read(pos+3))/10;
  if(cycles>0 && light>0){ day1.redefine(cycles, light, initHour); }
  // Redefine day 2
  cycles = EEPROM.read(pos+4);
  light = EEPROM.read(pos+5);
  initHour = float(EEPROM.read(pos+6))/10;
  if(cycles>0 && light>0){ day2.redefine(cycles, light, initHour); }
  // Redefine day 3
  cycles = EEPROM.read(pos+7);
  light = EEPROM.read(pos+8);
  initHour = float(EEPROM.read(pos+9))/10;
  if(cycles>0 && light>0){ day3.redefine(cycles, light, initHour); }
  // Redefine day 4
  cycles = EEPROM.read(pos+10);
  light = EEPROM.read(pos+11);
  initHour = float(EEPROM.read(pos+12))/10;
  if(cycles>0 && light>0){ day4.redefine(cycles, light, initHour); }
}

void regionSave(int fl, int reg){
  if(fl>=0 && fl<MAX_FLOOR){
    if(reg>=0 && reg<=MAX_REGION){
      int pos = solenoidValve::__TotalActuators*5+12;
      save_EEPROM(pos+1+fl, reg);
    }
    else{Serial.println(F("Region Save EEPROM Function: parameter region incorrect"));}
  }
  else{Serial.println(F("Region Save EEPROM Function: parameter floor incorrect"));}
}

void chargeLedRegion(){
  int pos = solenoidValve::__TotalActuators*5+12;
  int reg_1f = EEPROM.read(pos+1);
  int reg_2f = EEPROM.read(pos+2);
  int reg_3f = EEPROM.read(pos+3);
  int reg_4f = EEPROM.read(pos+4);

  // Disable all the leds that has to be off
  LED_Mod::enable(0, reg_1f);
  LED_Mod::enable(1, reg_2f);
  LED_Mod::enable(2, reg_3f);
  LED_Mod::enable(3, reg_4f);
}

void analogSaveFilter(int Type, int filt, float filterParam){
  if(Type>=0 && Type<=2){
    if(filt>=0 && filt<=2){
      int pos = 512+(1+sizeof(float)*4)*Type;
      save_EEPROM(pos, filt); 
      save_EEPROM(pos+1, filterParam); 
    }
    else{Serial.println(F("Analog Sensor Save Filter: Parameter filt incorrect"));}
  }
  else{Serial.println(F("Analog Sensor Save Filter: Parameter type incorrect"));}
}

void analogSaveModel(int Type, float a, float b, float c){
  if(Type>=0 && Type<=2){
    int pos = 512+(1+sizeof(float)*4)*Type+1+sizeof(float);
    save_EEPROM(pos, a);
    save_EEPROM(pos+sizeof(float), b);
    save_EEPROM(pos+sizeof(float)*2, c);
  }
  else{Serial.println(F("Analog Sensor Save Mode: Parameter type incorrect"));}
}

bool chargeAnalogParameters(int Type){
  if(Type>=0 && Type<=2){
    int pos = 512+(1+sizeof(float)*4)*Type; // Initial Position to save
    uint8_t filt = EEPROM.read(pos); // Filter
    float paramFilter, a, b, c; // Float values
    uint8_t deg = 0;
    EEPROM.get(pos+1, paramFilter);
    EEPROM.get(pos+1+sizeof(float), a);
    EEPROM.get(pos+1+sizeof(float)*2, b);
    EEPROM.get(pos+1+sizeof(float)*3, c);
    
    if(filt==0){analogSensor::ptr[Type]->notFilter();} // Not Filter
    else if(filt==1){ 
      // Exponential Filter
      if(paramFilter>0 && paramFilter<1){analogSensor::ptr[Type]->setExponentialFilter(paramFilter);}
      else{analogSensor::ptr[Type]->defaultFilter();} // Default Filter
    }
    else if(filt==2){
      // Kalman Filter
      if(paramFilter>0){analogSensor::ptr[Type]->setKalmanFilter(paramFilter);}
      else{analogSensor::ptr[Type]->defaultFilter();} // Default Filter
    }

    if(c!=0){deg = 3;}
    else if(b!=0){deg = 2;}
    else if(a!=0){deg = 1;}
    else{deg = 0;}
    if(deg==0 || !analogSensor::ptr[Type]->setModel(deg, a, b, c)){
      analogSensor::ptr[Type]->setModel(1, -22.969, 0.2155);
    }
    return true;
  }
  else{
    Serial.println(F("Analog Sensor Charge Parameter: Parameter type incorrect"));
    return false;
  }
}


/**/

void ultrasonicSaveFilter(int Type, int filt, float filterParam){
  if(Type>=0 && Type<=6){
    if(filt>=0 && filt<=2){
      int pos = 563+(2+sizeof(float)*3)*Type;
      save_EEPROM(pos, filt);
      save_EEPROM(pos+1, filterParam); 
    }
    else{Serial.println(F("Ultrasonic Sensor Save Filter: Parameter filt incorrect"));}
  }
  else{Serial.println(F("Ultrasonic Sensor Save Filter: Parameter type incorrect"));}
}

void ultrasonicSaveModel(int Type, int model, float modelParam, float height){
  if(Type>=0 && Type<=6){
    if(model>=0 && model<=2){
      if(modelParam>0){
        if(height>=UltraSonic::ptr[Type]->getMaxDist()){
          int pos = 563+(2+sizeof(float)*3)*Type+1+sizeof(float);
          save_EEPROM(pos, model);
          save_EEPROM(pos+1, modelParam);
          save_EEPROM(pos+1+sizeof(float), height);
        }
        else{Serial.println(F("Ultrasonic Sensor Save Mode: Parameter height incorrect"));}
      }
      else{Serial.println(F("Ultrasonic Sensor Save Mode: Parameter modelParam incorrect"));}
    }
    else{Serial.println(F("Ultrasonic Sensor Save Mode: Parameter model incorrect"));}
  }
  else{Serial.println(F("Ultrasonic Sensor Save Mode: Parameter type incorrect"));}
}

bool chargeUltrasonicParameters(int Type){
  if(Type>=0 && Type<=6){
    int pos = 563+(2+sizeof(float)*3)*Type; // Initial Position to save
    uint8_t filt = EEPROM.read(pos); // Filter
    uint8_t model = EEPROM.read(pos+1+sizeof(float)); // Model
    float paramFilter, paramModel, height; // Float values

    EEPROM.get(pos+1, paramFilter);
    EEPROM.get(pos+2+sizeof(float), paramModel);
    EEPROM.get(pos+2+sizeof(float)*2, height);
    
    if(filt==0){UltraSonic::ptr[Type]->notFilter();} // Not Filter
    else if(filt==1){ 
      // Exponential Filter
      if(paramFilter>0 && paramFilter<1){UltraSonic::ptr[Type]->setExponentialFilter(paramFilter);}
      else{UltraSonic::ptr[Type]->defaultFilter();} // Default Filter
    }
    else if(filt==2){
      // Kalman Filter
      if(paramFilter>0){UltraSonic::ptr[Type]->setKalmanFilter(paramFilter);}
      else{UltraSonic::ptr[Type]->defaultFilter();} // Default Filter
    }

    if(model<0 || model>2 || paramModel<=0 || height<UltraSonic::ptr[Type]->getMaxDist() || !UltraSonic::ptr[Type]->setModel(model, paramModel, height)){
      UltraSonic::ptr[Type]->setModel(1, 26.5, 88);
    }    
    return true;
  }
  else{
    Serial.println(F("Ultrasonic Sensor Charge Parameter: Parameter type incorrect"));
    return false;
  }
}

void pressureSave(int Type, float Press){
  if(Type==0){save_EEPROM(256, Press);}
  else if(Type==1){save_EEPROM(256+sizeof(float), Press);}
  else if(Type==2){save_EEPROM(256+sizeof(float)*2, Press);}
}

void chargePressureParameter(){
  float maxPress, minPress, criticalPress; // Float values

    EEPROM.get(256, maxPress);
    EEPROM.get(256+sizeof(float), minPress);
    EEPROM.get(256+sizeof(float)*2, criticalPress);

    if(max_pressure>0 && max_pressure!=maxPress && !isnan(maxPress)){
      max_pressure = maxPress;
      Serial.print(F("Max Pressure = "));
      Serial.print(max_pressure);
      Serial.println(F(" psi"));
    }
    if(min_pressure>0 && min_pressure!=minPress && !isnan(minPress)){
      min_pressure = minPress;
      Serial.print(F("Min Pressure = "));
      Serial.print(min_pressure);
      Serial.println(F(" psi"));
    }
    if(critical_pressure>0 && critical_pressure!=criticalPress && !isnan(criticalPress)){
      critical_pressure = criticalPress;
      Serial.print(F("Critical Pressure = "));
      Serial.print(critical_pressure);
      Serial.println(F(" psi"));
    }
}
