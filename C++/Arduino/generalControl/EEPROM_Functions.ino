void saveParamters_EEPROM(){
  byte cycleTime = int(float(solenoidValve::ptr[0]->getCycleTime())*4/60000);
  int gap = MAX_ACTUATORS+2;
  Serial.println(F("EEPROM: Saving all paramters values ")); 
  
  for( int i = 0 ; i<solenoidValve::__TotalActuators; i++ ){
    byte eeprom_byte = 0;
    if(solenoidValve::ptr[i]->isEnable()){
      eeprom_byte = round(float(solenoidValve::ptr[i]->getTimeOn())/1000);
    }
    else{
      eeprom_byte = round(float(solenoidValve::ptr[i]->getTimeOn())/1000) +128 ;
    }
    Serial.print(i); Serial.print(F(": ")); Serial.println(eeprom_byte);
    EEPROM.write(i, eeprom_byte);
  }
 
  Serial.print(MAX_ACTUATORS+1); Serial.print(F(": ")); Serial.println(cycleTime);
  EEPROM.write(MAX_ACTUATORS+1, cycleTime);

  for( int i = gap; i<gap+LED::__TotalLeds; i++ ){
    byte eeprom_byte = 0;
    if(LED::ptr[i-gap]->isEnable()==LOW){eeprom_byte = 1;}
    Serial.print(i); Serial.print(F(": ")); Serial.println(eeprom_byte);
    EEPROM.write(i, eeprom_byte);
  }
  
}

void clean_EEPROM(){
  Serial.println(F("EEPROM: Deleting Memory..."));
  for(int i=0; i<EEPROM.length(); i++){
    EEPROM.write(i, 0);
  }
  Serial.println(F("EEPROM: Memory Deleted"));
}

void print_EEPROM(){
  Serial.println(F("EEPROM: Printing Memory Info"));
  for(int i=0; i<EEPROM.length(); i++){
    Serial.print(i); Serial.print(F(": ")); Serial.println(EEPROM.read(i));
  }
}
