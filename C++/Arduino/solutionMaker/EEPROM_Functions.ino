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
  byte cal1 = EEPROM.read(0); // mg/rev motor1
  byte cal2 = EEPROM.read(1); // mg/rev motor2
  byte cal3 = EEPROM.read(2); // mg/rev motor3
  byte cal4 = EEPROM.read(3); // mg/rev motor4
  byte cal5 = EEPROM.read(4); // ml/s pump1
  byte cal6 = EEPROM.read(5); // ml/s pump2

  byte cal7 = EEPROM.read(6); // mg/l sol1
  byte cal8 = EEPROM.read(7); // mg/l sol2
  byte cal9 = EEPROM.read(8); // mg/l sol3
  byte cal10 = EEPROM.read(9); // mg/l sol4
  byte cal11 = EEPROM.read(10); // ph pump1
  byte cal12 = EEPROM.read(11); // ph pump2
  
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

  if(cal7!=0){
    sMaker.setCalibrationParameter1(cal7, 0);
  }
  if(cal8!=0){
    sMaker.setCalibrationParameter1(cal8, 1);
  }
  if(cal9!=0){
    sMaker.setCalibrationParameter1(cal9, 2);
  }
  if(cal10!=0){
    sMaker.setCalibrationParameter1(cal10,3);
  }
  if(cal11!=0){
    sMaker.setCalibrationParameter1(cal11, 4);
  }
  if(cal12!=0){
    sMaker.setCalibrationParameter1(cal12, 5);
  }
  
  if(pr){Serial.println(F("EEPROM calibration parameters readed and charged"));}
}

void write_EEPROM(unsigned int pos, byte val){
  EEPROM.write(pos, val);
  Serial.print(F("EEPROM: Parameter saved in pos: ")); Serial.print(pos); Serial.print(F("\tval= ")); Serial.println(val);
}
