// EZO.cpp

#include "EZO.h"

/***   EZO   ***/
// Statics variables definitions

EZO::EZO(uint8_t type)
  { if(type == EZO_PH || type == EZO_EC) __Type = type;
    else __Type = EZO_PH; // pHmeter by default
    if(__Type == EZO_PH){
      __I2CAddress = PH_I2C_ADDRESS;
    }
    else if(__Type == EZO_EC) {
      __I2CAddress = EC_I2C_ADDRESS;
    }

    // Default parameters
    __Value = 0;
    __Request = false;
    __Read = false;

    for(int i=0; i<LARGEST_STRING; i++){
      __SendData[i] = 0;
      __RecieveData[i] = 0;
    }
    for(int i=0; i<MAX_PARAM; i++){__ParamResponse[i] = "";}

    __CalibrationCount = 0;
    __CalibrationLines = 0;
    __CalibrationBytes = 0;
  }

void EZO::printAction(String act)
  { if(__Type == EZO_PH){
      Serial.print(F("PH")); 
    }
    else if(__Type == EZO_EC) {
      Serial.print(F("EC")); 
    }
    Serial.print(F(" EZO sensor: "));
    Serial.println(act);
  }

void EZO::sendCmd(String command)
  { command.toCharArray(__SendData, LARGEST_STRING);//command.length());
    __SendData[command.length()+1] = 0;
    Wire.beginTransmission(__I2CAddress); // call the circuit by its I2C address
    Wire.write(__SendData); // transmit the command
    Wire.endTransmission(); // end the I2C data transmission.
  }

void EZO::requestResponse()
  {  // Address, numberOfBytes, stop(1/0)
    Wire.requestFrom((uint8_t)__I2CAddress, (uint8_t)LARGEST_STRING, (uint8_t)1); // Call the circuit and request

    uint8_t code = 0;
    code = Wire.read();      // The first byte is the response code, we read this separately.
    switch (code) {
      case 1:                        // Succesful
        //printAction("Request Success"); // Just for debbugging
        __Request = false;
        break;
      case 2:                        // Failed
        printAction(F("Request Failed"));
        __Request = false;
        break;
      case 254:                      // Not ready
        printAction(F("Request Pending"));
        __ActualTime = millis();
        break;
      case 255:                      // There is not request
        printAction(F("No Request"));
        __ActualTime = millis();
        break;
      default:
        printAction(F("Wire Read failed"));
        break;
    }
  }

void EZO::fillRecieveDataArray()
  { // Erase all the array
    for(int i = 0; i<LARGEST_STRING; i++){__RecieveData[i]=0;}
    // Fill it again
    byte in_char;
    int i = 0;
    while (Wire.available()) {         // While bytes available
      in_char = Wire.read();
      __RecieveData[i] = in_char;
      i += 1;
      if (in_char == 0) {              // If we see null command.
        Wire.endTransmission();        // End I2C data transmission.
        break;
      }
    }
  }

void EZO::getParamResponse()
  { // Erase all the parameters
    for(int j=0; j<MAX_PARAM; j++){__ParamResponse[j] = "";}
    int k = 0;
    int l = 0;

    // Split the parameters
    for (int j = 1; j<LARGEST_STRING; j++){
      if(__RecieveData[j] == ','){ // Looking for ','
        for(int n=l; n<j; n++){
          if(__RecieveData[n] != '?'){
            __ParamResponse[k] += __RecieveData[n];
          }
        }
        k++;
        l = j+1;
      }
      else if(__RecieveData[j] == 0){ // Looking for '0' (null/terminator)
        for(int n=l; n<j; n++){
          if(__RecieveData[n] != '?'){
            __ParamResponse[k] += __RecieveData[n];
          }
        }
        break;
      }
    }
  }

