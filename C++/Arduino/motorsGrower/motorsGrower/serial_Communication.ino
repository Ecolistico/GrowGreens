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

      else if(inputstring.charAt(1)==zero_char+6){ // sequence() -> Form '06BoolDistX(4)DistY(4)
        long x_mm = inputstring.substring(3, 7).toInt();
        long y_mm = inputstring.substring(7, 11).toInt();
        switch(inputstring.charAt(2)){
          case zero_char:
            grower1.sequence(x_mm, y_mm);
            break;
          case zero_char+1:
            grower2.sequence(x_mm, y_mm);
            break;
          case zero_char+2:
            grower3.sequence(x_mm, y_mm);
            break;
          case zero_char+3:
            grower4.sequence(x_mm, y_mm);
            break;
          default:
            Serial.println(F("Parameter incorrect: Grower number do not exist"));
            break;
        }
      }

      else if(inputstring.charAt(1)==zero_char+7){ // continueSequence() -> Form '07Bool
        switch(inputstring.charAt(2)){
          case zero_char:
            grower1.continueSequence();
            break;
          case zero_char+1:
            grower2.continueSequence();
            break;
          case zero_char+2:
            grower3.continueSequence();
            break;
          case zero_char+3:
            grower4.continueSequence();
            break;
          default:
            Serial.println(F("Parameter incorrect: Grower number do not exist"));
            break;
        }
      }

      else if(inputstring.charAt(1)==zero_char+8){ // stopSequence() -> Form '08Bool
        switch(inputstring.charAt(2)){
          case zero_char:
            grower1.stopSequence();
            break;
          case zero_char+1:
            grower2.stopSequence();
            break;
          case zero_char+2:
            grower3.stopSequence();
            break;
          case zero_char+3:
            grower4.stopSequence();
            break;
          default:
            Serial.println(F("Parameter incorrect: Grower number do not exist"));
            break;
        }
      }

      else if(inputstring.charAt(1)==zero_char+9){ // stop() -> Form '09Bool
        switch(inputstring.charAt(2)){
          case zero_char:
            grower1.stop();
            break;
          case zero_char+1:
            grower2.stop();
            break;
          case zero_char+2:
            grower3.stop();
            break;
          case zero_char+3:
            grower4.stop();
            break;
          default:
            Serial.println(F("Parameter incorrect: Grower number do not exist"));
            break;
        }
      }
    }
    
    else if(inputstring.charAt(0)==zero_char+1){ // function of general information--> '1'
      
      if(inputstring.charAt(1)==zero_char){ // get and print MaxX and MaxY() -> Form '10Byte'
        switch (inputstring.charAt(2)){
          long maxX, maxY;
          case zero_char:
            maxX = grower1.getMaxDistanceX();
            maxY = grower1.getMaxDistanceY();
            Serial.println(String(maxX) + "," + String(maxY));
            break;
          case zero_char+1:
            maxX = grower2.getMaxDistanceX();
            maxY = grower2.getMaxDistanceY();
            Serial.println(String(maxX) + "," + String(maxY));
            break;
          case zero_char+2:
            maxX = grower3.getMaxDistanceX();
            maxY = grower3.getMaxDistanceY();
            Serial.println(String(maxX) + "," + String(maxY));
            break;
          case zero_char+3:
            maxX = grower4.getMaxDistanceX();
            maxY = grower4.getMaxDistanceY();
            Serial.println(String(maxX) + "," + String(maxY));
            break;
          default:
            Serial.println(F("Parameter incorrect: Grower number do not exist"));
            break;
        }
      }

      else if(inputstring.charAt(1)==zero_char+1){ // get and print position() -> Form '11Byte'
        switch (inputstring.charAt(2)){
          long posX, posY;
          case zero_char:
            posX = grower1.getXPosition();
            posY = grower1.getYPosition();
            Serial.println(String(posX) + "," + String(posY));
            break;
          case zero_char+1:
            posX = grower2.getXPosition();
            posY = grower2.getYPosition();
            Serial.println(String(posX) + "," + String(posY));
            break;
          case zero_char+2:
            posX = grower3.getXPosition();
            posY = grower3.getYPosition();
            Serial.println(String(posX) + "," + String(posY));
            break;
          case zero_char+3:
            posX = grower4.getXPosition();
            posY = grower4.getYPosition();
            Serial.println(String(posX) + "," + String(posY));
            break;
          default:
            Serial.println(F("Parameter incorrect: Grower number do not exist"));
            break;
        }
      }

      else if(inputstring.charAt(1)==zero_char+2){ // get and print isAvailable() -> Form '12Byte'
        switch (inputstring.charAt(2)){
          case zero_char:
            grower1.isAvailable();
            break;
          case zero_char+1:
            grower2.isAvailable();
            break;
          case zero_char+2:
            grower3.isAvailable();
            break;
          case zero_char+3:
            grower4.isAvailable();
            break;
          default:
            Serial.println(F("Parameter incorrect: Grower number do not exist"));
            break;
        }
      }
    }
    
    else if(inputstring.charAt(0)==zero_char+2){ // Enable/Disable functions --> '2'  
      if(inputstring.charAt(1)==zero_char){ // disable() -> Form '20Byte'
        switch (inputstring.charAt(2)){
          case zero_char:
            grower1.disable();
            break;
          case zero_char+1:
            grower2.disable();
            break;
          case zero_char+2:
            grower3.disable();
            break;
          case zero_char+3:
            grower4.disable();
            break;
          default:
            Serial.println(F("Parameter incorrect: Grower number do not exist"));
            break;
        }
      }

      else if(inputstring.charAt(1)==zero_char+1){ // enable() -> Form '21Byte'
        switch (inputstring.charAt(2)){
          case zero_char:
            grower1.enable();
            break;
          case zero_char+1:
            grower2.enable();
            break;
          case zero_char+2:
            grower3.enable();
            break;
          case zero_char+3:
            grower4.enable();
            break;
          default:
            Serial.println(F("Parameter incorrect: Grower number do not exist"));
            break;
        }
      }
    }

    else if(inputstring.charAt(0)==zero_char+3){ // EEPROM functions --> '3'
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
