void preconditions() {
  // Print sensor information before each irrigation cycle
  for(int i = 0; i<sconfig.analogs; i++) mySensors->_myAnalogs[i]->printRead();
  for(int i = 0; i<sconfig.scales; i++) mySensors->_myScales[i]->printRead();

  // Do we have enough water?
  if(mySensors->_myScales[0]->getWeight()-mySensors->_myScales[0]->getMinWeight()>h2oConsumption){
    // Do we have enough pressure?
    if(mySensors->_myAnalogs[1]->getValue()>pconfig.min_pressure) startIrrigation();
    else{
      myIrrigation->pressurizeAll();
      controlState.setState(10);
      updateSystemState();
    }
  }
  else{
    myIrrigation->depressurizeWater();
    controlState.setState(20);
    updateSystemState();
  }
}

void emergencyConditions() {
  if(mySensors->_myScales[0]->getWeight()>mySensors->_myScales[0]->getMinWeight()){
    if(mySensors->_myAnalogs[1]->getValue()<=pconfig.critical_pressure){
      emergencyFlag = true;
      myValves->enable(false);
      myIrrigation->pressurizeAll();
      controlState.setState(60);
      updateSystemState();
    }
  }
  else{
    emergencyFlag = true;
    myValves->enable(false);
    myIrrigation->keepConnected(false);
    myIrrigation->depressurizeWater();
    controlState.setState(70);
    updateSystemState();
  }
}

void startIrrigation() {
  // Store the initial value of the scale
  initialWeight = mySensors->_myScales[0]->getWeight();
  // Get water waste and print it
  h20waste = finalWeight - initialWeight; // Water waste
  Serial.print(F("debug,Solenoid System: Water Waste - "));
  Serial.print(h20waste);
  Serial.println(F(" liters"));
  // Include alarm if waste is too high > 5 liters
  if(h20waste>5) Serial.println(F("critical,Solenoid System: Water leak while solenoids are closed"));
  // Get water input by flowmeter 0
  if(0<sconfig.flowmeters) {
    mySensors->_myFlowmeters[0]->read();
    mySensors->_myFlowmeters[0]->printRead();
    mySensors->_myFlowmeters[0]->restartWater();
  }
  // Enable irrigation
  myIrrigation->keepConnected(true);
  if(!emergencyButtonFlag) myValves->enable(true);
  irrigationState.setState(1);
}

void irrigation() {
  if(myValves->getActualNumber()>=bconfig.floors*bconfig.solenoids*bconfig.regions){
     myIrrigation->keepConnected(false);
     finalWeight = mySensors->_myScales[0]->getWeight();
     h2oConsumption = initialWeight - finalWeight; // Water consumption
     if (h2oConsumption < 1) h2oConsumption = 1; // Avoid negatives number and zero water consumption. Min value is set as 1 liters as default

     // Do we have enough water
     if(mySensors->_myScales[0]->getWeight()-mySensors->_myScales[0]->getMinWeight()>h2oConsumption){
      // Do we have enough pressure?
      if(mySensors->_myAnalogs[1]->getValue()<=pconfig.min_pressure){
        myIrrigation->pressurizeAll();
        controlState.setState(10);
        updateSystemState();
      }
     }
     else {
        myIrrigation->depressurizeWater();
        controlState.setState(20);
        updateSystemState();

        }
     myValves->enable(false);
     irrigationState.setState(2);

     updateSystemState();
  }
  else if(!emergencyFlag) emergencyConditions();
}

void correctiveProcess(){
  if(controlState._state==10){
    if(mySensors->_myAnalogs[1]->getValue()>=pconfig.max_pressure){
      myIrrigation->Off(false);
      controlState.setState(0);
      updateSystemState();
    }
  }

  else if(controlState._state==20){
    if(mySensors->_myAnalogs[1]->getValue()<pconfig.free_pressure){
      myIrrigation->turnOnPump();
      controlState.setState(21);
      updateSystemState();
    }
  }

  else if(controlState._state==21){
    if(mySensors->_myScales[0]->getWeight()>=mySensors->_myScales[0]->getMaxWeight()){
      myIrrigation->pressurizeAll();
      myIrrigation->turnOffPump();
      controlState.setState(10);
      updateSystemState();
    }
  }

  else if(controlState._state==60){
    if(mySensors->_myAnalogs[1]->getValue()>=pconfig.max_pressure){
      myIrrigation->Off(true);
      if(!emergencyButtonFlag) myValves->enable(true);
      emergencyFlag = false;
      controlState.setState(0);
      updateSystemState();
    }
  }

  else if(controlState._state==70){
    if(mySensors->_myAnalogs[1]->getValue()<pconfig.free_pressure){
      myIrrigation->turnOnPump();
      controlState.setState(71);
      updateSystemState();
    }
  }

  else if(controlState._state==71){
    if(mySensors->_myScales[0]->getWeight()>=mySensors->_myScales[0]->getMaxWeight()){
      myIrrigation->pressurizeAll();
      myIrrigation->turnOffPump();
      controlState.setState(60);
      updateSystemState();
    }
  }
}

void mainControl(){
  if(irrigationState._state==0 && controlState._state==0 && bootParameters && firstHourUpdate) preconditions();
  else if(irrigationState._state==1) irrigation();
  else if(irrigationState._state==2 && (myValves->getTime()>myValves->getCycleTime() || myValves->getTime()<500)) irrigationState.setState(0);
  correctiveProcess();
}

void rememberState(){
  if(controlState._state==10) myIrrigation->pressurizeAll();
  else if(controlState._state==20) myIrrigation->depressurizeWater();
  else if(controlState._state==21){
    myIrrigation->depressurizeWater();
    controlState.setState(20);
  }
  else if(controlState._state==60){
    myIrrigation->pressurizeAll();
    controlState.setState(10);
  }
  else if(controlState._state==70){
    myIrrigation->keepConnected(false);
    myIrrigation->depressurizeWater();
    controlState.setState(20);
  }
  else if(controlState._state==71){
    myIrrigation->keepConnected(false);
    myIrrigation->depressurizeWater();
    controlState.setState(20);
  }
  else controlState.setState(0);

  updateSystemState();
  irrigationState.setState(0);
}
