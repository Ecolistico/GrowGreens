// solutionMaker.cpp
//
// Copyright (C) 2019 Grow

#include "sMakerCun.h"

/***   solutionMaker   ***/
// Statics variables definitions

solutionMaker::solutionMaker(){
    // Define motors pins
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

    __En = en;

    __MicroSteps = 0;
    __StepPerRev = 0;
    __PumpVelocity = 0;
    __limitS_salts=0;
    __moveCar = false;
    __dispensing = false;
    __HOMED = false;

    // Define the motors
    stepperS[0] = new AccelStepper(1, __StepS[0], __DirS[0]); //Car motor
    stepperS[1] = new AccelStepper(1, __StepS[1], __DirS[1]); //Cargo motor
    stepperS[2] = new AccelStepper(1, __StepS[2], __DirS[2]); //Releasemotor
    stepperS[3] = new AccelStepper(1, __StepS[3], __DirS[3]); //Dispenser motor
    stepperS[4] = new AccelStepper(1, __StepS[4], __DirS[4]); //Extra, not in use for the moment

    // Define pumps pins
    __Motor[0] = pump1;
    __Motor[1] = pump2;
    __PumpOn[0] = false;
    __PumpOn[1] = false;
    __HOMEING[0] = true;
    __HOMEING[1] = true;
    __HOMEING[2] = true;
    __limitS[0] = limitS1;
    __limitS[1] = limitS2;
    __limitS[2] = limitS3;
    __limitS[3] = limitS4;
    __limitS[4] = limitS5;

    // Define relay pins only one relay is in used by mixer motor
    __Relay[0] = relay1;
    __Relay[1] = relay2;
    __Relay[2] = relay3;
    __Relay[3] = relay4;



    // Default parameters
    __SteppersEnabled = false;
    for(int i=0; i<MAX_PUMPS_NUMBER;i++){
      __Calibration[i] = 1;
    }
    for(int i=0; i<SOLUTIONS;i++){
      __Calibration1[i] = 1;
    }

    //Set Limit Switches state
    for(int i=0; i<LIMIT_SWITCHES;i++){
      __limitS_State[i] = false;
    }

    __HOMED = false;

    // Atlas Scientific Sensors
    __pH = 0;
    __eC = 0;
    __ExportEzo = false;
    __ImportEzo = false;
    __SleepEzo = false;

    phMeter = new EZO(EZO_PH);
    ecMeter = new EZO(EZO_EC);

    //Define temperature tempSensor
    scale = new HX711;
    __Temp = 0;

    //Define new HX711 scale
    dhtSensor = new DHT(tempSens, DHTTYPE);

    // Filters
    __Filter = 0;
    __Alpha = 0; __Noise = 0; __Err = 0;
  }

