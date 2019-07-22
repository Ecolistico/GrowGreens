// solutionMaker.cpp
//
// Copyright (C) 2019 Grow

#include "solutionMaker.h"

/***   solutionMaker   ***/
// Statics variables definitions

solutionMaker::solutionMaker(
  uint8_t dirS1,
  uint8_t stepS1,
  uint8_t dirS2,
  uint8_t stepS2,
  uint8_t dirS3,
  uint8_t stepS3,
  uint8_t dirS4,
  uint8_t stepS4,
  uint8_t enable1,
  uint8_t enable2,
  uint8_t enable3,
  uint8_t enable4,
  uint8_t motor1A,
  uint8_t motor1B,
  uint8_t motor2A,
  uint8_t motor2B,
  uint8_t en1,
  uint8_t en2,
  uint8_t led1,
  uint8_t led2,
  uint8_t led3,
  uint8_t led4,
  uint8_t led5,
  uint8_t led6,
  uint8_t led7,
  uint8_t led8,
  uint8_t tempSens,
  uint8_t lcdButton
  ){
    // Define motors pins
    __DirS[0] = dirS1;
    __StepS[0] = stepS1;
    __DirS[1] = dirS2;
    __StepS[1] = stepS2;
    __DirS[2] = dirS3;
    __StepS[2] = stepS3;
    __DirS[3] = dirS4;
    __StepS[3] = stepS4;

    __En[0] = enable1;
    __En[1] = enable2;
    __En[2] = enable3;
    __En[3] = enable4;

    __MicroSteps = 0;
    __StepPerRev = 0;
    __PumpVelocity = 0;

    // Define the motors
    stepperS[0] = new AccelStepper(1, __StepS[0], __DirS[0]);
    stepperS[1] = new AccelStepper(1, __StepS[1], __DirS[1]);
    stepperS[2] = new AccelStepper(1, __StepS[2], __DirS[2]);
    stepperS[3] = new AccelStepper(1, __StepS[3], __DirS[3]);

    // Define pumps pins
    __Motor[0] = motor1A;
    __Motor[1] = motor1B;
    __Motor[2] = motor2A;
    __Motor[3] = motor2B;

    __En[4] = en1;
    __En[5] = en2;

    // Default parameters
    for(int i=0; i<MAX_SOLUTIONS_NUMBER+MAX_PUMPS_NUMBER;i++){
      __IsEnable[i] = false;
      __Available[i] = true;
      __Calibration[i] = 1;
    }
    __Work = false;

    // Define Led´s pin
    __Led[0] = led1;
    __Led[1] = led2;
    __Led[2] = led3;
    __Led[3] = led4;
    __Led[4] = led5;
    __Led[5] = led6;
    __Led[6] = led7;
    __Led[7] = led8;

    // Atlas Scientific Sensors
    __pH = 0;
    __eC = 0;
    __ExportEzo = false;
    __ImportEzo = false;
    __SleepEzo = false;

    phMeter = new EZO(EZO_PH);
    ecMeter = new EZO(EZO_EC);

    // Define temperature tempSensor
    ourWire = new OneWire(tempSens);
    //tempSensor = new DallasTemperature(&ourWire);
    tempSensor = new DallasTemperature(ourWire);
    __Temp = 0;

    // LCD screen object
    __LCDButton = lcdButton;
    lcd = new LiquidCrystal_I2C(LCD_I2C_DIR, LCD_COLUMNS, LCD_ROWS);
    __StatusLCD = false;
    __LCDLightOn = false;

    // Filters
    __Filter = 0 ;
    __Alpha = 0 ; __Noise = 0 ; __Err = 0 ;
  }

