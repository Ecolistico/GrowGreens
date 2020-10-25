#ifdef ARDUINO_SAM_DUE
  void writeEEPROM(int deviceaddress, unsigned int eeaddress, byte data) {
    Wire.beginTransmission(deviceaddress);
    Wire.write((int)(eeaddress >> 8));   // MSB
    Wire.write((int)(eeaddress & 0xFF)); // LSB
    Wire.write(data);
    Wire.endTransmission();
    delay(5);
  }
   
  byte readEEPROM(int deviceaddress, unsigned int eeaddress ) {
    byte rdata = 0xFF;
    Wire.beginTransmission(deviceaddress);
    Wire.write((int)(eeaddress >> 8));   // MSB
    Wire.write((int)(eeaddress & 0xFF)); // LSB
    Wire.endTransmission();
    Wire.requestFrom(deviceaddress,1);
    if (Wire.available()) rdata = Wire.read();
    return rdata;
  }
#endif

void clean_EEPROM(){
  Serial.println(F("warning,EEPROM: Deleting Memory..."));
  for(int i=0; i<8; i++){
    #ifdef ARDUINO_AVR_MEGA2560
      EEPROM.write(i, 0);
    #elif defined(ARDUINO_SAM_DUE)
      writeEEPROM(eeprom, i, 0);
    #endif
  }
  Serial.println(F("warning,EEPROM: Memory Deleted"));
}

void print_EEPROM(){
  Serial.println(F("EEPROM: Printing Memory Info"));
  for(int i=0; i<8; i++){
    Serial.print(i); Serial.print(F(": "));
    #ifdef ARDUINO_AVR_MEGA2560
      Serial.println(EEPROM.read(i));
    #elif defined(ARDUINO_SAM_DUE)
      Serial.println(readEEPROM(eeprom, i));
    #endif 
  }
}

void read_EEPROM(bool pr){
  #ifdef ARDUINO_AVR_MEGA2560
    byte mx1 = EEPROM.read(0);
    byte my1 = EEPROM.read(1);
    byte mx2 = EEPROM.read(2);
    byte my2 = EEPROM.read(3);
    byte mx3 = EEPROM.read(4);
    byte my3 = EEPROM.read(5);
    byte mx4 = EEPROM.read(6);
    byte my4 = EEPROM.read(7);
  #elif defined(ARDUINO_SAM_DUE)
    byte mx1 = readEEPROM(eeprom, 0);
    byte my1 = readEEPROM(eeprom, 1);
    byte mx2 = readEEPROM(eeprom, 2);
    byte my2 = readEEPROM(eeprom, 3);
    byte mx3 = readEEPROM(eeprom, 4);
    byte my3 = readEEPROM(eeprom, 5);
    byte mx4 = readEEPROM(eeprom, 6);
    byte my4 = readEEPROM(eeprom, 7);
  #endif 
  

  if(mx1!=MAX_X1){
    MAX_X1 = mx1;
    grower1.setMaxDistanceX(DEFAULT_MAX_X_DISTANCE_MM-MAX_X1);
  }
  if(my1!=MAX_Y1){
    MAX_Y1 = my1;
    grower1.setMaxDistanceY(DEFAULT_MAX_Y_DISTANCE_MM-MAX_Y1);
  }
  if(mx2!=MAX_X2){
    MAX_X2 = mx2;
    grower2.setMaxDistanceX(DEFAULT_MAX_X_DISTANCE_MM-MAX_X2);
  }
  if(my2!=MAX_Y2){
    MAX_Y2 = my2;
    grower2.setMaxDistanceY(DEFAULT_MAX_Y_DISTANCE_MM-MAX_Y2);
  }
  if(mx3!=MAX_X3){
    MAX_X3 = mx3;
    grower3.setMaxDistanceX(DEFAULT_MAX_X_DISTANCE_MM-MAX_X3);
  }
  if(my3!=MAX_Y3){
    MAX_Y3 = my3;
    grower3.setMaxDistanceY(DEFAULT_MAX_Y_DISTANCE_MM-MAX_Y3);
  }
  if(mx4!=MAX_X4){
    MAX_X4 = mx4;
    grower4.setMaxDistanceX(DEFAULT_MAX_X_DISTANCE_MM-MAX_X4);
  }
  if(my4!=MAX_Y4){
    MAX_Y4 = my4;
    grower4.setMaxDistanceY(DEFAULT_MAX_Y_DISTANCE_MM-MAX_Y4);
  }
  if(pr){Serial.println(F("EEPROM calibration parameters readed and charged"));}
}

