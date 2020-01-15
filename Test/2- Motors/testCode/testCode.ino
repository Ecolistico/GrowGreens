#include "growerStepper.h"

const byte gr_XStep1 = 27;
const byte gr_XDir1 = 29;
const byte gr_XStep2 = 33;
const byte gr_XDir2 = 35; 
const byte gr_YStep = 39;
const byte gr_YDir = 41; 
const byte gr_En = 31;

const byte gr_XHome1 = A13;
const byte gr_XHome2 = A14;
const byte gr_YHome = A15;

growerStepper grower(
  1,
  gr_XDir1, 
  gr_XStep1, 
  gr_XDir2, 
  gr_XStep2, 
  gr_YDir, 
  gr_YStep, 
  gr_XHome1, 
  gr_XHome2, 
  gr_YHome,
  gr_En
  );

// Serial comunication
String inputstring = "";
bool input_string_complete = false;
const char zero_char = char(48);

void serialEvent() {                                  //if the hardware serial port_0 receives a char
  inputstring = Serial.readStringUntil(13);           //read the string until we see a <CR>
  input_string_complete = true;                       //set the flag used to tell if we have received a completed string from the PC
  
  if(input_string_complete==true){
    if(inputstring.charAt(0)==zero_char){ // function of movement --> '0' 
      // home() -> Form '00Byte'
      if(inputstring.charAt(1)==zero_char){ grower.home(); }
      // calibration() -> Form '01'
      else if(inputstring.charAt(1)==zero_char+1){ grower.calibration(); }
      // moveX() -> Form '02Dist
      else if(inputstring.charAt(1)==zero_char+2){
        long some_mm = inputstring.substring(3).toInt();
        grower.moveX(some_mm);
      }
      // moveY() -> Form '03Dist
      else if(inputstring.charAt(1)==zero_char+3){
        long some_mm = inputstring.substring(3).toInt();
        grower.moveY(some_mm);
      }
      // moveXTo() -> Form '04Dist
      else if(inputstring.charAt(1)==zero_char+4){
        long some_mm = inputstring.substring(3).toInt();
        grower.moveXTo(some_mm);
      }
      // moveYTo() -> Form '05Dist
      else if(inputstring.charAt(1)==zero_char+5){
        long some_mm = inputstring.substring(3).toInt();
        grower.moveYTo(some_mm);
      }
      // sequence() -> Form '06DistX(4)DistY(4)
      else if(inputstring.charAt(1)==zero_char+6){
        long x_mm = inputstring.substring(2, 6).toInt();
        long y_mm = inputstring.substring(6, 10).toInt();
        grower.sequence(x_mm, y_mm);
      }
      // continueSequence() -> Form '07'
      else if(inputstring.charAt(1)==zero_char+7){ grower.continueSequence(); }
      // stopSequence() -> Form '08'
      else if(inputstring.charAt(1)==zero_char+8){ grower.stopSequence(); }
      // stop() -> Form '09'
      else if(inputstring.charAt(1)==zero_char+9){ grower.stop(); }
    }
    
    else if(inputstring.charAt(0)==zero_char+1){ // function of general information--> '1'
      if(inputstring.charAt(1)==zero_char){ // get and print MaxX and MaxY() -> Form '10'
        long maxX, maxY;
        maxX = grower.getMaxDistanceX();
        maxY = grower.getMaxDistanceY();
        Serial.println(String(maxX) + "," + String(maxY));
      }
      else if(inputstring.charAt(1)==zero_char+1){ // get and print position() -> Form '11'
        long posX, posY;
        posX = grower.getXPosition();
        posY = grower.getYPosition();
        Serial.println(String(posX) + "," + String(posY));
      }
      // get and print isAvailable() -> Form '12'
      else if(inputstring.charAt(1)==zero_char+2){ grower.isAvailable(); }
    }
    
    else if(inputstring.charAt(0)==zero_char+2){ // Enable/Disable functions --> '2'  
      // disable() -> Form '20'
      if(inputstring.charAt(1)==zero_char){ grower.disable(); }
      // enable() -> Form '21'
      else if(inputstring.charAt(1)==zero_char+1){ grower.enable(); }
    }
  }
  input_string_complete = false;
}

void setup() {
  Serial.begin(9600);
  Serial.println(F("Starting test"));
  grower.begin(LOW); // Not send to home
}

void loop() {
  grower.run();
}