void solutionMaker::begin(
  uint8_t steps_per_rev = MOTOR_STEP_PER_REV,
  uint8_t microStep = DEFAULT_MICROSTEP,
  uint8_t pump_velocity = PUMP_VELOCITY,
  bool invertPump = false
){    Serial.println(F("Solution Maker: Starting..."));

      // Init LCD screen
      lcd->init();

      // Turn on back light on LCD.
      lcd->backlight();
      __StatusLCD = true;

      // Show message in Screen.
      printLCD("Preparando");
      for(int i=0; i<MAX_SOLUTIONS_NUMBER+MAX_PUMPS_NUMBER+2;i++){
          pinMode(__Led[i], OUTPUT);
          digitalWrite(__Led[i], HIGH);
      }
      // Animate message
      unsigned long prepare = millis();
      while(millis()-prepare<3000){
          lcd->print(".");
          delay(250);
      }

      pinMode(__LCDButton, INPUT_PULLUP); // Set button as input pullup

      __StepPerRev = steps_per_rev;
      __MicroSteps = microStep;

      __PumpVelocity = pump_velocity;
      __InvertPump = invertPump;

      // Initial Configuration
      for(int i=0; i<MAX_SOLUTIONS_NUMBER; i++){
        stepperS[i]->setMaxSpeed(MOTOR_SPEED);
        stepperS[i]->setSpeed(0);
        stepperS[i]->setAcceleration(MOTOR_ACCEL);
        pinMode(__StepS[i], OUTPUT);
        pinMode(__DirS[i], OUTPUT);
      }
      for(int i=0; i<MAX_PUMPS_NUMBER*2; i++){
        pinMode(__Motor[i], OUTPUT);
      }
      for(int i=0; i<MAX_SOLUTIONS_NUMBER+MAX_PUMPS_NUMBER; i++){
        pinMode(__En[i], OUTPUT);
      }

      // Disable motors/pumps
      digitalWrite(__En[0], HIGH);
      digitalWrite(__En[1], HIGH);
      digitalWrite(__En[2], HIGH);
      digitalWrite(__En[3], HIGH);
      analogWrite(__En[4], LOW);
      analogWrite(__En[5], LOW);

      // Init Atlas Scientific Sensors
      phMeter->init();
      ecMeter->init();

      // Init temperature tempSensor
      tempSensor->begin();
      tempSensor->requestTemperatures();
      __Temp = tempSensor->getTempCByIndex(0);
      defaultFilter(); // Set by default exponential filter with alpha=0.2

      // Show message in Screen.
      printLCD("Preparado");

      // Turn off led´s
      for(int i=0; i<MAX_SOLUTIONS_NUMBER+MAX_PUMPS_NUMBER+2;i++){
        if(i!=6){digitalWrite(__Led[i], LOW); } // Except ready led
      }
      delay(3000);

      // Request phMeter and ecMeter readings
      EZOReadRequest(__Temp);

      // Erase message in Screen and turn off.
      lcd->clear();
      printLCD("T="+String(__Temp)+"C", String(__pH)+","+String(__eC));
      __LCDTemp = __Temp;
      __StatusLCD = false;
      __LCDLightOn = true;

      __ReadTime = millis();
      __LCDTime = millis();
      Serial.println(F("Solution Maker: Started Correctly"));
   }

bool solutionMaker::isWorking()
  { for(int i=0; i<MAX_SOLUTIONS_NUMBER+MAX_PUMPS_NUMBER; i++){
      if(!isAvailable(i)){return true;}
    }
    return false;
  }

void solutionMaker::enable(uint8_t actuator)
  { if(actuator<MAX_SOLUTIONS_NUMBER+MAX_PUMPS_NUMBER){ // If actuator in range
      if(!__IsEnable[actuator]){ // If disable
        if(actuator<MAX_SOLUTIONS_NUMBER){
          digitalWrite(__En[actuator], LOW);
        }
        else{
          analogWrite(__En[actuator], __PumpVelocity);
        }
        __IsEnable[actuator] = true;
        printAction("Enable", actuator);
        return;
      }
      printAction("Already enable", actuator);
      return;
    }
    printAction("Actuator does not exist", MAX_SOLUTIONS_NUMBER+MAX_PUMPS_NUMBER);
    return;
  }

void solutionMaker::disable(uint8_t actuator)
  { if(actuator<MAX_SOLUTIONS_NUMBER+MAX_PUMPS_NUMBER){ // If actuator in range
      if(__IsEnable[actuator]){ // If enable
        if(actuator<MAX_SOLUTIONS_NUMBER){
          digitalWrite(__En[actuator], HIGH);
        }
        else{
          analogWrite(__En[actuator], LOW);
        }
        __IsEnable[actuator] = false;
        printAction("Disable", actuator);
        return;
      }
      printAction("Already disable", actuator);
      return;
    }
    printAction("Actuator does not exist", MAX_SOLUTIONS_NUMBER+MAX_PUMPS_NUMBER);
    return;
  }

