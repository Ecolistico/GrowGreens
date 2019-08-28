void solenoidValverunAll(){
  for(int i=0; i<solenoidValve::__TotalActuators; i++){
    // initialPreconditions() in async() pass irrigationStage from 0 to 1
    finishInitialIrrigation(); // pass irrigationStage from 1 to 2
    // wait4MiddleIrrigation() in async() pass irrigationStage from 2 to 3
    // middlePreconditions() in async() pass irrigationStage from 3 to 4
    finishMiddleIrrigation(); // pass irrigationStage from 4 to 0
    solenoidValve::ptr[i]->run();
  }
}

void async(){
  asyncIrrigation(); // This functions check in what state has to be the valves of both kegs

  // If it is time check the initialPreconditions and if they pass startIrrigation
  if(irrigationStage==0 && IPC.state==0 && bootParameters && firstHourUpdate){
    initialPreconditions(true, &startIrrigation); // pass irrigationStage from 0 to 1. true parameter means it check before irrigation starts
  }
  // finishInitialIrrigation() in solenoidValverunAll() pass irrigationStage from 1 to 2
  
  wait4MiddleIrrigation(); // pass irrigationStage from 2 to 3
  
  // If it is time check the middlePreconditions and if they pass startMiddleIrrigation
  if(irrigationStage==3 && MPC.state==0){
    middlePreconditions(&startMiddleIrrigation); // pass irrigationStage from 3 to 4
  }

  // finishMiddleIrrigation() in solenoidValverunAll pass irrigationStage from 4 to 0

  runIPC(); // Correct state for nutrition irrigation
  runMPC(); // Correct state for water irrigation
  irrigationEmergency(); // Check for emergency situations that stop the irrigation and execute a process to fixed it
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



void initialPreconditions(bool before, void (*ptr2function)()){
  int count = 0;
  bool decition = false;
  float p1 = pressureSensorNutrition.getValue();
  
  if(lastSolution!=Irrigation.getSolution()){ // Is it time to change the solution?
    decition = true;
    lastSolution = Irrigation.getSolution(); // Update solution

    if(before){
      uint8_t inSol = Recirculation.getIn();
      Recirculation.setIn(WATER);
      if(Recirculation.moveIn()){ Serial.println(F("Recirculation moveIn(): request succes")); }
      else{ Serial.println(F("Recirculation moveIn(): pumpIn already working")); }
      Recirculation.setIn(inSol);
      
      IPC.setState(30); // Check IPC Process 30
    }
    else{
      Recirculation.releaseKegs(true); // Get out the solution from nutrition kegs
      checkRecirculation = false; // Disable the air/water sensor
      if(IPC.state==1){IPC.setState(51);} // Check IPC Process 51
      else{IPC.setState(31);} // Check IPC Process 31
    }
  } else{ count++; }
  
  // Else if it is not enough solution
  if(!decition && solutionConsumption>Recirculation.getVolKnut()){
    decition = true;
    Compressor.openFreeNut(); // Depressurize Nutrition Kegs
    if(IPC.state==1){IPC.setState(40);} // Check IPC Process 40
    else{IPC.setState(20);} // Check IPC Process 20
  } else{ count++; }
  
  // Else if it is not enough pressure
  if(!decition && p1<min_pressure){
    decition = true;
    Compressor.compressNut(); // Compress Nutrition Kegs
    IPC.setState(10); // Check IPC Process 10
  } else{ count++; }

  if(count>=3){ ptr2function(); } // Execute some function if all the preconditions are fulfilled
}

void startIrrigation(){
  updateDay(); // Update day in LED modules

  uint8_t inSol = Recirculation.getIn();
  Recirculation.setIn(WATER);
  if(Recirculation.moveIn()){ Serial.println(F("Recirculation moveIn(): request succes")); }
  else{ Serial.println(F("Recirculation moveIn(): pumpIn already working")); }
  Recirculation.setIn(inSol);
   
  uint8_t nite = inWhatFloorIsNight();
  // If the day change in the floors reorder valves
  if(night!=nite && nite>0){
    night = nite;
    Serial.println(F("Solenoid valves: Creating new routine"));
    solenoidValve::defaultOrder(night-1); // Setting new order
  }

  Compressor.keepConnected(true); // Keep connected nutrition kegs with air tank
  solenoidValve::enableGroup(true); // Enable Valves Group
  Irrigation.whatSolution(dateHour, dateMinute); // Update the solution parameter
  irrigationStage = 1; // Pass to the next irrigation Stage
}

void finishInitialIrrigation(){
  // If we are in irrigationStage 1 and the valve it is the 24 into de cycle
  if(irrigationStage==1 && solenoidValve::getActualNumber()==MAX_IRRIGATION_REGIONS*(MAX_FLOOR-1)){
    Compressor.keepConnected(false); // Disconnect nutrition kegs and air tank
    substractSolutionConsumption(!emergency); // Substract water consumption from the nutrition kegs volume and set consumption in zero
    float p2 = pressureSensorTank.getValue();
    if(p2<min_pressure){ // Check if pressure is low in air tank
      Compressor.compressTank(); // Compress air tank
      IPC.setState(1); // Check IPC Process 1
    }
    
    initialPreconditions(false, &doNothing); // Check initialPreconditions again. false parameter means it check before irrigation finish
    solenoidValve::enableGroup(false); // Disable Valves Group
    emergency = false; // Restart emergency variable
    irrigationStage = 2; // Pass to the next irrigation Stage
  }
}

void wait4MiddleIrrigation(){
  // // If we are in irrigationStage 2
  if(irrigationStage==2){
    unsigned long TimeOn = 0;
    unsigned long TotalTimeOn = 0;
    solenoidValve *pointer;
    for(int i=0; i<solenoidValve::__TotalActuators; i++){
      pointer = solenoidValve::ptr[i];
      if(pointer->getOrder()<solenoidValve::getActualNumber() && pointer->isEnable()){ TimeOn += pointer->getTimeOn(); }
      if(pointer->isEnable()){ TotalTimeOn += pointer->getTimeOn(); }
    }

    pointer = solenoidValve::ptr[0];
    // If already pass 3/4 of the cycle time off since the stage 1 should had finished
    if(pointer->getTime()>=TimeOn+(solenoidValve::getCycleTime()-TotalTimeOn)*0.75){
      irrigationStage = 3; // Pass to the next irrigation Stage
    }
  }
}

void middlePreconditions(void (*ptr2function)()){
  int count = 0;
  bool decition = false;
  float p3 = pressureSensorWater.getValue();

  // If there is not enough water
  if(h2oConsumption>Recirculation.getVolKh2o()){
    decition = true;
    Compressor.openFreeH2O(); // Depressurize Water Kegs
    MPC.setState(20); // Check MPC Process 20
  } else{ count++; }

  // If there is not enough pressure
  if(!decition && p3<min_pressure){
    decition = true;
    Compressor.compressH2O(); // Compress Water Kegs
    MPC.setState(10); // Check MPC Process 10
  } else{ count++; }

  if(count>=2){ ptr2function(); } // Execute some function if all the preconditions are fulfilled
}

void startMiddleIrrigation(){
  if(Recirculation.moveIn()){ Serial.println(F("Recirculation moveIn(): request succes")); }
  else{ Serial.println(F("Recirculation moveIn(): pumpIn already working")); }
      
  solenoidValve::enableGroup(true); // Enable Valves Group
  irrigationStage = 4; // Pass to the next irrigation Stage
}

void finishMiddleIrrigation(){
  // If we are in irrigationStage 4 and the following valve it does not exist then finished the cycle
  if(irrigationStage==4 && solenoidValve::getActualNumber()>=solenoidValve::__TotalActuators){
    substractWaterConsumption(!emergency); // Substract water consumption from the water kegs volume and set consumption in zero
    middlePreconditions(&doNothing); // Check middlePreconditions again
    solenoidValve::enableGroup(false); // Disable Valves Group
    emergency = false; // Restart emergency variable
    irrigationStage = 0; // Restart irrigation Process
  }
}

void doNothing(){} // Pass as it as reference parameter when initial or middle Preconditions are checked but not actions are expected

void irrigationEmergency(){
  // If we are in irrigationStage 1 and the system is supposed to be fine
  if(irrigationStage==1 && IPC.state==0){
    float p1 = pressureSensorNutrition.getValue();
    // If we detect air in irrigation line
    if(checkWaterIrrigation.getState()==AIR_STATE){
      Serial.println(F("IPC Warning: Probably there is air in irrigation line"));
      IPC.setState(70); // Check IPC Process 70
    }
    // If we detect low pressure
    else if(p1<critical_pressure){
      emergency = true; // Set emergency state
      Serial.println(F("IPC Emergency: There is not enough pressure to continue irrigation process"));
      solenoidValve::enableGroup(false); // Disable Valves Group
      Compressor.compressNut(); // Compress Nutrition Kegs
      IPC.setState(60); // Check IPC Process 60
    }
  }

  // If we are in irrigationStage 4 and the system is supposed to be fine
  else if(irrigationStage==4 && MPC.state==0){
    float p3 = pressureSensorWater.getValue();
    // If we detect air in irrigation line
    if(checkWaterIrrigation.getState()==AIR_STATE){
      Serial.println(F("MPC Warning: Probably there is air in irrigation line"));
      MPC.setState(70); // Check MPC Process 70
    }
    // If we detect low pressure
    else if(p3<critical_pressure){
      emergency = true; // Set emergency state
      Serial.println(F("MPC Emergency: There is not enough pressure to continue irrigation process"));
      solenoidValve::enableGroup(false); // Disable Valves Group
      Compressor.compressH2O(); // Compress Water Kegs
      MPC.setState(60); // Check IPC Process 60
    }
  }
}

void runIPC(){
  if(IPC.state==10){ // Compress nutrition kegs
    float p1 = pressureSensorNutrition.getValue();
    if(p1>=max_pressure){
      if(MPC.state==10 || MPC.state==60){ Compressor.Off(); Compressor.compressH2O(); } // Compress water kegs
      else{ Compressor.Off(); } // Turn off the compressor
      IPC.setState(0); // Return to normal state
    }
  }

  else if(IPC.state==20){ // Depressurize nutrition Kegs
    float p1 = pressureSensorNutrition.getValue();
    if(p1<=10){
      uint8_t resp = Recirculation.moveOut(solutionConsumption*1.15, NUTRITION_KEGS);
      if(resp==0){
        Serial.println(F("PumpOut is working on another process, please wait until it finished"));
        IPC.setState(250); // Check IPC Process 250
      }
      else if(resp==1){
        Serial.println(F("There is enough solution to fill nutrition kegs with recirculation"));
        IPC.setState(21); // Check IPC Process 21
      }
      else if(resp==2){
        Serial.println(F("There is not enough solution to fill nutrition kegs, adding extra water in solutionMaker"));
        IPC.setState(22); // Check IPC Process 22
      }
    }
  }

  else if(IPC.state==21){ // PumpOut working on fill nutrition kegs
    if(!Recirculation.getOutPump()){
      Compressor.closeFreeNut(); // Close Free Pressure Valve in Nutrition Kegs
      Compressor.compressNut(); // Compress nutrition kegs
      IPC.setState(10); // Check IPC Process 10
    }
  }

  else if(IPC.state==22){ // PumpOut and FillNutValve working on fill solutionMaker
    if(!Recirculation.getOutPump() && !Recirculation.getFSolValve()){
      requestSolution(); // Asking central computer that solutionMaker as to prepare a solution with the next parameters
      IPC.setState(23); // Check IPC Process 23
    }
  }

  else if(IPC.state==23){ // Waiting for central computer to confirm request
    if(CC.state==1){
      Serial.println(F("Central Computer confirms that solutionMaker accept the request of prepare a new solution"));
      IPC.setState(24); // Check IPC Process 24
    }
    else if(millis()-IPC.actualTime>=10000){
      IPC.setState(22); // Check IPC Process 22
    }
  }

  else if(IPC.state==24){ // Waiting for central computer to confirm solution is finished
    if(CC.state==2){
      Recirculation.moveSol();
      IPC.setState(25); // Check IPC Process 25
      CC.setState(0); // Return Computer Control to normal state
    }
  }

  else if(IPC.state==25){ // PumpSol filling nutrition kegs
    if(!Recirculation.getSolPump()){
      Compressor.closeFreeNut(); // Close Free Pressure Valve in Nutrition Kegs
      Compressor.compressNut(); // Compress nutrition kegs
      IPC.setState(10); // Check IPC Process 10
    }
  }

  else if(IPC.state==30){ // Changing solution. Moving water from recirculation
    if(!Recirculation.getInPump()){
      Recirculation.releaseKegs(true); // Get out the solution from nutrition kegs
      checkRecirculation = false; // Disable the air/water sensor
      IPC.setState(31); // Check IPC Process 31
    }
  }

  else if(IPC.state==31){ // Waiting 6 seconds before enable water/air sensor
    if(millis()-IPC.actualTime>=6000){
      checkRecirculation = true; // Enable the air/water sensor
      IPC.setState(32); // Check IPC Process 32
    }
  }

  else if(IPC.state==32){ // Getting out solution from nutrition kegs to recirculation
    // In theory, Recirculation.run closes the valve when the sensor detect air in line
    if(!Recirculation.getRSolValve()){
      Compressor.openFreeNut(); // Depressurize Nutrition Kegs
      if(Recirculation.moveIn()){ Serial.println(F("Recirculation moveIn(): request succes")); }
      else{ Serial.println(F("Recirculation moveIn(): pumpIn already working")); }
      chargeSolenoidParameters(Irrigation.getSolution()); // Update irrigation parameters
      // Update recirculation parameters
      Recirculation.setIn(Irrigation.getSolution()-1); // In Recirculation Sol1 = 0, etc.
      Recirculation.setOut(Irrigation.getSolution()-1); // In Recirculation Sol1 = 0, etc.
      IPC.setState(20); // Check IPC Process 20
    }
  }

  else if(IPC.state==40){ // Depressurize nutrition Kegs/Compress air tank
    float p1 = pressureSensorNutrition.getValue();
    float p2 = pressureSensorTank.getValue();
    if(p2>=max_pressure){
      if(MPC.state==10 || MPC.state==60){ Compressor.Off(); Compressor.compressH2O(); } // Compress water kegs
      else{ Compressor.Off(); } // Turn off the compressor
      IPC.setState(20); // Check IPC Process 20
    }
    else if(p1<=10){
      uint8_t resp = Recirculation.moveOut(solutionConsumption*1.15, NUTRITION_KEGS);
      if(resp==0){
        Serial.println(F("PumpOut is working on another process, please wait until it finished"));
        IPC.setState(251); // Check IPC Process 251
      }
      else if(resp==1){
        Serial.println(F("There is enough solution to fill nutrition kegs with recirculation"));
        IPC.setState(41); // Check IPC Process 41
      }
      else if(resp==2){
        Serial.println(F("There is not enough solution to fill nutrition kegs, adding extra water in solutionMaker"));
        IPC.setState(42); // Check IPC Process 22
      }
    }
  }

  else if(IPC.state==41){ // PumpOut working on fill nutrition kegs/Compress air tank
    float p2 = pressureSensorTank.getValue();
    if(p2>=max_pressure){
      if(MPC.state==10 || MPC.state==60){ Compressor.Off(); Compressor.compressH2O(); } // Compress water kegs
      else{ Compressor.Off(); } // Turn off the compressor
      IPC.setState(21); // Check IPC Process 21
    }
    else if(!Recirculation.getOutPump()){
      Compressor.closeFreeNut(); // Close Free Pressure Valve in Nutrition Kegs
      Compressor.compressNut(); // Compress nutrition kegs
      IPC.setState(10); // Check IPC Process 10
    }
  }

  else if(IPC.state==42){ // PumpOut and FillNutValve working on fill solutionMaker/Compress air tank
    float p2 = pressureSensorTank.getValue();
    if(p2>=max_pressure){
      if(MPC.state==10 || MPC.state==60){ Compressor.Off(); Compressor.compressH2O(); } // Compress water kegs
      else{ Compressor.Off(); } // Turn off the compressor
      IPC.setState(22); // Check IPC Process 22
    }
    else if(!Recirculation.getOutPump() && !Recirculation.getFSolValve()){
      requestSolution(); // Asking central computer that solutionMaker as to prepare a solution with the next parameters
      IPC.setState(43); // Check IPC Process 43
    }
  }

  else if(IPC.state==43){ // Waiting for central computer to confirm request/Compress air tank
    float p2 = pressureSensorTank.getValue();
    if(p2>=max_pressure){
      if(MPC.state==10 || MPC.state==60){ Compressor.Off(); Compressor.compressH2O(); } // Compress water kegs
      else{ Compressor.Off(); } // Turn off the compressor
      IPC.setState(23); // Check IPC Process 23
    }
    else if(CC.state==1){
      Serial.println(F("Central Computer confirms that solutionMaker accept the request of prepare a new solution"));
      IPC.setState(44); // Check IPC Process 44
    }
    else if(millis()-IPC.actualTime>=10000){
      IPC.setState(42); // Check IPC Process 42
    }
  }

  else if(IPC.state==44){ // Waiting for central computer to confirm solution is finished/Compress air tank
    float p2 = pressureSensorTank.getValue();
    if(p2>=max_pressure){
      if(MPC.state==10 || MPC.state==60){ Compressor.Off(); Compressor.compressH2O(); } // Compress water kegs
      else{ Compressor.Off(); } // Turn off the compressor
      IPC.setState(24); // Check IPC Process 24
    }
    else if(CC.state==2){
      Recirculation.moveSol();
      IPC.setState(45); // Check IPC Process 45
      CC.setState(0); // Return Computer Control to normal state
    }
  }
  
  else if(IPC.state==45){ // PumpSol filling nutrition kegs/Compress air tank
    float p2 = pressureSensorTank.getValue();
    if(p2>=max_pressure){
      if(MPC.state==10 || MPC.state==60){ Compressor.Off(); Compressor.compressH2O(); } // Compress water kegs
      else{ Compressor.Off(); } // Turn off the compressor
      IPC.setState(25); // Check IPC Process 25
    }
    else if(!Recirculation.getSolPump()){
      Compressor.closeFreeNut(); // Close Free Pressure Valve in Nutrition Kegs
      Compressor.compressNut(); // Compress nutrition kegs
      IPC.setState(10); // Check IPC Process 10
    }
  }

  else if(IPC.state==51){ // Waiting 6 seconds before enable water/air sensor/Compress air tank
    float p2 = pressureSensorTank.getValue();
    if(p2>=max_pressure){
      if(MPC.state==10 || MPC.state==60){ Compressor.Off(); Compressor.compressH2O(); } // Compress water kegs
      else{ Compressor.Off(); } // Turn off the compressor
      IPC.setState(31); // Check IPC Process 31
    }
    else if(millis()-IPC.actualTime>=6000){
      checkRecirculation = true; // Enable the air/water sensor
      IPC.setState(52); // Check IPC Process 52
    }
  }

  else if(IPC.state==52){ // Getting out solution from nutrition kegs to recirculation/Compress air tank
    float p2 = pressureSensorTank.getValue();
    if(p2>=max_pressure){
      if(MPC.state==10 || MPC.state==60){ Compressor.Off(); Compressor.compressH2O(); } // Compress water kegs
      else{ Compressor.Off(); } // Turn off the compressor
      IPC.setState(32); // Check IPC Process 32
    }
    // In theory, Recirculation.run closes the valve when the sensor detect air in line
    else if(!Recirculation.getRSolValve()){
      Compressor.openFreeNut(); // Depressurize Nutrition Kegs
      if(Recirculation.moveIn()){ Serial.println(F("Recirculation moveIn(): request succes")); }
      else{ Serial.println(F("Recirculation moveIn(): pumpIn already working")); }
      chargeSolenoidParameters(Irrigation.getSolution()); // Update irrigation parameters
      // Update recirculation parameters
      Recirculation.setIn(Irrigation.getSolution()-1); // In Recirculation Sol1 = 0, etc.
      Recirculation.setOut(Irrigation.getSolution()-1); // In Recirculation Sol1 = 0, etc.
      IPC.setState(40); // Check IPC Process 40
    }
  }

  else if(IPC.state==60){ // Emergency: Low Pressure
    float p1 = pressureSensorNutrition.getValue();
    if(p1>=min_pressure){
      if(MPC.state==10 || MPC.state==60){ Compressor.Off(); Compressor.compressH2O(); } // Compress water kegs
      else{ Compressor.Off(); } // Turn off the compressor
      solenoidValve::enableGroup(true); // Enable Valves Group
      IPC.setState(0); // Return to normal state      
    }
  }

  else if(IPC.state==70){ // Warning: Possible air in line
    if(millis()-IPC.actualTime>=5000){
      if(checkWaterIrrigation.getState()==AIR_STATE){
        if(irrigationStage==1){
          emergency = true; // Set emergency state
          Serial.println(F("IPC Emergency: There is air in irrigation line"));
          solenoidValve::enableGroup(false); // Disable Valves Group
          Recirculation.resetVolKnut(); // Volumen in nutrition kegs is 0
          substractSolutionConsumption(false); // Erase flow measurements in day floors without update solutionConsumption
          Compressor.keepConnected(false); // Disconnect nutrition kegs and air tank
          Compressor.openFreeNut(); // Depressurize Nutrition Kegs
          IPC.setState(71); // Check IPC Process 71
        }
        else{
          Serial.println(F("IPC React too slow: The irrigationStage finished before we can take some action"));
          IPC.setState(0); // Return to normal state
        }
      }
      else{
        Serial.println(F("IPC False Alarm: There is not air in irrigation line"));
        IPC.setState(0); // Return to normal state
      }
    }
  }

  else if(IPC.state==71){ // Emergency: Air in line. Depressurize Water Kegs
    float p1 = pressureSensorNutrition.getValue();
    if(p1<=10){
      uint8_t resp = Recirculation.moveOut(solutionConsumption*1.15, NUTRITION_KEGS);
      if(resp==0){
        Serial.println(F("PumpOut is working on another process, please wait until it finished"));
        IPC.setState(252); // Check IPC Process 252
      }
      else if(resp==1){
        Serial.println(F("There is enough solution to fill nutrition kegs with recirculation"));
        IPC.setState(72); // Check IPC Process 72
      }
      else if(resp==2){
        Serial.println(F("There is not enough solution to fill nutrition kegs, adding extra water in solutionMaker"));
        IPC.setState(73); // Check IPC Process 73
      }
    }
  }
  
  else if(IPC.state==72){ // Emergency: Air in line. PumpOut working on fill nutrition kegs
    if(!Recirculation.getOutPump()){
      Compressor.closeFreeNut(); // Close Free Pressure Valve in Nutrition Kegs
      Compressor.keepConnected(true); // Connect nutrition kegs and air tank
      Compressor.compressNut(); // Compress nutrition kegs
      IPC.setState(60); // Check IPC Process 60
    }
  }

  else if(IPC.state==73){ // Emergency: Air in line. PumpOut working on fill solutionMaker
    if(!Recirculation.getOutPump() && !Recirculation.getFSolValve()){
      requestSolution(); // Asking central computer that solutionMaker as to prepare a solution with the next parameters
      IPC.setState(74); // Check IPC Process 74
    }
  }

  else if(IPC.state==74){ // Emergency: Air in line. Waiting for central computer to confirm request
    if(CC.state==1){
      Serial.println(F("Central Computer confirms that solutionMaker accept the request of prepare a new solution"));
      IPC.setState(75); // Check IPC Process 75
    }
    else if(millis()-IPC.actualTime>=10000){
      IPC.setState(73); // Check IPC Process 73
    }
  }

  else if(IPC.state==75){ // Emergency: Air in line. Waiting for central computer to confirm solution is finished
    if(CC.state==2){
      Recirculation.moveSol();
      IPC.setState(76); // Check IPC Process 76
      CC.setState(0); // Return Computer Control to normal state
    }
  }

  else if(IPC.state==76){ // Emergency: Air in line. PumpSol filling nutrition kegs
    if(!Recirculation.getSolPump()){
      Compressor.closeFreeNut(); // Close Free Pressure Valve in Nutrition Kegs
      Compressor.keepConnected(true); // Connect nutrition kegs and air tank
      Compressor.compressNut(); // Compress nutrition kegs
      IPC.setState(60); // Check IPC Process 60
    }
  }
  
  else if(IPC.state==250){ // PumpOut working on another process
    if(!Recirculation.getOutPump()){
      IPC.setState(20); // Check IPC Process 20
    }
  }

  else if(IPC.state==251){ // PumpOut working on another process/Compress air tank
    float p2 = pressureSensorTank.getValue();
    if(p2>=max_pressure){
      if(MPC.state==10 || MPC.state==60){ Compressor.Off(); Compressor.compressH2O(); } // Compress water kegs
      else{ Compressor.Off(); } // Turn off the compressor
      IPC.setState(250); // Check IPC Process 250
    }
    else if(!Recirculation.getOutPump()){
      IPC.setState(40); // Check IPC Process 40
    }
  }

  else if(IPC.state==252){ // Emergency: Air in line. PumpOut working on another process
    if(!Recirculation.getOutPump()){
      IPC.setState(71); // Check IPC Process 71
    }
  }

}

void runMPC(){
  if(MPC.state==10){ // Compress water kegs
    float p3 = pressureSensorWater.getValue();
    if(p3>=max_pressure){
      if(IPC.state==10 || IPC.state==60){  Compressor.Off(); Compressor.compressNut(); } // Compress nutrition kegs
      else if(IPC.state==40 || IPC.state==41 || IPC.state==42 || IPC.state==43 || IPC.state==44 || 
      IPC.state==45 || IPC.state==51 || IPC.state==52 || IPC.state==251){  Compressor.Off(); Compressor.compressTank(); } // Compress just air tank
      else{ Compressor.Off(); } // Turn off the compressor
      MPC.setState(0); // Return to normal state
    }
  }

  else if(MPC.state==20){ // Depressurize Water Kegs
    float p3 = pressureSensorWater.getValue();
    if(p3<=10){
      uint8_t lastOut = Recirculation.getOut();
      Recirculation.setOut(WATER);
      uint8_t resp = Recirculation.moveOut(h2oConsumption*1.15, WATER_KEGS);
      Recirculation.setOut(lastOut);
      if(resp==0){
        Serial.println(F("PumpOut is working on another process, please wait until it finished"));
        MPC.setState(250); // Check MPC Process 250
      }
      else if(resp==1){
        Serial.println(F("There is enough solution to fill water kegs with recirculation"));
        MPC.setState(21); // Check MPC Process 21
      }
      else if(resp==2){
        Serial.println(F("There is not enough solution to fill water kegs, adding extra water"));
        MPC.setState(22); // Check MPC Process 22
      }
    }
  }

  else if(MPC.state==21){ // PumpOut working on fill water kegs
    if(!Recirculation.getOutPump()){
      Compressor.closeFreeH2O(); // Close Free Pressure Valve in Water Kegs
      Compressor.compressH2O(); // Compress water kegs
      MPC.setState(10); // Check MPC Process 10
    }
  }

  else if(MPC.state==22){ // PumpOut and FillH2OValve working on fill water kegs
    if(!Recirculation.getOutPump() && !Recirculation.getFH2OValve()){
      Compressor.closeFreeH2O(); // Close Free Pressure Valve in Water Kegs
      Compressor.compressH2O(); // Compress water kegs
      MPC.setState(10); // Check MPC Process 10
    }
  }

  else if(MPC.state==60){ // Emergency: Low Pressure
    float p3 = pressureSensorWater.getValue();
    if(p3>=min_pressure){
      if(IPC.state==10 || IPC.state==60){  Compressor.Off(); Compressor.compressNut(); } // Compress nutrition kegs
      else if(IPC.state==40 || IPC.state==41 || IPC.state==42 || IPC.state==43 || IPC.state==44 || 
      IPC.state==45 || IPC.state==51 || IPC.state==52 || IPC.state==251){  Compressor.Off(); Compressor.compressTank(); } // Compress just air tank
      else{ Compressor.Off(); } // Turn off the compressor
      solenoidValve::enableGroup(true); // Enable Valves Group
      MPC.setState(0); // Return to normal state
    }
  }

  else if(MPC.state==70){ // Warning: Possible air in line
    if(millis()-MPC.actualTime>=5000){
      if(checkWaterIrrigation.getState()==AIR_STATE){
        if(irrigationStage==4){
          emergency = true; // Set emergency state
          Serial.println(F("MPC Emergency: There is air in irrigation line"));
          solenoidValve::enableGroup(false); // Disable Valves Group
          Recirculation.resetVolKh2o(); // Volumen in water kegs is 0
          substractWaterConsumption(false); // Erase flow measurements in night floor without update h2oConsumption
          Compressor.openFreeH2O(); // Depressurize Water Kegs
          MPC.setState(71); // Check MPC Process 71
        }
        else{
          Serial.println(F("MPC React too slow: The irrigationStage finished before we can take some action"));
          MPC.setState(0); // Return to normal state
        }
      }
      else{
        Serial.println(F("MPC False Alarm: There is not air in irrigation line"));
        MPC.setState(0); // Return to normal state
      }
    }
  }

  else if(MPC.state==71){ // Emergency: Air in line. Depressurize Water Kegs
    float p3 = pressureSensorWater.getValue();
    if(p3<=10){
      uint8_t lastOut = Recirculation.getOut();
      Recirculation.setOut(WATER);
      uint8_t resp = Recirculation.moveOut(h2oConsumption*1.15, WATER_KEGS);
      Recirculation.setOut(lastOut);
      if(resp==0){
        Serial.println(F("PumpOut is working on another process, please wait until it finished"));
        MPC.setState(252); // Check MPC Process 252
      }
      else if(resp==1){
        Serial.println(F("There is enough solution to fill water kegs with recirculation"));
        MPC.setState(72); // Check MPC Process 72
      }
      else if(resp==2){
        Serial.println(F("There is not enough solution to fill water kegs, adding extra water"));
        MPC.setState(73); // Check MPC Process 73
      }
    }
  }

  else if(MPC.state==72){ // Emergency: Air in line. PumpOut working on fill water kegs
    if(!Recirculation.getOutPump()){
      Compressor.closeFreeH2O(); // Close Free Pressure Valve in Water Kegs
      Compressor.compressH2O(); // Compress water kegs
      MPC.setState(60); // Check MPC Process 60
    }
  }

  else if(MPC.state==73){ // Emergency: Air in line. PumpOut and FillH2OValve working on fill water kegs
    if(!Recirculation.getOutPump() && !Recirculation.getFH2OValve()){
      Compressor.closeFreeH2O(); // Close Free Pressure Valve in Water Kegs
      Compressor.compressH2O(); // Compress water kegs
      MPC.setState(60); // Check MPC Process 60
    }
  }
  
  else if(MPC.state==250){ // PumpOut working on another process
    if(!Recirculation.getOutPump()){
      MPC.setState(20); // Check MPC Process 20
    }
  }

  else if(MPC.state==252){ // Emergency: Air in line. PumpOut working on another process
    if(!Recirculation.getOutPump()){
      MPC.setState(71); // Check MPC Process 71
    }
  }
}
