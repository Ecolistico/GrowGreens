/*****   Lecture on serial Events   *****/

const char zero_char = char(48);

/*
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
*/
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

    if(parameter[0]==F("boot")){ // Functions to recieve variables when boot/rebooting
      // Form "boot,int[pumpInState],int[IPC],int[MPC],float[h2oConsumption],float[missedH2O]"
      if(!bootParameters){
        int pumpState = parameter[1].toInt();
        int ipc = parameter[2].toInt();
        int mpc = parameter[3].toInt();
        float h2oCons = parameter[4].toFloat();
        float missedH2O = parameter[5].toFloat();
        
        if(pumpState>=0 && ipc>=0 && mpc>=0 && h2oCons>=0 && missedH2O>=0){
          bootParameters = true; // Set bootParameters as true
          // Update consumption variables
          h2oConsumption = h2oCons;
          Serial.print(F("info,Serial Boot: Initial Water Consumption updated to "));
          Serial.print(h2oConsumption);
          Serial.println(F(" liters"));
          //rememberState(ipc, mpc, pumpState, missedH2O);
        }
        else Serial.println(F("error,Serial Boot: Parameter[1-10] incorrect"));
      }
      else Serial.println(F("error,Serial Boot: Parameters already setted"));
    }

    else if(parameter[0]==F("updateHour")){
      int hr = parameter[1].toInt();
      int mn = parameter[2].toInt();
      if(hr>=0 && hr<24 && mn>=0 && mn<60){
        if(dTime.hour!=hr || dTime.minute!=mn){
          dTime.hour = hr;
          dTime.minute = mn;
          Serial.println(F("Hour updated"));
          /*
          if(!firstHourUpdate){
            firstHourUpdate = true;
            updateDay();
            Irrigation.whatSolution(dateHour, dateMinute); // Update the solution parameter
          }
          */
        }
        else Serial.println(F("error,Serial updateHour: Hour is already updated"));
      }
      else Serial.println(F("error,Serial updateHour: Parameter[1,2] unknown"));
    }
    
    else if(parameter[0]==F("fan")){
      if(parameter[1]==F("systemEnable") || parameter[1]==F("systemEnable\n")) myFans->enable(true);
      else if(parameter[1]==F("systemDisable") || parameter[1]==F("systemDisable\n")) myFans->enable(false);
      else if(parameter[1]==F("enable")){
        int fl = parameter[2].toInt();
        myFans->_fan[fl]->enable(true);
      }
      else if(parameter[1]==F("disable") || parameter[1]==F("disable\n")){
        int fl = parameter[2].toInt();
        myFans->_fan[fl]->enable(false);
      }
      else if(parameter[1]==F("setTimeOn")){
        int fl = parameter[2].toInt();
        int t_on = parameter[3].toInt();
        fan_memory newParam = myMem.read_fan(fl);
        newParam.timeOn = t_on;
        myMem.save_fanParameters(fl, newParam);
        myFans->_fan[fl]->setTimeOn(t_on);
      }
      else if(parameter[1]==F("setCycleTime")){
        int fl = parameter[2].toInt();
        int t_cycle = parameter[3].toInt();
        fan_memory newParam = myMem.read_fan(fl);
        newParam.cycleTime = t_cycle;
        myMem.save_fanParameters(fl, newParam);
        myFans->_fan[fl]->setCycleTime(t_cycle);
      }
      else Serial.println(F("error,Serial Fan: Parameter[1] unknown"));
    }

    else if(parameter[0]==F("irrigationController")){
      if(parameter[1]==F("enable") || parameter[1]==F("enable\n")) myIrrigation->enable(true);
      else if(parameter[1]==F("disable") || parameter[1]==F("disable\n")) myIrrigation->enable(false);
      else Serial.println(F("error,Serial irrigationController: Parameter[1] unknown"));
    }

    else if(parameter[0]==F("sensor")){
      if(parameter[1]==F("analog")){
        if(parameter[2]==F("changePin")){
          int num = parameter[3].toInt();
          int pin = parameter[4].toInt();
          if(num>=0 && num<sconfig.analogs){
            analogSensor newParam = myMem.read_analog(num);
            newParam.pin = pin;
            myMem.save_analog(num, newParam);
            rebootFlag = true;
          }
          else Serial.println(F("error,Serial Sensor Analog setModel: Parameter[3] incorrect"));
        }
        else if(parameter[2]==F("setModel")){
          int num = parameter[3].toInt();
          float A = parameter[4].toFloat();
          float B = parameter[5].toFloat();
          float C = parameter[6].toFloat();
          if(num>=0 && num<sconfig.analogs) {
            analogSensor newParam = myMem.read_analog(num);
            newParam.A = A;
            newParam.B = B;
            newParam.C = C;
            myMem.save_analog(num, newParam);
            mySensors->_myAnalogs[num]->setModel(A, B, C);
          }
          else Serial.println(F("error,Serial Sensor Analog setModel: Parameter[3] incorrect"));
        }
        else if(parameter[2]==F("setFilter")){
          int num = parameter[3].toInt();
          int filter = parameter[4].toInt();
          float param = parameter[5].toFloat();
          if(num>=0 && num<sconfig.analogs && filter>=0 && filter<=2 && param>0) {
            analogSensor newParam = myMem.read_analog(num);
            newParam.filter = filter;
            newParam.filterParam = param;
            myMem.save_analog(num, newParam);
            mySensors->_myAnalogs[num]->setFilter(filter, param);
          }
          else Serial.println(F("error,Serial Sensor Analog setFilter: Parameter[3, 4, 5] incorrect"));
        }
        else if(parameter[2]==F("getRead")){
          int num = parameter[3].toInt();
          if(num>=0 && num<sconfig.analogs) mySensors->_myAnalogs[num]->printRead();
          else Serial.println(F("error,Serial Sensor Analog getRead: Parameter[3] incorrect"));
        }
        else Serial.println(F("error,Serial Sensor Analog: Parameter[2] unknown"));
      }
      else if(parameter[1]==F("flowmeter")){
        if(parameter[2]==F("changePin")){
          int num = parameter[3].toInt();
          int pin = parameter[4].toInt();
          if(num>=0 && num<sconfig.flowmeters){
            flowmeter newParam = myMem.read_flowmeter(num);
            newParam.pin = pin;
            myMem.save_flowmeter(num, newParam);
            rebootFlag = true;
          }
          else Serial.println(F("error,Serial Sensor Flowmeter changePin: Parameter[3] incorrect"));
        }
        else if(parameter[2]==F("calibrate")){
          int num = parameter[3].toInt();
          float k = parameter[4].toFloat();
          if(num>=0 && num<sconfig.flowmeters){
            flowmeter newParam = myMem.read_flowmeter(num);
            newParam.K = k;
            myMem.save_flowmeter(num, newParam);
            rebootFlag = true;
          }
          else Serial.println(F("error,Serial Sensor Flowmeter calibrate: Parameter[3] incorrect"));
        }
        else if(parameter[2]==F("getRead")){
          int num = parameter[3].toInt();
          if(num>=0 && num<sconfig.flowmeters) mySensors->_myFlowmeters[num]->printRead();
          else Serial.println(F("error,Serial Sensor Flowmeter getRead: Parameter[3] incorrect"));
        }
        else Serial.println(F("error,Serial Sensor Flowmeter: Parameter[2] unknown"));
      }
      else if(parameter[1]==F("scale")){
        if(parameter[2]==F("changePin")){
          int num = parameter[3].toInt();
          int pin1 = parameter[4].toInt();
          int pin2 = parameter[5].toInt();
          if(num>=0 && num<sconfig.scales){
            scale newParam = myMem.read_scale(num);
            newParam.pin1 = pin1;
            newParam.pin2 = pin2;
            myMem.save_scale(num, newParam);
            rebootFlag = true;
          }
          else Serial.println(F("error,Serial Sensor Scale changePin: Parameter[3] incorrect"));
        }
        else if(parameter[2]==F("setOffset")){
          int num = parameter[3].toInt();
          float offset = parameter[4].toFloat();
          if(num>=0 && num<sconfig.scales){
            scale newParam = myMem.read_scale(num);
            newParam.offset = offset;
            myMem.save_scale(num, newParam);
            mySensors->_myScales[num]->_sc->set_offset(offset);
          }
          else Serial.println(F("error,Serial Sensor Scale setOffset: Parameter[3] incorrect"));
        }
        else if(parameter[2]==F("setScale")){
          int num = parameter[3].toInt();
          float scaleParam = parameter[4].toFloat();
          if(num>=0 && num<sconfig.scales){
            scale newParam = myMem.read_scale(num);
            newParam.scale = scaleParam;
            myMem.save_scale(num, newParam);
            mySensors->_myScales[num]->_sc->set_scale(scaleParam);
          }
          else Serial.println(F("error,Serial Sensor Scale setScale: Parameter[3] incorrect"));
        }
        else if(parameter[2]==F("getRead")){
          int num = parameter[3].toInt();
          if(num>=0 && num<sconfig.scales) mySensors->_myScales[num]->printRead();
          else Serial.println(F("error,Serial Sensor Scale getRead: Parameter[3] incorrect"));
        }
        else if(parameter[2]==F("getRead_notScale")){
          int num = parameter[3].toInt();
          if(num>=0 && num<sconfig.scales) mySensors->_myScales[num]->printRead_notScale();
          else Serial.println(F("error,Serial Sensor Scale getRead_notScale: Parameter[3] incorrect"));
        }
        else if(parameter[2]==F("getRead_notOffset")){
          int num = parameter[3].toInt();
          if(num>=0 && num<sconfig.scales) mySensors->_myScales[num]->printRead_notOffset();
          else Serial.println(F("error,Serial Sensor Scale getRead_notOffset: Parameter[3] incorrect"));
        }
        else Serial.println(F("error,Serial Sensor Scale: Parameter[2] unknown"));
      }
      else if(parameter[1]==F("switch")){
        if(parameter[2]==F("changePin")){
          int num = parameter[3].toInt();
          int pin = parameter[4].toInt();
          if(num>=0 && num<sconfig.switches){
            switchSensor newParam = myMem.read_switch(num);
            newParam.pin = pin;
            myMem.save_switch(num, newParam);
            rebootFlag = true;
          }
          else Serial.println(F("error,Serial Sensor Switch changePin: Parameter[3] incorrect")); 
        }
        else if(parameter[2]==F("changeLogic")){
          int num = parameter[3].toInt();
          if(num>=0 && num<sconfig.switches){
            bool logic;
            if(parameter[4]==F("1\n") || parameter[4]==F("1") || parameter[4]==F("true\n") || parameter[4]==F("true") || parameter[4]==F("TRUE\n") || parameter[4]==F("TRUE")) logic = true;
            else logic = false;
            switchSensor newParam = myMem.read_switch(num);
            newParam.logic = logic;
            myMem.save_switch(num, newParam);
            mySensors->_mySwitches[num]->setLogic(logic);
          }
          else Serial.println(F("error,Serial Sensor Switch changePin: Parameter[3] incorrect")); 
        }
        else if(parameter[2]==F("getRead")){
          int num = parameter[3].toInt();
          if(num>=0 && num<sconfig.switches) mySensors->_mySwitches[num]->printRead();
          else Serial.println(F("error,Serial Sensor Switch getRead: Parameter[3] incorrect"));
        }
        else Serial.println(F("error,Serial Sensor Switch: Parameter[2] unknown"));
      }
      else if(parameter[1]==F("ultrasonic")){
        if(parameter[2]==F("changePin")){
          int num = parameter[3].toInt();
          int pin1 = parameter[4].toInt();
          int pin2 = parameter[5].toInt();
          if(num>=0 && num<sconfig.ultrasonics){
            ultrasonicSensor newParam = myMem.read_ultrasonic(num);
            newParam.pin1 = pin1;
            newParam.pin2 = pin2;
            myMem.save_ultrasonic(num, newParam);
            rebootFlag = true;
          }
          else Serial.println(F("error,Serial Sensor Ultrasonic changePin: Parameter[3] incorrect")); 
        }
        else if(parameter[2]==F("setModel")){
          int num = parameter[3].toInt();
          int model = parameter[4].toInt();
          float param = parameter[5].toInt();
          float height = parameter[6].toInt();
          if(num>=0 && num<sconfig.ultrasonics){
            ultrasonicSensor newParam = myMem.read_ultrasonic(num);
            newParam.model = model;
            newParam.param = param;
            newParam.height= height;
            myMem.save_ultrasonic(num, newParam);
            mySensors->_myUltrasonics[num]->setModel(model, param, height);
          }
          else Serial.println(F("error,Serial Sensor Ultrasonic changePin: Parameter[3] incorrect"));
        }
        // Missing take reading and calibrate functions
        else Serial.println(F("error,Serial Sensor Ultrasonic: Parameter[2] unknown"));
      }
      else Serial.println(F("error,Serial Sensor: Parameter[1] unknown"));
    }    
    
    else if(parameter[0]==F("solenoid")){
      if(parameter[1]==F("systemEnable") || parameter[1]==F("systemEnable\n")) myValves->enable(true);
      else if(parameter[1]==F("systemDisable") || parameter[1]==F("systemDisable\n")) myValves->enable(false);
      else if(parameter[1]==F("regionEnable")){
        int fl = parameter[2].toInt();
        int reg = parameter[3].toInt();
        if(fl>=0 && fl<bconfig.floors && (reg==0 || reg==1)){
          myValves->_floor[fl]->enable(true, reg);
        }
        else Serial.println(F("error,Serial Solenoid regionEnable: Parameter[2,3] unknown"));
      }
      else if(parameter[1]==F("regionDisable")){
        int fl = parameter[2].toInt();
        int reg = parameter[3].toInt();
        if(fl>=0 && fl<bconfig.floors && (reg==0 || reg==1)){
          myValves->_floor[fl]->enable(false, reg);
        }
        else Serial.println(F("error,Serial Solenoid regionDisable: Parameter[2,3] unknown"));
      }
      else if(parameter[1]==F("enable")){
        int fl = parameter[2].toInt();
        int reg = parameter[3].toInt();
        int valveNumber = parameter[4].toInt();
        if(fl>=0 && fl<bconfig.floors && (reg==0 || reg==1) && valveNumber>=0 && valveNumber<bconfig.solenoids){
          if(reg==0) myValves->_floor[fl]->_regA[valveNumber]->enable(true);
          else myValves->_floor[fl]->_regB[valveNumber]->enable(true);
        }
        else Serial.println(F("error,Serial Solenoid enable: Parameter[2,3,4] unknown"));
      }
      else if(parameter[1]==F("disable")){
        int fl = parameter[2].toInt();
        int reg = parameter[3].toInt();
        int valveNumber = parameter[4].toInt();
        if(fl>=0 && fl<bconfig.floors && (reg==0 || reg==1) && valveNumber>=0 && valveNumber<bconfig.solenoids){
          if(reg==0) myValves->_floor[fl]->_regA[valveNumber]->enable(false);
          else myValves->_floor[fl]->_regB[valveNumber]->enable(false);
        }
        else Serial.println(F("error,Serial Solenoid disable: Parameter[2,3,4] unknown"));
      }
      else if(parameter[1]==F("setTimeOn")){
        int fl = parameter[2].toInt();
        int reg = parameter[3].toInt();
        int valveNumber = parameter[4].toInt();
        int timeOn = abs(parameter[5].toInt());
        if(fl>=0 && fl<bconfig.floors && (reg==0 || reg==1) && valveNumber>=0 && valveNumber<bconfig.solenoids){
          if(reg==0) myValves->_floor[fl]->_regA[valveNumber]->setTimeOn(timeOn*1000);
          else myValves->_floor[fl]->_regB[valveNumber]->setTimeOn(timeOn*1000);
          myMem.save_irrigationParameters(fl, reg, valveNumber, timeOn);
        }
        else Serial.println(F("error,Serial Solenoid setTimeOn: Parameter[2,3,4] unknown"));
      }
      else if(parameter[1]==F("setCycleTime")){
        int cycleTime = abs(parameter[2].toInt());
        myValves->setCycleTime(cycleTime);
        bconfig.cycleTime = cycleTime;
        myMem.config_basic(bconfig);
      }
      else Serial.println(F("error,Serial Solenoid: Parameter[1] unknown"));
    }

    else if(parameter[0]==F("setPressure")){
      float pres = parameter[2].toFloat();
      if(pres>0){
        if(parameter[1]==F("max")){
          if(pres>pconfig.min_pressure && pres<=200){
            pconfig.max_pressure= pres;
            myMem.config_pressure(pconfig); 
          }
          else{Serial.println(F("error,Serial setPressure (max): Pressure has to be bigger than min_pressure and smaller than 200 psi"));}
        }
        else if(parameter[1]==F("min")){
          if(pres<pconfig.max_pressure){
            pconfig.min_pressure= pres;
            myMem.config_pressure(pconfig); 
          }
          else{Serial.println(F("error,Serial setPressure (min): Pressure has to be smaller than max_pressure"));}
        }
        else if(parameter[1]==F("critical")){
          if(pres<pconfig.min_pressure){
            pconfig.critical_pressure= pres;
            myMem.config_pressure(pconfig); 
          }
          else Serial.println(F("error,Serial setPressure (critical): Pressure has to be smaller than min_pressure"));
        }
        else if(parameter[1]==F("free")){
          if(pres<pconfig.critical_pressure){
            pconfig.free_pressure= pres;
            myMem.config_pressure(pconfig); 
          }
          else Serial.println(F("error,Serial setPressure (free): Pressure has to be smaller than critical_pressure"));
        }
        else Serial.println(F("error,Serial setPressure: Parameter[1] unknown"));
      }
      else Serial.println(F("error,Serial setPressure: Parameter[2] incorrect"));
    }
    
    else if(parameter[0]==F("eeprom")){ // Functions to manage EEPROM memory
      if(parameter[1]==F("clean") || parameter[1]==F("clean\n")){ myMem.clean();}
      else if(parameter[1]==F("print") || parameter[1]==F("print\n")){ myMem.print();}
      else if(parameter[1]==F("config_basic")) {
        int fl = parameter[2].toInt();
        int valves= parameter[3].toInt();
        int reg = parameter[4].toInt();
        int cycleTime = parameter[5].toInt();
        if(fl>=0 && fl<MAX_FLOOR_NUMBER && valves>=0 && valves<MAX_VALVES_PER_REGION && (reg==0 || reg==1)){
          bconfig.floors = fl;
          bconfig.solenoids = valves;
          bconfig.regions = reg;
          bconfig.cycleTime = cycleTime;
          myMem.config_basic(bconfig);
          rebootFlag = true;
        }
        else Serial.println(F("error,Serial EEPROM config_basic: Parameter[2,3,4,5] unknown"));
      }
      else if(parameter[1]==F("config_pressure")) {
        int max_pressure = parameter[2].toInt();
        int min_pressure = parameter[3].toInt();
        int critical_pressure = parameter[4].toInt();
        int free_pressure = parameter[5].toInt();
        if(free_pressure>0 && critical_pressure>free_pressure && min_pressure>critical_pressure && max_pressure>min_pressure && max_pressure<200){
          pconfig.max_pressure= max_pressure;
          pconfig.min_pressure = min_pressure;
          pconfig.critical_pressure = critical_pressure;
          pconfig.free_pressure = free_pressure;
          myMem.config_pressure(pconfig);
        }
        else Serial.println(F("error,Serial EEPROM config_pressure: Parameter[2,3,4,5] unknown"));
      }
      else if(parameter[1]==F("config_sensors")) {
        int analogs = abs(parameter[2].toInt());
        int flowmeters = abs(parameter[3].toInt());
        int scales = abs(parameter[4].toInt());
        int switches = abs(parameter[5].toInt());
        int ultrasonics = abs(parameter[6].toInt());
        sconfig.analogs = analogs;
        sconfig.flowmeters = flowmeters;
        sconfig.scales = scales;
        sconfig.switches = switches;
        sconfig.ultrasonics = ultrasonics;
        myMem.config_sensors(sconfig);
        rebootFlag = true;
      }
      else if(parameter[1]==F("config_logic")) {
        bool Vair_Logic = parameter[2].toInt();
        bool Vconnected_Logic = parameter[3].toInt();
        bool VFree_Logic = parameter[4].toInt();
        lconfig.Vair_Logic = Vair_Logic;
        lconfig.Vconnected_Logic = Vconnected_Logic;
        lconfig.VFree_Logic = VFree_Logic;
        myMem.config_logic(lconfig);
        rebootFlag = true;
      }
      else Serial.println(F("error,Serial EEPROM: Parameter[1] unknown"));
    }
    
    else Serial.println(F("error, Serial Command Unknown"));
  }
  input_string_complete = false;
}
