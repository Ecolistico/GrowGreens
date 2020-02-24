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

  Serial.flush();
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

  Serial.flush();
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
  else{ Serial.println(F("error,inWhatFloorIsNight(): Time information not updated yet")); return 0; }
}

void updateDay(){ 
  if(day1.getState()!=day1.isDay(dateHour, dateMinute)){
    if(day1.getState()){
      Serial.println(F("info,Day Started in Floor 1"));
      LED_Mod::turnOn(0);
      }
    else{
      Serial.println(F("info,Night Started in Floor 1"));
      LED_Mod::turnOff(0);
      }
  }
  if(day2.getState()!=day2.isDay(dateHour, dateMinute)){
    if(day2.getState()){
      Serial.println(F("info,Day Started in Floor 2"));
      LED_Mod::turnOn(1);
      }
    else{
      Serial.println(F("info,Night Started in Floor 2"));
      LED_Mod::turnOff(1);
      }
  }
  if(day3.getState()!=day3.isDay(dateHour, dateMinute)){
    if(
      day3.getState()){
        Serial.println(F("info,Day Started in Floor 3"));
        LED_Mod::turnOn(2);
      }
    else{
      Serial.println(F("info,Night Started in Floor 3"));
      LED_Mod::turnOff(2);
      }
  }
  if(day4.getState()!=day4.isDay(dateHour, dateMinute)){
    if(day4.getState()){
      Serial.println(F("info,Day Started in Floor 4"));
      LED_Mod::turnOn(3);
      }
    else{
      Serial.println(F("info,Night Started in Floor 4"));
      LED_Mod::turnOff(3);
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
  // Substract water consumption from the kegs
  Recirculation.addVolKnut(-consumption);
  
  // Erase the water consumption in that valves
  for(int i=0; i<solenoidValve::__TotalActuators; i++){
    pointer = solenoidValve::ptr[i];
    if(pointer->getFloor()!=nite){ pointer->restartH2O(false); }
  }

  if(updateConsumption){ // Update solutionConsumption parameter
    if(consumption>solutionConsumption){ solutionConsumption = consumption; }
    else{ solutionConsumption = (solutionConsumption+consumption)/2; }
    updateSystemState(); // Update system state
  }
}

void substractWaterConsumption(bool updateConsumption = false){
  uint8_t nite = inWhatFloorIsNight()-1;
  float consumption = 0; // Water consumption in the last stage
  solenoidValve *pointer;
  
  // Get the water consumption in night floor
  consumption = solenoidValve::getWaterByFloor(nite);
  // Substract water consumption from the kegs
  Recirculation.addVolKh2o(-consumption);
  
  // Erase the water consumption in that valves
  for(int i=0; i<solenoidValve::__TotalActuators; i++){
    pointer = solenoidValve::ptr[i];
    if(pointer->getFloor()==nite){ pointer->restartH2O(false); }
  }

  if(updateConsumption){ // Update h2oConsumption parameter
    if(consumption>h2oConsumption){ h2oConsumption = consumption; }
    else{ h2oConsumption = (h2oConsumption+consumption)/2; }
    updateSystemState(); // Update system state
  }
}

void rememberState(int ipc, int mpc, int pumpIn, float missedNut, float missedH2O) {
  bool b_ipc = true;
  bool b_mpc = true;

  // Remember pumpIn State
  if(pumpIn>=0 && pumpIn<=4){
    uint8_t inSol = Recirculation.getIn();
    Recirculation.setIn(pumpIn);
    if(Recirculation.moveIn()){ Serial.println(F("Recirculation moveIn(): request success from the last solution before failure")); }
    Recirculation.setIn(inSol);
  }

  // Remember nutrition kegs State
  if(ipc==0){ Serial.println(F("debug,IPC starts in default state"));} // Do nothing
  else if(ipc==1){ Compressor.compressTank(); } // Compress air tank
  else if(ipc==10){ Compressor.compressNut(); } // Compress Nutrition Kegs 
  else if(ipc==20 || ipc==40){
    if(ipc==40) { Compressor.compressTank(); } // Compress air tank
    Compressor.openFreeNut(); // Depressurize Nutrition Kegs 
  }
  else if(ipc==21 || ipc==22){
    Compressor.openFreeNut(); // Depressurize Nutrition Kegs
    float p1 = pressureSensorNutrition.getValue();
    if(p1<=free_pressure){
      uint8_t resp = Recirculation.moveOut(missedNut, NUTRITION_KEGS);
      if(resp==0){
        IPC.setState(250); // Check IPC Process 250
      }
      else if(resp==1){
        Serial.println(F("There is enough solution to fill nutrition kegs with recirculation"));
        IPC.setState(21); // Check IPC Process 21
      }
      else if(resp==2){
        Serial.println(F("info,There is not enough solution to fill nutrition kegs... adding extra water in solutionMaker"));
        IPC.setState(22); // Check IPC Process 22
      }
    }
    else {
      Compressor.openFreeNut(); // Depressurize Nutrition Kegs 
      IPC.setState(20); // Check IPC Process 20
    }
    b_ipc = false;
  }
  else if(ipc==23 || ipc==24 || ipc==43 || ipc==44 || ipc==74 || ipc==75){
    Compressor.openFreeNut(); // Depressurize Nutrition Kegs
    if(ipc==43 || ipc==44){ Compressor.compressTank(); } // Compress air tank
    requestSolution(); // Asking central computer that solutionMaker as to prepare a solution with the next parameters
    if(ipc==24 || ipc==44 || ipc==75){
      if(ipc==24){ IPC.setState(23); } // Check IPC Process 23
      else if(ipc==44){ IPC.setState(43); } // Check IPC Process 43
      else if(ipc==75){ IPC.setState(74); } // Check IPC Process 74
      b_ipc = false;
    }
  }
  else if(ipc==25 || ipc==45 || ipc==76){
    Compressor.openFreeNut(); // Depressurize Nutrition Kegs
    if(ipc==45) { Compressor.compressTank(); } // Compress air tank
    Recirculation.moveSol(); // Empty SMaker -> Kegs_nut
  }
  else if(ipc==30){ 
    uint8_t inSol = Recirculation.getIn();
    Recirculation.setIn(WATER);
    if(Recirculation.moveIn()){ Serial.println(F("Recirculation moveIn(WATER): request success")); }
    Recirculation.setIn(inSol);
  }
  else if(ipc==31 || ipc==35 || ipc==51 || ipc==55){
    if(ipc==51 || ipc==55){ Compressor.compressTank(); } // Compress air tank
    Recirculation.releaseKegs(true); // Get out the solution from nutrition kegs
    checkRecirculation = false; // Disable the air/water sensor
  }
  else if(ipc==32 || ipc==52){
    Recirculation.releaseKegs(true); // Get out the solution from nutrition kegs
    checkRecirculation = false; // Disable the air/water sensor
    if(ipc==32){ IPC.setState(31); } // Check IPC Process 31
    else if(ipc==52){ 
      Compressor.compressTank(); // Compress air tank
      IPC.setState(51); // Check IPC Process 51
    } 
    b_ipc = false;
  }
  else if(ipc==41 || ipc==42){
    Compressor.openFreeNut(); // Depressurize Nutrition Kegs
    Compressor.compressTank(); // Compress air tank
    float p1 = pressureSensorNutrition.getValue();
    if(p1<=free_pressure){
      uint8_t resp = Recirculation.moveOut(missedNut, NUTRITION_KEGS);
      if(resp==0){
        IPC.setState(251); // Check IPC Process 251
      }
      else if(resp==1){
        Serial.println(F("There is enough solution to fill nutrition kegs with recirculation"));
        IPC.setState(41); // Check IPC Process 41
      }
      else if(resp==2){
        Serial.println(F("info,There is not enough solution to fill nutrition kegs... adding extra water in solutionMaker"));
        IPC.setState(42); // Check IPC Process 22
      }
    }
    else {
      IPC.setState(40); // Check IPC Process 40
    }
    b_ipc = false;
  }
  else if(ipc==60){ 
    float p1 = pressureSensorNutrition.getValue();
    if(p1<critical_pressure){
      emergency = true; // Set emergency state
      Serial.println(F("error,IPC Emergency: There is not enough pressure to continue irrigation process"));
      solenoidValve::enableGroup(false); // Disable Valves Group
      Compressor.compressNut(); // Compress Nutrition Kegs
    }
    else{
      IPC.setState(0); // Ignore the emergency
      b_ipc = false;
    }
  }
  else if(ipc==70){ // Emergency Air in Line
    IPC.setState(0); // Ignore the emergency, because we are not sure if it was air or not
    b_ipc = false;
  }
  else if(ipc==71){
    emergency = true; // Set emergency state
    Serial.println(F("error,IPC Emergency: There is air in irrigation line"));
    solenoidValve::enableGroup(false); // Disable Valves Group
    Recirculation.resetVolKnut(); // Volumen in nutrition kegs is 0
    substractSolutionConsumption(false); // Erase flow measurements in day floors without update solutionConsumption
    Compressor.keepConnected(false); // Disconnect nutrition kegs and air tank
    Compressor.openFreeNut(); // Depressurize Nutrition Kegs
  }
  else if(ipc==72 || ipc==73){
    Compressor.openFreeNut(); // Depressurize Nutrition Kegs
    float p1 = pressureSensorNutrition.getValue();
    if(p1<=free_pressure){
      uint8_t resp = Recirculation.moveOut(missedNut, NUTRITION_KEGS);
      if(resp==0){
        IPC.setState(252); // Check IPC Process 252
      }
      else if(resp==1){
        Serial.println(F("There is enough solution to fill nutrition kegs with recirculation"));
        IPC.setState(72); // Check IPC Process 72
      }
      else if(resp==2){
        Serial.println(F("warning,There is not enough solution to fill nutrition kegs... adding extra water in solutionMaker"));
        IPC.setState(73); // Check IPC Process 73
      }
    }
    else {
      Compressor.openFreeNut(); // Depressurize Nutrition Kegs 
      IPC.setState(71); // Check IPC Process 40
    }
    b_ipc = false;
  }
  else if(ipc==250 || ipc==251 || ipc==252){
    Serial.println(F("warning,IPC: Pump was working on another process... waiting a little while"));
  }
  else{ 
    Serial.println(F("critical,IPC invalid value"));
    b_ipc = false;
  }

  // Remember water kegs State
  if(mpc==0){ Serial.println(F("debug,MPC starts in default state"));} // Do nothing
  else if(mpc==10){ Compressor.compressH2O(); } // Compress Water Kegs
  else if(mpc==20){ Compressor.openFreeH2O(); } // Depressurize Water Kegs
  else if(mpc==21 || mpc==22){
    Compressor.openFreeH2O();
    float p3 = pressureSensorWater.getValue();
    if(p3<=free_pressure){
      uint8_t lastOut = Recirculation.getOut();
      Recirculation.setOut(WATER);
      uint8_t resp = Recirculation.moveOut(missedH2O, WATER_KEGS);
      Recirculation.setOut(lastOut);
      if(resp==0){
        MPC.setState(250); // Check MPC Process 250
      }
      else if(resp==1){
        Serial.println(F("There is enough solution to fill water kegs with recirculation"));
        MPC.setState(21); // Check MPC Process 21
      }
      else if(resp==2){
        Serial.println(F("warning,There is not enough solution to fill water kegs... adding extra water"));
        MPC.setState(22); // Check MPC Process 22
      }
    }
    else {
      Compressor.openFreeH2O(); // Depressurize Water Kegs
      MPC.setState(20); // Check MPC Process 20
    }
    b_mpc = false;
  }
  else if(mpc==60){
    float p3 = pressureSensorWater.getValue();
    if(p3<critical_pressure){
      emergency = true; // Set emergency state
      Serial.println(F("error,MPC Emergency: There is not enough pressure to continue irrigation process"));
      solenoidValve::enableGroup(false); // Disable Valves Group
      Compressor.compressH2O(); // Compress Water Kegs
    }
  }
  else if(mpc==70){ // Emergency Air in Line
    MPC.setState(0); // Ignore the emergency, because we are not sure if it was air or not
    b_mpc = false;
  }
  else if(mpc==71){
    emergency = true; // Set emergency state
    Serial.println(F("error,MPC Emergency: There is air in irrigation line"));
    solenoidValve::enableGroup(false); // Disable Valves Group
    Recirculation.resetVolKh2o(); // Volumen in water kegs is 0
    substractWaterConsumption(false); // Erase flow measurements in night floor without update h2oConsumption
    Compressor.openFreeH2O(); // Depressurize Water Kegs
  }
  else if(mpc==72 || mpc==73){
    Compressor.openFreeH2O(); // Depressurize Water Kegs
    float p3 = pressureSensorWater.getValue();
    if(p3<=free_pressure){
      uint8_t lastOut = Recirculation.getOut();
      Recirculation.setOut(WATER);
      uint8_t resp = Recirculation.moveOut(missedH2O, WATER_KEGS);
      Recirculation.setOut(lastOut);
      if(resp==0){
        MPC.setState(252); // Check MPC Process 252
      }
      else if(resp==1){
        Serial.println(F("There is enough solution to fill water kegs with recirculation"));
        MPC.setState(72); // Check MPC Process 72
      }
      else if(resp==2){
        Serial.println(F("warning,There is not enough solution to fill water kegs... adding extra water"));
        MPC.setState(73); // Check MPC Process 73
      }
    }
    else if(mpc==250 || mpc==252){
      Serial.println(F("warning,MPC: Pump was working on another process... waiting a little while"));
    }
    else {
      MPC.setState(71); // Check MPC Process 71
    }
    b_mpc = false;
  }
  else{ 
    Serial.println(F("critical,MPC invalid value"));
    b_mpc = false;
  }

  if(b_ipc){ IPC.setState(ipc); } // Save IPC
  if(b_mpc){ MPC.setState(mpc); } // Save MPC
  if(!b_ipc || !b_mpc){ updateSystemState(); } // Update System State if we made some changes
}