void solutionMaker::turnOnPump(unsigned long time1, uint8_t pump)
  { if(pump<MAX_PUMPS_NUMBER){
      // If disable then enable
      if(!isEnable(pump+MAX_SOLUTIONS_NUMBER)){enable(pump+MAX_SOLUTIONS_NUMBER);}

      if(__InvertPump){
        digitalWrite(__Motor[pump*2], HIGH);
        digitalWrite(__Motor[pump*2+1], LOW);
      }
      else{
        digitalWrite(__Motor[pump*2], LOW);
        digitalWrite(__Motor[pump*2+1], HIGH);
      }
      digitalWrite(__Led[pump+MAX_SOLUTIONS_NUMBER], HIGH);
      __Available[pump+MAX_SOLUTIONS_NUMBER] = false;
      __PumpOnTime[pump] = time1;
      __PumpTime[pump] = millis();
      printAction("Turn On", MAX_SOLUTIONS_NUMBER+pump);
      return;
    }
    printAction("Pump does not exist", MAX_SOLUTIONS_NUMBER+MAX_PUMPS_NUMBER);
    return;
  }

void solutionMaker::turnOffPump(uint8_t pump)
  { if(pump<MAX_PUMPS_NUMBER){
      digitalWrite(__Motor[pump*2], LOW);
      digitalWrite(__Motor[pump*2+1], LOW);
      disable(MAX_SOLUTIONS_NUMBER+pump);
      digitalWrite(__Led[pump+MAX_SOLUTIONS_NUMBER], LOW);
      __Available[pump+MAX_SOLUTIONS_NUMBER] = true;
      printAction("Turn Off", MAX_SOLUTIONS_NUMBER+pump);
      return;
    }
    printAction("Pump does not exist", MAX_SOLUTIONS_NUMBER+MAX_PUMPS_NUMBER);
    return;
  }

unsigned long solutionMaker::TimeToML(float seconds, uint8_t pump)
  { if(seconds>0 && pump<MAX_PUMPS_NUMBER){
      return  seconds*(float(__Calibration[MAX_SOLUTIONS_NUMBER+pump])/100);
    }
    return 0;
  }

unsigned long solutionMaker::MLToTime(float mililiters, uint8_t pump)
  { if(mililiters>0 && pump<MAX_PUMPS_NUMBER){
      // __Calibration/100 = ml/s
      return mililiters/(float(__Calibration[MAX_SOLUTIONS_NUMBER+pump])/100);
    }
    return 0;
  }

long solutionMaker::RevToGrams(long rev, uint8_t st)
  { if(rev>0 && st<MAX_SOLUTIONS_NUMBER){
      return (rev*__Calibration[st]);
    }
    return -1;
  }

long solutionMaker::GramsToRev(long grams, uint8_t st)
  { if(grams>0 && st<MAX_SOLUTIONS_NUMBER){
      float resp = float(grams)/__Calibration[st];
      uint16_t intResp = int(resp);
      if(resp-intResp>0.5){return intResp+1;}
      else{return intResp;}
    }
    return -1;
  }

long solutionMaker::RevToSteps(long rev)
  { return rev*__MicroSteps*__StepPerRev;}

long solutionMaker::StepsToRev(long st)
  { return st/(__MicroSteps*__StepPerRev); }

void solutionMaker::moveStepper(long steps, uint8_t st)
  { // If disable then enable
    if(!isEnable(st)){enable(st);}
    resetPosition(st);
    stepperS[st]->move(steps);
    digitalWrite(__Led[st], HIGH);
    __Available[st] = false;
  }

void solutionMaker::resetPosition(uint8_t st)
  { if(st<MAX_SOLUTIONS_NUMBER){
      stepperS[st]->setCurrentPosition(0);
      printAction("Reset Position", st);
      return;
    }
    printAction("Cannot reset position. Motor does not exist", MAX_SOLUTIONS_NUMBER+MAX_PUMPS_NUMBER);
    return;
  }

void solutionMaker::printAction(String act, uint8_t actuator)
  {   Serial.print(F("Solution Maker ("));
      switch(actuator){
        case 0:
          Serial.print(F("Motor 1"));
          break;
        case 1:
          Serial.print(F("Motor 2"));
          break;
        case 2:
          Serial.print(F("Motor 3"));
          break;
        case 3:
          Serial.print(F("Motor 4"));
          break;
        case 4:
          Serial.print(F("Pump 1"));
          break;
        case 5:
          Serial.print(F("Pump 2"));
          break;
        default:
          Serial.print(F("Actuator does not match"));
          break;
      }
      Serial.print(F("): "));
      Serial.println(act);
  }

