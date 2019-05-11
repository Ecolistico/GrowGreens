void solenoid_setup(){
  // solenoidValve.begin(group, floor, pos, t_On, t_Off);
  EV1A1.begin(0, 0, 0, 3, 600); EV1A2.begin(0, 0, 1, 4, 600); EV1A3.begin(0, 0, 2, 5, 600); EV1A4.begin(0, 0, 3, 6, 600);
  EV1B1.begin(0, 0, 4, 3, 600); EV1B2.begin(0, 0, 5, 4, 600); EV1B3.begin(0, 0, 6, 5, 600); EV1B4.begin(0, 0, 7, 6, 600);
  EV2A1.begin(0, 1, 0, 3, 600); EV2A2.begin(0, 1, 1, 4, 600); EV2A3.begin(0, 1, 2, 5, 600); EV2A4.begin(0, 1, 3, 6, 600);
  EV2B1.begin(0, 1, 4, 3, 600); EV2B2.begin(0, 1, 5, 4, 600); EV2B3.begin(0, 1, 6, 5, 600); EV2B4.begin(0, 1, 7, 6, 600);
  EV3A1.begin(0, 2, 0, 3, 600); EV3A2.begin(0, 2, 1, 4, 600); EV3A3.begin(0, 2, 2, 5, 600); EV3A4.begin(0, 2, 3, 6, 600);
  EV3B1.begin(0, 2, 4, 3, 600); EV3B2.begin(0, 2, 5, 4, 600); EV3B3.begin(0, 2, 6, 5, 600); EV3B4.begin(0, 2, 7, 6, 600);
  EV4A1.begin(0, 3, 0, 3, 600); EV4A2.begin(0, 3, 1, 4, 600); EV4A3.begin(0, 3, 2, 5, 600); EV4A4.begin(0, 3, 3, 6, 600);
  EV4B1.begin(0, 3, 4, 3, 600); EV4B2.begin(0, 3, 5, 4, 600); EV4B3.begin(0, 3, 6, 5, 600); EV4B4.begin(0, 3, 7, 6, 600);
  
  Serial.println(F("Solenoid valves: Creating routine by default"));
  if(solenoidValve::ptr[0]->reOrderAll(HIGH, HIGH)){
  Serial.println(F("Routine created correctly"));
  }else{Serial.println(F("Routine cannot be configured correctly"));}
  
}

void sensors_setup(){
  Serial.println(F("Setting up sensors..."));
  pressureSensor.defaultFilter(); // Default Kalman Filter
  if(pressureSensor.setModel(1, -22.969, 0.2155) ){
    Serial.println(F("Sensors started correctly"));
     pressureSensor.printModel();
     pressureSensor.printFilter();
  }
  else{Serial.println(F("Sensors started incorrectly"));}
}
