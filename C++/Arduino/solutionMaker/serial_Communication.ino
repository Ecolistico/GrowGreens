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
    if(inputstring.charAt(0)==zero_char){ // action Functions --> '0'
      
      if(inputstring.charAt(1)==zero_char){ // dispense() and dispenseAcid() -> Form '00ByteNumber' (Number can be int, long or float)
        if(inputstring.charAt(2)-zero_char<MAX_SOLUTIONS_NUMBER){ //dispense()
          byte act = inputstring.charAt(2)-zero_char;
          long grams = inputstring.substring(3).toInt();
          sMaker.dispense(grams, act);
          sMaker.eventLCD();
        }
        else if(inputstring.charAt(2)-zero_char>=MAX_SOLUTIONS_NUMBER && inputstring.charAt(2)-zero_char<MAX_SOLUTIONS_NUMBER+MAX_PUMPS_NUMBER){ //dispenseAcid()
          byte act = inputstring.charAt(2)-MAX_SOLUTIONS_NUMBER-zero_char;
          float mililiters = inputstring.substring(3).toFloat();
          sMaker.dispenseAcid(mililiters, act);
          sMaker.eventLCD();
        }
        else{Serial.println(F("Parameter incorrect: Actuator does not exist"));}
      }

      else if(inputstring.charAt(1)==zero_char+9){ // stop() -> Form '09Byte'
        if(inputstring.charAt(2)-zero_char<MAX_SOLUTIONS_NUMBER+MAX_PUMPS_NUMBER){
          byte act = inputstring.charAt(2)-zero_char;
          sMaker.stop(act);
          sMaker.eventLCD();
        }
        else{Serial.println(F("Parameter incorrect: Actuator does not exist"));}
      }
    }
    
    else if(inputstring.charAt(0)==zero_char+1){ // calibration(motors/Pumps) Functions --> '1'
      
      if(inputstring.charAt(1)==zero_char){ // stepperCalibration() and stepperCalibration() -> Form '10ByteNumber' (Number can be int, long or float)
        if(inputstring.charAt(2)-zero_char<MAX_SOLUTIONS_NUMBER){ //stepperCalibration()
          byte act = inputstring.charAt(2)-zero_char;
          long rev = inputstring.substring(3).toInt();
          sMaker.stepperCalibration(rev, act);
          sMaker.eventLCD();
        }
        else if(inputstring.charAt(2)-zero_char>=MAX_SOLUTIONS_NUMBER && inputstring.charAt(2)-zero_char<MAX_SOLUTIONS_NUMBER+MAX_PUMPS_NUMBER){ //pumpCalibration()
          byte act = inputstring.charAt(2)-MAX_SOLUTIONS_NUMBER-zero_char;
          float Time = inputstring.substring(3).toFloat();
          sMaker.pumpCalibration(Time, act);
          sMaker.eventLCD();
        }
        else{Serial.println(F("Parameter incorrect: Actuator does not exist"));}
      }

      else if(inputstring.charAt(1)==zero_char+1){ // setCalibrationParameter() -> Form '11ByteByte'
        if(inputstring.charAt(2)-zero_char<MAX_SOLUTIONS_NUMBER+MAX_PUMPS_NUMBER){
          byte act = inputstring.charAt(2)-zero_char;
          int param = inputstring.substring(3).toInt();
          if(param>=0 && param<=255){
            sMaker.setCalibrationParameter(param, act);
            sMaker.eventLCD();
            write_EEPROM(act, param);
          }
          else{Serial.println(F("Parameter incorrect: Parameter must be a byte [0-255]"));}
        }
        else{Serial.println(F("Parameter incorrect: Actuator does not exist"));}
      }
    }

    else if(inputstring.charAt(0)==zero_char+2){ // calibration(acid/condictivity functions) Functions --> '2'
      
    }
    
    else if(inputstring.charAt(0)==zero_char+3){ // EZOsensors(ph/ec) Functions --> '3'
      
      if(inputstring.charAt(1)==zero_char){ // EZOisEnable(sensorType) -> Form '30Byte'
        byte sensorType = inputstring.charAt(2)-zero_char;
        if(sensorType == EZO_PH){
          if(sMaker.EZOisEnable(sensorType)) Serial.println(F("Solution Maker (phMeter): Enable"));
          else Serial.println(F("Solution Maker (phMeter): Disable"));
        }
        else if(sensorType == EZO_EC){
          if(sMaker.EZOisEnable(sensorType)) Serial.println(F("Solution Maker (ecMeter): Enable"));
          else Serial.println(F("Solution Maker (ecMeter): Disable"));
        }
        else{Serial.println(F("Parameter incorrect: EZO sensor does not match a type"));}
      }

      else if(inputstring.charAt(1)==zero_char+1){ // EZOcalibration(sensorType, act, value) -> Form '31ByteByteFloat'
        byte sensorType = inputstring.charAt(2)-zero_char;
        byte act = inputstring.charAt(3)-zero_char;
        float val = inputstring.substring(4).toFloat();
        sMaker.EZOcalibration(sensorType, act, val);
      }

      else if(inputstring.charAt(1)==zero_char+2){ // EZOexportCal(sensorType) -> Form '32Byte'
        byte sensorType = inputstring.charAt(2)-zero_char;
        sMaker.EZOexportCal(sensorType);
      }

      else if(inputstring.charAt(1)==zero_char+3){ // EZOimport(start) -> Form '33Bool'
        byte start = inputstring.charAt(2)-zero_char;
        if(start == 0) sMaker.EZOimport(false);
        else if(start == 1) sMaker.EZOimport(true);
        else Serial.println(F("Parameter incorrect. It has to be 0/1"));
        
      }
      
      else if(inputstring.charAt(1)==zero_char+4){ // EZOimportCalibration(sensorType, parameters) -> Form '34ByteString'
        byte sensorType = inputstring.charAt(2)-zero_char;
        String parameters = inputstring.substring(3);
        sMaker.EZOimportCalibration(sensorType, parameters);
      }

      else if(inputstring.charAt(1)==zero_char+5){ // EZOsleep() -> Form '35'
        if(!sMaker.EZOisSleep()) Serial.println(F("Solution Maker (EZO Sensors): Enter into sleep mode"));
        sMaker.EZOsleep();
      }

      else if(inputstring.charAt(1)==zero_char+6){ // EZOawake() -> Form '36'
        if(sMaker.EZOisSleep()) Serial.println(F("Solution Maker (EZO Sensors): Awaking sensors"));
        sMaker.EZOawake();
      }
    }
    
    else if(inputstring.charAt(0)==zero_char+4){ // Filters Functions --> '4'
      if(inputstring.charAt(1)==zero_char){ // notFilter() -> Form '40'
        sMaker.notFilter();
      }
      else if(inputstring.charAt(1)==zero_char+1){ // defaultFilter() -> Form '41'
        sMaker.defaultFilter();
      }
      else if(inputstring.charAt(1)==zero_char+2){ // setExpontentialFilter() -> Form '42Float'
        float alpha = inputstring.substring(2).toFloat();
        sMaker.setExponentialFilter(alpha);
      }
      else if(inputstring.charAt(1)==zero_char+3){ // setKalmanFilter() -> Form '43Float'
        float noise = inputstring.substring(2).toFloat();
        sMaker.setKalmanFilter(noise);
      }
      else if(inputstring.charAt(1)==zero_char+4){ // printFilter() -> Form '44'
        sMaker.printFilter();
      }
    }
    
    else if(inputstring.charAt(0)==zero_char+5){ // EEPROM Functions --> '5'
      if(inputstring.charAt(1)==zero_char && inputstring.charAt(2)==zero_char+3){ // print_EEPROM() -> Form '503'
        print_EEPROM();
      }
      else if(inputstring.charAt(1)==zero_char+1 && inputstring.charAt(2)==zero_char+3){ // read_EEPROM() -> Form '513'
        read_EEPROM(HIGH);
      }
      else if(inputstring.charAt(1)==zero_char+2 && inputstring.charAt(2)==zero_char+3){ // clean_EEPROM() -> Form '523'
        clean_EEPROM();
      }
    }
  }
  input_string_complete = false;
}
