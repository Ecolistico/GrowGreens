/*****   Lecture on serial Events   *****/
/*
 * The structure of the commands is specified on each function
 * 
 * Notes: Values of 1, 2, 3, 4, 5, 7, 9, etc... are char type, and its value is assigned according to the ASCII Table starting at char(48)="0"
 * For further information see ASCII Table
*/
const char zero_char = char(48);

void serialEvent() {                                  //if the hardware serial port_0 receives a char
  inputstring = Serial.readStringUntil(13);           //read the string until we see a <CR>
  input_string_complete = true;                       //set the flag used to tell if we have received a completed string from the PC
  Serial.println(F("Request accepted"));
  if(input_string_complete==true){
    String parameter[5];
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

    // prepare()
    // -> Form "prepare,float[liters],int[sol],float[ph],float[ec]"
    if(parameter[0]==F("prepare")){
      float liters = parameter[1].toFloat();
      byte sol = parameter[2].toInt();
      float ph = parameter[3].toFloat();
      float ec = parameter[4].toFloat();
      if(liters>0){
        if(sol>=0 && sol<4){
          if(ph>0 && ph<14){
            if(ec>0){
              Serial.println(F("Request accepted"));
              Serial.print(F("info,Preparing "));
              Serial.print(liters);
              Serial.print(F(" liters of solution "));
              Serial.print(sol+1);
              Serial.print(F(" with ph="));
              Serial.print(ph);
              Serial.print(F("and ec="));
              Serial.println(ec);
              
              sMaker.prepareSolution(liters, ph, ec);
              //sMaker.eventLCD();  
            }
            else {Serial.println(F("error,prepare(): ec has to be positive"));}
          }
          else {Serial.println(F("error,prepare(): ph out of range [0-14]"));}
        }
        else {Serial.println(F("error,prepare(): solution out of range [0-3]"));}
      }
      else {Serial.println(F("error,prepare(): liters has to be positive"));}
    }

    // solFinished() ?
    else if(parameter[0]==F("?solutionFinished") || parameter[0]==F("?solutionFinished\n")){
      if(sMaker.SolFinished){Serial.println(F("Solution Finished"));}
      else{Serial.println(F("warning,Solution is not ready"));}
    }
    
    // dispense() 
    // -> Form "dispense,int[Motor],long[grams]"
    else if(parameter[0]==F("dispense")){
      int act = parameter[1].toInt();
      long grams = parameter[2].toFloat();
      if(act>=0 && act<MAX_STEPPERS){
        if(grams>0){
          //sMaker.dispense(long(grams*1000), act);
          //sMaker.eventLCD(); 
        }
        else{ Serial.println(F("error,dispense(): Grams has to be positive"));}
      }
      else{ Serial.println(F("error,dispense(): Motor out of range [0-3]"));}
    }
    
    // dispenseAcid()
    // -> Form "dispenseAcid,int[pump],float[mililiters]"
    else if(parameter[0]==F("dispenseAcid")){
      int act = parameter[1].toInt();
      float mililiters = parameter[2].toFloat();
      if(act>=0 && act<MAX_PUMPS_NUMBER){
        if(mililiters>0){
          sMaker.dispenseAcid(mililiters, act);
          Serial.println(F("Dispensing"));
          //sMaker.eventLCD(); 
        }
        else{ Serial.println(F("error,dispenseAcid(): Mililiters has to be positive"));}
      }
      else{ Serial.println(F("error,dispenseAcid(): Pump out of range [0-1]"));}
    }

    // Peristaltic pumps test()
    // -> Form "pump,int[pump],float[mililiters]"
    else if(parameter[0]==F("pump")){
      int act = parameter[1].toInt();
      float mililiters = parameter[2].toFloat();
      if(act<SOLUTIONS){
        if(mililiters!=0){
          for (uint8_t i = act; i < SOLUTIONS; i++){
            sMaker.dispensePeristaltic(mililiters, i);
          }
          testAct = act;
          testingPump = true;
          Serial.println(F("Test dispensing"));
          //sMaker.eventLCD(); 
        }
        else{ Serial.println(F("error,dispensePeristaltic(): Mililiters has to be positive"));}
      }
      else{ Serial.println(F("error,dispensePeristaltic(): Pump out of range"));}
    }
    
    // stop()
    else if(parameter[0]==F("stop")){
      int act = parameter[2].toInt();
      
      // -> Form "stop,motor,int[motor]"
      if(parameter[1]==F("motor")){ 
        if(act>=0 && act<MAX_STEPPERS){
          //sMaker.turnOff(act);
          //sMaker.eventLCD();
        }
        else{ Serial.println(F("error,stop(): Motor out of range [0-3]"));}
      }

      // -> Form "stop,pump,int[pump]"
      else if(parameter[1]==F("pump")){ 
        if(act>=0 && act<MAX_PUMPS_NUMBER){
          sMaker.turnOffPump(act);
          //sMaker.eventLCD();
        }
        else{ Serial.println(F("error,stop(): Pump out of range [0-1]"));}
      }

      // -> Form "stop,mix"
      else if (parameter[1]==F("mix")){
        
      }
      else{Serial.println(F("error,stop(): Parameter[1] does not match a type"));}
    }

    // calibration()
    else if(parameter[0]==F("calibration")){
      
      // -> Form "calibration,motor,int[motor],long[revolutions]"
      if(parameter[1]==F("motor")){
        int act = parameter[2].toInt();
        long rev = parameter[3].toInt();
        if(act>=0 && act<MAX_STEPPERS){
          if(rev>0){
            //sMaker.stepperCalibration(rev, act);
            //sMaker.eventLCD();
          }
          else{ Serial.println(F("error,calibration(): Revotutions has to be positive"));}
        }
        else{ Serial.println(F("error,calibration(): Motor out of range [0-3]"));}
      }
      
      // -> Form "calibration,pump,int[pump],long[Time]"
      else if(parameter[1]==F("pump")){
        int act = parameter[2].toInt();
        float Time = parameter[3].toFloat();
        if(act>=0 && act<MAX_PUMPS_NUMBER){
          if(Time>0){
            sMaker.pumpCalibration(Time, act);
            //sMaker.eventLCD();
          }
          else{ Serial.println(F("error,calibration(): Time has to be positive"));}
        }
        else{ Serial.println(F("error,calibration(): Pump out of range [0-1]"));}
      }
      
      // Set parameters for motors/pumps
      // -> Form "calibration,setCalSol,int[id],int[param]" remember real slope parameter = param/700
      else if(parameter[1]==F("setCalSol")){
        int id= parameter[2].toInt();
        int param = parameter[3].toInt();
        if(id>=0 && id<SOLUTIONS){
          if(param>=0 && param<=255){
            sMaker.setCalibrationParameter1(param/700, id);
            sMaker.write_EEPROM(8+id, int(param));
          }
          else{Serial.println(F("error,calibration(): Parameter must be a byte [0-255]"));}
        }
        else{Serial.println(F("error,calibration(): Actuator out of range [0-5]"));}
      }
      // -> Form "calibration,setCalPump,int[id],int[param]"
      else if(parameter[1]==F("setCalPump")){
        int id= parameter[2].toInt();
        int param = parameter[3].toInt();
        if(id>=0 && id<MAX_PUMPS_NUMBER){
          if(param>=0 && param<=255){
            sMaker.setCalibrationParameter(param, id);
            sMaker.write_EEPROM(8+SOLUTIONS+id, param);
          }
          else{Serial.println(F("error,calibration(): Parameter must be a byte [0-255]"));}
        }
        else{Serial.println(F("error,calibration(): Actuator out of range [0-5]"));}
      }
      // -> Form "calibration,setCalPumpSpeed,int[id],int[param]"
      else if(parameter[1]==F("setCalPumpSpeed")){
        int id= parameter[2].toInt();
        int param = parameter[3].toInt();
        if(id>=0 && id<MAX_PUMPS_NUMBER){
          if(param>=0 && param<=255){
            sMaker.__PumpVelocity[id]=param;
            sMaker.write_EEPROM(8+SOLUTIONS+MAX_PUMPS_NUMBER+id, param);
          }
          else{Serial.println(F("error,calibration(): Parameter must be a byte [0-255]"));}
        }
        else{Serial.println(F("error,calibration(): Pump out of range"));}
      }
      // -> Form "calibration,setCalPumpPins,int[id],int[param]"
      else if(parameter[1]==F("setCalPumpSpeed")){
        int id= parameter[2].toInt();
        int param = parameter[3].toInt();
        if(id>=0 && id<MAX_PUMPS_NUMBER){
          if(param>=0 && param<=255){
            sMaker.__Motor[id]=param;
            sMaker.write_EEPROM(8+SOLUTIONS+2*MAX_PUMPS_NUMBER+id, param);
          }
          else{Serial.println(F("error,calibration(): Parameter must be a byte [0-255]"));}
        }
        else{Serial.println(F("error,calibration(): Pump out of range"));}
      }
      // -> Form "calibration,relayTime,int[id],int[param]" param in seconds
      else if(parameter[1]==F("relayTime")){
        int id= parameter[2].toInt();
        int param = parameter[3].toInt();
        if(id>=0 && id<RELAY_NUMBER){
          if(param>=0 && param<=600){
            sMaker.__relay_action_time[id]=param*1000;
            sMaker.write_EEPROM(8+SOLUTIONS+3*MAX_PUMPS_NUMBER+5*MAX_STEPPERS+RELAY_NUMBER+id, param/3);
          }
          else{Serial.println(F("error,calibration(): Parameter must be within [0-600] seconds"));}
        }
        else{Serial.println(F("error,calibration(): Relay out of range"));}
      }
  
    }
    //Set basic config EEPROM
    else if(parameter[0]==F("set,")){
      // -> Form "set,solutions,param"
      if(parameter[1]==F("solutions")){
        int param= parameter[2].toInt();
        if(param>0 && param<=SOLUTIONS){
          sMaker.__solutions=param;
          sMaker.write_EEPROM(0, param);
        }
        else{
          Serial.println(F("error, Must not exceed max solutions amount"));
        }
      }
      // -> Form "set,pumps,param"
      else if(parameter[1]==F("pumps")){
        int param= parameter[2].toInt();
        if(param>0 && param<=MAX_PUMPS_NUMBER){
          sMaker.__max_pumps_number=param;
          sMaker.write_EEPROM(1, param);
        }
        else{
          Serial.println(F("error, Must not exceed max pumps amount"));
        }
      }
       // -> Form "set,cellOffset,param"
      else if(parameter[1]==F("cellOffset")){
        int param = parameter[2].toInt();
        if(param>0 && param<=2550){
          sMaker.__loadCellOffset=param;
          sMaker.write_EEPROM(2, int(-param/10));
        }
        else{
          Serial.println(F("error, If offset is bigger to 2550 plase go to IT department"));
        }
      }
      // -> Form "set,cellScale,param"
      else if(parameter[1]==F("cellScale")){
        int param = -parameter[2].toInt();
        if(param>0 && param<=2550){
          sMaker.__loadCellCalibration=param;
          sMaker.write_EEPROM(3, int(-param/10));
        }
        else{
          Serial.println(F("error, If scale factor is bigger to 2550 plase go to IT department"));
        }
      }
      // -> Form "set,steppers,param"
      else if(parameter[1]==F("steppers")){
        int param = parameter[2].toInt();
        if(param>0 && param<=MAX_STEPPERS){
          sMaker.__max_steppers=param;
          sMaker.write_EEPROM(4, param);
        }
        else{
          Serial.println(F("error, Must not exceed max steppers amount"));
        }
      }
      // -> Form "set,cycle,param"
      else if(parameter[1]==F("steppers")){
        int param = parameter[2].toInt();
        if(param>0 && param<=3600){
          sMaker.__max_steppers=param*1000;
          sMaker.write_EEPROM(5, int(param/15));
        }
        else{
          Serial.println(F("error, Cycle must be lower than 3600s or 1h"));
        }
      }
      // -> Form "set,relays,param"
      else if(parameter[1]==F("relays")){
        int param = parameter[2].toInt();
        if(param>0 && param<=RELAY_NUMBER){
          sMaker.__relay_number=param;
          sMaker.write_EEPROM(6, param);
        }
        else{
          Serial.println(F("error, Must not exceed max relay amount"));
        }
      }
    }

    // ezo()
    else if(parameter[0]==F("ezo")){
      int sensorType = 10;
      String sensorString = "";
      if(parameter[1]==F("ph")){
        sensorType = EZO_PH;
        sensorString = "phmeter";
        if (parameter[2]==F("read")){
          Serial.println(sMaker.__pH);
        }else if (parameter[2]==F("get")){
          int param = parameter[3].toInt();
          float ph=0;
          for (int i=0;i<param;i++){
            //EEPROM.get(4000-i*8,ph);
            Serial.print(ph);
            if(i<param-1){
              Serial.print(F(", "));
            }
          }
        }
      }
      else if(parameter[1]==F("ec")){
        sensorType = EZO_EC;
        sensorString = "ecmeter";
        if (parameter[2]==F("read")){
          Serial.println(sMaker.__eC);
        }else if (parameter[2]==F("get")){
          int param = parameter[3].toInt();
          float ec=0;
          for (int i=0;i<param;i++){
            //EEPROM.get(3996-i*8,ec);
            Serial.print(ec);
            if(i<param-1){
              Serial.print(F(", "));
            }
          }
        }
      }
      if(sensorType==EZO_PH || sensorType==EZO_EC){
        if(parameter[2]==F("restartData")){
          data=0;
        }
        // -> Form "ezo,sensorType[ph/ex],isEnable"
        else if(parameter[2]==F("isEnable") || parameter[2]==F("isEnable\n")){
          if(sMaker.EZOisEnable(sensorType)){
            Serial.print(sensorString);
            Serial.println(F(": Enable"));
          }
          else{
            Serial.print(sensorString);
            Serial.println(F(": Disable"));
          }
        }
        // -> Form "ezo,sensorType[ph/ex],calibration,int[action],float[val]"
        else if(parameter[2]==F("calibration")){
          int action = parameter[3].toInt();
          float val = parameter[4].toFloat();
          sMaker.EZOcalibration(sensorType, action, val);
        }
        else if(parameter[2]==F("exportCal") || parameter[2]==F("exportCal\n")){ // -> Form "ezo,sensorType[ph/ex],exportCal"
          sMaker.EZOexportCal(sensorType); 
        }
        else if(parameter[2]==F("startImport") || parameter[2]==F("startImport\n")){ // -> Form "ezo,sensorType[ph/ex],startImport"
          sMaker.EZOimport(true); 
        }
        else if(parameter[2]==F("stopImport") || parameter[2]==F("stopImport\n")){ // -> Form "ezo,sensorType[ph/ex],stopImport"
          sMaker.EZOimport(false); 
        } 
        else if(parameter[2]==F("importCal") || parameter[2]==F("importCal\n")){ // -> Form "ezo,sensorType[ph/ex],importCal"
          sMaker.EZOimportCalibration(sensorType, parameter[3]); 
        }
        else if(parameter[2]==F("sleep") || parameter[2]==F("sleep\n")){ // -> Form "ezo,sensorType[ph/ex],sleep"
          if(!sMaker.EZOisSleep()) Serial.println(F("warning,EZO Sensors: Enter into sleep mode"));
          sMaker.EZOsleep();
        }
        else if(parameter[2]==F("awake") || parameter[2]==F("awake\n")){ // -> Form "ezo,sensorType[ph/ex],awake"
          if(sMaker.EZOisSleep()) Serial.println(F("warning,EZO Sensors: Awaking sensors"));
          sMaker.EZOawake();
        }
        else{Serial.println(F("error,ezo(): Parameter[2] does not match a type"));}
      }
      else{Serial.println(F("error,ezo(): Parameter[1] does not match a type"));}
    }

    // filter()
    else if(parameter[0]==F("filter")){
      if(parameter[1]==F("notFilter") || parameter[1]==F("notFilter\n")){ // -> Form "filter,notFilter"
        sMaker.notFilter(); 
      }
      else if(parameter[1]==F("default") || parameter[1]==F("default\n")){ // -> Form "filter,defaultFilter"
        sMaker.defaultFilter(); 
      } 
      else if(parameter[1]==F("exponential")){ // -> Form "filter,exponential,float[alpha]"
        float alpha = parameter[2].toFloat();
        if(alpha>0 && alpha<1) sMaker.setExponentialFilter(alpha);
        else Serial.println(F("error,filter(): alpha out of range [0-1]"));
      }
      else if(parameter[1]==F("kalman")){ // -> Form "filter,exponential,kalman,float[noise]"
        float noise = parameter[2].toFloat();
        if(noise>0) sMaker.setKalmanFilter(noise);
        else Serial.println(F("error,filter(): noise has to be positive"));
      }
      else if(parameter[1]==F("print") || parameter[1]==F("print\n")){ // -> Form "filter,print"
        sMaker.printFilter(); 
      }
      else{Serial.println(F("error,filter(): Parameter[1] does not match a type"));}
    }
    //scale loadcell
    // -> Form "sensor,scale,function..."
    else if(parameter[0]==F("sensor")){
      if(parameter[1]==F("scale")){
          // -> Form "sensor,scale,setOffset,#" Set calibration Offset
          if(parameter[2]==F("setOffset")){
            long offset = parameter[3].toInt();
            sMaker.scale->set_offset(offset);
            sMaker.__loadCellOffset=offset;
            sMaker.write_EEPROM(2, int(-offset/10));
          }
          // -> Form "sensor,scale,setScale,#" Set calibration scale
          else if(parameter[2]==F("setScale")){
            float scaleParam = parameter[3].toFloat();
            sMaker.scale->set_scale(scaleParam);
            sMaker.__loadCellCalibration=scaleParam;
            sMaker.write_EEPROM(3, int(-scaleParam/10));
          }
          /*// -> Form "sensor,scale,getOffset" Check calibration offset
          else if(parameter[2]==F("getOffset")){
           if(num>=0 && num<sconfig.scales){
              long value = mySensors->_myScales[num]->_sc->get_offset();
               mySensors->_myScales[num]->printVal(String(value));
            }
          }
          // -> Form "sensor,scale,getScale" Check calibration scale
          else if(parameter[2]==F("getScale")){
            if(num>=0 && num<sconfig.scales){
              float value = mySensors->_myScales[num]->_sc->get_scale();
               mySensors->_myScales[num]->printVal(String(value));
            }
          }*/
          // -> Form "sensor,scale,getRead" Get scale reading with factor
          else if(parameter[2]==F("getRead")){
            Serial.print(F("Scale reading with factor: "));
            Serial.println(sMaker.scale->get_units());
          }
          // -> Form "sensor,scale,getRead_notScale" Get scale reading without scale factor
          else if(parameter[2]==F("getRead_notScale")){
            Serial.print(F("Scale reading without factor: "));
            Serial.println(sMaker.scale->read());
          }
          // -> Form "sensor,scale,getRead_notOffset" Get scale reading without offset
          else if(parameter[2]==F("getRead_notOffset")){
            Serial.print(F("Scale reading without factor and no offset: "));
            Serial.println(sMaker.scale->get_value());
          }
          else Serial.println(F("error,Serial Sensor Scale: Parameter[2] incorrect"));
        }
        else Serial.println(F("error,Serial Sensor Scale: Parameter[1] incorrect"));
      }

    // eeprom()
    else if(parameter[0]==F("eeprom")){
      if(parameter[1]==F("read") || parameter[1]==F("read\n")){ // -> Form "eeprom,read"
        sMaker.read_EEPROM(HIGH); 
      }
      else if(parameter[1]==F("clean") || parameter[1]==F("clean\n")){ // -> Form "eeprom,clean"
        sMaker.clean_EEPROM(); 
      }
      else if(parameter[1]==F("write") || parameter[1]==F("write\n")){ // -> Form "eeprom,write"
        int pos = parameter[2].toInt();
        byte val = parameter[3].toInt();
        sMaker.write_EEPROM(pos,val); 
      }
    }

    else{Serial.println(F("warning, Serial Command Unknown"));}
  }
  input_string_complete = false;
}