void solutionMaker::printEZOAction(String act, uint8_t sensorType)
  {   Serial.print(F("Solution Maker (EZO "));
      switch(sensorType){
        case EZO_PH:
          Serial.print(F("PH"));
          break;
        case EZO_EC:
          Serial.print(F("EC"));
          break;
        default:
          Serial.print(F("Unkwown"));
          break;
      }
      Serial.print(F(" Sensor): "));
      Serial.println(act);
  }

void solutionMaker::printLCD(String main, String subAction = "")
  { lcd->clear();
    if(!__StatusLCD){
      __StatusLCD = true;
      lcd->backlight();
    }
    lcd->print(main);
    lcd->setCursor(0, 1);
    lcd->print(subAction);
  }

void solutionMaker::checkButtonLCD()
  { if(!digitalRead(__LCDButton) && !__LCDLightOn){
      __LCDLightOn = true;
      // Erase message in Screen and rewrite.
      printLCD("T="+String(__Temp)+"C", String(__pH)+","+String(__eC));
      __StatusLCD = false;
      __LCDTemp = __Temp;
      __LCDTime = millis();
      Serial.println(F("Solution Maker (LCD Screen): Button Pressed"));
    }
  }

float solutionMaker::filter(float val, float preVal)
  {  switch(__Filter){
      case 0:
        return val;
        break;
      case 1:
        return exponential_filter(__Alpha, val, preVal);
        break;
      case 2:
        return kalman_filter(val, preVal);
        break;
    }
  }

float solutionMaker::exponential_filter(float alpha, float t, float t_1)
  { if(isnan(alpha) || isnan(t) || isnan(t_1)){
      return t;
    }
    else{
      return (t*alpha+(1-alpha)*t_1);
    }
  }

float solutionMaker::kalman_filter(float t, float t_1)
  { if( __Err==0 || isnan(t) || isnan(t_1) ){
      if(__Err==0){
        __Err = 0.1;
      }
      return t;
    }
    else{
      float kalman_gain = __Err/(__Err+__Noise);
      __Err = (1-kalman_gain)*__Err;
      return (t_1+kalman_gain*(t-t_1));
    }
  }

void solutionMaker::readTemp()
  { tempSensor->requestTemperatures();
    float temp= tempSensor->getTempCByIndex(0);
    if(temp>0) __Temp = filter(temp, __Temp);
  }

void solutionMaker::EZOReadRequest(float temp, bool sleep = false)
  { if(EZOisEnable(EZO_PH) && EZOisEnable(EZO_EC)){
      if(!__SleepEzo && !__ImportEzo){ // If sensors are not sleeping and are not importing cal.
        phMeter->readWithTempCompensation(temp);
        ecMeter->readWithTempCompensation(temp);
      }
      else if(sleep){ // If we want to force to awake the sensors
        __SleepEzo = false;
        phMeter->readWithTempCompensation(temp);
        ecMeter->readWithTempCompensation(temp);
      }
    }
    else{Serial.println(F("Solution Maker (EZO Sensors): They are not available"));}
  }

void solutionMaker::readRequest()
  { // Take temperature readings every 5 seconds
    if(millis()-__ReadTime>5000){
      __ReadTime = millis();
      __pH = phMeter->getValue(); // Save last read from phmeter
      __eC = ecMeter->getValue(); // Save last read from ecMeter
      readTemp(); // Read temp
      EZOReadRequest(__Temp); // Request new read
    }
  }

void solutionMaker::EZOexportFinished()
  { if(EZOisEnable(EZO_PH) && EZOisEnable(EZO_PH) && __ExportEzo) __ExportEzo = false;  }

bool solutionMaker::dispense(long some_grams, uint8_t st)
  { if(st<MAX_SOLUTIONS_NUMBER){ // Check that stepper exist
      if(some_grams>0){ // Check that parameter is correct
        long rev = GramsToRev(some_grams, st);
        if(rev>0){
          long steps = RevToSteps(rev);
          if(steps>0){
            moveStepper(steps, st);
            printAction("Dispensing " + String(some_grams) + " g", st);
            return true;
          }
          printAction("RevToSteps equation problem", st);
          return false;
        }
        printAction("GramsToRev equation problem", st);
        return false;
      }
      printAction("Grams parameter incorrect", st);
      return false;
    }
    printAction("Cannot dispense. Motor does not exist", MAX_SOLUTIONS_NUMBER+MAX_PUMPS_NUMBER);
    return false;
  }

