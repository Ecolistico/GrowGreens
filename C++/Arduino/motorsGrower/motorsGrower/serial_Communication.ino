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
    String parameter[7];
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

    // home()
    if(parameter[0]==F("home")){
      int gr = parameter[1].toInt();
      switch (gr){
        case 1:
          grower1.home();
          break;
        case 2:
          grower2.home();
          break;
        case 3:
          grower3.home();
          break;
        case 4:
          grower4.home();
          break;
        default:
          Serial.println(F("error,home(): Grower number does not exist"));
          break;
      }
    }

    // home2()
    else if(parameter[0]==F("home2")){
      int gr = parameter[1].toInt();
      switch (gr){
        case 1:
          grower1.home2();
          break;
        case 2:
          grower2.home2();
          break;
        case 3:
          grower3.home2();
          break;
        case 4:
          grower4.home2();
          break;
        default:
          Serial.println(F("error,home2(): Grower number does not exist"));
          break;
      }
    }

    // calibration()
    else if(parameter[0]==F("calibration")){
      int gr = parameter[1].toInt();
      switch(gr){
        case 1:
          grower1.calibration();
          break;
        case 2:
          grower2.calibration();
          break;
        case 3:
          grower3.calibration();
          break;
        case 4:
          grower4.calibration();
          break;
        default:
          Serial.println(F("error,calibration(): Grower number does not exist"));
          break;
      }
    }

    // moveX()
    else if(parameter[0]==F("moveX")){
      int gr = parameter[1].toInt();
      long some_mm = parameter[2].toInt();
      switch(gr){
        case 1:
          grower1.moveX(some_mm);
          break;
        case 2:
          grower2.moveX(some_mm);
          break;
        case 3:
          grower3.moveX(some_mm);
          break;
        case 4:
          grower4.moveX(some_mm);
          break;
        default:
          Serial.println(F("error,moveX(): Grower number does not exist"));
          break;
      }
    }

    // moveY()
    else if(parameter[0]==F("moveY")){
      int gr = parameter[1].toInt();
      long some_mm = parameter[2].toInt();
      switch(gr){
        case 1:
          grower1.moveY(some_mm);
          break;
        case 2:
          grower2.moveY(some_mm);
          break;
        case 3:
          grower3.moveY(some_mm);
          break;
        case 4:
          grower4.moveY(some_mm);
          break;
        default:
          Serial.println(F("error,moveY(): Grower number does not exist"));
          break;
      }
    }

    // movePosXY()
    else if(parameter[0]==F("movePosXY")){
      int gr = parameter[1].toInt();
      long x_mm = parameter[2].toInt();
      long y_mm = parameter[3].toInt();
      switch(gr){
        case 1:
          grower1.movePosXY(x_mm, y_mm);
          break;
        case 2:
          grower2.movePosXY(x_mm, y_mm);
          break;
        case 3:
          grower3.movePosXY(x_mm, y_mm);
          break;
        case 4:
          grower4.movePosXY(x_mm, y_mm);
          break;
        default:
          Serial.println(F("error,sequence(): Grower number does not exist"));
          break;
      }
    }

    // moveXTo()
    else if(parameter[0]==F("moveXTo")){
      int gr = parameter[1].toInt();
      long some_mm = parameter[2].toInt();
      switch(gr){
        case 1:
          grower1.moveXTo(some_mm);
          break;
        case 2:
          grower2.moveXTo(some_mm);
          break;
        case 3:
          grower3.moveXTo(some_mm);
          break;
        case 4:
          grower4.moveXTo(some_mm);
          break;
        default:
          Serial.println(F("error,moveXTo(): Grower number does not exist"));
          break;
      }
    }

    // moveYTo()
    else if(parameter[0]==F("moveYTo")){
      int gr = parameter[1].toInt();
      long some_mm = parameter[2].toInt();
      switch(gr){
        case 1:
          grower1.moveYTo(some_mm);
          break;
        case 2:
          grower2.moveYTo(some_mm);
          break;
        case 3:
          grower3.moveYTo(some_mm);
          break;
        case 4:
          grower4.moveYTo(some_mm);
          break;
        default:
          Serial.println(F("error,moveYTo(): Grower number does not exist"));
          break;
      }
    }

    // sequence()
    else if(parameter[0]==F("sequence")){
      int gr = parameter[1].toInt();
      long x_mm = parameter[2].toInt();
      long y_mm = parameter[3].toInt();
      bool capture = true;
      if (parameter[4]==F("false") || parameter[4]==F("false\n")) capture = false;
      switch(gr){
        case 1:
          grower1.sequence(x_mm, y_mm, capture);
          break;
        case 2:
          grower2.sequence(x_mm, y_mm, capture);
          break;
        case 3:
          grower3.sequence(x_mm, y_mm, capture);
          break;
        case 4:
          grower4.sequence(x_mm, y_mm, capture);
          break;
        default:
          Serial.println(F("error,sequence(): Grower number does not exist"));
          break;
      }
    }

    // sequence_n()
    else if(parameter[0]==F("sequence_n")){
      int gr = parameter[1].toInt();
      uint8_t nodes_x = parameter[2].toInt();
      uint8_t nodes_y = parameter[3].toInt();
      bool capture = true;
      if (parameter[4]==F("false") || parameter[4]==F("false\n")) capture = false;
      switch(gr){
        case 1:
          grower1.sequence_n(nodes_x, nodes_y, capture);
          break;
        case 2:
          grower2.sequence_n(nodes_x, nodes_y, capture);
          break;
        case 3:
          grower3.sequence_n(nodes_x, nodes_y, capture);
          break;
        case 4:
          grower4.sequence_n(nodes_x, nodes_y, capture);
          break;
        default:
          Serial.println(F("error,sequence_n(): Grower number does not exist"));
          break;
      }
    }

    // continueSequence()
    else if(parameter[0]==F("continueSequence")){
      int gr = parameter[1].toInt();
      switch(gr){
        case 1:
          grower1.continueSequence();
          break;
        case 2:
          grower2.continueSequence();
          break;
        case 3:
          grower3.continueSequence();
          break;
        case 4:
          grower4.continueSequence();
          break;
        default:
          Serial.println(F("error,continueSequence(): Grower number does not exist"));
          break;
      }
    }

    // stopSequence()
    else if(parameter[0]==F("stopSequence")){
      int gr = parameter[1].toInt();
      switch(gr){
        case 1:
          grower1.stopSequence();
          break;
        case 2:
          grower2.stopSequence();
          break;
        case 3:
          grower3.stopSequence();
          break;
        case 4:
          grower4.stopSequence();
          break;
        default:
          Serial.println(F("error,stopSequence(): Grower number does not exist"));
          break;
      }
    }

    // stop()
    else if(parameter[0]==F("stop")){
      int gr = parameter[1].toInt();
      switch(gr){
        case 1:
          grower1.stop();
          break;
        case 2:
          grower2.stop();
          break;
        case 3:
          grower3.stop();
          break;
        case 4:
          grower4.stop();
          break;
        default:
          Serial.println(F("error,stop(): Grower number does not exist"));
          break;
      }
    }

    // maxDistance()
    else if(parameter[0]==F("maxDistance")){
      int gr = parameter[1].toInt();
      long maxX, maxY;
      bool success = false;
      switch (gr){
        case 1:
          maxX = grower1.getMaxDistanceX();
          maxY = grower1.getMaxDistanceY();
          success = true;
          break;
        case 2:
          maxX = grower2.getMaxDistanceX();
          maxY = grower2.getMaxDistanceY();
          success = true;
          break;
        case 3:
          maxX = grower3.getMaxDistanceX();
          maxY = grower3.getMaxDistanceY();
          success = true;
          break;
        case 4:
          maxX = grower4.getMaxDistanceX();
          maxY = grower4.getMaxDistanceY();
          success = true;
          break;
        default:
          Serial.println(F("error,maxDistance(): Grower number does not exist"));
          break;
      }
      if(success){
          Serial.print(F("warning,Grower"));
          Serial.print(gr);
          Serial.print(": maxX=");
          Serial.print(maxX);
          Serial.print(F(" - maxY="));
          Serial.println(maxY);
      }
    }

    // position()
    else if(parameter[0]==F("position")){
      int gr = parameter[1].toInt();
      bool success = false;
      long posX, posY;
      switch(gr){
        case 1:
          posX = grower1.getXPosition();
          posY = grower1.getYPosition();
          success = true;
          break;
        case 2:
          posX = grower2.getXPosition();
          posY = grower2.getYPosition();
          success = true;
          break;
        case 3:
          posX = grower3.getXPosition();
          posY = grower3.getYPosition();
          success = true;
          break;
        case 4:
          posX = grower4.getXPosition();
          posY = grower4.getYPosition();
          success = true;
          break;
        default:
          Serial.println(F("error,position(): Grower number does not exist"));
          break;
      }
      if(success){
          Serial.print(F("warning,Grower"));
          Serial.print(gr);
          Serial.print(": posX=");
          Serial.print(posX);
          Serial.print(F(" - posY="));
          Serial.println(posY);
      }
    }

    // available()
    else if(parameter[0]==F("available")){
      int gr = parameter[1].toInt();
      switch(gr){
        case 1:
          grower1.isAvailable();
          break;
        case 2:
          grower2.isAvailable();
          break;
        case 3:
          grower3.isAvailable();
          break;
        case 4:
          grower4.isAvailable();
          break;
        default:
          Serial.println(F("error,available(): Grower number does not exist"));
          break;
      }
    }

    // enable()
    else if(parameter[0]==F("enable")){
      int gr = parameter[1].toInt();
      switch(gr){
        case 1:
          grower1.enable();
          break;
        case 2:
          grower2.enable();
          break;
        case 3:
          grower3.enable();
          break;
        case 4:
          grower4.enable();
          break;
        default:
          Serial.println(F("error,enable(): Grower number does not exist"));
          break;
      }
    }

    // disable()
    else if(parameter[0]==F("disable")){
      int gr = parameter[1].toInt();
      switch(gr){
        case 1:
          grower1.disable();
          break;
        case 2:
          grower2.disable();
          break;
        case 3:
          grower3.disable();
          break;
        case 4:
          grower4.disable();
          break;
        default:
          Serial.println(F("error,disable(): Grower number does not exist"));
          break;
      }
    }

    // eeprom
    else if(parameter[0]==F("eeprom")){
      if(parameter[1]==F("print") || parameter[1]==F("print\n")){ print_EEPROM(); }
      else if(parameter[1]==F("read") || parameter[1]==F("read\n")){ read_EEPROM(HIGH); }
      else if(parameter[1]==F("clean")|| parameter[1]==F("clean\n")){ clean_EEPROM(); }
      else{ Serial.println(F("warning,eeprom(): parameter[1] does not match a type")); }
    }

    // Config EEPROM
    else if(parameter[0]==F("config")){
      int gr = parameter[1].toInt();
      int steps = parameter[2].toInt();
      int microsteps = parameter[3].toInt();
      int pulleyTeeth = parameter[4].toInt();
      int xTooth = parameter[5].toInt();
      int yTooth = parameter[6].toInt();
      saveConfig(gr, steps, microsteps, pulleyTeeth, xTooth, yTooth);
    }

    else{ Serial.println(F("warning, Serial Command Unknown")); }
  }
  input_string_complete = false;
}