void EZO::decodeResults()
  { if(__ParamResponse[0] == F("EXPORT")){
      if(__ParamResponse[2].length()>=1 && __CalibrationCount == 0){
        __CalibrationLines = __ParamResponse[1].toInt();
        __CalibrationBytes = __ParamResponse[2].toInt();
        __CalibrationCount++;
        String act = "There are " + String(__CalibrationLines) + " lines and " + String(__CalibrationBytes) + " bytes to export";
        printAction(act);
        exportCalibration(false);
      }
      else{ printAction(F("Something wrong")); }
    }

    else if(__ParamResponse[0] == F("STATUS")){ // Device status
      String act = "Status - Restart reason = ";
      switch (__ParamResponse[1][0]){
        case 'P':
          act += "Powered Off,";
          break;
        case 'S':
          act += "Software Reset,";
          break;
        case 'B':
          act += "Brown Out,";
          break;
        case 'W':
          act += "Watchdog,";
          break;
        case 'U':
          act += "Unkwown,";
          break;
        default:
          act += "Unkwown,";
          break;
      }
      act += " Voltage = "+__ParamResponse[2]+"V";
      printAction(act);
    }

    else if(__ParamResponse[0] == F("I")){ // Device information
      String act = "This is a " + __ParamResponse[1] + " sensor, Firmware= "+__ParamResponse[2];
      printAction(act);
    }

    else if(__ParamResponse[0] == F("L")){ // Led Status
      String act = "Led Status = ";
      if(__ParamResponse[1][0] == '1') { act+= "On"; }
      else if(__ParamResponse[1][0] == '0') { act += "Off"; }
      printAction(act);
    }

    else if(__ParamResponse[0] == F("PLOCK")){ // Plock Status
      String act = "Plock Status = ";
      if(__ParamResponse[1][0] == '1') { act+= "disable"; }
      else if(__ParamResponse[1][0] == '0') { act += "enable"; }
      printAction(act);
    }

    else if(__ParamResponse[0] == F("T")){ // Temperature saved for compensation
      String act = "Compensation Temperature = ";
      act += __ParamResponse[1];
      printAction(act);
    }

    else if(__ParamResponse[0] == F("O")){ // Parameter Status
      String act = "Exporting ";
      int par = 0;
      int count = 0;
      bool ec = false;
      bool tds = false;
      bool s = false;
      bool sg = false;

      for(int j=1; j<MAX_PARAM; j++){
        if(__ParamResponse[j] == F("EC")){ec = true; par++;}
        else if(__ParamResponse[j] == F("TDS")){tds = true; par++;}
        else if(__ParamResponse[j] == F("S")){s = true; par++;}
        else if(__ParamResponse[j] == F("SG")){sg = true; par++;}
      }
      act += String(par);
      act += " parameters (";

      if(ec){
        act += "EC";
        count++;
        if(count != par) act += "/";
        else act += ").";
      }
      if(tds){
        act += "TDS";
        count++;
        if(count != par) act += "/";
        else act += ").";
      }
      if(s){
        act += "S";
        count++;
        if(count != par) act += "/";
        else act += ").";
      }
      if(sg){
        act += "SG";
        count++;
        if(count != par) act += "/";
        else act += ").";
      }
      printAction(act);
    }

    else if(__ParamResponse[0] == F("SLOPE")){ // Slope Saved
      String act = "The slope of the pHmeter is ";
      act += __ParamResponse[1];
      act += "% close from ideal acid and ";
      act += __ParamResponse[2];
      act += "% close from ideal base";
      printAction(act);
    }

    else if(__ParamResponse[0] == F("K")){ // Slope Saved
      if(__ParamResponse[1] != F("")) {
        String act = "Probe ";
        act += __ParamResponse[1];
        act += " is configured";
        printAction(act);
      }
    }

    else if(__Read){ // If we are waiting for a read lecture
      __Read = false;
      __Value = __ParamResponse[0].toFloat();
      String act = "";
      if(__Type==EZO_PH){act+= "PH read = ";}
      else if(__Type==EZO_EC){act+= "EC read = ";}
      act += __ParamResponse[0];
      //printAction(act); // Just for debbugging
    }

    else if(__CalibrationCount!=0){
      if(__ParamResponse[0] != F("*DONE")){
        printAction(__ParamResponse[0]);
        __CalibrationCount++;
        exportCalibration(false);
      }
      else {
        printAction(F("Export calibration finished"));
        __CalibrationCount = 0;
      }

    }

    // Just for debugging
    //else { Serial.println(__ParamResponse[0]); }
  }