void solutionMaker::stepperCalibration(long rev, uint8_t st)
  { if(st<MAX_SOLUTIONS_NUMBER){
      long steps = RevToSteps(abs(rev));
      moveStepper(steps, st);
      printAction("Running " + String(rev) + "revolutions for calibration purpose", st);
      return;
    }
    printAction("Cannot run calibration. Motor does not exist", MAX_SOLUTIONS_NUMBER+MAX_PUMPS_NUMBER);
  }

bool solutionMaker::dispenseAcid(float some_ml, uint8_t pump)
  { if(pump<MAX_PUMPS_NUMBER){ // Check that pumps exists
      if(some_ml>0){ // Check that parameter is correct
        unsigned long time1 = MLToTime(some_ml, pump)*1000; // *1000 to convert s to ms
        if(time1>0){ // Check equation result
          turnOnPump(time1, pump);
          printAction("Dispensing " + String(some_ml) + " ml", pump+MAX_SOLUTIONS_NUMBER);
          return true;
        }
        printAction("MLToTime equation problem", pump+MAX_SOLUTIONS_NUMBER);
        return false;
      }
      printAction("Mililiters parameter incorrect", pump+MAX_SOLUTIONS_NUMBER);
      return false;
    }
    printAction("Cannot dispense. Pump does not exist", MAX_SOLUTIONS_NUMBER+MAX_PUMPS_NUMBER);
    return false;
  }

void solutionMaker::pumpCalibration(float time1, uint8_t pump)
  { if(pump<MAX_PUMPS_NUMBER){ // Check that the pump exists
      turnOnPump(abs(time1*1000), pump);
    }
    printAction("Cannot run calibration. Pump does not exist", MAX_SOLUTIONS_NUMBER+MAX_PUMPS_NUMBER);
  }

void solutionMaker::setCalibrationParameter(uint8_t param, uint8_t actuator)
  { if(actuator<MAX_SOLUTIONS_NUMBER+MAX_PUMPS_NUMBER){
      __Calibration[actuator] = param;
      printAction(String(param) + " is the new calibration param", actuator);
      return;
    }
      printAction("Cannot set calibration Param. Actuator does not exist", MAX_SOLUTIONS_NUMBER+MAX_PUMPS_NUMBER);
  }

bool solutionMaker::isEnable(uint8_t actuator)
  { if(actuator<MAX_SOLUTIONS_NUMBER+MAX_PUMPS_NUMBER){return __IsEnable[actuator];}
    return false;
  }

bool solutionMaker::isAvailable(uint8_t actuator)
  { if(actuator<MAX_SOLUTIONS_NUMBER+MAX_PUMPS_NUMBER){return __Available[actuator];}
    return false;
  }

long solutionMaker::getGrams(uint8_t st)
  { if(st<MAX_SOLUTIONS_NUMBER){
      long steps = stepperS[st]->currentPosition();
      long rev = StepsToRev(steps);
      long gr = RevToGrams(rev, st);
      return gr;
    }
    else{return -1;}
  }

void solutionMaker::stop(uint8_t actuator)
  { if(actuator<MAX_SOLUTIONS_NUMBER+MAX_PUMPS_NUMBER){
      if(actuator<MAX_SOLUTIONS_NUMBER){
        stepperS[actuator]->setAcceleration(MOTOR_ACCEL*10);
        if(stepperS[actuator]->speed()<0){stepperS[actuator]->setSpeed(1);}
        else{stepperS[actuator]->setSpeed(-1);}
        stepperS[actuator]->stop();
      }
      else{turnOffPump(actuator-MAX_SOLUTIONS_NUMBER);}
      printAction("Stopped", actuator);
      return;
    }
    printAction("Cannot stop. Actuator does not exist", MAX_SOLUTIONS_NUMBER+MAX_PUMPS_NUMBER);
    return;
  }

void solutionMaker::notFilter()
  { __Filter = 0 ; __Alpha = 0; __Noise = 0; __Err = 0; printFilter();}

void solutionMaker::defaultFilter()
  { __Filter = 1 ; __Alpha = 0.2 ; printFilter();}

