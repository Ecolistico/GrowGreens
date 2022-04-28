// solutionMaker.cpp
//
// Copyright (C) 2019 Grow

#include "sMakerCunTFT.h"
#include <Wire.h>

/***   solutionMaker   ***/
// Statics variables definitions

solutionMaker::solutionMaker(){
  Wire.begin();
  PREPARE_CYCLE=3600000; //Default 12 hrs before EEPROM read
  //Setting defaults before EEPROM
  __solutions = 10;
  __max_pumps_number = 2;
  __max_steppers =  5;
  __relay_number = 4;
  __limit_switches = 5;

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
  for (int i=0;i<__max_steppers;i++){
    __MOTOR_SPEED[i]=MOTOR_SPEED;
    __MOTOR_ACCEL[i]=MOTOR_ACCEL;
  }

  __loadCellCalibration = loadCellCalibration;
  tareAux=true;
  dispenseDelay = millis();
  dispenseAux=true;
  __PreTimer = millis();

  __En = en;

  __MicroSteps = 0;
  __StepPerRev = 0;
  __limitS_salts=0;
  __moveCar = false;
  __dispensing = false;
  __HOMED = false;

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
  __HOMEING[0] = true;
  __HOMEING[1] = true;
  __HOMEING[2] = true;
  SolFinished=true;

  //Define limitSwitch pins before EEPROM
  __limitS[0] = limitS1;
  __limitS[1] = limitS2;
  __limitS[2] = limitS3;
  __limitS[3] = limitS4;
  __limitS[4] = limitS5;

  // Define relay pins before EEPROM
  __Relay[0] = relay1;
  __Relay[1] = relay2;
  __Relay[2] = relay3;
  __Relay[3] = relay4;
  for (int i=0;i<__relay_number;i++){
    __relay_action_time[i]=RELAY_ACTION_TIME;
  }



  // Default parameters
  __SteppersEnabled = false;
  for(int i=0; i<__max_pumps_number;i++){
    __Calibration[i] = 6.16; //ml in 10 s
    __CalibrationDEF[i] = 6.16;
  }
  for(int i=0; i<__solutions;i++){
    __Calibration1[i] = 0;
    __Calibration1DEF[i] = 0;
  }
  __Calibration1DEF[0] = 0.0870;
  __Calibration1DEF[1] = 0.0277;
  __Calibration1DEF[2] = 0.3440;
  __Calibration1DEF[3] = 0.2870;
  __Calibration1DEF[4] = 0.1440;
  __Calibration1[0] = 0.0870;
  __Calibration1[1] = 0.0277;
  __Calibration1[2] = 0.3440;
  __Calibration1[3] = 0.2870;
  __Calibration1[4] = 0.1440;

  __microsCalibration =22.4;

   __Calibration1[8] = 1; //acid pH
  //debugAux=millis();

  //Set Limit Switches state
  for(int i=0; i<LIMIT_SWITCHES;i++){
    __limitS_State[i] = false;
  }

  __HOMED = false;
  prepareTimeState = true;

  // Atlas Scientific Sensors
  //__pH = phMeter->getValue();
  //__eC = ecMeter->getValue();
  PH_Target = 5.75;
  EC_Target = 1625;
  __pHtarget = PH_Target;
  __ECtarget = EC_Target;
  __literTarget = 1500;
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
  // Define the motors
  for (int i=0;i<__max_steppers;i++){
    stepperS[i] = new AccelStepper(1, __StepS[i], __DirS[i]);
  }

  Serial.println(F("debug, Solution Maker: Starting..."));

  __StepPerRev = MOTOR_STEP_PER_REV;
  //__PumpVelocity = PUMP_VELOCITY;
  __MicroSteps = DEFAULT_MICROSTEP;

  //Micro stepps setup
  for (int i=0; i<__max_steppers*2; i++){
    pinMode(__mS[i],OUTPUT);
  }
  if(__MicroSteps == 16){ //DEFAULT_MICROSTEP 16
    for (int i=0; i<__max_steppers*2; i++){
      digitalWrite(__mS[i], HIGH);
    }
  }else if(__MicroSteps == 8){ //DEFAULT_MICROSTEP 8
    for (int i=0; i<__max_steppers*2; i = i+2){
      digitalWrite(__mS[i], HIGH);
    }
    for (int i=1; i<__max_steppers*2; i = i+2){
      digitalWrite(__mS[i], LOW);
    }
  } else { //DEFAULT_MICROSTEP 4
    for (int i=0; i<__max_steppers*2; i++){
      digitalWrite(__mS[i], LOW);
      __MicroSteps = 4;
    }
    Serial.println(F("debug, Solution Maker: Microsteppin to 1/4"));
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

  for (int i = 0; i < LIMIT_SWITCHES; i++) {
    pinMode(__limitS[i],INPUT_PULLUP);
  }
  checkLS();
  Serial.println(__limitS_State[0]);
  Serial.println(__limitS_State[1]);
  Serial.println(__limitS_State[2]);

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
  pinMode(EZO_PH_OFF, OUTPUT);
  pinMode(EZO_EC_OFF, OUTPUT);
  digitalWrite(EZO_PH_OFF, HIGH); //Turned ON
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
    return mililiters/(float(__Calibration[pump])/10);
  }
  return 0;
}

