// Analizar si dichas funciones son requeridas
/*
void reorderAllSolenoids(bool a, bool b){
  Serial.print(F("Solenoid valves: Creating routine "));
  if(a){Serial.print("H");}else{Serial.print("L");}
  if(b){Serial.println("H");}else{Serial.println("L");}
  if(solenoidValve::ptr[0]->reOrderAll(a, b)){
  Serial.println(F("Solenoid valves: Routine created correctly"));
  }else{Serial.println(F("Solenoid valves: Routine cannot be configured correctly"));}
}

void reorderSolenoids_byFloor(byte fl){
  byte real_fl = fl-1;
  Serial.print(F("Solenoids valves: Sending ")); Serial.print(fl); Serial.println(F(" floor at the end of the routine"));
  if(solenoidValve::ptr[0]->reOrder_byFloor(real_fl)){
  Serial.println(F("Solenoid valves: Routine created correctly"));
  }else{Serial.println(F("Solenoid valves: Routine cannot be configured correctly"));}
}
*/