bool solutionMaker::setExponentialFilter(float alpha = 0.5)
  { if(alpha>0 && alpha<1){
      __Filter = 1; __Alpha = alpha;
      printFilter();
      return true;
    }
    else{
      Serial.println(F("Solution Maker: Parameter alpha wrong"));
      return false;
    }
  }

bool solutionMaker::setKalmanFilter(float noise)
  { if(noise!=0){
      __Filter = 2;
      __Noise = noise; // Enviromental Noise
      __Err = 1; // Initial Error
      printFilter();
      return true;
    }
    else{
      Serial.print(F("Solution Maker: Parameter noise wrong"));
      return false;
    }
  }

void solutionMaker::printFilter()
  { Serial.print(F("Solution Maker (Temperature readings): "));
    switch(__Filter){
      case 0:
      Serial.println(F("It is not using any filter..."));
      break;
    case 1:
      Serial.print(F("It is using an exponencial filter with Alpha = "));
      Serial.println(__Alpha);
      break;
    case 2:
      Serial.print(F("It is using a Kalman filter with Noise = "));
      Serial.print(__Noise); Serial.print(F(" and Actual Error = "));
      Serial.println(__Err);
      break;
    default:
      Serial.println(F("Filters parameters unknowns"));
    }
  }

bool solutionMaker::EZOisEnable(byte sensorType)
  { if(sensorType == EZO_PH){ return phMeter->isEnable(); }
    else if(sensorType == EZO_EC){ return ecMeter->isEnable(); }
    return false;
  }

void solutionMaker::EZOcalibration(byte sensorType, byte act, float value)
  { if(sensorType == EZO_PH && EZOisEnable(EZO_PH)){
      phMeter->calibration(act, value);
    }
    else if(sensorType == EZO_EC && EZOisEnable(EZO_EC)){
      ecMeter->calibration(act, value);
    }
    else{printEZOAction("Sensor does not match a type or is in another request", sensorType);}
  }

void solutionMaker::EZOexportCal(byte sensorType)
  { if(!__ExportEzo){
      if(sensorType == EZO_PH && EZOisEnable(EZO_PH)){
        __ExportEzo = true;
        phMeter->exportCal();
      }
      else if(sensorType == EZO_EC && EZOisEnable(EZO_EC)){
        __ExportEzo = true;
        ecMeter->exportCal();
      }
      else{printEZOAction("Sensor does not match a type or is in another request", sensorType);}
    }
    else{printEZOAction("Another export is running", sensorType);}
  }

void solutionMaker::EZOimportCalibration(byte sensorType, String parameters)
  { if(sensorType == EZO_PH && EZOisEnable(EZO_PH)){
      phMeter->importCalibration(parameters);
    }
    else if(sensorType == EZO_EC && EZOisEnable(EZO_EC)){
      ecMeter->importCalibration(parameters);
    }
    else{printEZOAction("Sensor does not match a type or is in another request", sensorType);}
  }

void solutionMaker::EZOimport(bool start)
  { if(start!=__ImportEzo){
      __ImportEzo = start;
      if(__ImportEzo) Serial.println(F("Solution Maker (EZO Sensors): Starting Calibration Import"));
      else Serial.println(F("Solution Maker (EZO Sensors): Stopping Calibration Import"));
    }
    else{
      if(start) Serial.println(F("Solution Maker (EZO Sensors): Calibration Import already started"));
      else Serial.println(F("Solution Maker (EZO Sensors): Calibration Import already stopped"));
    }
  }

void solutionMaker::EZOsleep()
  { if(!__SleepEzo){
      if(EZOisEnable(EZO_PH) && EZOisEnable(EZO_EC)){
        __SleepEzo = true;
        phMeter->sleep();
        ecMeter->sleep();
      }
      else{Serial.println(F("Solution Maker (EZO Sensors): They are in another request"));}
    }
    else{Serial.println(F("Solution Maker (EZO Sensors): They are already in sleep mode"));}
  }

void solutionMaker::EZOawake()
  { if(__SleepEzo){
      EZOReadRequest(__Temp, true); // Take a read to awake the sensors
    }
    else{Serial.println(F("Solution Maker (EZO Sensors): They are already awake"));}
  }

bool solutionMaker::EZOisSleep()
  { return __SleepEzo; }