void EZO::init()
  { Wire.begin();
    printAction(F("Started corrrectly"));
  }

bool EZO::isEnable()
  { return !__Request; }

float EZO::getValue()
  { return __Value; }

void EZO::calibration(byte act, float value)
  { if(!__Request){
      if(__Type == EZO_PH){
        if(value>=0 && value<=14){
          String cmd = "cal,";
          if(act<=4){
            if(act<=2){__Time = 900;}
            else{__Time = 300;}
            switch(act){
              case 0: // Low calibration
                cmd += "low,";
                cmd += String(value);
                printAction("Low Calibration," + String(value));
                break;
              case 1: // Mid calibration
                cmd += "mid,";
                cmd += String(value);
                printAction("Mid Calibration," + String(value));
                break;
              case 2: // High calibration
                cmd += "high,";
                cmd += String(value);
                printAction("High Calibration," + String(value));
                break;
              case 3: // Clear calibration params
                cmd += "clear";
                printAction(F("Clear Calibration"));
                break;
              case 4: // Ask if the device is calibrated
                cmd += "?";
                printAction(F("Ask Calibration"));
                break;
              default: // Do nothing if act is not recognized
                break;
            }
            sendCmd(cmd);
            __Request = true;
            __ActualTime = millis();
          }
          else{printAction(F("Parameter act incorrect. Action undefined"));}
        }
        else{printAction(F("Parameter value incorrect. It has to be between 0-14"));}
      }
      else if(__Type == EZO_EC){
        if(value>=0){
          String cmd = "cal,";
          if(act<=5){
            if(act<=3){__Time = 600;}
            else{__Time = 400;}
            switch(act){
              case 0: // Dry calibration
                cmd += "dry";
                printAction("Dry Calibration," + String(value));
                break;
              case 1: // Single point calibration
                cmd += String(int(value));
                printAction("Single Point Calibration," + String(value));
                break;
              case 2: // Low calibration
                cmd += "low,";
                cmd += String(value);
                printAction("low Calibration," + String(value));
                break;
              case 3: // High calibration params
                cmd += "high,";
                cmd += String(value);
                printAction("High Calibration," + String(value));
                break;
              case 4: // Clear calibration params
                cmd += "clear";
                printAction(F("Clear Calibration"));
                break;
              case 5: // Ask if the device is calibrated
                cmd += "?";
                printAction(F("Ask Calibration"));
                break;
              default: // Do nothing if act is not recognized
                break;
            }
            sendCmd(cmd);
            __Request = true;
            __ActualTime = millis();
          }
          else{printAction(F("Parameter act incorrect. Action undefined"));}
        }
        else{printAction(F("Parameter value incorrect. It has to be bigger than zero"));}
      }
      else{printAction(F("Sensor does not match a type"));}
    }
    else{printAction(F("Busy, waiting response for before command"));}
  }

void EZO::exportCalibration(bool act)
  { if(!__Request){
      if(__Type == EZO_PH) __Time = 300;
      else if(__Type == EZO_EC) __Time = 400;
      if(act){
        sendCmd(F("export,?"));
      }
      else{
        sendCmd(F("export"));
      }
      __Request = true;
      __ActualTime = millis();
    }
    else{printAction(F("Busy, waiting response for before command"));}
  }

void EZO::exportCal()
  { exportCalibration(true);
    printAction(F("Starting Calibration sequence"));
  }

void EZO::importCalibration(String parameters)
  { if(!__Request){
      if(__Type == EZO_PH) __Time = 300;
      else if(__Type == EZO_EC) __Time = 400;
      String cmd = "import, " + String(parameters);
      sendCmd(cmd);
      __Request = true;
      __ActualTime = millis();
      printAction(F("Importing calibration parameters"));
    }
    else{printAction(F("Busy, waiting response for before command"));}
  }