long solutionMaker::RevToSteps(float rev){ return rev*__MicroSteps*__StepPerRev;}

float solutionMaker::StepsToRev(long st){ return st/(__MicroSteps*__StepPerRev);}

float solutionMaker::balanceEC(float EC_init, float EC_final, float liters, uint8_t salt){
  float deltaEC = EC_final-EC_init;
  float mg = 0;
  if(deltaEC>=0){ // We need add solution powder
    float slope = float(__Calibration1[salt]); // Get the slope in (mg/l)/(uS/cm)
    float mgPerLiter = deltaEC*slope; // Get mg/l needed
    mg = mgPerLiter*liters; // Get mg needed
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
    Serial.println(F("EC is to high, water needed"));
  }
  return mg/1000.00; // Return grams
}
float solutionMaker::balanceMicros(float EC_init, float EC_final, float liters){
  float deltaEC = EC_final-EC_init;
  float ml = 0;
  if(deltaEC>=0){ // We need add solution powder
    float slope = float(__microsCalibration/1000); // Get the slope in (mg/l)/(uS/cm)
    float mlPerLiter = deltaEC*slope; // Get ml/1000 l needed
    ml = mlPerLiter*liters/1000; // Get mg needed
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
    Serial.println(F("EC is to high, water needed"));
  }
  return ml; // Return grams
}

float solutionMaker::balancePH(float PH_init, float PH_final, float liters, uint8_t pump){
  if(PH_init-PH_final>=0.1){
    float PH_acid = float(__Calibration1[__solutions-__max_pumps_number+pump]); // Get the ph of the acid
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
}

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

void solutionMaker::moveCar(int dist){
  moveStepper(RevToSteps(dist/8), CarMotor);
  return;
}

void solutionMaker::home(){
  checkLS();
  if(__limitS_State[carHomeLS]  && __HOMEING[carHomeLS]){
    Serial.println("Car at home");
    homeTimer=millis();
  }
  if(__limitS_State[cargoLS] && __HOMEING[CargoMotor]){
    Serial.println("Cargo bay at home");

  }
  if(__limitS_State[openLS] && __HOMEING[ReleaseMotor]){
    Serial.println("Hatch at home");
  }

  __HOMEING[CarMotor]=!__limitS_State[carHomeLS];
  __HOMEING[CargoMotor]=!__limitS_State[cargoLS];
  __HOMEING[ReleaseMotor]=!__limitS_State[openLS];

  if (!__HOMED){
    if (__HOMEING[CarMotor] && !__HOMEING[CargoMotor] && !__HOMEING[ReleaseMotor] && stepperS[CarMotor]->distanceToGo()==0) moveStepper(RevToSteps(-150), CarMotor);
    if (__HOMEING[CargoMotor] && stepperS[CargoMotor]->distanceToGo()==0) moveStepper(RevToSteps(-1), CargoMotor);
    if (__HOMEING[ReleaseMotor] && stepperS[ReleaseMotor]->distanceToGo()==0) moveStepper(RevToSteps(-1), ReleaseMotor);

    for (int i = 0; i<3; i++){
      if (!__HOMEING[i]){
        stepperS[i]->setCurrentPosition(0);
        stepperS[i]->stop();
      }
    }
  }




    if(!__HOMEING[CarMotor] && !__HOMEING[CargoMotor] && !__HOMEING[ReleaseMotor] && millis()-homeTimer>1500) {
      __HOMED=true;
      Serial.println("Homeing completed");
      //delay(1000);
      //scale->tare();
      __limitS_salts=0;
      __moveCar = false;
    }

  return;
}

void solutionMaker::checkLS(){
  for (int i=0;i<__limit_switches;i++){
    __limitS_State[i]=digitalRead(__limitS[i]);
  }
}

void solutionMaker::printAction(String act, uint8_t actuator, uint8_t level){ //default level=0
  if(level==0){ Serial.print(F("debug,")); } // Debug
  else if(level==1){ Serial.print(F("info,")); } // Info
  else if(level==2){ Serial.print(F("warning,")); } // Warning
  else if(level==3){ Serial.print(F("error,")); } // Error
  else if(level==4){ Serial.print(F("critical,")); } // Error

  Serial.print(F("Solution Maker ("));

  if(actuator==0){Serial.print(F("Pump 1"));}
  else if(actuator==1){Serial.print(F("Pump 2"));}
  else{Serial.print(F("Actuator does not match"));}

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
    __eC = ecMeter->getValue();
    EZOReadRequest(__Temp, false);
    __ReadTime = millis();
  }
}