void saveCalibrationParam(byte gr){
  long xdist, ydist;
  
  switch(gr){
    case 0:
      xdist = DEFAULT_MAX_X_DISTANCE_MM-grower1.getMaxDistanceX();
      ydist = DEFAULT_MAX_Y_DISTANCE_MM-grower1.getMaxDistanceY();
      Serial.println(F("Saving in EEPROM calibration parameters for Grower 1"));
      #ifdef ARDUINO_AVR_MEGA2560
        EEPROM.write(0, xdist);
        EEPROM.write(1, ydist);
      #elif defined(ARDUINO_SAM_DUE)
        writeEEPROM(eeprom, 0, xdist);
        writeEEPROM(eeprom, 1, ydist);
      #endif 
      break;
    case 1:
      xdist = DEFAULT_MAX_X_DISTANCE_MM-grower2.getMaxDistanceX();
      ydist = DEFAULT_MAX_Y_DISTANCE_MM-grower2.getMaxDistanceY();
      Serial.println(F("Saving in EEPROM calibration parameters for Grower 2"));
      #ifdef ARDUINO_AVR_MEGA2560
        EEPROM.write(2, xdist);
        EEPROM.write(3, ydist);
      #elif defined(ARDUINO_SAM_DUE)
        writeEEPROM(eeprom, 2, xdist);
        writeEEPROM(eeprom, 3, ydist);
      #endif 
      break;
    case 2:
      xdist = DEFAULT_MAX_X_DISTANCE_MM-grower3.getMaxDistanceX();
      ydist = DEFAULT_MAX_Y_DISTANCE_MM-grower3.getMaxDistanceY();
      Serial.println(F("Saving in EEPROM calibration parameters for Grower 3"));
      #ifdef ARDUINO_AVR_MEGA2560
        EEPROM.write(4, xdist);
        EEPROM.write(5, ydist);
      #elif defined(ARDUINO_SAM_DUE)
        writeEEPROM(eeprom, 4, xdist);
        writeEEPROM(eeprom, 5, ydist);
      #endif 
      break;
    case 3:
      xdist = DEFAULT_MAX_X_DISTANCE_MM-grower4.getMaxDistanceX();
      ydist = DEFAULT_MAX_Y_DISTANCE_MM-grower4.getMaxDistanceY();
      Serial.println(F("Saving in EEPROM calibration parameters for Grower 4"));
      #ifdef ARDUINO_AVR_MEGA2560
        EEPROM.write(6, xdist);
        EEPROM.write(7, ydist);
      #elif defined(ARDUINO_SAM_DUE)
        writeEEPROM(eeprom, 6, xdist);
        writeEEPROM(eeprom, 7, ydist);
      #endif 
      break;
    default:
      break;
  }
  read_EEPROM(LOW);
}

void check_CalibrationParameters(){
  if(DEFAULT_MAX_X_DISTANCE_MM-MAX_X1!=grower1.getMaxDistanceX() || DEFAULT_MAX_Y_DISTANCE_MM-MAX_Y1!=grower1.getMaxDistanceY()){
    saveCalibrationParam(0);
  }
  if(DEFAULT_MAX_X_DISTANCE_MM-MAX_X2!=grower2.getMaxDistanceX() || DEFAULT_MAX_Y_DISTANCE_MM-MAX_Y2!=grower2.getMaxDistanceY()){
    saveCalibrationParam(1);
  }
  if(DEFAULT_MAX_X_DISTANCE_MM-MAX_X3!=grower3.getMaxDistanceX() || DEFAULT_MAX_Y_DISTANCE_MM-MAX_Y3!=grower3.getMaxDistanceY()){
    saveCalibrationParam(2);
  }
  if(DEFAULT_MAX_X_DISTANCE_MM-MAX_X4!=grower4.getMaxDistanceX() || DEFAULT_MAX_Y_DISTANCE_MM-MAX_Y4!=grower4.getMaxDistanceY()){
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
