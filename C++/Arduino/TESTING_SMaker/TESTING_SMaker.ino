#include "sMakerPeristaltics.h"
#include "TouchscreenObjects.h"
solutionMaker sMaker;
SMakerTouchScreen sMakerTFT;

// Serial comunication
String inputstring = "";
bool input_string_complete = false;

// Serial Communication
void serialEvent();

float mediciones=millis();
//unsigned long contadorMinutos = millis(); //variable temporal para imprimir control de tiempo
int minutos =0;
int data=1;

// Debug
int phPin = A7;    // select the input pin for the potentiometer
float phValue = 0;  // variable to store the value coming from the sensor
unsigned long timer = 0;
unsigned long eepromTimer = 0;
unsigned long TFTtimer = 0;
unsigned long PUMPtimer = 0;
unsigned long MIXERtimer = 0;
unsigned long PUMPmanual_max = 0;
unsigned long MIXERmanual_max = 0;
bool PUMPprev = false;
bool MIXERprev = false;
float x1 = 0;
float x2 = 0;
float pHX1= 0;
float pHX2 = 0;
float pHslope = 0;
float bSlope = 0;
uint8_t testAct = 0;
bool testingPump = false;

//***** Modify to enable or disable at start, false sets to disable when restart, true sets it to enable at start.
bool systemRunning = false;

float exponential_filter(float alpha, float t, float t_1)
  { if(isnan(alpha) || isnan(t) || isnan(t_1)){
      return t;
    }
    else{
      return (t*alpha+(1-alpha)*t_1);
    }
  }

float overSample(uint8_t readings = 10){
  float val = 0;
  for(int i = 0; i<readings; i++) val = val + analogRead(phPin);
  return float(val)/float(readings);
}

void setup() {
  Serial.begin(115200);
  pinMode(phPin, INPUT);
  Serial.println(F("Reading EEPROM"));
  sMaker.read_EEPROM(HIGH); // Charge calibration parameters
  sMaker.begin();
  sMakerTFT.begin();
  sMakerTFT.importParameters(
    sMaker.__pHtarget,
    sMaker.__ECtarget,
    sMaker.__ECmax,
    sMaker.__CalibrationLiquids,
    sMaker.__Calibration,
    sMaker.saltsOrder
    );
  sMaker.systemRunning = systemRunning;
  sMakerTFT.systemRunning = systemRunning;
  sMaker.readRequest();
  /*EEPROM.put(4000,sMaker.__pH);
  EEPROM.put(3996,sMaker.__eC);*/
  phValue = overSample();
  timer = millis();
  TFTtimer = millis();
  eepromTimer = millis();
  PUMPtimer = millis();
  PUMPmanual_max = 60*1000UL;
  MIXERmanual_max = 60*1000UL;
  testingPump = false;
}