void solutionMaker::EZOexportFinished(){
  if(EZOisEnable(EZO_PH) && EZOisEnable(EZO_PH) && __ExportEzo) __ExportEzo = false;
}

void solutionMaker::relayControl(int rel){
  if(__RelayState[rel] && __HOMED){
    __RelayState[rel] = LOW;
    SolFinished = false;
    __RelayTime[rel] = millis();
    Serial.println(F("warning,Solution Maker: mixing to finish"));
  }
  else if(!__RelayState[rel] && millis()-__RelayTime[rel]>__relay_action_time[rel] && !SolFinished){
    digitalWrite(__Relay[rel], LOW);
    Serial.println(F("info,Solution Finished"));
    Serial.println(F("Solution Finished"));
    SolFinished = true;
  }
}

void solutionMaker::dispense(float some_mg){
  if (__scaleMedition > some_mg*0.97){
    stopStepper(DispenserMotor);
    Serial.print(F("Finished with salt "));
    Serial.print(__limitS_salts);
    Serial.print(F(" dispensed mg "));
    Serial.println(some_mg);
    __limitS_salts +=1;
    scale->tare();
    //delay(1000);
    __dispensing = false;
    __moveCar = false;
    tareAux = true;
    dispenseAux=true;
  } else {
    //Serial.println(stepperS[CarMotor]->distanceToGo());
    if (stepperS[CarMotor]->distanceToGo()==0) {
      if (stepperS[DispenserMotor]->distanceToGo()==0){
        Serial.println(F("-----------------------"));
        Serial.print(F("Dispensing salt "));
        Serial.print(__limitS_salts+1);
        Serial.print(F(": "));
        Serial.println(some_mg);
        Serial.println(F("-----------------------"));
        moveStepper(32000, DispenserMotor);//arbitrary steps unitll loadcell reads correct value
      }
    }
  }
}

