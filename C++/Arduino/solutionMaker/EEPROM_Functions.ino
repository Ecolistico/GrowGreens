void clean_EEPROM(){
  Serial.println(F("EEPROM: Deleting Memory..."));
  for(int i=0; i<(MAX_SOLUTIONS_NUMBER+MAX_PUMPS_NUMBER)*2; i++){
    EEPROM.write(i, 0);
  }
  Serial.println(F("EEPROM: Memory Deleted"));
}

void print_EEPROM(){
  Serial.println(F("EEPROM: Printing Memory Info"));
  for(int i=0; i<(MAX_SOLUTIONS_NUMBER+MAX_PUMPS_NUMBER)*2; i++){
    Serial.print(i); Serial.print(F(": ")); Serial.println(EEPROM.read(i));
  }
}

void read_EEPROM(bool pr){
  byte cal1 = EEPROM.read(0); // gram/rev motor1
  byte cal2 = EEPROM.read(1); // gram/rev motor2
  byte cal3 = EEPROM.read(2); // gram/rev motor3
  byte cal4 = EEPROM.read(3); // gram/rev motor4
  byte cal5 = EEPROM.read(4); // ml/s pump1
  byte cal6 = EEPROM.read(5); // ml/s pump2
  
  if(cal1!=0){
    sMaker.setCalibrationParameter(cal1, 0);
  }
  if(cal2!=0){
    sMaker.setCalibrationParameter(cal2, 1);
  }
  if(cal3!=0){
    sMaker.setCalibrationParameter(cal3, 2);
  }
  if(cal4!=0){
    sMaker.setCalibrationParameter(cal4, 3);
  }
  if(cal5!=0){
    sMaker.setCalibrationParameter(cal5, 4);
  }
  if(cal6!=0){
    sMaker.setCalibrationParameter(cal6, 5);
  }
  if(pr){Serial.println(F("EEPROM calibration parameters readed and charged"));}
}

void write_EEPROM(unsigned int pos, byte val){
  EEPROM.write(pos, val);
  Serial.print(F("EEPROM: Parameter saved in pos: ")); Serial.print(pos); Serial.print(F("\tval= ")); Serial.println(val);
}
