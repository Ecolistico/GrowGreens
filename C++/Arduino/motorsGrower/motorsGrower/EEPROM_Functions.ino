void clean_EEPROM(){
  Serial.println(F("EEPROM: Deleting Memory..."));
  for(int i=0; i<8; i++){
    EEPROM.write(i, 0);
  }
  Serial.println(F("EEPROM: Memory Deleted"));
}

void print_EEPROM(){
  Serial.println(F("EEPROM: Printing Memory Info"));
  for(int i=0; i<8; i++){
    Serial.print(i); Serial.print(F(": ")); Serial.println(EEPROM.read(i));
  }
}

void read_EEPROM(bool pr){
  byte mx1 = EEPROM.read(0);
  byte my1 = EEPROM.read(1);
  byte mx2 = EEPROM.read(2);
  byte my2 = EEPROM.read(3);
  byte mx3 = EEPROM.read(4);
  byte my3 = EEPROM.read(5);
  byte mx4 = EEPROM.read(6);
  byte my4 = EEPROM.read(7);

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
      EEPROM.write(0, xdist);
      EEPROM.write(1, ydist);
      break;
    case 1:
      xdist = DEFAULT_MAX_X_DISTANCE_MM-grower2.getMaxDistanceX();
      ydist = DEFAULT_MAX_Y_DISTANCE_MM-grower2.getMaxDistanceY();
      Serial.println(F("Saving in EEPROM calibration parameters for Grower 2"));
      EEPROM.write(2, xdist);
      EEPROM.write(3, ydist);
      break;
    case 2:
      xdist = DEFAULT_MAX_X_DISTANCE_MM-grower3.getMaxDistanceX();
      ydist = DEFAULT_MAX_Y_DISTANCE_MM-grower3.getMaxDistanceY();
      Serial.println(F("Saving in EEPROM calibration parameters for Grower 3"));
      EEPROM.write(4, xdist);
      EEPROM.write(5, ydist);
      break;
    case 3:
      xdist = DEFAULT_MAX_X_DISTANCE_MM-grower4.getMaxDistanceX();
      ydist = DEFAULT_MAX_Y_DISTANCE_MM-grower4.getMaxDistanceY();
      Serial.println(F("Saving in EEPROM calibration parameters for Grower 4"));
      EEPROM.write(6, xdist);
      EEPROM.write(7, ydist);
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
