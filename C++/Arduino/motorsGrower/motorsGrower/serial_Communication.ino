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
    if(inputstring.charAt(0)==zero_char){ // function of movement --> '0' 
      
      if(inputstring.charAt(1)==zero_char){ // home() -> Form '00Byte'
        switch (inputstring.charAt(2)){
          case zero_char:
            grower1.home();
            break;
          case zero_char+1:
            grower2.home();
            break;
          case zero_char+2:
            grower3.home();
            break;
          case zero_char+3:
            grower4.home();
            break;
          default:
            Serial.println(F("Parameter incorrect: Grower number do not exist"));
            break;
        }
      }
      
      else if(inputstring.charAt(1)==zero_char+1){ // calibration() -> Form '01Byte'
        switch(inputstring.charAt(2)){
          case zero_char:
            grower1.calibration();
            break;
          case zero_char+1:
            grower2.calibration();
            break;
          case zero_char+2:
            grower3.calibration();
            break;
          case zero_char+3:
            grower3.calibration();
            break;
          default:
            Serial.println(F("Parameter incorrect: Grower number do not exist"));
            break;
        }
      }

      else if(inputstring.charAt(1)==zero_char+2){ // moveX() -> Form '02BoolDist
        long some_mm = inputstring.substring(3).toInt();
        switch(inputstring.charAt(2)){
          case zero_char:
            grower1.moveX(some_mm);
            break;
          case zero_char+1:
            grower2.moveX(some_mm);
            break;
          case zero_char+2:
            grower3.moveX(some_mm);
            break;
          case zero_char+3:
            grower4.moveX(some_mm);
            break;
          default:
            Serial.println(F("Parameter incorrect: Grower number do not exist"));
            break;
        }
      }

      else if(inputstring.charAt(1)==zero_char+3){ // moveY() -> Form '03BoolDist
        long some_mm = inputstring.substring(3).toInt();
        switch(inputstring.charAt(2)){
          case zero_char:
            grower1.moveY(some_mm);
            break;
          case zero_char+1:
            grower2.moveY(some_mm);
            break;
          case zero_char+2:
            grower3.moveY(some_mm);
            break;
          case zero_char+3:
            grower4.moveY(some_mm);
            break;
          default:
            Serial.println(F("Parameter incorrect: Grower number do not exist"));
            break;
        }
      }

      else if(inputstring.charAt(1)==zero_char+4){ // moveXTo() -> Form '04BoolDist
        long some_mm = inputstring.substring(3).toInt();
        switch(inputstring.charAt(2)){
          case zero_char:
            grower1.moveXTo(some_mm);
            break;
          case zero_char+1:
            grower2.moveXTo(some_mm);
            break;
          case zero_char+2:
            grower3.moveXTo(some_mm);
            break;
          case zero_char+3:
            grower4.moveXTo(some_mm);
            break;
          default:
            Serial.println(F("Parameter incorrect: Grower number do not exist"));
            break;
        }
      }

      else if(inputstring.charAt(1)==zero_char+5){ // moveYTo() -> Form '05BoolDist
        long some_mm = inputstring.substring(3).toInt();
        switch(inputstring.charAt(2)){
          case zero_char:
            grower1.moveYTo(some_mm);
            break;
          case zero_char+1:
            grower2.moveYTo(some_mm);
            break;
          case zero_char+2:
            grower3.moveYTo(some_mm);
            break;
          case zero_char+3:
            grower4.moveYTo(some_mm);
            break;
          default:
            Serial.println(F("Parameter incorrect: Grower number do not exist"));
            break;
        }
      }
    }
    
    else if(inputstring.charAt(0)==zero_char+1){} // function --> '1' 
    else if(inputstring.charAt(0)==zero_char+2){} // function --> '2'
    // etc...
  }
  input_string_complete = false;
}