void loop() {
  /*if (millis()-contadorMinutos>60000){
    contadorMinutos = millis();
    minutos++;
    Serial.print("Minuto ");
    Serial.println(minutos);
  }*/
  sMaker.run();
  //if(!sMaker.__HOMED)SMTFT.changeScreen(ECOLISTICO,1000);
    
  /*if (sMakerTFT.saveEEPROM == true){
    sMaker.saveTFTdata();
    sMakerTFT.saveEEPROM = false;
  }*/
  if (millis()-eepromTimer >1000){
    for (int i=0;i< sizeof(sMaker.__EEPROM)/sizeof(sMaker.__EEPROM[0]);i++){
      sMakerTFT.printEEPROM(sMaker.__EEPROM[i],i);
    }
    eepromTimer = millis();
  }
  if (sMakerTFT.CleanEEPROM == true){
    sMaker.clean_EEPROM();
    sMakerTFT.CleanEEPROM = false;
  }
  if (sMakerTFT.DefaultEEPROM == true){
    sMaker.default_EEPROM();
    sMaker.read_EEPROM(HIGH);
    sMakerTFT.DefaultEEPROM = false;
    sMakerTFT.importParameters(
      sMaker.__pHtarget,
      sMaker.__ECtarget,
      sMaker.__ECmax,
      sMaker.__CalibrationLiquidsDefMed,
      sMaker.__CalibrationDEF,
      sMaker.saltsOrder
    );
  }
  if (sMakerTFT.SaveEEPROM == true){
    sMaker.__pHtarget = sMakerTFT.__pHtarget;
    sMaker.__ECtarget = sMakerTFT.__ECtarget;
    sMaker.__ECmax = sMakerTFT.__ECmax;
    for (int i=0; i<SOLIDSALTS; i++){
      sMaker.__CalibrationLiquids[i] = sMakerTFT.__SaltmgL[i];
      sMaker.__Calibration[i]=sMakerTFT.__Sflow[i];
      sMaker.__EEPROMstruct.__Calibration[i] = sMakerTFT.__Sflow[i];
      sMaker.__EEPROMstruct.__CalibrationLiquids[i] = sMakerTFT.__SaltmgL[i];
      sMaker.__EEPROMstruct.__saltOrders[i] = sMakerTFT.saltsOrder[i];
    }
    sMaker.__EEPROMstruct.__ECtarget = sMakerTFT.__ECtarget;
    sMaker.__EEPROMstruct.__ECmax = sMakerTFT.__ECmax;
    sMaker.save_EEPROM();
    sMaker.read_EEPROM(HIGH);
    sMakerTFT.SaveEEPROM = false;
  }
  if (sMakerTFT.tareSCALE == true){
    sMaker.scale->tare();
    sMakerTFT.tareSCALE = false;
  }
  if (sMakerTFT.calibSCALE == true){
    sMaker.scale->set_scale(1);
    float  readForFactor = sMaker.scale->get_units(10);
    float newScaleFactor = readForFactor/sMakerTFT._CalibWeight;
    sMaker.__loadCellCalibration = newScaleFactor;
    sMakerTFT.calibSCALE = false;
    sMaker.scale->set_scale(newScaleFactor);
  }
  if (sMakerTFT.calibPUMP == true){
    testingPump=true;
    testAct = sMakerTFT.pumpToCalib;
    for (uint8_t i = sMakerTFT.pumpToCalib; i < SOLUTIONS; i++){
       sMaker.dispensePeristaltic(100, i);
    }
    Serial.println(F("Dispensing 100 ml for calibration"));
    sMakerTFT.calibPUMP = false;
  }
  
  if (sMakerTFT.SaveSCALE == true){
    sMaker.__EEPROMstruct.loadCellCal = sMaker.__loadCellCalibration;
    sMaker.myMem->put(0,sMaker.__EEPROMstruct);
    sMakerTFT.SaveSCALE = false;
  }
  if (sMakerTFT.__pumpState){
    if (!PUMPprev){
      PUMPtimer = millis();
      PUMPprev = true;
    }
    if (millis()-PUMPtimer<sMakerTFT._pumpMinutes*PUMPmanual_max){
      digitalWrite(sMaker.__Relay[1], HIGH);
    } else {
      digitalWrite(sMaker.__Relay[1], LOW);
      sMakerTFT.__pumpStop = false;
      PUMPprev = false;
      sMakerTFT.__pumpState = false;
      sMakerTFT.__pumpSTATEtxt = "ON";
    }
  }
  if (sMakerTFT.__pumpStop){
    digitalWrite(sMaker.__Relay[1], LOW);    
    sMakerTFT.__pumpStop = false;
    PUMPprev = false;
  }
  if (sMakerTFT.__mixerState){
    if (!MIXERprev){
      MIXERtimer = millis();
      MIXERprev = true;
    }
    if (millis()-MIXERtimer<sMakerTFT._mixerMinutes*MIXERmanual_max){
      digitalWrite(sMaker.__Relay[0], HIGH);
    } else {
      digitalWrite(sMaker.__Relay[0], LOW);
      sMakerTFT.__mixerStop = false;
      MIXERprev = false;
      sMakerTFT.__mixerState = false;
      sMakerTFT.__mixerSTATEtxt = "ON";
    }
  }
  if (sMakerTFT.__mixerStop){
    digitalWrite(sMaker.__Relay[0], LOW);
    sMakerTFT.__mixerStop = false;
    MIXERprev = false;
  }
  if (sMaker.systemRunning != sMakerTFT.systemRunning){
    sMaker.systemRunning = sMakerTFT.systemRunning;
    Serial.print("System changed state and is now ");
    if(sMaker.systemRunning)Serial.println("RUNNING");
    else Serial.println("PAUSED");
  }
  if (sMakerTFT.show == true && sMaker.stopShow == false){
    sMaker.show = true;
  }
  if (sMaker.stopShow == true){
    sMakerTFT.show = false;
    sMaker.stopShow = false;
  }
  
  if (sMaker.__Hum > 50.00){
    digitalWrite(sMaker.__Relay[2], HIGH);
  }
  if (sMaker.__Hum < 40.00){
    digitalWrite(sMaker.__Relay[2], LOW);
  }
  if (sMakerTFT._ECEZOsteps == 1){
    sMaker.EZOcalibration(EZO_EC, 0, 12880);
    sMakerTFT._ECEZOsteps = 0;
  }
  if (sMakerTFT._ECEZOsteps == 2){
    sMaker.EZOcalibration(EZO_EC, 1, 12880);
    sMakerTFT._ECEZOsteps = 0;
  }
  if (sMakerTFT._pHSlopeSteps == 1){
    pHX1 = phValue;
    sMakerTFT._pHSlopeSteps = 0;
  }
  if (sMakerTFT._pHSlopeSteps == 2){
    pHX2 = phValue;
    sMakerTFT._pHSlopeSteps = 3;
  }
  if (sMakerTFT._pHSlopeSteps == 3){
    pHslope = 3/(pHX2-pHX1);
    bSlope = 4-pHX1*pHslope;
    Serial.print("x1 slope ");
    Serial.println(pHX1);
    Serial.print("x2 slope ");
    Serial.println(pHX2);
    Serial.print("pH slope ");
    Serial.println(pHslope);
    Serial.print("pH b ");
    Serial.println(bSlope);
    sMakerTFT._pHSlopeSteps = 0;
  }
  if(millis()-timer > 500){
    timer = millis();
    if (exponential_filter(0.01, overSample(), phValue)>1.001*phValue ||exponential_filter(0.01, overSample(), phValue)<0.999*phValue) phValue = exponential_filter(0.05, overSample(), phValue);
    //x2 = -1.48 + 0.0199*phValue + -0.00000673*phValue*phValue;
    //x1 = 0.01418269231*phValue - 0.338;
    x1 = .0179*phValue-2.54;
    //Serial.print(F("pH sensor: pH read = "));
    //Serial.println(x2, 4);  
    //Serial.println(x1, 4); 
    //Serial.println(); 
    //Serial.println(phValue, 4); 
  }

  if(millis()-TFTtimer>50 && sMaker.__dispensing == false){
    sMakerTFT.run();
    sMakerTFT.importData(
      sMaker.__Temp,
      sMaker.__Hum,
      //sMaker.__pH,
      x1, //agregar valor de ph desde sensor analogico a librería, remover EZO PH
      //phValue,
      sMaker.__eC, 
      sMaker.__mgPowder,
      sMaker.__scaleMedition
    );
    TFTtimer = millis();
  }
  if(millis()-TFTtimer>500 && sMaker.__dispensing == true){
    sMakerTFT.run();
    sMakerTFT.importData(
      sMaker.__Temp,
      sMaker.__Hum,
      //sMaker.__pH,
      x1, //agregar valor de ph desde sensor analogico a librería, remover EZO PH
      //phValue,
      sMaker.__eC, 
      sMaker.__mgPowder,
      sMaker.__scaleMedition
    );
    TFTtimer = millis();
  }
  if (testingPump){
    if (sMaker.stepperS[testAct]->distanceToGo() == 0){
      for (uint8_t i = 0; i<8; i++){
        sMaker.stopStepper(i);
      }
      testingPump = false;
    }
  }
  
  /*if (millis()-mediciones>1800000 && data<337){
    sMaker.readRequest();
    EEPROM.put(4000-8*data,sMaker.__pH);
    EEPROM.put(3996-8*data,sMaker.__eC);
    data++;
  }else{data=0;}*/
     
}
