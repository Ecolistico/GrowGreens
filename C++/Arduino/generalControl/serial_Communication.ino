/*****   Lecture on serial Events   *****/

const char zero_char = char(48);

void requestSolution(){ // Form -> "?solutionMaker,float[liters],int[sol],float[ph],int[ec]"
  Serial.print(F("?solutionMaker,"));
  Serial.print(US6.getVolume()); // Liters
  Serial.print(F(",")); 
  Serial.print(Recirculation.getOut()); // Solution 
  Serial.print(F(","));
  Serial.print(Irrigation.getPH(Recirculation.getOut())); // ph
  Serial.print(F(",")); 
  Serial.println(Irrigation.getEC(Recirculation.getOut())); // ec
}

void updateSystemState(){ 
  Recirculation.updateState();
  // Form -> "updateSystemState,int[sol],float[volNut],float[volH2O],float[consNut],float[consH2O],
  //          int[pumpIn],int[IPC],int[MPC],float[missedNut],float[missedH2O]"
  Serial.print(F("updateSystemState,"));
  Serial.print(Recirculation.getOut()); // Solution
  Serial.print(F(",")); 
  Serial.print(Recirculation.getVolKnut()); // volNut
  Serial.print(F(","));
  Serial.print(Recirculation.getVolKh2o()); // volH2O
  Serial.print(F(","));
  Serial.print(solutionConsumption); // consNut
  Serial.print(F(",")); 
  Serial.print(h2oConsumption); // consH2O
  Serial.print(F(","));
  Serial.print(Recirculation.InpumpWorkIn()); // pumpIn
  Serial.print(F(","));
  Serial.print(IPC.state); // IPC
  Serial.print(F(",")); 
  Serial.print(MPC.state); // MPC
  Serial.print(F(","));
  Serial.print(Recirculation.getVolCnut()); // missedNut
  Serial.print(F(","));
  Serial.println(Recirculation.getVolCh2o()); // missedH2O
}

