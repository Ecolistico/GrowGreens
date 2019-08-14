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
  if(irrigationStage==0 && IPC.state==0 && bootParameters){
    initialPreconditions(&startIrrigation); // pass irrigationStage from 0 to 1
  }
  // finishInitialIrrigation() in solenoidValverunAll() pass irrigationStage from 1 to 2
  
  wait4MiddleIrrigation(); // pass irrigationStage from 2 to 3
  
  // If it is time check the middlePreconditions and if they pass startMiddleIrrigation
  if(irrigationStage==3 && MPC.state==0){
    middlePreconditions(&startMiddleIrrigation); // pass irrigationStage from 3 to 4
  }

  // finishMiddleIrrigation() in solenoidValverunAll pass irrigationStage from 4 to 0
  
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



void initialPreconditions(void (*ptr2function)()){
  int count = 0;
  bool decition = false;
  float p1 = pressureSensorNutrition.getValue();
  
  if(!decition && lastSolution!=nextSolution){ // Is it time to change the solution?
    decition = true;
    uint8_t inSol = Recirculation.getIn();
    Recirculation.setIn(WATER);
    Recirculation.moveIn(); // Add bool to avoid recall the function before it finished
    Recirculation.setIn(inSol);
    if(IPC.state==1){IPC.setState(50);}
    else{IPC.setState(30);}
  } else{ count++; }
  
  // Else if it is not enough solution
  /* // Check this line */
  if(!decition && solutionConsumption>Recirculation.getVolKnut()){
    decition = true;
    Compressor.openFreeNut(); // Depressurize Nutrition Kegs
    if(IPC.state==1){IPC.setState(40);}
    else{IPC.setState(20);}
  } else{ count++; }
  
  // Else if it is not enough pressure
  /* // Check this line */
  if(!decition && p1<min_pressure){
    decition = true;
    Compressor.compressNut(); // Compress Nutrition Kegs
    IPC.setState(10);
  } else{ count++; }

  if(count>=3){ ptr2function(); } // Execute some function if all the preconditions are fulfilled
}

void startIrrigation(){
  updateDay(); // Update day in LED modules
  
  /* Pendiente
   *  Recircular agua del ciclo anterior, qué hacer en caso de que la bomba se ecnuentre ocupada
  */
   
  uint8_t nite = inWhatFloorIsNight();
  // If the day change in the floors reorder valves
  if(night!=nite && nite>0){
    night = nite;
    Serial.println(F("Solenoid valves: Creating new routine"));
    solenoidValve::defaultOrder(night-1); // Setting new order
  }

  Compressor.keepConnected(true); // Keep connected nutrition kegs with air tank
  solenoidValve::enableGroup(true); // Enable Valves Group
  irrigationStage = 1; // Pass to the next irrigation Stage
}

void finishInitialIrrigation(){
  // If we are in irrigationStage 1 and the valve it is the 24 into de cycle
  if(irrigationStage==1 && solenoidValve::getActualNumber()==MAX_IRRIGATION_REGIONS*(MAX_FLOOR-1)){
    Compressor.keepConnected(false); // Disconnect nutrition kegs and air tank
    
    float p2 = pressureSensorTank.getValue();
    if(p2<min_pressure){ // Check if pressure is low in air tank
      IPC.setState(1);
    }

    // Substract water consumption from the nutrition kegs volume
    initialPreconditions(&doNothing); // Check initialPreconditions again
    
    solenoidValve::enableGroup(false); // Disable Valves Group
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
  if(!decition && h2oConsumption>Recirculation.getVolKh2o()){
    decition = true;
    Compressor.openFreeH2O(); // Depressurize Water Kegs
    MPC.setState(20);
  } else{ count++; }

  // If there is not enough pressure
  if(!decition && p3<min_pressure){
    decition = true;
    Compressor.compressH2O(); // Compress Water Kegs
  } else{ count++; }

  if(count>=2){ ptr2function(); } // Execute some function if all the preconditions are fulfilled
}

void startMiddleIrrigation(){
  /* Pendiente
   *  Recircular agua del ciclo anterior, qué hacer en caso de que la bomba se ecnuentre ocupada
  */
  solenoidValve::enableGroup(true); // Enable Valves Group
  irrigationStage = 4; // Pass to the next irrigation Stage
}

void finishMiddleIrrigation(){
  // If we are in irrigationStage 4 and the following valve it does not exist then finished the cycle
  if(irrigationStage==4 && solenoidValve::getActualNumber()>=solenoidValve::__TotalActuators){
    // Substract water consumption from the water kegs volumen
    middlePreconditions(&doNothing); // Check middlePreconditions again
    
    solenoidValve::enableGroup(false); // Disable Valves Group
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
      IPC.setState(70);
    }
    // If we detect low pressure
    else if(p1<critical_pressure){
      Serial.println(F("IPC Emergency: There is not enough pressure to continue irrigation process"));
      solenoidValve::enableGroup(false); // Disable Valves Group
      Compressor.compressNut(); // Compress Nutrition Kegs
      IPC.setState(60);
    }
  }

  // If we are in irrigationStage 4 and the system is supposed to be fine
  else if(irrigationStage==4 && MPC.state==0){
    float p3 = pressureSensorWater.getValue();
    // If we detect air in irrigation line
    if(checkWaterIrrigation.getState()==AIR_STATE){
      Serial.println(F("MPC Warning: Probably there is air in irrigation line"));
      MPC.setState(70);
    }
    // If we detect low pressure
    else if(p3<critical_pressure){
      Serial.println(F("MPC Emergency: There is not enough pressure to continue irrigation process"));
      solenoidValve::enableGroup(false); // Disable Valves Group
      Compressor.compressH2O(); // Compress Water Kegs
      MPC.setState(60);
    }
  }
}