void EZO::reboot()
  { if(!__Request){
      sendCmd(F("factory"));
      printAction(F("Rebooting"));
    }
    else{printAction(F("Busy, waiting response for before command"));}
  }

void EZO::find()
  { if(!__Request){
      if(__Type == EZO_PH) __Time = 300;
      else if(__Type == EZO_EC) __Time = 400;
      sendCmd(F("find"));
      __Request = true;
      __ActualTime = millis();
      printAction(F("Looking for the sensor. Blinking white led"));
    }
    else{printAction(F("Busy, waiting response for before command"));}
  }

void EZO::information()
  { if(!__Request){
      if(__Type == EZO_PH) __Time = 300;
      else if(__Type == EZO_EC) __Time = 400;
      sendCmd(F("i"));
      __Request = true;
      __ActualTime = millis();
      printAction(F("Asking for sensor information"));
    }
    else{printAction(F("Busy, waiting response for before command"));}
  }

void EZO::changeI2Caddress(int newAddress)
  { if(!__Request){
      if(newAddress>7 && newAddress<125){
        String cmd = "I2C," + String(newAddress);
        sendCmd(cmd);
        __Request = true;
        __I2CAddress = newAddress;
        printAction("Changing I2C address to " + String(newAddress));
      }
      else{printAction(F("New address invalid. It has to be between 8-124"));}
    }
    else{printAction(F("Busy, waiting response for before command"));}
  }

void EZO::led(byte act)
  { if(!__Request){
      if(act<3){
        if(__Type == EZO_PH) __Time = 300;
        else if(__Type == EZO_EC) __Time = 400;
        switch(act){
          case 0: // Turn off led
            sendCmd(F("l,0"));
            printAction("Turn off LED" + String(act));
            break;
          case 1: // Turn on led
            sendCmd(F("l,1"));
            printAction("Turn on LED" + String(act));
            break;
          case 2: // Led status
            sendCmd(F("l,?"));
            printAction("Asking LED status" + String(act));
            break;
          default:
            break;
        }
        __Request = true;
        __ActualTime = millis();
      }
    }
    else{printAction(F("Busy, waiting response for before command"));}
  }

void EZO::plock(byte act)
  { if(!__Request){
      if(act<3){
        if(__Type == EZO_PH) __Time = 300;
        else if(__Type == EZO_EC) __Time = 400;
        switch(act){
          case 0: // Disable plock
            sendCmd(F("plock,0"));
            printAction("Enable Plock" + String(act));
            break;
          case 1: // Enable plock
            sendCmd(F("plock,1"));
            printAction("Disable Plock" + String(act));
            break;
          case 2: // Plock status
            sendCmd(F("plock,?"));
            printAction("Plock status" + String(act));
            break;
          default:
            break;
        }
        __Request = true;
        __ActualTime = millis();
      }
    }
    else{printAction(F("Busy, waiting response for before command"));}
  }

void EZO::read()
  { if(!__Request){
      if(__Type == EZO_PH){__Time = 900;}
      else if(__Type == EZO_EC){__Time = 600;}
      sendCmd(F("r"));
      // printAction("Asking for single reading"); // Just for debbugging
      __Request = true;
      __Read = true;
      __ActualTime = millis();
    }
    else{printAction(F("Busy, waiting response for before command"));}
  }

void EZO::readWithTempCompensation(float temp)
  { if(!__Request){
      if(temp>0){
        if(__Type == EZO_PH){__Time = 900;}
        else if(__Type == EZO_EC){__Time = 600;}
        String cmd = "rT," + String(temp);
        sendCmd(cmd);
        // printAction("Asking for single reading with temperature compensation"); // Just for debbugging
        __Request = true;
        __Read = true;
        __ActualTime = millis();
      }
      else{printAction(F("Temperature parameter has to be positive"));}
    }
    else{printAction(F("Busy, waiting response for before command"));}
  }

void EZO::sleep()
  { if(!__Request){
    sendCmd(F("sleep"));
    printAction(F("Sleep Mode"));
    }
    else{printAction(F("Busy, waiting response for before command"));}
  }

