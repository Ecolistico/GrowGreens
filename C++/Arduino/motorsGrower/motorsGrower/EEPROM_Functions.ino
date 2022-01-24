void begin_EEPROM(){
  Wire.begin();
  if (myEEPROM.begin() == false)
  {
    Serial.println("No memory detected. Freezing.");
    while (1);
  }
  Serial.println("Memory detected!");
}

void write_EEPROM(int pos, byte val){
  byte auxVal = myEEPROM.read(pos);  
  if(val!=auxVal){
    Serial.print(F("debug,Saving in EEPROM on position("));
    Serial.print(pos);
    Serial.print(F(") Value="));
    Serial.println(val);
    myEEPROM.write(pos, val);
  }
  else{ 
    Serial.print(F("warning,EEPROM on position("));
    Serial.print(pos);
    Serial.print(F(") Value="));
    Serial.print(val);
    Serial.println(F(" already saved"));
  }   
}

void clean_EEPROM(){
  Serial.println(F("warning,EEPROM: Deleting Memory..."));
  for(int i=0; i<1000; i++){
    write_EEPROM(i, 0);
  }
  Serial.println(F("warning,EEPROM: Memory Deleted"));
}

void print_EEPROM(){
  Serial.println(F("info,EEPROM: Printing Memory Info"));
  for(int i=0; i<1000; i++){
    Serial.print(i); Serial.print(F(": ")); Serial.println(myEEPROM.read(i));
  }
}

void read_EEPROM(bool pr){
  unsigned long mx1 = 0;
  unsigned long my1 = 0;
  unsigned long mx2 = 0;
  unsigned long my2 = 0;
  unsigned long mx3 = 0;
  unsigned long my3 = 0;
  unsigned long mx4 = 0;
  unsigned long my4 = 0;
  
  myEEPROM.get(10,  mx1);
  myEEPROM.get(20,  my1);
  myEEPROM.get(110, mx2);
  myEEPROM.get(120, my2);
  myEEPROM.get(210, mx3);
  myEEPROM.get(220, my3);
  myEEPROM.get(310, mx4);
  myEEPROM.get(320, my4);
  
  if(mx1!=MAX_X1){
    MAX_X1 = mx1;
    grower1.setMaxDistanceX(MAX_X1);
  }
  if(my1!=MAX_Y1){
    MAX_Y1 = my1;
    grower1.setMaxDistanceY(MAX_Y1);
  }
  if(mx2!=MAX_X2){
    MAX_X2 = mx2;
    grower2.setMaxDistanceX(MAX_X2);
  }
  if(my2!=MAX_Y2){
    MAX_Y2 = my2;
    grower2.setMaxDistanceY(MAX_Y2);
  }
  if(mx3!=MAX_X3){
    MAX_X3 = mx3;
    grower3.setMaxDistanceX(MAX_X3);
  }
  if(my3!=MAX_Y3){
    MAX_Y3 = my3;
    grower3.setMaxDistanceY(MAX_Y3);
  }
  if(mx4!=MAX_X4){
    MAX_X4 = mx4;
    grower4.setMaxDistanceX(MAX_X4);
  }
  if(my4!=MAX_Y4){
    MAX_Y4 = my4;
    grower4.setMaxDistanceY(MAX_Y4);
  }
  if(pr){Serial.println(F("EEPROM calibration parameters readed and charged"));}
}

