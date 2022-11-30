// solutionMakerPeristaltics.cpp
//
// Copyright (C) 2019 Grow

#include "sMakerPeristaltics.h"
#include <Wire.h>

/***   solutionMaker   ***/
// Statics variables definitions

solutionMaker::solutionMaker(){
  Wire.begin();
  PREPARE_CYCLE=3600000; //Default 1 hrs before EEPROM read
  //Setting defaults before EEPROM
  __solutions = 10;
  __max_pumps_number = 2;
  __max_steppers =  8;
  __relay_number = 4;

  // Define motors pins defaults before EEPROM
  __DirS[0] = dirS1;
  __StepS[0] = stepS1;
  __DirS[1] = dirS2;
  __StepS[1] = stepS2;
  __DirS[2] = dirS3;
  __StepS[2] = stepS3;
  __DirS[3] = dirS4;
  __StepS[3] = stepS4;
  __DirS[4] = dirS5;
  __StepS[4] = stepS5;
  __DirS[5] = dirS6;
  __StepS[5] = stepS6;
  __DirS[6] = dirS7;
  __StepS[6] = stepS7;
  __DirS[7] = dirS8;
  __StepS[7] = stepS8;
  for (int i=0;i<__max_steppers;i++){
    __MOTOR_SPEED[i]=MOTOR_SPEED;
    __MOTOR_ACCEL[i]=MOTOR_ACCEL;
  }
  __MicroSteps = 8;
  __StepPerRev = 200;

  __loadCellCalibration = loadCellCalibration;
  tareAux=true;
  dispenseDelay = millis();
  __PreTimer = millis();

  __En = en;

  

  systemRunning = false;
  __stopped = false;
  show      = false;
  stopShow  = false;
  __dispensing = true;
  runStepper = true;

  //Motor default order  before EEPROM 
  CarMotor = 0;
  DispenserMotor = 3;
  CargoMotor = 1;
  ReleaseMotor = 2;
  ExtraMotor = 4;

  //Pumps default order
  microsPump=0;
  acidPump=1;

  //LS default order before EEPROM
  carHomeLS = 0; //LS 1 normally closed
  dispenserLS = 3; //LS 4 normally open
  carEndLS = 4; //LS 5 normally closed
  cargoLS = 1; //LS 2 normally closed
  openLS = 2; //LS 3 normally closed

  // Define pumps pins defaults before EEPROM
  __Motor[0] = pumpA1;
  __Motor[1] = pumpB1;

  __enableMotorA = pumpEnA;
  __enableMotorB = pumpEnB;
  __PumpOn[0] = false;
  __PumpOn[1] = false;
  __PumpVelocity[0] = 0;
  __PumpVelocity[1] = 0;
  for (int i = 0; i<RELAY_NUMBER; i++){
    SolFinished[i]=true;
  }
  
  // Define relay pins before EEPROM
  __Relay[0] = relay1;
  __Relay[1] = relay2;
  __Relay[2] = relay3;
  __Relay[3] = relay4;
  for (int i=0;i<__relay_number;i++){
    __relay_action_time[i]=RELAY_ACTION_TIME;
  }
  __relay_action_time[1] = 600000;



  // Default parameters
  __SteppersEnabled = false;
 for(int i=0; i<__solutions;i++){
    __CalibrationLiquids[i] = 0;
    saltsOrder[i] = 0;
  }

  __microsCalibration =22.4;

  //debugAux=millis();

  prepareTimeState = true;

  // Atlas Scientific Sensors
  //__pH = phMeter->getValue();
  //__eC = ecMeter->getValue();
  __pHtarget = 6.00;
  __ECtarget = 1800;
  __ECmax = 1800;
  __literTarget = 1500;
  runPeristaltics = true;
  __ExportEzo = false;
  __ImportEzo = false;
  __SleepEzo = false;

  //phMeter = new EZO(EZO_PH);
  ecMeter = new EZO(EZO_EC);

  //Define temperature tempSensor
  scale = new HX711;
  __Temp = 0;

  //Define new HX711 scale
  dhtSensor = new DHT(DHTSens, DHTTYPE);

  //Define mmemory
  myMem = new ExternalEEPROM;
  // Filters
  __Filter = 1;
  __Alpha = 0.1; __Noise = 0; __Err = 0;
}