void solutionMaker::begin(
  uint8_t steps_per_rev,
  uint8_t microStep,
  uint8_t pump_velocity
){
Serial.println(F("debug, Solution Maker: Starting..."));

//Micro stepps setup
for (int i=0; i<MAX_STEPPERS*2; i++){
  pinMode(__mS[i],OUTPUT);
}
if(microStep == 16){ //DEFAULT_MICROSTEP 16
  for (int i=0; i<MAX_STEPPERS*2; i++){
    digitalWrite(__mS[i], HIGH);
  }
}else if(microStep == 8){ //DEFAULT_MICROSTEP 8
  for (int i=0; i<MAX_STEPPERS*2; i = i+2){
    digitalWrite(__mS[i], HIGH);
  }
  for (int i=1; i<MAX_STEPPERS*2; i = i+2){
    digitalWrite(__mS[i], LOW);
  }
} else { //DEFAULT_MICROSTEP 4
  for (int i=0; i<MAX_STEPPERS*2; i++){
    digitalWrite(__mS[i], LOW);
  }
  Serial.println(F("debug, Solution Maker: Microsteppin to 1/4"));
}



__StepPerRev = steps_per_rev;
if (microStep==16 || microStep==8){
  __MicroSteps = microStep;
} else {
  __MicroSteps = 4;
}

__PumpVelocity = pump_velocity;

// Initial Configuration
for(int i=0; i<MAX_STEPPERS; i++){
  stepperS[i]->setMaxSpeed(MOTOR_SPEED);
  stepperS[i]->setSpeed(0);
  stepperS[i]->setAcceleration(MOTOR_ACCEL);
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

for(int i=0; i<MAX_PUMPS_NUMBER; i++){
  pinMode(__Motor[i], OUTPUT);
  digitalWrite(__Motor[i], LOW);
}

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
phMeter->init();
ecMeter->init();

// Init temperature tempSensor
dhtSensor->begin();
//__Temp = dhtSensor->readTemperature();
__Temp = 20;
defaultFilter(); // Set by default exponential filter with alpha=0.2

//Init scale
__loadCellCalibration = loadCellCalibration;
scale->begin(loadCellDT,loadCellSCK);
scale->set_scale(__loadCellCalibration);
scale->tare();
Serial.print(F("debug,Solution Maker: First scale medition for calibration purpose "));
Serial.println(scale->get_units(5));
delay(1000);

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
  if(pump<MAX_PUMPS_NUMBER){
    digitalWrite(__Motor[pump], HIGH);
    __PumpOnTime[pump] = time1;
    __PumpTime[pump] = millis();
    __PumpOn[pump] = true;
    printAction(F("Turned pump On"), MAX_STEPPERS+pump, 0);
    return;
  }
  printAction(F("Pump does not exist"), MAX_STEPPERS+MAX_PUMPS_NUMBER, 3);
  return;
}

void solutionMaker::turnOffPump(uint8_t pump){
  if(pump<MAX_PUMPS_NUMBER){
    digitalWrite(__Motor[pump], LOW);
    __PumpOnTime[pump] = 0;
    printAction(F("Turned pump Off"), MAX_STEPPERS+pump, 0);
    __PumpOn[pump] = false;
    return;
  }
  printAction(F("Pump does not exist"), MAX_STEPPERS+MAX_PUMPS_NUMBER, 3);
  return;
}

unsigned long solutionMaker::TimeToML(float seconds, uint8_t pump){
  if(seconds>0 && pump<MAX_PUMPS_NUMBER){
    return  seconds*(float(__Calibration[pump])/100);
  }
  return 0;
}

unsigned long solutionMaker::MLToTime(float mililiters, uint8_t pump){
  if(mililiters>0 && pump<MAX_PUMPS_NUMBER){
    // __Calibration/100 = ml/s
    return mililiters/(float(__Calibration[pump])/100);
  }
  return 0;
}

long solutionMaker::RevToSteps(float rev){ return rev*__MicroSteps*__StepPerRev;}

float solutionMaker::StepsToRev(long st){ return st/(__MicroSteps*__StepPerRev);}

float solutionMaker::balanceEC(float EC_init, float EC_final, float liters, uint8_t salt){
  float deltaEC = EC_final-EC_init;
  float mg = 0;
  if(deltaEC>=0){ // We need add solution powder
    float slope = float(__Calibration1[salt])*0.004; // Get the slope in (mg/l)/(uS/cm)
    float mgPerLiter = deltaEC*slope; // Get mg/l needed
    mg = mgPerLiter*liters; // Get mg needed
  }else{
    Serial.println(F("EC is to high, water needed"));
  }
  return mg; // Return mg
}

float solutionMaker::balancePH(float PH_init, float PH_final, float liters, uint8_t pump){
  if(PH_init-PH_final>=0.1){
    float PH_acid = float(__Calibration1[SOLUTIONS-2+pump])*0.05; // Get the ph of the acid
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
  if(st<MAX_STEPPERS){
    stepperS[st]->setCurrentPosition(0);
    stepperS[st]->moveTo(0);
    printAction(F("Stopped Stepper"), st, 0);
    return;
  }
  printAction(F("Cannot stop Stepper. Motor does not exist"), MAX_STEPPERS, 3);
  return;
}

void solutionMaker::resetPosition(uint8_t st){
  if(st<MAX_STEPPERS){
    stepperS[st]->setCurrentPosition(0);
    //printAction(F("Reset Position"), st, 0);
    return;
  }
  printAction(F("Cannot reset position. Motor does not exist"), MAX_STEPPERS, 3);
  return;
}

void solutionMaker::moveCar(int dist){
  moveStepper(RevToSteps(dist/8), CarMotor);
  return;
}

void solutionMaker::home(){
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
    checkLS();
    if(!__HOMEING[CarMotor] && !__HOMEING[CargoMotor] && !__HOMEING[ReleaseMotor] && millis()-homeTimer>500) {
      __HOMED=true;
      Serial.println("Homeing completed");
      scale->tare();
      __limitS_salts=0;
      __moveCar = false;
    }


    if(__limitS_State[carHomeLS] && __HOMEING[CarMotor]){
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

  return;
}

void solutionMaker::checkLS(){
  for (int i=0;i<LIMIT_SWITCHES;i++){
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

  if(actuator==0){Serial.print(F("Motor 1"));}
  else if(actuator==1){Serial.print(F("Motor 2"));}
  else if(actuator==2){Serial.print(F("Motor 3"));}
  else if(actuator==3){Serial.print(F("Motor 4"));}
  else if(actuator==4){Serial.print(F("Motor 5"));}
  else if(actuator==5){Serial.print(F("Pump 1"));}
  else if(actuator==6){Serial.print(F("Pump 2"));}
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
  if(temp>0) __Temp = filter(temp, __Temp);
}

void solutionMaker::EZOReadRequest(float temp, bool sleep){ //sleep default = false
  if(EZOisEnable(EZO_PH) && EZOisEnable(EZO_EC)){
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
  else{Serial.println(F("debug,Solution Maker (EZO Sensors): They are not available"));}
}

void solutionMaker::readRequest(){ // Take temperature readings every 5 seconds
  if(millis()-__ReadTime>5000){
    __ReadTime = millis();
    __pH = phMeter->getValue(); // Save last read from phmeter
    __eC = ecMeter->getValue(); // Save last read from ecMeter
    __scaleMedition=scale->get_units(1);
    Serial.print("Scale med = ");
    Serial.println(__scaleMedition);
    Serial.print("Salt number ");
    Serial.println(__limitS_salts);
    Serial.println(StepsToRev(stepperS[CarMotor]->currentPosition()));
    Serial.println((StepsToRev(stepperS[CarMotor]->currentPosition())*8)/100);
    Serial.println(round((StepsToRev(stepperS[CarMotor]->currentPosition())*8)/100));
    readTemp(); // Read temp
    EZOReadRequest(__Temp); // Request new read
  }
}

void solutionMaker::EZOexportFinished(){
  if(EZOisEnable(EZO_PH) && EZOisEnable(EZO_PH) && __ExportEzo) __ExportEzo = false;
}

void solutionMaker::relayControl(){
  if(!digitalRead(__Relay[MIXER_RELAY]) && __RelayState[MIXER_RELAY]){
    __RelayState[MIXER_RELAY] = LOW;
    __RelayTime[MIXER_RELAY] = millis();
    Serial.println(F("warning,Solution Maker: mixing to finish"));
  }
  else if(!digitalRead(__Relay[MIXER_RELAY]) && !__RelayState[MIXER_RELAY] && millis()-__RelayTime[MIXER_RELAY]>RELAY_ACTION_TIME){
    digitalWrite(__Relay[MIXER_RELAY], !__RelayState[MIXER_RELAY]);
    Serial.println(F("info,Solution Finished"));
    Serial.println(F("Solution Finished"));
  }
}

void solutionMaker::dispense(long some_mg){
  if (__scaleMedition > some_mg*0.97){
    stopStepper(DispenserMotor);
    Serial.print(F("Finished with salt "));
    Serial.print(__limitS_salts);
    Serial.print(F(" dispensed mg "));
    Serial.println(some_mg);
    scale->tare();
    __dispensing = false;
    __moveCar = false;
  } else {
    //Serial.println(stepperS[CarMotor]->distanceToGo());
    if (stepperS[CarMotor]->distanceToGo()==0) {
      if (stepperS[DispenserMotor]->distanceToGo()==0){
        Serial.println(F("Dispensing salt"));
        moveStepper(3200, DispenserMotor);//100 arbitrary steps unitll loadcell reads correct value
      }
    }
  }
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
  if(pump<MAX_PUMPS_NUMBER){ // Check that the pump exists
    turnOnPump(abs(time1*1000), pump);
  }
  else{printAction(F("Cannot run calibration. Pump does not exist"), MAX_STEPPERS+MAX_PUMPS_NUMBER, 3);}
}

void solutionMaker::setCalibrationParameter(uint8_t param, uint8_t pump){
  if(pump<MAX_PUMPS_NUMBER){
    __Calibration[pump] = param;
    printAction(String(param) + " is the new calibration param (motors/pumps)",  MAX_STEPPERS + pump, 0);
    return;
  }
    printAction(F("Cannot set calibration Param. Actuator does not exist"), 7, 3);
}

void solutionMaker::setCalibrationParameter1(uint8_t param, uint8_t salt){ //salt or acid
  if(salt<SOLUTIONS){
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
  if(sensorType == EZO_PH){ return phMeter->isEnable(); }
  else if(sensorType == EZO_EC){ return ecMeter->isEnable(); }
  return false;
}

void solutionMaker::EZOcalibration(uint8_t sensorType, uint8_t act, float value){
  if(sensorType == EZO_PH && EZOisEnable(EZO_PH)){
    phMeter->calibration(act, value);
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
      phMeter->exportCal();
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
    phMeter->importCalibration(parameters);
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
      phMeter->sleep();
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
  phMeter->run(); // Handle phMeter functions
  ecMeter->run(); // Handle ecMeter functions
  EZOexportFinished(); // Check when EZO sensors finished export cal.
  checkLS();

  // For all the motors
  for(int i=0; i<MAX_STEPPERS; i++){
    stepperS[i]->run();
  }

  // For all the pumps
  for(int i=0; i<MAX_PUMPS_NUMBER; i++){
    if(millis()-__PumpTime[i]>__PumpOnTime[i] && __PumpOn[i]){
      turnOffPump(i);
    }
  }
  if (!__HOMED) {
    home();
  } else if (__HOMED){
    prepareSolution(500, 5.0, 2000, __limitS_salts);
    relayControl();
  }
}

void solutionMaker::prepareSolution(float liters, float ph, float ec, uint8_t salt){
  float mgPowder = balanceEC(__eC, ec, liters, salt);
  float mlAcid = balancePH(__pH, ph, liters, 0); // Solution 1 -> Acid 1
  if (!__limitS_State[cargoLS] && !__limitS_State[carEndLS]) {
    moveStepper(RevToSteps(-1), CargoMotor);
    __HOMEING[CargoMotor]=!__limitS_State[cargoLS];
  }
  else if (__limitS_State[cargoLS] && __HOMEING[CargoMotor] && !__limitS_State[carEndLS]) {
    stepperS[CargoMotor]->setCurrentPosition(0);
    stepperS[CargoMotor]->stop();
    __HOMEING[CargoMotor]=!__limitS_State[cargoLS];
  }
  if (!__limitS_State[carEndLS] && !__moveCar){ //No en el final
      moveCar(1000);
      __moveCar = true;
      Serial.println("moving car to dispensers");
  } else if(!__limitS_State[dispenserLS] && __moveCar && __dispensing) {
    //stepperS[CarMotor]->moveTo(stepperS[CarMotor]->currentPosition());
    __limitS_salts = __limitS_salts+round((StepsToRev(stepperS[CarMotor]->currentPosition())*8)/100);
    stepperS[CarMotor]->setCurrentPosition(0);
    stepperS[CarMotor]->stop();
    if(mgPowder>0){
      //Serial.println("dispensing");
      dispense(long(mgPowder/20));
    } else {
      __dispensing = false;
      __moveCar = false;
      Serial.println("Dispenser finished with 0 mg");
    }

  } else if(__limitS_State[dispenserLS] && __moveCar && !__dispensing) {
    __dispensing = true;
  } else if (__limitS_State[carEndLS]){
    __moveCar = false;
    if (carEndDetected == false){
      endTimer = millis();
      carEndDetected = true;
    }
    if (millis()-endTimer > 500){
      stepperS[CarMotor]->setCurrentPosition(0);
      stepperS[CarMotor]->stop();
    }

    if(__limitS_State[cargoLS] && stepperS[CargoMotor]->distanceToGo()==0 && millis()-endTimer > 1000){
      moveStepper(RevToSteps(4), CargoMotor);
      moveStepper(RevToSteps(0.3), ReleaseMotor);
    }
    if (!__limitS_State[cargoLS] && stepperS[CargoMotor]->distanceToGo()==0 && stepperS[ReleaseMotor]->distanceToGo()==0 & millis()-endTimer > 5000){
      Serial.print(F("debug,Solution Maker: mgPowder="));
      Serial.print(mgPowder);
      Serial.print(F(", mlAcid="));
      Serial.println(mlAcid);
      __RelayState[MIXER_RELAY] = HIGH;
      digitalWrite(__Relay[MIXER_RELAY], !__RelayState[MIXER_RELAY]);
      __HOMED=false;
    }

  }
  else if (!__limitS_State[carEndLS]){
    carEndDetected = false;
  }
  if(mlAcid>0) dispenseAcid(mlAcid, 0);
}