void solutionMaker::eventLCD()
  { String Main = "";
    String subAct = "";
    bool doubleAct = false;

    for(int i=0; i<MAX_SOLUTIONS_NUMBER+MAX_PUMPS_NUMBER; i++){
      if(!isAvailable(i)){ // If something is working
        // Print main message
        //if(Main.length()<=1){Main += "Dispensando ";}
        if(i<MAX_SOLUTIONS_NUMBER && Main.length()<=1){
          Main += "Sol";
        }
        else if(i>=MAX_SOLUTIONS_NUMBER && i<MAX_SOLUTIONS_NUMBER+MAX_PUMPS_NUMBER){
          if(Main.length()<2){
            Main += "Ac";
          }
          else if(Main.length()>=3 && Main.length()<6){Main += "/Ac";}
        }

        // Print subAct message
        if(subAct.length()<1 && i<MAX_SOLUTIONS_NUMBER){subAct += String(i+1);}
        else if(subAct.length()>=1 && i<MAX_SOLUTIONS_NUMBER){subAct += "," + String(i+1);}
        else if(i>=MAX_SOLUTIONS_NUMBER && i<MAX_SOLUTIONS_NUMBER+MAX_PUMPS_NUMBER){
          for(int j = 0; j<Main.length(); j++){
            if(Main[j]=='/' && !doubleAct){
              subAct += "/";
              doubleAct = true;
            }
          }
          if(i-MAX_SOLUTIONS_NUMBER==0){subAct += String(i-MAX_SOLUTIONS_NUMBER+1);}
          else{
            if(doubleAct){
              String subString = "";
              for(int j = 0; j<subAct.length(); j++){
                if(subAct[j] == '/'){
                  subString = subAct.substring(j);
                }
              }
              if(subString.length()<=1){subAct += String(i-MAX_SOLUTIONS_NUMBER+1);}
              else{subAct += "," + String(i-MAX_SOLUTIONS_NUMBER+1);}
            }
            else{
              if(subAct.length()<1){subAct += String(i-MAX_SOLUTIONS_NUMBER+1);}
              else if(subAct.length()>=1){subAct += "," + String(i-MAX_SOLUTIONS_NUMBER+1);}
            }
          }
        }
      }
    }
    printLCD(Main, subAct);
  }

void solutionMaker::run()
  { readRequest(); // Request read of temperature, ph and ec
    phMeter->run(); // Handle phMeter functions
    ecMeter->run(); // Handle ecMeter functions
    EZOexportFinished(); // Check when EZO sensors finished export cal.
    checkButtonLCD(); // Check if LCD button is pressed

    if(!__Work && isWorking()){
      __Work = true;
      digitalWrite(__Led[7], HIGH);
      digitalWrite(__Led[6], LOW);
    }
    else if(__Work && !isWorking()){
      __Work = false;
      digitalWrite(__Led[7], LOW);
      digitalWrite(__Led[6], HIGH);
    }

    if(!__Work && __StatusLCD){
      __LCDLightOn = true;
      // Erase message in Screen and rewrite.
      printLCD("T="+String(__Temp)+"C", String(__pH)+","+String(__eC));
      __StatusLCD = false;
      __LCDTemp = __Temp;
      __LCDTime = millis();
    }
    else if(!__Work && !__StatusLCD && abs(__LCDTemp-__Temp)>0.5){
      __LCDLightOn = true;
      // Erase message in Screen and rewrite.
      printLCD("T="+String(__Temp)+"C", String(__pH)+","+String(__eC));
      __StatusLCD = false;
      __LCDTemp = __Temp;
      __LCDTime = millis();
    }
    if(!__Work && millis()-__LCDTime>10000 && __LCDLightOn){
      __LCDLightOn = false;
      lcd->noBacklight();
    }

    // For all the motors
    for(int i=0; i<MAX_SOLUTIONS_NUMBER; i++){
      stepperS[i]->run();
      // If it is not running and is not Available
      if(!stepperS[i]->isRunning() && !isAvailable(i)){
        __Available[i] = true;
        digitalWrite(__Led[i], LOW);
        disable(i);
        eventLCD();
        printAction(String(getGrams(i)) + " grams were dispensed", i);
      }
    }

    // For all the pumps
    for(int i=0; i<MAX_PUMPS_NUMBER; i++){
      if(!isAvailable(MAX_SOLUTIONS_NUMBER+i) && millis()-__PumpTime[i]>__PumpOnTime[i]){
        turnOffPump(i);
        eventLCD();
      }
    }
  }

void solutionMaker::prepareSolution(float liters, byte sol, float ph, float ec)
  {
    
  }