void getConfig() {
  byte steps1 = 0;
  byte microst1 = 0;
  byte pulley1 = 0;
  byte xt1 = 0;
  byte yt1 = 0;
  
  byte steps2 = 0;
  byte microst2 = 0;
  byte pulley2 = 0;
  byte xt2 = 0;
  byte yt2 = 0;
  
  byte steps3 = 0;
  byte microst3 = 0;
  byte pulley3 = 0;
  byte xt3 = 0;
  byte yt3 = 0;

  byte steps4 = 0;
  byte microst4 = 0;
  byte pulley4 = 0;
  byte xt4 = 0;
  byte yt4 = 0;

  myEEPROM.get(30,  steps1);
  myEEPROM.get(32,  microst1);
  myEEPROM.get(34,  pulley1);
  myEEPROM.get(36,  xt1);
  myEEPROM.get(38,  yt1);
  if(steps1!=0) stepsPerRev1 = steps1;
  else stepsPerRev1 = MOTOR_STEP_PER_REV;
  if(microst1!=0) microstep1 = microst1;
  else microstep1 = DEFAULT_MICROSTEP;
  if(pulley1!=0) pulleyTeeth1 = pulley1;
  else pulleyTeeth1 = DEFAULT_PULLEY_TEETH;
  if(xt1!=0) xTooth1 = xt1;
  else xTooth1 = DEFAULT_X_MM_TOOTH;
  if(yt1!=0) yTooth1 = yt1;
  else yTooth1 = DEFAULT_Y_MM_TOOTH;
  
  
  myEEPROM.get(130,  steps2);
  myEEPROM.get(132,  microst2);
  myEEPROM.get(134,  pulley2);
  myEEPROM.get(136,  xt2);
  myEEPROM.get(138,  yt2);
  if(steps2!=0) stepsPerRev2 = steps2;
  else stepsPerRev2 = MOTOR_STEP_PER_REV;
  if(microst2!=0) microstep2 = microst2;
  else microstep2 = DEFAULT_MICROSTEP;
  if(pulley2!=0) pulleyTeeth2 = pulley2;
  else pulleyTeeth2 = DEFAULT_PULLEY_TEETH;
  if(xt2!=0) xTooth2 = xt2;
  else xTooth2 = DEFAULT_X_MM_TOOTH;
  if(yt2!=0) yTooth2 = yt2;
  else yTooth2 = DEFAULT_Y_MM_TOOTH;
  
  
  myEEPROM.get(230,  steps3);
  myEEPROM.get(232,  microst3);
  myEEPROM.get(234,  pulley3);
  myEEPROM.get(236,  xt3);
  myEEPROM.get(238,  yt3);
  if(steps3!=0) stepsPerRev3 = steps3;
  else stepsPerRev3 = MOTOR_STEP_PER_REV;
  if(microst3!=0) microstep3 = microst3;
  else microstep3 = DEFAULT_MICROSTEP;
  if(pulley3!=0) pulleyTeeth3 = pulley3;
  else pulleyTeeth3 = DEFAULT_PULLEY_TEETH;
  if(xt3!=0) xTooth3 = xt3;
  else xTooth3 = DEFAULT_X_MM_TOOTH;
  if(yt3!=0) yTooth3 = yt3;
  else yTooth3 = DEFAULT_Y_MM_TOOTH;
  
  myEEPROM.get(330,  steps4);
  myEEPROM.get(332,  microst4);
  myEEPROM.get(334,  pulley4);
  myEEPROM.get(336,  xt4);
  myEEPROM.get(338,  yt4);
  if(steps4!=0) stepsPerRev4 = steps4;
  else stepsPerRev4 = MOTOR_STEP_PER_REV;
  if(microst4!=0) microstep4 = microst4;
  else microstep4 = DEFAULT_MICROSTEP;
  if(pulley4!=0) pulleyTeeth4 = pulley4;
  else pulleyTeeth4 = DEFAULT_PULLEY_TEETH;
  if(xt4!=0) xTooth4 = xt4;
  else xTooth4 = DEFAULT_X_MM_TOOTH;
  if(yt4!=0) yTooth4 = yt4;
  else yTooth4 = DEFAULT_Y_MM_TOOTH;

  Serial.println(F("EEPROM configuration parameters readed and charged"));
}


