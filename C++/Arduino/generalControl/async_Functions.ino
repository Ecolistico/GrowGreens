/* Falta completar:
 *  irrigationPreconditions():
 *    - Casos de recirculación
 *    - Preparación de nuevas soluciones
 *    - Llenado de agua
 *     
 *  irrigationEmergency():
 *    - Llenar kegs nutrición o agua a falta de agua
 * 
 *  Habilitar riego cuando se termine de llenar/preparar la solución
 */

void async(){
  asyncIrrigation(); // This functions check in what state has to be the solenoids of the kegs
  irrigationPreconditions(); // Enable the Group of solenoids when they reached the preconditions
  irrigationEmergency(); // The emergency situations that stop the irrigation and execute another action to fixed it
}

void asyncIrrigation(){
  bool irrigationState = LOW;
  
  for(int i=0; i<solenoidValve::__TotalActuators; i++){ // Check for all the solenoids
    if(solenoidValve::ptr[i]->getState()){ // If some solenoid is ON
      irrigationState = HIGH;
      if(isDayInThatSolenoid(i)){
        IrrigationKegsNutrition.turnOn();
        IrrigationKegsH2O.turnOff();
      }
      else{
        IrrigationKegsNutrition.turnOff();
        IrrigationKegsH2O.turnOn();
      }
      break;
    }
  }

  if(!irrigationState){
    IrrigationKegsNutrition.turnOff();
    IrrigationKegsH2O.turnOff();
  }
}

void irrigationPreconditions(){
  uint8_t acNumber = solenoidValve::getActualNumber();
  uint8_t nite = inWhatFloorIsNight();
  float h2o_consumption = 0; // Water consumption in the last stage
  float p1 = pressureSensorNutrition.getValue(); // Pressure in kegs nutrition
  float p2 = pressureSensorTank.getValue(); // Pressure in air tank
  float p3 = pressureSensorWater.getValue(); // Pressure in kegs water
  int count = 0; // Restart/continue cycle counter
  
  // Start the cycle
  if(acNumber>=solenoidValve::__TotalActuators && !solenoidValve::isEnableGroup() && IPC==0){
    // Get the water consumption in day floors
    for(int i=0; i<MAX_FLOOR; i++){
      if(i!=nite){h2o_consumption += solenoidValve::getWaterByFloor(i);}
    }

    if(h2o_consumption>=(Recirculation.getVolKnut()/2) && IPC==0){ // Is there enough solution?
      Serial.println(F("There is not enough solution to start irrigation"));
      
      if(solutionIn!=0 && UltraSonic::ptr[solutionIn]->getVolume()>h2o_consumption*1.15){ // Is there enough solution to recirculate
        Serial.println(F("Asking to recirculate"));
        // recirculate(solutionIn, h2o_consumption*1.15) 
      }
      else{ // We have to prepare solution
        Serial.println(F("Asking to prepare solution"));
        // fillSolutionMaker(solutionIn, h2o_consumption*1.15);
        // prepareSolution(solutionIn, UltraSonic::ptr[solutionIn]->getVolume())
      }
      IPC = 1; // Not enough solution
    }
    else{count++;}
    
    if(p1<min_pressure && IPC==0){ // Do we have the correct pressure?
      Serial.println("There is not enough pressure to start irrigation");
      uint8_t compressorMode = Compressor.getMode();
      if(compressorMode==7){
        Serial.println(F("Compressor is already filling water kegs, opening valves to fill everything"));
        Compressor.setMode(8); // Get the correct pressure in all the tanks
      }
      else if(compressorMode!=8 && compressorMode==6){
        Serial.println(F("Using compressor to get the correct pressure in Water Kegs"));
        Compressor.setMode(6); // Get the correct pressure in nutrition kegs  
      }
      else{Serial.println(F("Compressor is already in a process that correct the problem"));}
      
      IPC = 2; // Not enough pressure in solution kegs
    }
    else{count++;}

    /*** Check if this condition is really necessary ***/
    if(abs(p1-p2)>0.05*p1 && IPC==0){ // Is pressure in kegs nutrition equal to pressure in air tank?
      Serial.println("Pressure in nutrition kegs and air tank are different");
      Compressor.setMode(2); // Equal pressure in kegs nutrition and air tank
      IPC = 3; // Not same pressure in solution kegs and air tank
    }
    else{count++;} 
    // If we reached all the pre-conditions then enable de Group
    if(count>=3){
      Serial.println("Preconditions for Nutrition Irrigation Reached");
      updateDay(); // Update days and turn On/Off the LED modules
      // If we change the solution then recharge the paramaters
      if(lastSolutionTimesCharged != solutionIn){
        lastSolutionTimesCharged = solutionIn;
        Serial.println(F("Solenoid valves: Setting Parameters from EEPROM"));
        chargeSolenoidParameters(solutionIn);
      }
      // If now is night in another floor
      if(night!=nite && nite>0){
        night = nite;
        Serial.println(F("Solenoid valves: Creating new routine"));
        solenoidValve::defaultOrder(night-1); // Setting new order
      }
      solenoidValve::enableGroup(true);
    }
  }
 
   // Start the water irrigation
   else if(acNumber==MAX_IRRIGATION_REGIONS*(MAX_FLOOR-1) && !solenoidValve::isEnableGroup() && IPC==0){
    // Get the water consumption in night floor
    h2o_consumption = solenoidValve::getWaterByFloor(nite);
    
    if(h2o_consumption>(Recirculation.getVolKh2o()/2) && IPC==0){ // Is there enough water?
      Serial.println(F("There is not enough water to start irrigation"));
      
      if(UltraSonic::ptr[0]->getVolume()>h2o_consumption*1.15){ // Is there enough water to recirculate
        Serial.println(F("Asking to recirculate"));
        // recirculate(0, h2o_consumption*1.15) 
      }
      else{ // We have to fill kegs with water
        Serial.println(F("Filling water kegs from pipe"));
        // fillH2OKegs(h2o_consumption*1.15);
      }
      IPC = 4; // Not enough water in kegs water
    }
    else{count++;}
    if(p3<min_pressure && IPC==0){ // Do we have the correct pressure?
      Compressor.setMode(7); // Get the correct pressure in water kegs
      IPC = 5; // Not enough pressure in kegs water
    }
    else{count++;}
    // If we reached all the pre-conditions then enable de Group
    if(count>=2){
      Serial.println("Preconditions for Water Irrigation Reached");
      solenoidValve::enableGroup(true); 
    }
  }
  
}