void solutionMaker::begin(){
  Serial.println(F("debug, Solution Maker: Starting..."));

  __StepPerRev = MOTOR_STEP_PER_REV;

  // Define the motors
  for (int i=0;i<__max_steppers;i++){
    stepperS[i] = new AccelStepper(1, __StepS[i], __DirS[i]);
  }
  // Initial Configuration
  for(int i=0; i<__max_steppers; i++){
    stepperS[i]->setMaxSpeed(__MOTOR_SPEED[i]);
    stepperS[i]->setSpeed(0);
    stepperS[i]->setAcceleration(__MOTOR_ACCEL[i]);
    pinMode(__StepS[i], OUTPUT);
    pinMode(__DirS[i], OUTPUT);
    // Check if we want this configuration of pinsInverted
    /* setPinsInverted parameters:
    1- bool directionInvert = false
    2- bool 	stepInvert = false
    2- bool 	enableInvert = false
    */
    stepperS[i]->setPinsInverted(false,false,false);
  }
  //Pin to enable stepper motors
  pinMode(__En, OUTPUT);
  // Disable motors/pumps
  digitalWrite(__En, HIGH);
  
  //Setup DC peristaltic pumps
  for(int i=0; i<__max_pumps_number; i++){
    pinMode(__Motor[i], OUTPUT);
    digitalWrite(__Motor[i], LOW);
  }
  pinMode(__enableMotorA,OUTPUT);
  pinMode(__enableMotorB,OUTPUT);
  digitalWrite(__enableMotorA, LOW);
  digitalWrite(__enableMotorB, LOW);

  pinMode(pumpA2,OUTPUT);
  pinMode(pumpB2,OUTPUT);
  digitalWrite(pumpA2, LOW);
  digitalWrite(pumpB2, LOW);

  // By default turn off relay at start
  for (int i = 0; i < RELAY_NUMBER; i++) {
    pinMode(__Relay[i], OUTPUT);
    __RelayState[i] = LOW;
    digitalWrite(__Relay[i], __RelayState[i]);
  }

  // Init Atlas Scientific Sensors
  //pinMode(EZO_PH_OFF, OUTPUT);
  pinMode(EZO_EC_OFF, OUTPUT);
  //digitalWrite(EZO_PH_OFF, HIGH); //Turned ON
  digitalWrite(EZO_EC_OFF, HIGH); //Turned ON
  //phMeter->init();
  ecMeter->init();

  // Init temperature tempSensor
  dhtSensor->begin();
  __Temp = dhtSensor->readTemperature();
  __Hum = dhtSensor->readHumidity();
  //__Temp = 20;
  defaultFilter(); // Set by default exponential filter with alpha=0.2

  //Init scale

  scale->begin(loadCellDT,loadCellSCK);
  scale->set_scale(__loadCellCalibration);
  //delay(1000);
  scale->tare();
  Serial.print(F("debug,Solution Maker: First scale medition for calibration purpose "));
  Serial.println(scale->get_units(5));
  //delay(1000);

  // Request phMeter and ecMeter readings
  EZOReadRequest(__Temp, false);

  Serial.println(F("debug,Solution Maker: Started Correctly"));
}

void solutionMaker::enable(uint8_t motor){
  digitalWrite(__En, LOW);
  printAction(F("Steppers enabled"), motor);
}

void solutionMaker::disable(){
  if(digitalRead(__En)==LOW){
    digitalWrite(__En, HIGH);
    printAction(F("Steppers disabled"),0);
    return;
  }
  printAction(F("Steppers already disabled"),0);
  return;
}

void solutionMaker::turnOnPump(unsigned long time1, uint8_t pump){
  if(pump<__max_pumps_number){
    if (pump == 0)digitalWrite(__enableMotorA, HIGH);
    if (pump == 1)digitalWrite(__enableMotorB, HIGH);
    Serial.print("Pump pin ");
    Serial.println(__Motor[pump]);
    digitalWrite(__Motor[pump], HIGH);
    __PumpOnTime[pump] = time1;
    __PumpTime[pump] = millis();
    if (!__PumpOn[pump])printAction(F("Turned pump On"), __max_steppers+pump, 0);
    __PumpOn[pump] = true;
    return;
  }
  printAction(F("Pump does not exist"), __max_steppers+__max_pumps_number, 3);
  return;
}