void saveCalibrationParam(byte gr){
  unsigned long xdist, ydist;
  
  switch(gr){
    case 0:
      xdist = grower1.getMaxDistanceX();
      ydist = grower1.getMaxDistanceY();
      Serial.println(F("Saving in EEPROM calibration parameters for Grower 1"));
      myEEPROM.put(10, xdist);
      myEEPROM.put(20, ydist);
      break;
    case 1:
      xdist = grower2.getMaxDistanceX();
      ydist = grower2.getMaxDistanceY();
      Serial.println(F("Saving in EEPROM calibration parameters for Grower 2"));
      myEEPROM.put(110, xdist);
      myEEPROM.put(120, ydist);
      break;
    case 2:
      xdist = grower3.getMaxDistanceX();
      ydist = grower3.getMaxDistanceY();
      Serial.println(F("Saving in EEPROM calibration parameters for Grower 3"));
      myEEPROM.put(210, xdist);
      myEEPROM.put(220, ydist);
      break;
    case 3:
      xdist = grower4.getMaxDistanceX();
      ydist = grower4.getMaxDistanceY();
      Serial.println(F("Saving in EEPROM calibration parameters for Grower 4"));
      myEEPROM.put(310, xdist);
      myEEPROM.put(320, ydist);
      break;
    default:
      break;
  }
  read_EEPROM(LOW);
}

void saveConfig(byte gr, byte stepsPerRev, byte microStep, byte pulleyTeeth, byte xTooth, byte yTooth){
  switch(gr){
    case 0:
      Serial.println(F("Saving in EEPROM configuration parameters for Grower 1"));
      myEEPROM.put(30, stepsPerRev);
      myEEPROM.put(32, microStep);
      myEEPROM.put(34, pulleyTeeth);
      myEEPROM.put(36, xTooth);
      myEEPROM.put(38, yTooth);
      break;
    case 1:
      Serial.println(F("Saving in EEPROM configuration parameters for Grower 2"));
      myEEPROM.put(130, stepsPerRev);
      myEEPROM.put(132, microStep);
      myEEPROM.put(134, pulleyTeeth);
      myEEPROM.put(136, xTooth);
      myEEPROM.put(138, yTooth);
      break;
    case 2:
      Serial.println(F("Saving in EEPROM configuration parameters for Grower 3"));
      myEEPROM.put(230, stepsPerRev);
      myEEPROM.put(232, microStep);
      myEEPROM.put(234, pulleyTeeth);
      myEEPROM.put(236, xTooth);
      myEEPROM.put(238, yTooth);
      break;
    case 3:
      Serial.println(F("Saving in EEPROM configuration parameters for Grower 4"));
      myEEPROM.put(330, stepsPerRev);
      myEEPROM.put(332, microStep);
      myEEPROM.put(334, pulleyTeeth);
      myEEPROM.put(336, xTooth);
      myEEPROM.put(338, yTooth);
      break;
    default:
      break;
  }
}

void check_CalibrationParameters(){
  if(MAX_X1!=grower1.getMaxDistanceX() || MAX_Y1!=grower1.getMaxDistanceY()){
    saveCalibrationParam(0);
  }
  if(MAX_X2!=grower2.getMaxDistanceX() || MAX_Y2!=grower2.getMaxDistanceY()){
    saveCalibrationParam(1);
  }
  if(MAX_X3!=grower3.getMaxDistanceX() || MAX_Y3!=grower3.getMaxDistanceY()){
    saveCalibrationParam(2);
  }
  if(MAX_X4!=grower4.getMaxDistanceX() || MAX_Y4!=grower4.getMaxDistanceY()){
    saveCalibrationParam(3);
  }
}

void update_CalibrationParameters(){
  if(millis()-EEPROM_timer>5000){
    EEPROM_timer = millis();
    if(!grower1.isInCalibration() && !grower2.isInCalibration() && !grower3.isInCalibration() && !grower4.isInCalibration()){
      check_CalibrationParameters();  
    }
  }
}