void serialEvent(){                                  //if the hardware serial port_0 receives a char
  inputstring = Serial.readStringUntil(13);           //read the string until we see a <CR>
  input_string_complete = true;                       //set the flag used to tell if we have received a completed string from the PC
  
  if(input_string_complete==true){
    String parameter[12];
    int k = 0;
    int l = 0;
    // Split the parameters
    for (int j = 0; j<inputstring.length(); j++){
      if(inputstring[j] == ','){ // Looking for ','
        parameter[k] = inputstring.substring(l,j);
        k++;
        l = j+1;
      }
      else if(j==inputstring.length()-1){
        parameter[k] = inputstring.substring(l,j+1);
      }
    }
    
    if(parameter[0]==F("actuator")){ // Functions executed in Actuator class
      if(parameter[1]==F("setTime")){ // Function setTime -> Form "actuator,setTime,type[0-3],floor[0-3],timeOn,cycleTime"
        int Type = parameter[2].toInt();
        int Floor = parameter[3].toInt();
        int timeOn = parameter[4].toInt();
        int cyceTime = parameter[5].toInt();
        if(timeOn<cyceTime && Type>=0 && Floor>=0 && Type<MAX_TYPES_ACTUATOR && Floor<MAX_FLOOR){
          Actuator::setTimeAll(Type, Floor, timeOn, cyceTime);
        }
       else{Serial.println(F("error,Actuator(): Parameter[2-5] wrong"));}
      }
      else if(parameter[1]==F("setTimeOn")){  // Function setTimeOn -> Form "actuator,setTimeOn,type[0-3],floor[0-3],timeOn"
        int Type = parameter[2].toInt();
        int Floor = parameter[3].toInt();
        int timeOn = parameter[4].toInt();
        if(Type>=0 && Floor>=0 && Type<MAX_TYPES_ACTUATOR && Floor<MAX_FLOOR){
          Actuator::setTimeOnAll(Type, Floor, timeOn);  
        }
        else{Serial.println(F("error,Actuator(): Parameter[2-3] wrong"));}
      }
      else if(parameter[1]==F("setCycleTime")){ // Function setCycleTime -> Form "actuator,setCycleTime,type[0-3],floor[0-3],cycleTime"
        int Type = parameter[2].toInt();
        int Floor = parameter[3].toInt();
        int cycleTime = parameter[4].toInt();
        if(Type>=0 && Floor>=0 && Type<MAX_TYPES_ACTUATOR && Floor<MAX_FLOOR){
          Actuator::setCycleTimeAll(Type, Floor, cycleTime);
        }
        else{Serial.println(F("error,Actuator(): Parameter[2-3] wrong"));}
      }
      else if(parameter[1]==F("getTime")){ // Function getTime -> Form "actuator,getTime,type[0-3],floor[0-3]"
        int Type = parameter[2].toInt();
        int Floor = parameter[3].toInt();
        if(Type>=0 && Floor>=0 && Type<MAX_TYPES_ACTUATOR && Floor<MAX_FLOOR){
          Actuator::getTimeAll(Type, Floor);
        }
        else{Serial.println(F("error,Actuator(): Parameter[2-3] wrong"));}
      }
      else if(parameter[1]==F("getTimeOn")){ // Function getTimeOn -> Form "actuator,getTimeOn,type[0-3],floor[0-3]"
        int Type = parameter[2].toInt();
        int Floor = parameter[3].toInt();
        if(Type>=0 && Floor>=0 && Type<MAX_TYPES_ACTUATOR && Floor<MAX_FLOOR){
          Actuator::getTimeOnAll(Type, Floor);
        }
        else{Serial.println(F("error,Actuator(): Parameter[2-3] wrong"));}
      }
      else if(parameter[1]==F("getCycleTime")){ // Function getCycleTime -> Form "actuator,getCycleTime,type[0-3],floor[0-3]"
        int Type = parameter[2].toInt();
        int Floor = parameter[3].toInt();
        if(Type>=0 && Floor>=0 && Type<MAX_TYPES_ACTUATOR && Floor<MAX_FLOOR){
          Actuator::getCycleTimeAll(Type, Floor);
        }
        else{Serial.println(F("error,Actuator(): Parameter[2-3] wrong"));}
      }
      else{Serial.println(F("error,Actuator(): Parameter[1] unknown"));}
    }

    else if(parameter[0]==F("solenoid")){ // Functions executed in solenoidValve class
      if(parameter[1]==F("enableGroup")){ // Function enableGroup -> Form "solenoid,enableGroup,bool(0/1)"
        if(parameter[2].toInt()==1){ solenoidValve::enableGroup(true); }
        else if(parameter[2].toInt()==0){ solenoidValve::enableGroup(false); }
        else{ Serial.println(F("error,solenoidValve enableGroup() parameter wrong. It has to be 0/1"));}
      }
      
      else if(parameter[1]==F("setCycleTime")){ // Function setCycleTime -> Form "solenoid,setCycleTime,int[min]"
        // Check that conditions to save in EEPROM are correct
        int inputTime_minutes = parameter[2].toInt();
        solenoidSaveCycleTime(inputTime_minutes);
      }
      
      else if(parameter[1]==F("setTimeOn")){ // Function setTimeOn -> Form "solenoid,setTimeOn,floor,region,solution,int[s]"
        // Check that conditions to save in EEPROM are correct
        int fl = parameter[2].toInt();
        int reg = parameter[3].toInt();
        int sol = parameter[4].toInt();
        int inputTime_seconds = parameter[5].toInt();
        solenoidSaveTimeOn(fl, reg, sol, inputTime_seconds);
      }

      else if(parameter[1]==F("charge") || parameter[1]==F("charge\n")){
        chargeSolenoidParameters(Recirculation.getOut()+1); // Update irrigationparameters
        Serial.println(F("info,Irrigation Parameters charged"));
      }
      
      else{Serial.println(F("error,Solenoid(): Parameter[1] unknown"));}
    }

    else if(parameter[0]==F("multiday")){ // Functions to manage multiDay objects
      if(parameter[1]==F("redefine")){ // Function redifine -> Form "multiday,redefine,floor,int[cycles],int[%light],float[initialHour]"
        int fl = parameter[2].toInt();
        int cycles = parameter[3].toInt();
        int light = parameter[4].toInt();
        float initHour = parameter[5].toFloat();
        multidaySave(fl, cycles, light, initHour);
      }
      else{Serial.println(F("error,Multiday(): Parameter[1] unknown"));}
    }

    else if(parameter[0]==F("region")){ // Function to save in EEPROM the working stage by floor
      // Auxiliar functions executed in LED_Mod and solenoidValve class
      if(parameter[1]==F("save")){ // Functions enable -> Form "region,save,int[floor],int[region]"
        if(firstHourUpdate){
          int fl = parameter[2].toInt();
          int reg = parameter[3].toInt();
          if(fl>=0 && fl<MAX_FLOOR){
            if(reg>=0 && reg<=MAX_REGION*2){
              // Save parameters in EEPROM
              regionSave(fl, reg); 
              // Enable LED modules
              enableLED(fl, reg);

              int nite = inWhatFloorIsNight();
              if(nite>0){
                for(int i=0; i<MAX_FLOOR; i++){
                  // Turn on/off according to the day state
                  if(i==nite-1){LED_Mod::turnOff(i);}
                  else{LED_Mod::turnOn(i);}
                } 
              }
              // Enable solenoidValves
              enableSolenoid(fl, reg);
            }
            else{Serial.println(F("error,LED Enable(): Parameter region incorrect"));}
          }
          else{Serial.println(F("error,LED Enable(): Parameter floor incorrect"));} 
        }
        else{Serial.println(F("error,LED Enable(): Wait until the system update the hour"));} 
      }
      else{Serial.println(F("error,LED(): Parameter[1] unknown"));}
    }

    else if(parameter[0]==F("irrigation")){
      if(parameter[1]==F("save")){
        if(parameter[2]==F("general")){
          int cyclesPerDay = parameter[3].toInt(); 
          int initialHour = parameter[4].toInt();
          irrigationSave(cyclesPerDay, initialHour);
        }
        else if(parameter[2]==F("solution")){
          int sol = parameter[3].toInt();
          int order = parameter[4].toInt();
          int percent = parameter[5].toInt();
          int ec =  parameter[6].toInt();
          float ph = parameter[7].toFloat();
          solutionSave(sol, order, percent, ec, ph);
        } 
      }
      else if(parameter[1]==F("charge") || parameter[1]==F("charge\n")){
        chargeIrrigationParameters();
      }
      else{ Serial.println(F("error,Irrigation(): Parameter[1] unknown"));}
    }
    
    else if(parameter[0]==F("updateHour")){ // Function updateHour -> Form "updateHour,int[hour],int[minute]"
      int hr = parameter[1].toInt();
      int mn = parameter[2].toInt();
      if(hr>=0 && hr<24 && mn>=0 && mn<60){
        if(dateHour!=hr || dateMinute!=mn){
          dateHour = hr;
          dateMinute = mn;
          fixD.updateLedState(dateHour);
          Serial.println(F("Hour updated"));
          if(!firstHourUpdate){
            firstHourUpdate = true;
            updateDay();
            Irrigation.whatSolution(dateHour, dateMinute); // Update the solution parameter
          }
        }
        else{Serial.println(F("error,Update(): Hour is already updated"));}
      }
      else{Serial.println(F("error,Update(): Hour/Minute wrong"));}
    }

    else if(parameter[0]==F("analog")){ // Functions executed in analogSensor class
      int sens = -1;
      if(parameter[1]==F("nutrition")){sens = 0;}
      else if(parameter[1]==F("tank")){sens = 1;}
      else if(parameter[1]==F("water")){sens = 2;}
      else{ sens = -1;}
      if(sens>=0){
        if(parameter[2]==F("setModel")){ // Function setModel -> Form "analog,type,setModel,c,b,a"
          float c = parameter[3].toFloat();
          float b = parameter[4].toFloat();
          float a = parameter[5].toFloat();
          uint8_t deg = 0;
          if(c!=0){deg = 3;}
          else if(b!=0){deg = 2;}
          else if(a!=0){deg = 1;}
          else{deg = 0;}
          if(!analogSensor::ptr[sens]->setModel(deg, a, b, c)){
            Serial.println(F("error,Analog setModel(): Cannot set Model"));
          }
          else{analogSaveModel(sens, a, b, c);}
        }
        
        else if(parameter[2]==F("notFilter") || parameter[2]==F("notFilter\n")){ // Function notFilter -> Form "analog,type,notFilter"
          analogSensor::ptr[sens]->notFilter();
          analogSaveFilter(sens, 0, 0);
        }
        
        else if(parameter[2]==F("defaultFilter") || parameter[2]==F("defaultFilter\n")){ // Function defaultFilter -> Form "analog,type,defaultFilter"
          analogSensor::ptr[sens]->defaultFilter();
          analogSaveFilter(sens, 1, 0.25);
        }
        
        else if(parameter[2]==F("setExponentialFilter")){ // Function setExponentialFilter -> Form "analog,type,setExponentialFilter,float[alpha]"
          float alfa = parameter[3].toFloat();
          if(alfa>0 && alfa<1){
            if(!analogSensor::ptr[sens]->setExponentialFilter(alfa)){
              Serial.println(F("error,Analog setExponentialFilter(): Cannot set exponential filter"));
            }
            else{analogSaveFilter(sens, 1, alfa);}
          }
          else{Serial.println(F("error,Analog setExponentialFilter(): Parameter alfa incorrect"));}
        }
        
        else if(parameter[2]==F("setKalmanFilter")){ // Function setKalmanFilter -> Form "analog,type,setKalmanFilter,float[noise]"
          float noise = parameter[3].toFloat();
          if(noise>0){
            if(!analogSensor::ptr[sens]->setKalmanFilter(noise)){
              Serial.println(F("error,Analog setKalmanFilter(): Cannot set Kalman filter"));
            }
            else{analogSaveFilter(sens, 2, noise);}
          }
          else{Serial.println(F("error,Analog setKalmanFilter(): Parameter noise incorrect"));}
        }
        
        else{Serial.println(F("error,Analog Sensor(): Parameter[2] unknown"));}
      }
      else{Serial.println(F("error,Analog Sensor(): Sensor unknown"));}
    }


    else if(parameter[0]==F("ultrasonic")){ // Functions executed in UltraSonic class
      int sens = -1;
      if(parameter[1]==F("recirculation")){sens = 0;}
      else if(parameter[1]==F("sol1")){sens = 1;}
      else if(parameter[1]==F("sol2")){sens = 2;}
      else if(parameter[1]==F("sol3")){sens = 3;}
      else if(parameter[1]==F("sol4")){sens = 4;}
      else if(parameter[1]==F("water")){sens = 5;}
      else if(parameter[1]==F("solmaker")){sens = 6;}
      else{ sens = -1;}
      if(sens>=0){
        if(parameter[2]==F("setModel")){ // Function setModel -> Form "ultrasonic,type,setModel,int[model],float[paramM],float[height]"
          int model = parameter[3].toInt();
          float param = parameter[4].toFloat();
          float height = parameter[5].toFloat();
          if(model>=0 && model<=2){
            if(param>0){
              if(height>=UltraSonic::ptr[sens]->getMaxDist()){
                if(!UltraSonic::ptr[sens]->setModel(model, param, height)){
                  Serial.println(F("error,Ultrasonic setModel(): Cannot set Model"));
                }
                else{ultrasonicSaveModel(sens, model, param, height);}
              }
              else{Serial.println(F("error,Ultrasonic setModel(): Parameter height incorrect"));}
            }
            else{Serial.println(F("error,Ultrasonic setModel(): Parameter param(Model) incorrect"));}
          }
          else{Serial.println(F("error,Ultrasonic setModel(): Parameter model incorrect"));}
        }
        
        else if(parameter[2]==F("notFilter") || parameter[2]==F("notFilter\n")){ // Function notFilter -> Form "ultrasonic,type,notFilter"
          UltraSonic::ptr[sens]->notFilter();
          ultrasonicSaveFilter(sens, 0, 0);
        }
        
        else if(parameter[2]==F("defaultFilter") || parameter[2]==F("defaultFilter\n")){ // Function defaultFilter -> Form "ultrasonic,type,defaultFilter"
          UltraSonic::ptr[sens]->defaultFilter();
          ultrasonicSaveFilter(sens, 1, 0.25);
        }
        
        else if(parameter[2]==F("setExponentialFilter")){ // Function setExponentialFilter -> Form "ultrasonic,type,setExponentialFilter,float[alpha]"
          float alfa = parameter[3].toFloat();
          if(alfa>0 && alfa<1){
            if(!UltraSonic::ptr[sens]->setExponentialFilter(alfa)){
              Serial.println(F("error,Ultrasonic setExponentialFilter(): Cannot set exponential filter"));
            }
            else{ultrasonicSaveFilter(sens, 1, alfa);}
          }
          else{Serial.println(F("error,Ultrasonic setExponentialFilter(): Parameter alfa incorrect"));}
        }
        
        else if(parameter[2]==F("setKalmanFilter")){ // Function setKalmanFilter -> Form "ultrasonic,type,setKalmanFilter,float[noise]"
          float noise = parameter[3].toFloat();
          if(noise>0){
            if(!UltraSonic::ptr[sens]->setKalmanFilter(noise)){
              Serial.println(F("error,Ultrasonic setKalmanFilter(): Cannot set Kalman filter"));
            }
            else{ultrasonicSaveFilter(sens, 2, noise);}
          }
          else{Serial.println(F("error,Ultrasonic setKalmanFilter(): Parameter noise incorrect"));}
        }
        else{Serial.println(F("error,Ultrasonic Sensor(): Parameter[2] unknown"));}
      }
      else{Serial.println(F("error,Ultrasonic Sensor(): Sensor unknown"));}
    }

    else if(parameter[0]==F("hvac")){ // Functions executed in controllerHVAC class
      if(parameter[1]==F("changeMode")){ // Function changeMode -> Form "hvac,changeMode,Mode[off/cool/heat]"
        if(parameter[2]==F("off") || parameter[2]==F("off\n")){ HVAC.changeMode(OFF_MODE); }
        else if(parameter[2]==F("cool") || parameter[2]==F("cool\n")){ HVAC.changeMode(COOL_MODE); }
        else if(parameter[2]==F("heat") || parameter[2]==F("heat\n")){ HVAC.changeMode(HEAT_MODE); }
        else{Serial.println(F("error,HVAC changeMode Function: Parameter[2] mode unknown"));}
      }
      else if(parameter[1]==F("changeFan")){ // Function changeMode -> Form "hvac,changeMode,Mode[auto/on]"
        if(parameter[2]==F("auto") || parameter[2]==F("auto\n")){ HVAC.changeFan(AUTO_FAN); }
        else if(parameter[2]==F("on") || parameter[2]==F("on\n")){ HVAC.changeFan(ON_FAN); }
        else{Serial.println(F("error,HVAC changeFan Function: Parameter[1] unknown"));}
      }
      else{Serial.println(F("error,HVAC Functions: Parameter[1] function unknown"));}
    }

    else if(parameter[0]==F("setPressure")){ // Functions executed in controllerHVAC class
      float pres = parameter[2].toFloat();
      if(pres>0){
        if(parameter[1]==F("max")){
          if(pres>min_pressure && pres<=165){
            max_pressure = pres;
            pressureSave(0, pres);  
          }
          else{Serial.println(F("error,Set Max Pressure(): Pressure has to be bigger than min_pressure and smaller than 165 psi"));}
        }
        else if(parameter[1]==F("min")){
          if(pres<max_pressure){
            min_pressure = pres;
            pressureSave(1, pres);
          }
          else{Serial.println(F("error,Set Min Pressure(): Pressure has to be smaller than max_pressure"));}
        }
        else if(parameter[1]==F("critical")){
          if(pres<min_pressure){
            critical_pressure = pres;
            pressureSave(2, pres);
          }
          else{Serial.println(F("error,Set Critical Pressure(): Pressure has to be smaller than min_pressure"));}
        }
        else{ Serial.println(F("error,Set Pressure(): Parameter[1] function unknown")); }
      }
      else{ Serial.println(F("error,Set Pressure(): Parameter[2] pressure incorrect")); }
    }

    else if(parameter[0]==F("boot")){ // Functions to recieve variables when boot/rebooting
      // Form "boot,int[lastSol],float[volumenNut],float[volumeH2O],float[solConsumption],
      // float[h2oConsumption],int[pumpInState],int[IPC],int[MPC],float[missedNut],float[missedH2O]"
      if(!bootParameters){
        int lastSol = parameter[1].toInt();
        float vnut = parameter[2].toFloat();
        float vh2o = parameter[3].toFloat();
        float solCons = parameter[4].toFloat();
        float h2oCons = parameter[5].toFloat();
        int pumpInState = parameter[6].toInt();
        int ipc = parameter[7].toInt();
        int mpc = parameter[8].toInt();
        float missedNut = parameter[9].toFloat();
        float missedH2O = parameter[10].toFloat();
        
        if(lastSol>=0 && lastSol<4 && vnut>=0 && vh2o>=0 && solCons>=0 && h2oCons>=0 && pumpInState>=0 &&
        ipc>=0 && mpc>=0 && missedNut>=0 && missedH2O>=0){
          bootParameters = true; // Set bootParameters as true
          lastSolution = lastSol+1; // solution using for irrigation class
          // Update Recirculation class parameters
          Recirculation.setIn(lastSol);
          Recirculation.setOut(lastSol);
          chargeSolenoidParameters(lastSolution); // Update irrigationparameters
          // Update Recirculation parameters
          Recirculation.addVolKnut(vnut);
          Recirculation.addVolKh2o(vh2o);
          // Update consumption variables
          solutionConsumption = solCons;
          h2oConsumption = h2oCons;
          Serial.print(F("Boot: Last Solution to be irrigated is "));
          Serial.println(lastSolution);
          Serial.print(F("Boot: Nutrition Kegs Volume Updated to "));
          Serial.print(Recirculation.getVolKnut());
          Serial.println(F(" liters"));
          Serial.print(F("Boot: Water Kegs Volume Updated to "));
          Serial.print(Recirculation.getVolKh2o());
          Serial.println(F(" liters"));
          Serial.print(F("Boot: Initial Nutrition Kegs Consumption updated to "));
          Serial.print(solutionConsumption);
          Serial.println(F(" liters"));
          Serial.print(F("Boot: Initial Water Kegs Consumption updated to "));
          Serial.print(h2oConsumption);
          Serial.println(F(" liters"));
          rememberState(ipc, mpc, pumpInState, missedNut, missedH2O);
        }
        else{ Serial.println(F("error,boot(): Parameter[1-10] incorrect")); } 
      }
      else{ Serial.println(F("error,Set Initial Parameters(): Parameters already setted")); } 
    }

    else if(parameter[0]==F("solutionMaker")){ // Coordinate action with solutionMaker
      if(parameter[1]==F("accept") || parameter[1]==F("accept\n")){ // Computer informs that solutionMaker accepts the last request
          CC.setState(1);
          updateSystemState();
        }
        else if(parameter[1]==F("finished") || parameter[1]==F("finished\n")){ // Computer informs that solutionMaker finished the last request
          CC.setState(2);
          updateSystemState();
        }
        else{Serial.println(F("error,solutionMaker(): Parameter[1] unknown"));}
    }
    
    else if(parameter[0]==F("eeprom")){ // Functions to manage EEPROM memory
      if(parameter[1]==F("clean") || parameter[1]==F("clean\n")){ clean_EEPROM();}
      else if(parameter[1]==F("print") || parameter[1]==F("print\n")){ print_EEPROM();}
      else{Serial.println(F("error,EEPROM(): Parameter[1] unknown"));}
    }

    else if(parameter[0]==F("debug")){ // Functions to debug EEPROM memory
      if(parameter[1]==F("print") || parameter[1]==F("print\n")){ Serial.println(US0.getDistance());}
      else if(parameter[1]==F("nextIrrigationStage") || parameter[1]==F("nextIrrigationStage\n")){ 
        Serial.println(F("warning,Debug(): Passing to the next Irrigation Stage"));
        irrigationStage++;
      }
      else if(parameter[1]==F("irrigation")){
        if(parameter[2]==F("whatSolution") || parameter[2]==F("whatSolution\n"))
        { Serial.print(F("info,Debug(): ")); Serial.println(Irrigation.whatSolution(dateHour, dateMinute)); }
        else if(parameter[2]==F("getEC") || parameter[2]==F("getEC\n"))
        { Serial.print(F("info,Debug(): ")); Serial.println(Irrigation.getEC(parameter[3].toInt())); }
        else if(parameter[2]==F("getPH") || parameter[2]==F("getPH\n"))
        { Serial.print(F("info,Debug(): ")); Serial.println(Irrigation.getPH(parameter[3].toInt())); }
      }
      else if(parameter[1]==F("requestSolution") || parameter[1]==F("requestSolution\n"))
      { requestSolution(); }
      else if(parameter[1]==F("updateSystemState") || parameter[1]==F("updateSystemState\n"))
      { updateSystemState(); }
    }
    
    else{Serial.println(F("error, Serial Command Unknown"));}
  
  }
  input_string_complete = false;
}
