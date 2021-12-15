void preconditions() {
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
    if(mySensors->_myAnalogs[1]->getValue()<=pconfig.min_pressure){
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
  initialWeight = mySensors->_myScales[0]->getWeight();
  myIrrigation->keepConnected(true);
  myValves->enable(true);
  irrigationState.setState(1);
}

void irrigation() {
  if(myValves->getActualNumber()>=bconfig.floors*bconfig.solenoids*bconfig.regions){
     myIrrigation->keepConnected(false);
     h2oConsumption = initialWeight - mySensors->_myScales[0]->getWeight();
     // Pressure low in air tank
     if(mySensors->_myAnalogs[0]->getValue()<=pconfig.min_pressure){
      myIrrigation->pressurizeAir();
      controlState.setState(1);
      updateSystemState();
     }
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
      if(controlState._state==1){
        myIrrigation->pressurize_depressurize();
        controlState.setState(40);
        updateSystemState();
      }
      else{
        myIrrigation->depressurizeWater();
        controlState.setState(20);
        updateSystemState();
      }
     }
     myValves->enable(false);
     irrigationState.setState(2);
     updateSystemState();
  }
  else emergencyConditions();
}

void correctiveProcess(){
  if(controlState._state==1){
    if(mySensors->_myAnalogs[0]->getValue()>=pconfig.max_pressure){
      myIrrigation->Off(false);
      controlState.setState(0);
      updateSystemState();
    }
  }

  else if(controlState._state==10){
    if(mySensors->_myAnalogs[1]->getValue()>=pconfig.max_pressure){
      myIrrigation->Off(true);
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

  else if(controlState._state==40){
    if(mySensors->_myAnalogs[0]->getValue()>=pconfig.max_pressure){
      myIrrigation->Off(false);
      myIrrigation->depressurizeWater();
      controlState.setState(20);
      updateSystemState();
    }
    else if(mySensors->_myAnalogs[1]->getValue()<pconfig.free_pressure){
      myIrrigation->turnOnPump();
      controlState.setState(41);
      updateSystemState();
    }
  }

  else if(controlState._state==41){
    if(mySensors->_myAnalogs[0]->getValue()>=pconfig.max_pressure){
      myIrrigation->Off(false);
      myIrrigation->depressurizeWater();
      controlState.setState(21);
      updateSystemState();
    }
    else if(mySensors->_myScales[0]->getWeight()>=mySensors->_myScales[0]->getMaxWeight()){
      myIrrigation->pressurizeAll();
      myIrrigation->turnOffPump();
      controlState.setState(10);
      updateSystemState();
    }
  }

  else if(controlState._state==60){
    if(mySensors->_myAnalogs[1]->getValue()>=pconfig.max_pressure){
      myIrrigation->Off(true);
      myValves->enable(true);
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
  if(irrigationState._state==1) irrigation();
  if(irrigationState._state==2 && myValves->getTime()>myValves->getCycleTime()) irrigationState.setState(0);
  correctiveProcess();
}

void rememberState(){
  if(controlState._state==1) myIrrigation->pressurizeAir();
  else if(controlState._state==10) myIrrigation->pressurizeAll();
  else if(controlState._state==20) myIrrigation->depressurizeWater();
  else if(controlState._state==21){
    myIrrigation->depressurizeWater();
    controlState.setState(20);
  }
  else if(controlState._state==40){
    myIrrigation->keepConnected(false);
    myIrrigation->pressurize_depressurize();
  }
  else if(controlState._state==41){
    myIrrigation->keepConnected(false);
    myIrrigation->pressurize_depressurize();
    controlState.setState(40);
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