void solutionMaker::dispenseMicros(float some_ml, uint8_t pump){
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

void solutionMaker::pumpCalibration(float time1, uint8_t pump){
  if(pump<__max_pumps_number){ // Check that the pump exists
    turnOnPump(abs(time1*1000), pump);
  }
  else{printAction(F("Cannot run calibration. Pump does not exist"), __max_steppers+__max_pumps_number, 3);}
}

void solutionMaker::setCalibrationParameter(float param, uint8_t pump){
  if(pump<__max_pumps_number){
    __Calibration[pump] = param;
    printAction(String(param) + " is the new calibration param (motors/pumps)",  __max_steppers + pump, 0);
    return;
  }
    printAction(F("Cannot set calibration Param. Actuator does not exist"), 7, 3);
}

void solutionMaker::setCalibrationParameter1(float param, uint8_t salt){ //salt or acid
  if(salt<__solutions){
    __Calibration1[salt] = param;
    printAction(String(param) + " is the new calibration param (ph/ec equations)", 1, 0);
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
  readRequest(); // Request read of temperature, ph and ec
  //phMeter->run(); // Handle phMeter functions
  ecMeter->run(); // Handle ecMeter functions
  EZOexportFinished(); // Check when EZO sensors finished export cal.
  checkLS();

  // For all the relays
  for (int i=0;i<__relay_number;i++){
    relayControl(i);
  }

  // For all the motors
  for(int i=0; i<__max_steppers; i++){
    stepperS[i]->run();
  }

  // For all the pumps
  for(int i=0; i<__max_pumps_number; i++){
    if(millis()-__PumpTime[i]>__PumpOnTime[i] && __PumpOn[i]){
      turnOffPump(i);
    }
  }
  if (!__HOMED) {
    home();
  } else if (__HOMED && prepareTimeState){
    prepareSolution(__literTarget, __pHtarget, __ECtarget, __limitS_salts);
  } else if (__HOMED && !prepareTimeState){
    for (int i=0;i<__relay_number;i++){
      if(!SolFinished && __relay_action_time[i]!=0)digitalWrite(__Relay[i], HIGH);
    }
    if (millis()-prepareTimeAux>PREPARE_CYCLE) {
      prepareTimeState=true;
      __PreTimer =  millis();
    }
  }
}

void solutionMaker::prepareSolution(float liters, float ph, float ec, uint8_t salt){
  float mgPowder = balanceEC(__eC, ec, liters, salt);
  __mgPowder=mgPowder;
  float mlMicros = balanceMicros(__eC, ec, liters);
  //Testing pumps
  //float mlAcid = balancePH(__pH, ph, liters, 0); // Solution 1 -> Acid 1
  //float mlAcid=500;
  //float mlAcid2=500;
  if (millis()-__PreTimer<PreTime){
    __RelayState[1] = HIGH;
    Serial.println(F("Turned relay 1 ON"));
    Serial.print(F("Micros to dispense "));
    Serial.println(mlMicros);
  }else if (__eC < __ECtarget*0.85){
    //Cargo bay tries to remain up in every single step of preparation until the end
    if (!__limitS_State[cargoLS] && !__limitS_State[carEndLS]) { //Move cargo bay up to home
      moveStepper(RevToSteps(-1), CargoMotor);
      __HOMEING[CargoMotor]=!__limitS_State[cargoLS];
    }
    else if (__limitS_State[cargoLS] && __HOMEING[CargoMotor] && !__limitS_State[carEndLS]) { //Cargo bay home reached, stop moving
      stepperS[CargoMotor]->setCurrentPosition(0);
      stepperS[CargoMotor]->stop();
      __HOMEING[CargoMotor]=!__limitS_State[cargoLS];
    }

    if (!__limitS_State[carEndLS] && !__moveCar){ //If not at end limit switch, moves from home towards dispensers
        moveCar(1040);
        __moveCar = true;
        Serial.println("moving car to dispensers");
    } else if(!__limitS_State[dispenserLS] && __moveCar && __dispensing) { //If a dispenser is found and dispensing is true (can dispense)
      //stepperS[CarMotor]->moveTo(stepperS[CarMotor]->currentPosition());
      //__limitS_salts = __limitS_salts+round((StepsToRev(stepperS[CarMotor]->currentPosition())*8)/100);
      if((StepsToRev(stepperS[CarMotor]->currentPosition())*8)>50){ //current step position / (200 steps per rev at 1/4 micro with 8mm per rev) bigger than 50mm
        stepperS[CarMotor]->setCurrentPosition(0);
        stepperS[CarMotor]->stop();
        Serial.println(F("Car motor stopped to dispense"));
      }

      if (dispenseAux==true){
        dispenseDelay=millis();
        dispenseAux=false;
        Serial.println(F("---dispense time started---"));
      }
      if(millis()-dispenseDelay>60000){
        if(tareAux==true){
          /*Serial.println(F("-----------------------"));
          Serial.print(F("EC calculated grams = "));
          Serial.println(mgPowder);
          Serial.println(F("-----------------------"));*/
          Serial.println(F("---scale tared---"));
          scale->tare();
          tareAux=false;
        }
      }
      if(millis()-dispenseDelay>61000){
        if(mgPowder>0){
          //Serial.println("dispensing");
          dispense(mgPowder);
        } else {
          __dispensing = false;
          __moveCar = false;
          Serial.println("Dispenser finished with 0 mg");
        }
      }

    } else if(__limitS_State[dispenserLS] && __moveCar && !__dispensing) { //if dispensing is false
      __dispensing = true;
    } else if (__limitS_State[carEndLS]){ //if car end limit switch found
      __moveCar = false;
      if (carEndDetected == false){
        endTimer = millis();
        carEndDetected = true;
      }
      if (millis()-endTimer > 500){
        stepperS[CarMotor]->setCurrentPosition(0);
        stepperS[CarMotor]->stop();
      }

      //Throw down cargo
      if(__limitS_State[cargoLS] && stepperS[CargoMotor]->distanceToGo()==0 && millis()-endTimer > 1000){
        moveStepper(RevToSteps(5), CargoMotor);
        moveStepper(RevToSteps(0.3), ReleaseMotor);
      }
      //if cargo thrown down and 5 secs passed after finding end
      if (!__limitS_State[cargoLS] && stepperS[CargoMotor]->distanceToGo()==0 && stepperS[ReleaseMotor]->distanceToGo()==0 & millis()-endTimer > 5000){
        dispenseMicros(mlMicros, microsPump);

        //Serial.print(F(", mlAcid="));
        //Serial.println(mlAcid);
        for (int i=0;i<__relay_number;i++){
          if (__relay_action_time[i]!=0)__RelayState[i] = HIGH;
        }
        __HOMED=false;
        prepareTimeState = false;
        prepareTimeAux = millis();
      }
    }
    else if (!__limitS_State[carEndLS]){
      carEndDetected = false;
    }    
  }


  
}

void solutionMaker::clean_EEPROM(){
  Serial.println(F("warning,EEPROM: Deleting Memory..."));
  for (int i=0;i<BaseEEPROM+SOLUTIONS+4*MAX_PUMPS_NUMBER+5*MAX_STEPPERS+2*RELAY_NUMBER+2*LIMIT_SWITCHES;i++){
    __EEPROM[i] = 0;
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

    for (int i=0;i<BaseEEPROM+SOLUTIONS+4*MAX_PUMPS_NUMBER+5*MAX_STEPPERS+2*RELAY_NUMBER+2*LIMIT_SWITCHES;i++){
      __EEPROM[i] = myMem->read(i);
    }

    Serial.println(F("EEPROM read correctly"));
    if (pr == true){
      if(__EEPROM[0]!=0){
        __solutions = __EEPROM[0];
        for (int i=0;i<__EEPROM[0];i++){
          if (__EEPROM[BaseEEPROM+i] !=0)setCalibrationParameter1(float(__EEPROM[BaseEEPROM+i])/725, i);
        }
      }
      else{Serial.println(F("warning, Solutions number in EEPROM = 0: Not possible to load salt and acid calibration parameters"));}
      if(__EEPROM[1]!=0){
        __max_pumps_number = __EEPROM[1];
        for (int i=0;i<__EEPROM[1];i++){
          if (__EEPROM[BaseEEPROM+SOLUTIONS+i] !=0)setCalibrationParameter(__EEPROM[BaseEEPROM+SOLUTIONS+i]/100.00, i);
          if (__EEPROM[BaseEEPROM+SOLUTIONS+MAX_PUMPS_NUMBER+i] !=0)__PumpVelocity[i]=__EEPROM[BaseEEPROM+SOLUTIONS+MAX_PUMPS_NUMBER+i];
          if (__EEPROM[BaseEEPROM+SOLUTIONS+2*MAX_PUMPS_NUMBER+i] !=0)__Motor[i]=__EEPROM[BaseEEPROM+SOLUTIONS+2*MAX_PUMPS_NUMBER+i];
          pumpOrderCfg(i,__EEPROM[BaseEEPROM+SOLUTIONS+3*MAX_PUMPS_NUMBER+i]);
        }
      }
      else{Serial.println(F("warning, Acid pump number in EEPROM = 0: Not possible to load pump calibration parameters"));}
      if(__EEPROM[2]!=0){
        __loadCellOffset = __EEPROM[2];
      }
      if(__EEPROM[3]!=0){
        __loadCellCalibration = -__EEPROM[3]*2;
      }
      if(__EEPROM[4]!=0){
        __max_steppers = __EEPROM[4];
        for (int i=0;i<__EEPROM[4];i++){
          motorOrderCfg(i,__EEPROM[BaseEEPROM+SOLUTIONS+4*MAX_PUMPS_NUMBER+i]);
          __MOTOR_SPEED[i] =__EEPROM[BaseEEPROM+SOLUTIONS+4*MAX_PUMPS_NUMBER+MAX_STEPPERS+i]*10;
          __MOTOR_ACCEL[i] =__EEPROM[BaseEEPROM+SOLUTIONS+4*MAX_PUMPS_NUMBER+2*MAX_STEPPERS+i]*10;
          __StepS[i] =__EEPROM[BaseEEPROM+SOLUTIONS+4*MAX_PUMPS_NUMBER+3*MAX_STEPPERS+i];
          __DirS[i] =__EEPROM[BaseEEPROM+SOLUTIONS+4*MAX_PUMPS_NUMBER+4*MAX_STEPPERS+i];
        }
      }
      if(__EEPROM[5]!=0){
        PREPARE_CYCLE = __EEPROM[5]*20*10000; //Time given in seconds divided by 15 (this allows up to 1 hour)
      }
      if(__EEPROM[6]!=0){
        __relay_number = __EEPROM[6];
        for (int i=0;i<__EEPROM[6];i++){
          __Relay[i]=__EEPROM[BaseEEPROM+SOLUTIONS+4*MAX_PUMPS_NUMBER+5*MAX_STEPPERS+i];
          __relay_action_time[i]=__EEPROM[BaseEEPROM+SOLUTIONS+4*MAX_PUMPS_NUMBER+5*MAX_STEPPERS+RELAY_NUMBER+i]*3*1000; //Since max storage is 255, storage is setup in seconds and divided by 3 allowing a max of 765 seconds
        }
      }
      if(__EEPROM[7]!=0){
        __limit_switches = __EEPROM[7];
        for (int i=0;i<__EEPROM[7];i++){
          limitOrderCfg(i,__EEPROM[BaseEEPROM+SOLUTIONS+4*MAX_PUMPS_NUMBER+5*MAX_STEPPERS+2*RELAY_NUMBER+i]);
          __limitS[i]=__EEPROM[BaseEEPROM+SOLUTIONS+4*MAX_PUMPS_NUMBER+5*MAX_STEPPERS+2*RELAY_NUMBER+LIMIT_SWITCHES+i];
        }
      }
      if(__EEPROM[8]!=0){
        __pHtarget = float(__EEPROM[8]*3/100.00);
      }
      if(__EEPROM[9]!=0){
        __ECtarget = __EEPROM[9]*10;

      }
      myMem->get(sizeof(__EEPROM)/sizeof(__EEPROM[0])+1, __microsCalibration);
      Serial.println(F("EEPROM calibration parameters readed and charged"));
    }
  }
}

void solutionMaker::default_EEPROM(bool pr){
   if (pr == true){
     __EEPROM[0] = SOLUTIONS;
    for (int i=0;i<SOLUTIONS;i++){
      __EEPROM[BaseEEPROM+i] = uint8_t(__Calibration1DEF[i]*725);
    }
    __EEPROM[1] = MAX_PUMPS_NUMBER;
      for (int i=0;i<MAX_PUMPS_NUMBER;i++){
        __EEPROM[BaseEEPROM+SOLUTIONS+i]=uint8_t(__CalibrationDEF[i]*100);
        __EEPROM[BaseEEPROM+SOLUTIONS+MAX_PUMPS_NUMBER+i]=PUMP_VELOCITY;
      }
      __EEPROM[BaseEEPROM+SOLUTIONS+2*MAX_PUMPS_NUMBER+0] = pumpA1;
      __EEPROM[BaseEEPROM+SOLUTIONS+2*MAX_PUMPS_NUMBER+1] = pumpB1;
      __EEPROM[BaseEEPROM+SOLUTIONS+3*MAX_PUMPS_NUMBER+0] = 0;
      __EEPROM[BaseEEPROM+SOLUTIONS+3*MAX_PUMPS_NUMBER+1] = 1;
      __EEPROM[2] = LoadCellOffset;
      __EEPROM[3] = uint8_t(-loadCellCalibration/2);
      __EEPROM[4] = MAX_STEPPERS;
      __EEPROM[BaseEEPROM+SOLUTIONS+4*MAX_PUMPS_NUMBER+0] = 0;
      __EEPROM[BaseEEPROM+SOLUTIONS+4*MAX_PUMPS_NUMBER+1] = 3;
      __EEPROM[BaseEEPROM+SOLUTIONS+4*MAX_PUMPS_NUMBER+2] = 1;
      __EEPROM[BaseEEPROM+SOLUTIONS+4*MAX_PUMPS_NUMBER+3] = 2;
      __EEPROM[BaseEEPROM+SOLUTIONS+4*MAX_PUMPS_NUMBER+4] = 4;
      __EEPROM[BaseEEPROM+SOLUTIONS+4*MAX_PUMPS_NUMBER+MAX_STEPPERS+0] = 200;
      __EEPROM[BaseEEPROM+SOLUTIONS+4*MAX_PUMPS_NUMBER+MAX_STEPPERS+1] = 200;
      __EEPROM[BaseEEPROM+SOLUTIONS+4*MAX_PUMPS_NUMBER+MAX_STEPPERS+2] = 200;
      __EEPROM[BaseEEPROM+SOLUTIONS+4*MAX_PUMPS_NUMBER+MAX_STEPPERS+3] = 200;
      __EEPROM[BaseEEPROM+SOLUTIONS+4*MAX_PUMPS_NUMBER+MAX_STEPPERS+4] = 200;
      __EEPROM[BaseEEPROM+SOLUTIONS+4*MAX_PUMPS_NUMBER+2*MAX_STEPPERS+0] = 50;
      __EEPROM[BaseEEPROM+SOLUTIONS+4*MAX_PUMPS_NUMBER+2*MAX_STEPPERS+1] = 50;
      __EEPROM[BaseEEPROM+SOLUTIONS+4*MAX_PUMPS_NUMBER+2*MAX_STEPPERS+2] = 50;
      __EEPROM[BaseEEPROM+SOLUTIONS+4*MAX_PUMPS_NUMBER+2*MAX_STEPPERS+3] = 50;
      __EEPROM[BaseEEPROM+SOLUTIONS+4*MAX_PUMPS_NUMBER+2*MAX_STEPPERS+4] = 50;
      __EEPROM[BaseEEPROM+SOLUTIONS+4*MAX_PUMPS_NUMBER+3*MAX_STEPPERS+0] = stepS1;
      __EEPROM[BaseEEPROM+SOLUTIONS+4*MAX_PUMPS_NUMBER+3*MAX_STEPPERS+1] = stepS2;
      __EEPROM[BaseEEPROM+SOLUTIONS+4*MAX_PUMPS_NUMBER+3*MAX_STEPPERS+2] = stepS3;
      __EEPROM[BaseEEPROM+SOLUTIONS+4*MAX_PUMPS_NUMBER+3*MAX_STEPPERS+3] = stepS4;
      __EEPROM[BaseEEPROM+SOLUTIONS+4*MAX_PUMPS_NUMBER+3*MAX_STEPPERS+4] = stepS5;
      __EEPROM[BaseEEPROM+SOLUTIONS+4*MAX_PUMPS_NUMBER+4*MAX_STEPPERS+0] = dirS1;
      __EEPROM[BaseEEPROM+SOLUTIONS+4*MAX_PUMPS_NUMBER+4*MAX_STEPPERS+1] = dirS2;
      __EEPROM[BaseEEPROM+SOLUTIONS+4*MAX_PUMPS_NUMBER+4*MAX_STEPPERS+2] = dirS3;
      __EEPROM[BaseEEPROM+SOLUTIONS+4*MAX_PUMPS_NUMBER+4*MAX_STEPPERS+3] = dirS4;
      __EEPROM[BaseEEPROM+SOLUTIONS+4*MAX_PUMPS_NUMBER+4*MAX_STEPPERS+4] = dirS5;
      __EEPROM[5]= uint8_t(3600000/20/10000); //12 hours/20/10000
      __EEPROM[6] = RELAY_NUMBER;
      __EEPROM[BaseEEPROM+SOLUTIONS+4*MAX_PUMPS_NUMBER+5*MAX_STEPPERS+0] = relay1;
      __EEPROM[BaseEEPROM+SOLUTIONS+4*MAX_PUMPS_NUMBER+5*MAX_STEPPERS+1] = relay2;
      __EEPROM[BaseEEPROM+SOLUTIONS+4*MAX_PUMPS_NUMBER+5*MAX_STEPPERS+2] = relay3;
      __EEPROM[BaseEEPROM+SOLUTIONS+4*MAX_PUMPS_NUMBER+5*MAX_STEPPERS+3] = relay4;
      for (int i=0;i<RELAY_NUMBER;i++){
        __EEPROM[BaseEEPROM+SOLUTIONS+4*MAX_PUMPS_NUMBER+5*MAX_STEPPERS+RELAY_NUMBER+i]=RELAY_ACTION_TIME/3/1000; //Since max storage is 255, storage is setup in seconds and divided by 3 allowing a max of 765 seconds
      }
      __EEPROM[7] = LIMIT_SWITCHES;
      __EEPROM[BaseEEPROM+SOLUTIONS+4*MAX_PUMPS_NUMBER+5*MAX_STEPPERS+2*RELAY_NUMBER+0] = 0;
      __EEPROM[BaseEEPROM+SOLUTIONS+4*MAX_PUMPS_NUMBER+5*MAX_STEPPERS+2*RELAY_NUMBER+1] = 3;
      __EEPROM[BaseEEPROM+SOLUTIONS+4*MAX_PUMPS_NUMBER+5*MAX_STEPPERS+2*RELAY_NUMBER+2] = 4;
      __EEPROM[BaseEEPROM+SOLUTIONS+4*MAX_PUMPS_NUMBER+5*MAX_STEPPERS+2*RELAY_NUMBER+3] = 1;
      __EEPROM[BaseEEPROM+SOLUTIONS+4*MAX_PUMPS_NUMBER+5*MAX_STEPPERS+2*RELAY_NUMBER+4] = 2;
      __EEPROM[BaseEEPROM+SOLUTIONS+4*MAX_PUMPS_NUMBER+5*MAX_STEPPERS+2*RELAY_NUMBER+LIMIT_SWITCHES+0] = limitS1;
      __EEPROM[BaseEEPROM+SOLUTIONS+4*MAX_PUMPS_NUMBER+5*MAX_STEPPERS+2*RELAY_NUMBER+LIMIT_SWITCHES+1] = limitS2;
      __EEPROM[BaseEEPROM+SOLUTIONS+4*MAX_PUMPS_NUMBER+5*MAX_STEPPERS+2*RELAY_NUMBER+LIMIT_SWITCHES+2] = limitS3;
      __EEPROM[BaseEEPROM+SOLUTIONS+4*MAX_PUMPS_NUMBER+5*MAX_STEPPERS+2*RELAY_NUMBER+LIMIT_SWITCHES+3] = limitS4;
      __EEPROM[BaseEEPROM+SOLUTIONS+4*MAX_PUMPS_NUMBER+5*MAX_STEPPERS+2*RELAY_NUMBER+LIMIT_SWITCHES+4] = limitS5;

      __EEPROM[8] =  uint8_t(PH_Target/3*100);
      __EEPROM[9] =  uint8_t(EC_Target/10);

    for (int i=0;i<10+SOLUTIONS+4*MAX_PUMPS_NUMBER+5*MAX_STEPPERS+2*RELAY_NUMBER+2*LIMIT_SWITCHES;i++){
      myMem->write(i,__EEPROM[i]);
      myMem->put(sizeof(__EEPROM)/sizeof(__EEPROM[0])+1, 22.4);
    }

    Serial.println(F("EEPROM calibration parameters set as DEFAULT"));
  }
}

void solutionMaker::save_EEPROM(){
  for (int i=0;i<SOLUTIONS;i++){
    __EEPROM[BaseEEPROM+i] = uint8_t(__Calibration1[i]*725);
    Serial.println(__EEPROM[BaseEEPROM+i]);
  }
  for (int i=0;i<MAX_PUMPS_NUMBER;i++){
    __EEPROM[BaseEEPROM+SOLUTIONS+i]=uint8_t(__Calibration[i]*100);
    __EEPROM[BaseEEPROM+SOLUTIONS+MAX_PUMPS_NUMBER+i]=PUMP_VELOCITY;
  }
  __EEPROM[BaseEEPROM+SOLUTIONS+3*MAX_PUMPS_NUMBER+0] = microsPump;
  __EEPROM[BaseEEPROM+SOLUTIONS+3*MAX_PUMPS_NUMBER+1] = acidPump;
    
  __EEPROM[3] = uint8_t(-__loadCellCalibration/2);
  __EEPROM[5]= uint8_t(PREPARE_CYCLE/20/10000); //12 hours/20/10000

  __EEPROM[8] =  uint8_t(__pHtarget/3*100);
  __EEPROM[9] =  uint8_t(__ECtarget/10);

  for (int i=0;i<BaseEEPROM+SOLUTIONS+4*MAX_PUMPS_NUMBER+5*MAX_STEPPERS+2*RELAY_NUMBER+2*LIMIT_SWITCHES;i++){
    myMem->write(i,__EEPROM[i]);
  }

  Serial.println(F("EEPROM calibration parameters saved"));
}

void solutionMaker::write_EEPROM(unsigned int pos, byte val){
  myMem->write(pos, val);
  Serial.print(F("EEPROM: Parameter saved in pos: ")); Serial.print(pos); Serial.print(F("\tval= ")); Serial.println(val);
}

void solutionMaker::motorOrderCfg(byte id, byte param){
  switch (id) {
  case 0:
    CarMotor = param;
    Serial.print("CarMotor in order ");
    Serial.println(CarMotor);
    break;
  case 1:
    DispenserMotor = param;
    Serial.print("DispenserMotor in order ");
    Serial.println(DispenserMotor);
    break;
  case 2:
    CargoMotor = param;
    Serial.print("CargoMotor in order ");
    Serial.println(CargoMotor);
    break;
  case 3:
    ReleaseMotor = param;
    Serial.print("ReleaseMotor in order ");
    Serial.println(ReleaseMotor);
    break;
  case 4:
    ExtraMotor = param;
    Serial.print("ExtraMotor in order ");
    Serial.println(ExtraMotor);
    break;
  default:
    Serial.println(F("Current SMaker code used only 5 motors"));
    break;
  }
}

void solutionMaker::pumpOrderCfg(byte id, byte param){
  switch (id) {
  case 0:
    microsPump = param;
    Serial.print("Micros pump from eeprom  ");
    Serial.println(microsPump);
    break;
  case 1:
    acidPump = param;
    Serial.print("Acid pump from eeprom  ");
    Serial.println(acidPump);
    break;
  default:
    Serial.println(F("Current SMaker code used only 2 pumps"));
    break;
  }
}

void solutionMaker::limitOrderCfg(byte id, byte param){
  switch (id) {
  case 0:
    carHomeLS = param;
    Serial.print("carHomeLS in order ");
    Serial.println(carHomeLS);
    break;
  case 1:
    dispenserLS = param;
    Serial.print("dispenserLS in order ");
    Serial.println(dispenserLS);
    Serial.print("dispenserLS pin ");
    Serial.println(__limitS[dispenserLS]);
    break;
  case 2:
    carEndLS = param;
    Serial.print("carEndLS in order ");
    Serial.println(carEndLS);
    break;
  case 3:
    cargoLS = param;
    Serial.print("cargoLS in order ");
    Serial.println(cargoLS);
    break;
  case 4:
    openLS = param;
    Serial.print("openLS in order ");
    Serial.println(openLS);
    break;
  default:
    Serial.println(F("Current SMaker code used only 5 limit switches"));
    break;
  }
}