void solutionMaker::turnOffPump(uint8_t pump){
  if(pump<__max_pumps_number){
    digitalWrite(__Motor[pump], LOW);
    digitalWrite(__enableMotorA, LOW);
    digitalWrite(__enableMotorB, LOW);
    __PumpOnTime[pump] = 0;
    printAction(F("Turned pump Off"), __max_steppers+pump, 0);
    __PumpOn[pump] = false;
    return;
  }
  printAction(F("Pump does not exist"), __max_steppers+__max_pumps_number, 3);
  return;
}

unsigned long solutionMaker::TimeToML(float seconds, uint8_t pump){
  if(seconds>0 && pump<__max_pumps_number){
    return  seconds*(float(__Calibration[pump])/10);
  }
  return 0;
}

unsigned long solutionMaker::MLToTime(float mililiters, uint8_t pump){
  if(mililiters>0 && pump<__max_pumps_number){
    // __Calibration/100 = ml/s
    return mililiters/(float(__Calibration[pump])/100);
  }
  return 0;
}

long solutionMaker::RevToSteps(float rev){ return rev*__MicroSteps*__StepPerRev;}

float solutionMaker::StepsToRev(long st){ return st/(__MicroSteps*__StepPerRev);}

float solutionMaker::mlToSteps(float ml, uint8_t st){return (ml/__Calibration[st]) * (__MicroSteps*__StepPerRev);}

float solutionMaker::balanceLiquidSalts(float EC_init, float EC_final, float liters, uint8_t st){
  float deltaEC = (EC_final-EC_init)/__ECmax;
  float ml = 0;
  if(deltaEC>=0){ // We need add solution powder
    if (deltaEC>0.05){
      deltaEC = 0.05;
    }
    ml = float(__CalibrationLiquids[st]*deltaEC); // Get ml
    /*if (millis()-debugAux > 1000){
      Serial.println(F("-----------------------"));
      Serial.print(F("EC target = "));
      Serial.println(EC_final);
      Serial.print(F("EC meassurment = "));
      Serial.println(EC_init);
      Serial.print(F("Liters = "));
      Serial.println(liters);
      Serial.print(F("EC calculated grams = "));
      Serial.println(mg/1000.00);
      Serial.println(F("-----------------------"));
      debugAux=millis();
    }*/
  }else{
    Serial.println(F("EC is too high, water needed"));
  }
  return ml; // Return grams
}

/*float solutionMaker::balancePH(float PH_init, float PH_final, float liters, uint8_t pump){
  if(PH_init-PH_final>=0.1){
    float PH_acid = float(__CalibrationSolids[__solutions-__max_pumps_number+pump]); // Get the ph of the acid
    float ml = (pow(10, -1*PH_init)-pow(10, -1*PH_final))/(pow(10, -1*PH_final)-pow(10, -1*PH_acid))*liters*1000;
    return ml; // Returns the ml of the acid that are needed
  }
  else if(PH_init-PH_final>=0 && PH_init-PH_final<0.1){ // Do nothing
    return 0;
  }
  else {
    //Serial.println(F("error,Solution Maker: balancePH function error"));
    return 0;// Key Error: ph is already to low it is necessary to send an alarm
  }
}*/

void solutionMaker::moveStepper(long steps, uint8_t st){
  enable(st);
  //resetPosition(st);
  stepperS[st]->move(steps);
  return;
}

void solutionMaker::stopStepper(uint8_t st){
  if(st<__max_steppers){
    stepperS[st]->setCurrentPosition(0);
    stepperS[st]->moveTo(0);
    printAction(F("Stopped Stepper"), st, 0);
    return;
  }
  printAction(F("Cannot stop Stepper. Motor does not exist"), __max_steppers, 3);
  return;
}

void solutionMaker::resetPosition(uint8_t st){
  if(st<__max_steppers){
    stepperS[st]->setCurrentPosition(0);
    //printAction(F("Reset Position"), st, 0);
    return;
  }
  printAction(F("Cannot reset position. Motor does not exist"), __max_steppers, 3);
  return;
}

void solutionMaker::printAction(String act, uint8_t actuator, uint8_t level){ //default level=0
  if(level==0){ Serial.print(F("debug,")); } // Debug
  else if(level==1){ Serial.print(F("info,")); } // Info
  else if(level==2){ Serial.print(F("warning,")); } // Warning
  else if(level==3){ Serial.print(F("error,")); } // Error
  else if(level==4){ Serial.print(F("critical,")); } // Error

  Serial.print(F("Solution Maker (Pump "));
  Serial.print(String(actuator));
  Serial.print(F("):\t"));
  Serial.println(act);
}

