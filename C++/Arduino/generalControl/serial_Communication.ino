/*****   Lecture on serial Events   *****/
/*
 * The structure of the commands are: 
 * 1      2     3           4                5              6               7             8
 * char char  char  # of parameters  lenght_parameter1  parameter1  lenght_parameter2  parameter2
 * Where:
 * 1- Refers to the class
 * 2- Refers to the function
 * 3- Refers to object
 * 4- Number of parameters given
 * 5- Lenght of the 1st parameter
 * 6- Parameter 1
 * 7- Length of the 2nd parameter
 * 8- Parameter 2
 * 9- ...
 * 
 * Notes: Values of 1, 2, 3, 4, 5, 7, 9, etc... are char type, and its value is assigned according to the ASCII Table starting at char(48)="0"
 * For further information see ASCII Table
*/
const char zero_char = char(48);

void serialEvent() {                                  //if the hardware serial port_0 receives a char
  inputstring = Serial.readStringUntil(13);           //read the string until we see a <CR>
  input_string_complete = true;                       //set the flag used to tell if we have received a completed string from the PC
  
  if(input_string_complete==true){

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
        int newEnable = inputstring.substring(2).toInt(); // Form = 03Int
        if(newEnable==0 || newEnable==1){solenoidValve::ptr[0]->enable(newEnable);}
        else{Serial.println(F("Bool Parameter Incorrect. It has to be 0 or 1"));}
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

    if(inputstring.charAt(0)==zero_char+1){ // time info --> '1'
      if(inputstring.charAt(1)==zero_char){ // get hour/minute Function --> '0'
         String newHour = inputstring.substring(2,4); // Form = 10Int(2)Int(2)
         String newMinute = inputstring.substring(4,6);
         if(newHour.toInt()>=0 && newHour.toInt()<24 && newMinute.toInt()>=0 && newMinute.toInt()<60){
          dateHour = newHour.toInt();
          dateMinute = newMinute.toInt();
          Serial.print(F("Recieved Hour Data ")); Serial.print(newHour); Serial.print(F(":")); Serial.print(newMinute); Serial.print(F(" hrs")); 
          if(day1.getState()!=day1.run(dateHour, dateMinute)){
            Serial.print(F("Floor 1: ")); 
            if(day1.getState()){Serial.println(F("Starting Day"));}else{Serial.println(F("Starting Night"));}
            
          }
          if(day2.getState()!=day2.run(dateHour, dateMinute)){
            Serial.print(F("Floor 2: ")); if(day2.getState()){Serial.println(F("Starting Day"));}else{Serial.println(F("Starting Night"));}  
          }
          if(day3.getState()!=day3.run(dateHour, dateMinute)){
            Serial.print(F("Floor 3: ")); if(day3.getState()){Serial.println(F("Starting Day"));}else{Serial.println(F("Starting Night"));}  
          }
          if(day4.getState()!=day4.run(dateHour, dateMinute)){
            Serial.print(F("Floor 4: ")); if(day4.getState()){Serial.println(F("Starting Day"));}else{Serial.println(F("Starting Night"));}  
          }
          
         }
         else{Serial.println(F("Time Format Incorrect."));}    
      }
      
    } 
    
    
  }
  
  input_string_complete = false;
}