void irrigationEmergency(){
  uint8_t acNumber = solenoidValve::getActualNumber();
  float p1 = pressureSensorNutrition.getValue(); // Pressure in kegs nutrition
  float p3 = pressureSensorWater.getValue(); // Pressure in kegs water

  // 1) If there is air in the line, the solenoid is ON and the Group is enable
  if(checkWaterIrrigation.getState() && solenoidValve::ptr[acNumber]->getState() && solenoidValve::isEnableGroup()){ 
    Serial.println("Air in irrigation line");
    solenoidValve::enableGroup(false); // Disable irrigation
    if(isDayInThatSolenoid(acNumber)){
      // Fill kegs nutrition  
    }
    else{
      // Fill kegs water  
    }
  }

  // If p1<Pcrítica when Nutrition Irrigation
  if(p1<critical_pressure && isDayInThatSolenoid(acNumber) && solenoidValve::isEnableGroup()){
    Serial.println("Nutrition Pressure is under the critical value");
    solenoidValve::enableGroup(false); // Disable irrigation
    Compressor.setMode(6); // Get the correct pressure in nutrition kegs
  }

  // If p3<Pcrítica when Water Irrigation
  else if(p3<critical_pressure && !isDayInThatSolenoid(acNumber) && solenoidValve::isEnableGroup()){
    Serial.println("Water Pressure is under the critical value");
    solenoidValve::enableGroup(false); // Disable irrigation
    Compressor.setMode(7); // Get the correct pressure in water kegs
  }
  
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
  else{ return 0; }
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