void solutionMaker::printEZOAction(String act, uint8_t sensorType, uint8_t level){ //default level = 0
  if(level==0){ Serial.print(F("debug,")); } // Debug
  else if(level==1){ Serial.print(F("info,")); } // Info
  else if(level==2){ Serial.print(F("warning,")); } // Warning
  else if(level==3){ Serial.print(F("error,")); } // Error
  else if(level==4){ Serial.print(F("critical,")); } // Error

  Serial.print(F("Solution Maker (EZO "));

  if(sensorType==EZO_PH){Serial.print(F("PH"));}
  else if(sensorType==EZO_EC){Serial.print(F("EC"));}
  else{Serial.print(F("Unkwown"));}

  Serial.print(F(" Sensor): "));
  Serial.println(act);
}

float solutionMaker::filter(float val, float preVal){
  switch(__Filter){
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

float solutionMaker::exponential_filter(float alpha, float t, float t_1){
  if(isnan(alpha) || isnan(t) || isnan(t_1)){
    return t;
  }
  else{
    return (t*alpha+(1-alpha)*t_1);
  }
}

float solutionMaker::kalman_filter(float t, float t_1){
  if( __Err==0 || isnan(t) || isnan(t_1) ){
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

void solutionMaker::readTemp(){
  float temp = dhtSensor->readTemperature();
  float hum = dhtSensor->readHumidity();
  //float temp = 25;
  if(temp>0) __Temp = filter(temp, __Temp);
  if(temp>0) __Hum = filter(hum, __Hum);
}

void solutionMaker::EZOReadRequest(float temp, bool sleep){ //sleep default = false
  if(EZOisEnable(EZO_PH) || EZOisEnable(EZO_EC)){
    if(!__SleepEzo && !__ImportEzo){ // If sensors are not sleeping and are not importing cal.
      //phMeter->readWithTempCompensation(temp);
      ecMeter->readWithTempCompensation(temp);
    }
    else if(sleep){ // If we want to force to awake the sensors
      __SleepEzo = false;
      //phMeter->readWithTempCompensation(temp);
      ecMeter->readWithTempCompensation(temp);
    }
  }
  else{Serial.println(F("debug,Solution Maker (EZO Sensors): They are not available"));}
}

void solutionMaker::readRequest(){ // Take temperature readings every 5 seconds
  /*if(millis()-__ReadTime>1000){
    if (millis()-__ReadTime>3000 && readAux==true){
      __scaleMedition=scale->get_units();
      __eC = ecMeter->getValue(); // Save last read from ecMeter
      //__pH = phMeter->getValue(); // Save last read from phmeter
      //Serial.print(F("Scale med = "));
      //Serial.println(__scaleMedition);
      //Serial.print(F("PH ="));
      //Serial.println(__pH);
      //Serial.print(F("EC = "));
      //Serial.println(__eC);
      readAux=false;
    } else if (millis()-__ReadTime>5000){
      __ReadTime = millis();
      readAux=true;
    }

    //Serial.print(F("Salt number "));
    //Serial.println(__limitS_salts);
    //Serial.println(StepsToRev(stepperS[CarMotor]->currentPosition()));
    //Serial.println((StepsToRev(stepperS[CarMotor]->currentPosition())*8)/100);
    //Serial.println(round((StepsToRev(stepperS[CarMotor]->currentPosition())*8)/100));
    //EZOReadRequest(__Temp); // Request new read
  }*/

  if(millis()-__ReadTime>1000){
    readTemp(); // Read temp
    __scaleMedition=scale->get_units();
    //Serial.println(__scaleMedition);
    __eC = ecMeter->getValue();
    EZOReadRequest(__Temp, false);
    __ReadTime = millis();
  }
}

void solutionMaker::EZOexportFinished(){
  if(EZOisEnable(EZO_PH) && EZOisEnable(EZO_PH) && __ExportEzo) __ExportEzo = false;
}

void solutionMaker::relayControl(int rel){
  if(__RelayState[rel]){
    __RelayState[rel] = LOW;
    SolFinished[rel] = false;
    __RelayTime[rel] = millis();
    Serial.println(F("warning,Solution Maker: mixing"));
  }
  else if(!__RelayState[rel] && millis()-__RelayTime[rel]>__relay_action_time[rel] && !SolFinished[rel]){
    digitalWrite(__Relay[rel], LOW);
    Serial.println(F("info,Solution Finished"));
    Serial.println(F("Solution Finished"));
    SolFinished[rel] = true;
  }
}

void solutionMaker::dispensePeristaltic(float some_ml, uint8_t pump){
  if(some_ml!=0){ // Check that parameter is correct
    unsigned long steps = mlToSteps(some_ml, pump); // *1000 to convert s to ms
    if(steps>0){ // Check equation result
      moveStepper(steps, pump);
      printAction("Dispensing " + String(some_ml) + " ml" , pump, 0);
      return;
    }
    printAction(F("MLToSteps equation problem"), pump, 3);
    return;
  }
  printAction(F("Mililiters parameter incorrect"), pump, 3);
  return;
}

void solutionMaker::dispenseAcid(float some_ml, uint8_t pump){
  if(some_ml>0){ // Check that parameter is correct
    unsigned long time1 = MLToTime(some_ml, pump)*1000; // *1000 to convert s to ms
    if(time1>0){ // Check equation result
      turnOnPump(time1, pump);
      printAction("Dispensing " + String(some_ml) + " ml", pump, 0);
      return;
    }
    printAction(F("MLToTime equation problem"), pump, 3);
    return;
  }
  printAction(F("Mililiters parameter incorrect"), pump, 3);
  return;
}

void solutionMaker::pumpCalibration(float revs, uint8_t pump){
  if(pump<__max_pumps_number){ // Check that the pump exists
    moveStepper(RevToSteps(revs),pump);
  }
  else{printAction(F("Cannot run calibration. Pump does not exist"), __max_steppers+__max_pumps_number, 3);}
}

void solutionMaker::setCalibrationParameter(float param, uint8_t pump){
  if(pump<__max_steppers){
    __Calibration[pump] = param;
    //printAction(String(param) + " is the new calibration param (motors/pumps)",  __max_steppers + pump, 0);
    return;
  }
    printAction(F("Cannot set calibration Param. Actuator does not exist"), 7, 3);
}

void solutionMaker::setCalibrationParameter1(float param, uint8_t salt){ //salt or acid
  if(salt<__max_steppers){
    __CalibrationLiquids[salt] = param;
    //printAction(String(param) + " is the new calibration param (ph/ec equations)", 1, 0);
    return;
  }
    printAction(F("Cannot set calibration Param. Actuator does not exist"), 1, 3);
}

void solutionMaker::notFilter(){ __Filter = 0; __Alpha = 0; __Noise = 0; __Err = 0; printFilter();}

void solutionMaker::defaultFilter(){ __Filter = 1; __Alpha = 0.2; printFilter();}

bool solutionMaker::setExponentialFilter(float alpha){ //alpha default = 0.5
  if(alpha>0 && alpha<1){
    __Filter = 1; __Alpha = alpha;
    printFilter();
    return true;
  }
  else{
    Serial.println(F("error,Solution Maker: Parameter alpha wrong"));
    return false;
  }
}

bool solutionMaker::setKalmanFilter(float noise){
  if(noise!=0){
    __Filter = 2;
    __Noise = noise; // Enviromental Noise
    __Err = 1; // Initial Error
    printFilter();
    return true;
  }
  else{
    Serial.print(F("error,Solution Maker: Parameter noise wrong"));
    return false;
  }
}

void solutionMaker::printFilter(){
  Serial.print(F("debug,Solution Maker (Temperature readings): "));
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

bool solutionMaker::EZOisEnable(uint8_t sensorType){
  if(sensorType == EZO_PH){ return false;/*phMeter->isEnable()*/ }
  else if(sensorType == EZO_EC){ return ecMeter->isEnable(); }
  return false;
}

void solutionMaker::EZOcalibration(uint8_t sensorType, uint8_t act, float value){
  if(sensorType == EZO_PH && EZOisEnable(EZO_PH)){
    //phMeter->calibration(act, value);
  }
  else if(sensorType == EZO_EC && EZOisEnable(EZO_EC)){
    ecMeter->calibration(act, value);
  }
  else{printEZOAction(F("Sensor does not match a type or is in another request"), sensorType, 3);}
}

void solutionMaker::EZOexportCal(uint8_t sensorType){
  if(!__ExportEzo){
    if(sensorType == EZO_PH && EZOisEnable(EZO_PH)){
      __ExportEzo = true;
      //phMeter->exportCal();
    }
    else if(sensorType == EZO_EC && EZOisEnable(EZO_EC)){
      __ExportEzo = true;
      ecMeter->exportCal();
    }
    else{printEZOAction(F("Sensor does not match a type or is in another request"), sensorType, 3);}
  }
  else{printEZOAction(F("Another export is running"), sensorType, 3);}
}

void solutionMaker::EZOimportCalibration(uint8_t sensorType, String parameters){
  if(sensorType == EZO_PH && EZOisEnable(EZO_PH)){
    //phMeter->importCalibration(parameters);
  }
  else if(sensorType == EZO_EC && EZOisEnable(EZO_EC)){
    ecMeter->importCalibration(parameters);
  }
  else{printEZOAction(F("Sensor does not match a type or is in another request"), sensorType, 3);}
}

void solutionMaker::EZOimport(bool start){
  if(start!=__ImportEzo){
    __ImportEzo = start;
    if(__ImportEzo) Serial.println(F("warning,Solution Maker (EZO Sensors): Starting Calibration Import"));
    else Serial.println(F("warning,Solution Maker (EZO Sensors): Stopping Calibration Import"));
  }
  else{
    if(start) Serial.println(F("error,Solution Maker (EZO Sensors): Calibration Import already started"));
    else Serial.println(F("error,Solution Maker (EZO Sensors): Calibration Import already stopped"));
  }
}

void solutionMaker::EZOsleep(){
  if(!__SleepEzo){
    if(EZOisEnable(EZO_PH) && EZOisEnable(EZO_EC)){
      __SleepEzo = true;
      //phMeter->sleep();
      ecMeter->sleep();
    }
    else{Serial.println(F("error,Solution Maker (EZO Sensors): They are in another request"));}
  }
  else{Serial.println(F("warning,Solution Maker (EZO Sensors): They are already in sleep mode"));}
}

void solutionMaker::EZOawake(){
  if(__SleepEzo){
    EZOReadRequest(__Temp, true); // Take a read to awake the sensors
  }
  else{Serial.println(F("warning,Solution Maker (EZO Sensors): They are already awake"));}
}

bool solutionMaker::EZOisSleep(){ return __SleepEzo; }

void solutionMaker::run(){
  // Check sensors constantly
  readRequest(); // Request read of temperature, ph and ec
  //phMeter->run(); // Handle phMeter functions
  ecMeter->run(); // Handle ecMeter functions
  EZOexportFinished(); // Check when EZO sensors finished export cal.

  // For all the motors
  for(int i=0; i<__max_steppers; i++){
    runStepper = true;
    for (int j = 1; j<= i; j++){
      if (i!=0 && stepperS[i-j]->distanceToGo() != 0 && runStepper == true){
        runStepper = false;
      } 
    }
    if (runStepper == true){
        stepperS[i]->run();
    }    
  }

  // For all the relays check them constantly
  for (int i=0;i<__relay_number;i++){
    if(!__dispensing)relayControl(i);
  }


  if (show){
      tareTime = 5000;
      showMultiplier = 0.1;
      prepareTimeState = true;
  } else {
    tareTime = 60000;
      showMultiplier = 1;
  }

  // Second stage, if homed and time between preparations has passed, prepare solution
  if (prepareTimeState && (systemRunning || show) ){
    prepareSolution(__literTarget, __pHtarget, __ECtarget);
    
  }
  // If system is off only relays for mixing will continue working
  else if (prepareTimeState && !systemRunning){ 
    prepareTimeAux = millis();
    prepareTimeState=false; 
    Serial.println("Running relays");
    for (int i=0;i<__relay_number;i++){
      SolFinished[i]=false;
    __RelayTime[i]=millis();
    }
  }
  // Third, if not ready to run cycle, check and run relays
  else if (!prepareTimeState){
    for (int i=0;i<__relay_number;i++){
      if(!SolFinished[i] && __relay_action_time[i]!=0)digitalWrite(__Relay[i], HIGH);
      else digitalWrite(__Relay[i], LOW);
    }
    if (millis()-prepareTimeAux>PREPARE_CYCLE) {
      Serial.println(F("Finished waiting cycle"));
      Serial.print("Time waited ");
      Serial.print(PREPARE_CYCLE/1000/60ul);
      Serial.println(" minutes");
      Serial.print("Sistem current status is ");
      if (systemRunning)Serial.println("RUNNING");
      else Serial.println("PAUSED");
      prepareTimeState=true;
      __PreTimer =  millis();
      tareAux = true;
      runPeristaltics = true;
    }
  }

    if (stepperS[0]->distanceToGo() == 0 &&
      stepperS[1]->distanceToGo() == 0 &&
      stepperS[2]->distanceToGo() == 0 &&
      stepperS[3]->distanceToGo() == 0 &&
      stepperS[4]->distanceToGo() == 0 &&
      stepperS[5]->distanceToGo() == 0 &&
      stepperS[6]->distanceToGo() == 0 &&
      stepperS[7]->distanceToGo() == 0 ){
        
        if (__dispensing){
          __dispensing = false;
          disable();
          Serial.println(F("Finished dispensing liquids"));
        }
    }else{__dispensing = true;}
}

void solutionMaker::prepareSolution(float liters, float ph, float ec){
  //First before running turn recirculation pump 1 ON
  if (millis()-__PreTimer<PreTime){
    digitalWrite(__Relay[1], HIGH);
  }//Check for Conductivity below parameters
  else if (__eC > __ECtarget*(1.00-DEVIATION)){
    digitalWrite(__Relay[1], LOW);
    prepareTimeAux = millis();
    prepareTimeState=false;
    Serial.println(F("System EC is in good condition"));
    Serial.print(F("Waiting "));
    Serial.print(PREPARE_CYCLE/1000/60ul);
    Serial.println(F(" minutes"));
    Serial.print(F("Turning pump for "));
    Serial.print(__relay_action_time[1]/1000/60ul);
    Serial.println(F(" minutes"));
    SolFinished[1]=false;
    __RelayTime[1]=millis();

  }
  //Second check if conductivity is below minimum target
  else if (__eC < __ECtarget*(1.00-DEVIATION) && runPeristaltics){
    digitalWrite(__Relay[1], LOW);

    for (uint8_t i = 0;i<__max_steppers;i++){
      mlSalts[i] = balanceLiquidSalts(__eC, ec, liters, i);
      dispensePeristaltic(mlSalts[i], i); 
    }

    runPeristaltics = false;

    for (int i=0;i<__relay_number;i++){
          if (__relay_action_time[i]!=0)__RelayState[i] = HIGH;
          Serial.print(F("Turning relay"));
          Serial.print(i);
          Serial.print(F(" for"));
          Serial.print(__relay_action_time[i]);
          Serial.println(F("ms after dispensing liquids"));
        }
        prepareTimeState = false;
        prepareTimeAux = millis();
        Serial.print(F("Finished calculating, next check cycle in "));
        Serial.print(PREPARE_CYCLE/1000/60ul);
        Serial.println(F(" minutes"));
      
  }
  
}

void solutionMaker::clean_EEPROM(){
  Serial.println(F("warning,EEPROM: Deleting Memory..."));
  for (int i=0;i<1000;i++){
    myMem->write(i,0);
  }
}

void solutionMaker::print_EEPROM(int i){
  Serial.println(F("EEPROM: Printing Memory Info"));
  Serial.print(i); Serial.print(F(": ")); Serial.println(myMem->read(i));
}

void solutionMaker::read_EEPROM(bool pr){
  if (myMem->begin() == false)
  {
    Serial.println("No memory detected.");      ;
  }else {
    Serial.println("Memory detected!");

    myMem->get(0, __EEPROMstruct);
    Serial.println(F("EEPROM read correctly"));
    if (pr == true){
      if(__EEPROMstruct.Solutions!=0){
        __solutions = __EEPROMstruct.Solutions;
        for (int i=0;i<__EEPROMstruct.Solutions;i++){
          setCalibrationParameter1(__EEPROMstruct.__CalibrationLiquids[i], i);
          setCalibrationParameter(__EEPROMstruct.__Calibration[i], i);
          saltsOrder[i] = __EEPROMstruct.__saltOrders[i];
        }
      }
      if(__EEPROMstruct.loadCellCal!=0){
        __loadCellCalibration = __EEPROMstruct.loadCellCal;
      }
      if(__EEPROMstruct.cycle!=0){
        PREPARE_CYCLE = __EEPROMstruct.cycle*20*10000ul;
      }
      if(__EEPROMstruct.Relays!=0){
         __relay_number = __EEPROMstruct.Relays;
         for (int i=0;i<__EEPROMstruct.Relays;i++){
          __relay_action_time[i]=__EEPROMstruct.__RelayTime[i]*6000ul; //Since max storage is 255, storage is setup in seconds and divided by 3 allowing a max of 765 seconds
        }
      }
      if(__EEPROMstruct.__ECtarget!=0){
        __ECtarget = __EEPROMstruct.__ECtarget;
      }
      if(__EEPROMstruct.__ECmax!=0){
        __ECmax = __EEPROMstruct.__ECmax;
      }
      Serial.println(F("EEPROM calibration parameters readed and charged"));
      for (int i = 0;i<sizeof(__EEPROMstruct);i++){
        __EEPROM[i] = myMem->read(i);
      }
    }
  }
}

void solutionMaker::default_EEPROM(){
  __EEPROMstruct.Solutions = SOLUTIONS;
  __EEPROMstruct.loadCellCal = loadCellCalibration;
  __EEPROMstruct.cycle = 18; //uint8_t(3600000/20/10000) 1 hours/20/10000
  __EEPROMstruct.Relays = RELAY_NUMBER;
  __EEPROMstruct.__ECtarget = 2114.8;
  __EEPROMstruct.__ECmax = 2455.00;
  //Parametro de calibración de bomba para manguera 4/6mm
  __EEPROMstruct.__Calibration[0] = 1.0;
  __EEPROMstruct.__Calibration[1] = 1.0;
  __EEPROMstruct.__Calibration[2] = 1.0;
  __EEPROMstruct.__Calibration[3] = 1.0;
  __EEPROMstruct.__Calibration[4] = 1.0;
  __EEPROMstruct.__Calibration[5] = 1.0;
  __EEPROMstruct.__Calibration[6] = 1.0;
  __EEPROMstruct.__Calibration[7] = 1.0;

  //Default for "ml for 1500l of SOLUCION MEDIA MAX (100%)"
  __EEPROMstruct.__CalibrationLiquids[0] = 2129.03;
  __EEPROMstruct.__CalibrationLiquids[1] = 3160.28;
  __EEPROMstruct.__CalibrationLiquids[2] = 2031.14;
  __EEPROMstruct.__CalibrationLiquids[3] = 1963.31;
  __EEPROMstruct.__CalibrationLiquids[4] = 1812.47;
  __EEPROMstruct.__CalibrationLiquids[5] = 2169.53;
  __EEPROMstruct.__CalibrationLiquids[6] = 2098.8;
  __EEPROMstruct.__CalibrationLiquids[7] = 0;
  __EEPROMstruct.__RelayTime[0] = 60000/6000;
  __EEPROMstruct.__RelayTime[1] = 600000/6000;
  __EEPROMstruct.__RelayTime[2] = 60000/6000;
  __EEPROMstruct.__RelayTime[3] = 60000/6000;
  __EEPROMstruct.__saltOrders[0] = 1;
  __EEPROMstruct.__saltOrders[1] = 3;
  __EEPROMstruct.__saltOrders[2] = 5;
  __EEPROMstruct.__saltOrders[3] = 6;
  __EEPROMstruct.__saltOrders[4] = 7;
  __EEPROMstruct.__saltOrders[5] = 8;
  __EEPROMstruct.__saltOrders[6] = 10;
  __EEPROMstruct.__saltOrders[7] = 0;
  
  myMem->put(0, __EEPROMstruct);
  

  Serial.println(F("EEPROM calibration parameters set as DEFAULT"));
}

void solutionMaker::save_EEPROM(){
  myMem->put(0,__EEPROMstruct);
  Serial.println(F("EEPROM calibration parameters saved"));
}

void solutionMaker::write_EEPROM(unsigned int pos, byte val){
  myMem->write(pos, val);
  Serial.print(F("EEPROM: Parameter saved in pos: ")); Serial.print(pos); Serial.print(F("\tval= ")); Serial.println(val);
}


