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
    
    else if(inputstring.charAt(0)==zero_char+1){ // calibration Functions --> '1'
      
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

    else if(inputstring.charAt(0)==zero_char+2){ // Filters Functions --> '2'
      if(inputstring.charAt(1)==zero_char){ // notFilter() -> Form '20'
        sMaker.notFilter();
      }
      else if(inputstring.charAt(1)==zero_char+1){ // defaultFilter() -> Form '21'
        sMaker.defaultFilter();
      }
      else if(inputstring.charAt(1)==zero_char+2){ // setExpontentialFilter() -> Form '22Float'
        float alpha = inputstring.substring(2).toFloat();
        sMaker.setExponentialFilter(alpha);
      }
      else if(inputstring.charAt(1)==zero_char+3){ // setKalmanFilter() -> Form '23Float'
        float noise = inputstring.substring(2).toFloat();
        sMaker.setKalmanFilter(noise);
      }
      else if(inputstring.charAt(1)==zero_char+4){ // printFilter() -> Form '24'
        sMaker.printFilter();
      }
    }
    
    else if(inputstring.charAt(0)==zero_char+3){ // EEPROM Functions --> '3'
      if(inputstring.charAt(1)==zero_char && inputstring.charAt(2)==zero_char+3){ // print_EEPROM() -> Form '303'
        print_EEPROM();
      }
      else if(inputstring.charAt(1)==zero_char+1 && inputstring.charAt(2)==zero_char+3){ // read_EEPROM() -> Form '313'
        read_EEPROM(HIGH);
      }
      else if(inputstring.charAt(1)==zero_char+2 && inputstring.charAt(2)==zero_char+3){ // clean_EEPROM() -> Form '323'
        clean_EEPROM();
      }
    }
  }
  input_string_complete = false;
}
