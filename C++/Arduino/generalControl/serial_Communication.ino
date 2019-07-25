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
  
  if(input_string_complete==true){
    String parameter[8];
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
    
    if(parameter[0]=="actuator"){ // Functions executed in Actuator class
      if(parameter[1]=="setTime"){ // Function setTime -> Form "actuator,setTime,type[0-3],floor[0-3],timeOn,cycleTime"
        int Type = parameter[2].toInt();
        int Floor = parameter[3].toInt();
        int timeOn = parameter[4].toInt();
        int cyceTime = parameter[5].toInt();
        if(timeOn<cyceTime && Type>=0 && Floor>=0 && Type<MAX_TYPES_ACTUATOR && Floor<MAX_TYPES_ACTUATOR){
          Actuator::setTimeAll(Type, Floor, timeOn, cyceTime);
        }
       else{Serial.println(F("Actuator Functions: Parameter[2-5] wrong"));}
      }
      else if(parameter[1]=="setTimeOn"){  // Function setTimeOn -> Form "actuator,setTimeOn,type[0-3],floor[0-3],timeOn"
        int Type = parameter[2].toInt();
        int Floor = parameter[3].toInt();
        int timeOn = parameter[4].toInt();
        if(Type>=0 && Floor>=0 && Type<MAX_TYPES_ACTUATOR && Floor<MAX_TYPES_ACTUATOR){
          Actuator::setTimeOnAll(Type, Floor, timeOn);  
        }
        else{Serial.println(F("Actuator Functions: Parameter[2-3] wrong"));}
      }
      else if(parameter[1]=="setCycleTime"){ // Function setCycleTime -> Form "actuator,setCycleTime,type[0-3],floor[0-3],cycleTime"
        int Type = parameter[2].toInt();
        int Floor = parameter[3].toInt();
        int cycleTime = parameter[4].toInt();
        if(Type>=0 && Floor>=0 && Type<MAX_TYPES_ACTUATOR && Floor<MAX_TYPES_ACTUATOR){
          Actuator::setCycleTimeAll(Type, Floor, cycleTime);
        }
        else{Serial.println(F("Actuator Functions: Parameter[2-3] wrong"));}
      }
      else if(parameter[1]=="getTime"){ // Function getTime -> Form "actuator,getTime,type[0-3],floor[0-3]"
        int Type = parameter[2].toInt();
        int Floor = parameter[3].toInt();
        if(Type>=0 && Floor>=0 && Type<MAX_TYPES_ACTUATOR && Floor<MAX_TYPES_ACTUATOR){
          Serial.println(Actuator::getTimeAll(Type, Floor));
        }
        else{Serial.println(F("Actuator Functions: Parameter[2-3] wrong"));}
      }
      else if(parameter[1]=="getTimeOn"){ // Function getTimeOn -> Form "actuator,getTimeOn,type[0-3],floor[0-3]"
        int Type = parameter[2].toInt();
        int Floor = parameter[3].toInt();
        if(Type>=0 && Floor>=0 && Type<MAX_TYPES_ACTUATOR && Floor<MAX_TYPES_ACTUATOR){
          Serial.println(Actuator::getTimeOnAll(Type, Floor));
        }
        else{Serial.println(F("Actuator Functions: Parameter[2-3] wrong"));}
      }
      else if(parameter[1]=="getCycleTime"){ // Function getCycleTime -> Form "actuator,getCycleTime,type[0-3],floor[0-3]"
        int Type = parameter[2].toInt();
        int Floor = parameter[3].toInt();
        if(Type>=0 && Floor>=0 && Type<MAX_TYPES_ACTUATOR && Floor<MAX_TYPES_ACTUATOR){
          Serial.println(Actuator::getCycleTimeAll(Type, Floor));
        }
        else{Serial.println(F("Actuator Functions: Parameter[2-3] wrong"));}
      }
      else{Serial.println(F("Actuator Functions: Parameter[1] unknown"));}
    }

    
    /*
    if(inputstring.charAt(0)==zero_char){ // solenoidValve --> '0'
      if(inputstring.charAt(1)==zero_char){ // setCycleTime Function --> '0'
        float inputTime_minutes = inputstring.substring(2).toFloat(); // Form = 00Float
        unsigned long inputTime_ms = long(inputTime_minutes*60000);
        solenoidValve::ptr[0]->setCycleTime(inputTime_ms);
      }
      else if(inputstring.charAt(1)==zero_char+1){ // setTimeOn Function --> '1'
        float inputTime_seconds= inputstring.substring(3).toFloat(); // Form = 01ValveFloat
        unsigned long inputTime_ms = long(inputTime_seconds*1000);
        if(inputstring.charAt(2)>=zero_char && inputstring.charAt(2)<zero_char+solenoidValve::__TotalActuators){
          solenoidValve::ptr[inputstring.charAt(2)-48]->setTimeOn(inputTime_ms);
          }
        else{Serial.println(F("Solenoid Number Parameter Incorrect"));}
      }
      else if(inputstring.charAt(1)==zero_char+2){ // reOrder Function --> '2'
        int newOrder = inputstring.substring(3).toInt(); // Form = 02ValveInt
        if(inputstring.charAt(2)>=zero_char && inputstring.charAt(2)<zero_char+solenoidValve::__TotalActuators){
          solenoidValve::ptr[inputstring.charAt(2)-48]->reOrder(newOrder);
          }
        else{Serial.println(F("Solenoid Number Parameter Incorrect"));}
      }    
      else if(inputstring.charAt(1)==zero_char+3){ // enable Function --> '3'
        if(inputstring.charAt(2)>=zero_char && inputstring.charAt(2)<zero_char+solenoidValve::__TotalActuators){
          int newEnable = inputstring.substring(3).toInt(); // Form = 03ValveInt
          if(newEnable==0 || newEnable==1){solenoidValve::ptr[inputstring.charAt(2)-48]->enable(newEnable);}
          else{Serial.println(F("Bool Parameter Incorrect. It has to be 0 or 1"));}
        }
        else{Serial.println(F("Solenoid Number Parameter Incorrect"));}  
      }
      else if(inputstring.charAt(1)==zero_char+4){ // restartH2O --> '4'
        if(inputstring.charAt(2)>=zero_char && inputstring.charAt(2)<zero_char+solenoidValve::__TotalActuators){ // Form = 04ValveBool
          int restartAll = inputstring.substring(3).toInt();
          if(restartAll==0){solenoidValve::ptr[inputstring.charAt(2)-48]->restartH2O();}
          else if(restartAll==1){solenoidValve::ptr[inputstring.charAt(2)-48]->restartAllH2O();}
          else{Serial.println(F("Bool Parameter Incorrect. It has to be 0 or 1"));}  
          }
        else{Serial.println(F("Solenoid Number Parameter Incorrect."));}
      }
      else if(inputstring.charAt(1)==zero_char+5){ // reOrder_byFloor Function --> '5' // Form = 05Byte
        int newFloor = inputstring.substring(2).toInt();
        if(newFloor>=0 && newFloor<MAX_FLOOR){solenoidValve::ptr[0]->reOrder_byFloor(newFloor); }
        else{Serial.println(F("Byte Parameter Incorrect. It has to be less than the number of floors into the container"));}
      }
      else if(inputstring.charAt(1)==zero_char+6){ // reOrderAll Function --> '6'
        int newOrder = inputstring.substring(2).toInt(); // Form = 06Byte
        switch(newOrder){
          case 0:
            solenoidValve::ptr[0]->reOrderAll(LOW, LOW);
            break;
          case 1:
           solenoidValve::ptr[0]->reOrderAll(LOW, HIGH);
           break;
          case 2:
           solenoidValve::ptr[0]->reOrderAll(HIGH, HIGH);
           break;
          case 3:
           solenoidValve::ptr[0]->reOrderAll(HIGH, LOW);
           break;
          default:
            Serial.println(F("Byte Parameter Incorrect. It has to be less than 4"));
            break;
        }
      }
      else if(inputstring.charAt(1)==zero_char+7){ // defaultOrder Function --> '7'
        int nightFloor = inputstring.substring(2).toInt()-1; // Form = 07Byte
        if(nightFloor>=0 && nightFloor<MAX_FLOOR){solenoidValve::ptr[0]->defaultOrder(nightFloor);}
        else{Serial.print(F("Byte Parameter Incorrect. It has to be between [1,")); Serial.print(MAX_FLOOR); Serial.println(F("]"));}
      }
    }

    else if(inputstring.charAt(0)==zero_char+1){ // time/led info --> '1'
      if(inputstring.charAt(1)==zero_char){ // get hour/minute Function --> '0'
         String newHour = inputstring.substring(2,4); // Form = 10Int(2)Int(2)
         String newMinute = inputstring.substring(4,6);
         if(newHour.toInt()>=0 && newHour.toInt()<24 && newMinute.toInt()>=0 && newMinute.toInt()<60){
          dateHour = newHour.toInt();
          dateMinute = newMinute.toInt();
          Serial.print(F("Recieved Hour Data ")); Serial.print(newHour); Serial.print(F(":")); Serial.print(newMinute); Serial.println(F(" hrs")); 
          
          if(day1.getState()!=day1.run(dateHour, dateMinute)){
            Serial.print(F("Floor 1: ")); 
            if(day1.getState()){
              Serial.println(F("Starting Day"));
              LED::ptr[0]->turnOn(1);
            }
            else{
              Serial.println(F("Starting Night"));
              LED::ptr[0]->turnOff(1);
              solenoidValve::ptr[0]->defaultOrder(1);
            }
          }
          if(day2.getState()!=day2.run(dateHour, dateMinute)){
            Serial.print(F("Floor 2: ")); 
            if(day2.getState()){
              Serial.println(F("Starting Day"));
              LED::ptr[0]->turnOn(2);
            }
            else{
              Serial.println(F("Starting Night"));
              LED::ptr[0]->turnOff(2);
              solenoidValve::ptr[0]->defaultOrder(2);
            }  
          }
          if(day3.getState()!=day3.run(dateHour, dateMinute)){
            Serial.print(F("Floor 3: ")); 
            if(day3.getState()){
              Serial.println(F("Starting Day"));
              LED::ptr[0]->turnOn(3);
            }
            else{
              Serial.println(F("Starting Night"));
              LED::ptr[0]->turnOff(3);
              solenoidValve::ptr[0]->defaultOrder(3);
            }  
          }
          if(day4.getState()!=day4.run(dateHour, dateMinute)){
            Serial.print(F("Floor 4: ")); 
            if(day4.getState()){
              Serial.println(F("Starting Day"));
              LED::ptr[0]->turnOn(4);
            }
            else{
              Serial.println(F("Starting Night"));
              LED::ptr[0]->turnOff(4);
              solenoidValve::ptr[0]->defaultOrder(4);
            }  
          }
          
         }
         else{Serial.println(F("Time Format Incorrect."));}    
      }
      else if(inputstring.charAt(1)==zero_char+1){ // enable LED Function --> '1'
        String en = inputstring.substring(2,3); // Form = 11Bool(1)Byte(1)Byte(1)
        String fl = inputstring.substring(3,4);
        String zn = inputstring.substring(4,5);
        if( (en.toInt()==0 || en.toInt()==1) && fl.toInt()<=4 && fl.toInt()>=1 && zn.toInt()>=1 && zn.toInt()<=4){
          bool Enable = en.toInt(); 
          byte Floor = fl.toInt(); 
          byte Zone = zn.toInt();
          LED::ptr[0]->enable(Enable, Floor, Zone);
        }
        else if(en.toInt()!=0 && en.toInt()!=1){Serial.println(F("LED: Parameter Enable has to be 0 or 1"));}
        else if(fl.toInt()<=4 || fl.toInt()>=1){Serial.println(F("LED: Parameter Floor has to be between [1,4]"));}
        else if(zn.toInt()>=1 || zn.toInt()<=4){Serial.println(F("LED: Parameter Zone has to be between [1,4]"));}
        else{Serial.println(F("LED: Parameters Incorrect. Unkown Reason."));}
      }  
    }
     
    else if(inputstring.charAt(0)==zero_char+2){ // EPPROM info --> '2'
      if(inputstring.charAt(1)==zero_char){ // clean_EEPROM Function --> '0'
        String confirm = inputstring.substring(2); // Form = 20clean
        if(confirm=="clean"){clean_EEPROM();}
        else{Serial.println(F("EEPROM: Key Confirmation is necesary to execute this action"));}
      }
      else if(inputstring.charAt(1)==zero_char+1){ // print_EEPROM Function --> '1'
        String confirm = inputstring.substring(2); // Form = 21print
        if(confirm=="print"){print_EEPROM();}
        else{Serial.println(F("EEPROM: Key Confirmation is necesary to execute this action"));}
      }
      else if(inputstring.charAt(1)==zero_char+2){ // saveParamters_EEPROM Function --> '2'
        String confirm = inputstring.substring(2); // Form = 22save
        if(confirm=="save"){saveParamters_EEPROM();}
        else{Serial.println(F("EEPROM: Key Confirmation is necesary to execute this action"));}
      }
    }
    */
    /***** Pendiente de aquí para abajo *****/
    /*
    // Revisar pendientes MultiDay
    else if(inputstring.charAt(0)==zero_char+3){ // MultiDay info --> '3'
      if(inputstring.charAt(1)==zero_char){ // redefine Function --> '0'
        // Pendiente programar // Form = 30byteFloatFloat
      }
    }

    // Revisar pendientes analogSensor
    else if(inputstring.charAt(0)==zero_char+4){ // analogSensor info --> '4'
      if(inputstring.charAt(1)==zero_char && inputstring.charAt(2)==zero_char){ // analogSensor object #1 --> '0' 
        // setModel function --> '0'
        // Form = 400byteFloatFloatFloat
      }
      else if(inputstring.charAt(1)==zero_char && inputstring.charAt(2)==zero_char+1){ // analogSensor object #1 --> '0' 
              // notFilter function --> '1'
              // Form = 401
      }
      else if(inputstring.charAt(1)==zero_char && inputstring.charAt(2)==zero_char+2){ // analogSensor object #1 --> '0' 
              // defaultFilter function --> '2'
              // Form = 402
      }
      else if(inputstring.charAt(1)==zero_char && inputstring.charAt(2)==zero_char+3){ // analogSensor object #1 --> '0' 
              // setExponentialFilter function --> '3'
              // Form = 403Float // Float must be between [0,1]
      }
      else if(inputstring.charAt(1)==zero_char && inputstring.charAt(2)==zero_char+4){ // analogSensor object #1 --> '0' 
              // setKalmanFilter function --> '4'
              // Form = 404Float // Float is noise
      }
    }
    
    // Revisar pendientes HVAC controller
    else if(inputstring.charAt(0)==zero_char+5){ // HVAC controller info --> '5'
      if(inputstring.charAt(1)==zero_char){ // changeMode Function --> '0'
        String mod = inputstring.substring(2); // Form = 50Mode
        if( mod.toInt()>=0 && mod.toInt()<3 ){
          byte Mode = mod.toInt();
          HVAC.changeMode(Mode);
        }
        else{Serial.println(F("HVAC Controller: Mode Parameter Incorrect"));}
      }
      else if(inputstring.charAt(1)==zero_char+1){ // changeFan Function --> '1'
        String fan = inputstring.substring(2); // Form = 50fan
        if( fan.toInt()==0 || fan.toInt()==1){
          byte fanMode = fan.toInt();
          HVAC.changeFan(fanMode);
        }
        else{Serial.println(F("HVAC Controller: Fan Parameter Incorrect"));}
      }
    }
    */
  }
  input_string_complete = false;
}