void EZO::infoStatus()
  { if(!__Request){
      if(__Type == EZO_PH) __Time = 300;
      else if(__Type == EZO_EC) __Time = 400;
      sendCmd(F("status"));
      printAction(F("Asking for sensor status"));
      __Request = true;
      __ActualTime = millis();
    }
    else{printAction(F("Busy, waiting response for before command"));}
  }

void EZO::tempCompensation(bool act, float temp = 0)
  { if(!__Request){
      if(act<2){
        if(temp>0){
          if(__Type == EZO_PH) __Time = 300;
          else if(__Type == EZO_EC) __Time = 400;
          String cmd;
          if(act){ // Set new temperature
            cmd = "t," + String(temp);
            sendCmd(cmd);
            printAction("New temperature for compensate " + String(temp));
          }
          else{ // Ask for temp saved
            sendCmd(F("t,?"));
            printAction(F("Asking for temperature saved for compensation"));
          }
          __Request = true;
          __ActualTime = millis();
        }
        else{printAction(F("Temperature parameter has to be positive"));}
      }
      else{printAction(F("Action type does not match"));}
    }
    else{printAction(F("Busy, waiting response for before command"));}
  }

void EZO::slope()
  { if(__Type == EZO_PH){
      if(!__Request){
        __Time = 300;
        sendCmd(F("slope,?"));
        printAction(F("Asking for slope value"));
        __Request = true;
        __ActualTime = millis();
      }
      else{printAction(F("Busy, waiting response for before command"));}
    }
    else{printAction(F("Sensor type does not have slope function"));}
  }

void EZO::setProbe(bool act, float probeType)
  { if(__Type == EZO_EC){
      if(!__Request){
        if(act){
          if(probeType == 0.1 || probeType == 1.0 || probeType == 10){
            __Time = 400;
            String cmd = "k,"+ String(probeType);
            sendCmd(cmd);
            printAction("Settting probe type to " + String(probeType));
            __Request = true;
            __ActualTime = millis();
          }
          else{printAction(F("Probe type does not match a type"));}
        }
        else{
          __Time = 400;
          sendCmd(F("k,?"));
          printAction(F("Asking what type of probe is setted "));
          __Request = true;
          __ActualTime = millis();
        }
      }
      else{printAction(F("Busy, waiting response for before command"));}
    }
    else{printAction(F("Sensor type does not have setProbe function"));}

  }

void EZO::enableParameters(bool en, byte parameter)
  { if(__Type == EZO_EC){
        if(parameter<=4){
          String cmd = "o,";
          String enable;
          if(en) enable = "1";
          else enable = "0";
          __Time = 400;

          switch(parameter){
            case 0: // Enable/Disable electrical condictivity
              cmd += "EC,";
              cmd += enable;
              if(en) printAction(F("Enable EC parameter"));
              else printAction(F("Disable EC parameter"));
              break;
            case 1: // Enable/Disable total disolved solids
              cmd += "TDS,";
              cmd += enable;
              if(en) printAction(F("Enable TDS parameter"));
              else printAction(F("Disable TDS parameter"));
              break;
            case 2: // Enable/Disable salinity
              cmd += "S,";
              cmd += enable;
              if(en) printAction(F("Enable S parameter"));
              else printAction(F("Disable S parameter"));
              break;
            case 3: // Enable/Disable specific gravity
              cmd += "SG,";
              cmd += enable;
              if(en) printAction(F("Enable SG parameter"));
              else printAction(F("Disable SG parameter"));
              break;
            case 4: // Parameter status
              cmd += "?";
              printAction(F("Asking parameter status"));
              break;
            default: // Else do nothing
              break;
          }
          sendCmd(cmd);
          __Request = true;
          __ActualTime = millis();
        }
        else{printAction(F("Parameter wrong. It has to be a number between 0-4"));}
      }
    else{printAction(F("Sensor type does not have enableParameters function"));}
  }

void EZO::run()
  { // If there is a request for the sensor wait and then get the response
    if(millis()-__ActualTime>__Time && __Request){
      requestResponse();
      fillRecieveDataArray();
      getParamResponse();
      decodeResults();
    }
  }
