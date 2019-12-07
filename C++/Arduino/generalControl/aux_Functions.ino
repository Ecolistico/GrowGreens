// Emergency
void emergencyStop() {
  if(digitalRead(emergencyUser)!=emergencyButton && !debounce) {
    buttonTime = millis();
    debounce = true;
  }

  if(millis()-buttonTime > debounceTime && debounce) {
    buttonTime = millis();
    debounce = false;
    bool but = digitalRead(emergencyUser);
    if(but != emergencyButton) {
      emergencyButton = but;
    }
  }
}

// Ask again for boot parameters after a while without response
void boot(){
  if(!bootParameters && millis()-bootTimer>20000){
    bootTimer=millis();
    Serial.println(F("?boot"));
  }
}

// Print Log from sensors when the information is relevant
void logSens() {
  if(millis()-logSensTime>10000){
    logSensTime = millis();
    if(Recirculation.getInPump() || Recirculation.getRSolValve() || Recirculation.getRH2OValve()){
      Serial.print(F("Level Recirculation Tank: ")); Serial.print(US0.getVolume()); Serial.println(F(" liters"));
      if(Recirculation.getInValve(0)){
        Serial.print(F("Level Solution1: ")); Serial.print(US1.getVolume()); Serial.println(F(" liters"));
      }
      if(Recirculation.getInValve(1)){
        Serial.print(F("Level Solution2: ")); Serial.print(US2.getVolume()); Serial.println(F(" liters"));
      }
      if(Recirculation.getInValve(2)){
        Serial.print(F("Level Solution3: ")); Serial.print(US3.getVolume()); Serial.println(F(" liters"));
      }
      if(Recirculation.getInValve(3)){
        Serial.print(F("Level Solution4: ")); Serial.print(US4.getVolume()); Serial.println(F(" liters"));
      }
      if(Recirculation.getInValve(4)){
        Serial.print(F("Level Water: ")); Serial.print(US5.getVolume()); Serial.println(F(" liters"));  
      }
    }
    if(Recirculation.getOutPump()){
      if(Recirculation.getOutValve(0)){
        Serial.print(F("Level Solution1: ")); Serial.print(US1.getVolume()); Serial.println(F(" liters"));
      }
      if(Recirculation.getOutValve(1)){
        Serial.print(F("Level Solution2: ")); Serial.print(US2.getVolume()); Serial.println(F(" liters"));
      }
      if(Recirculation.getOutValve(2)){
        Serial.print(F("Level Solution3: ")); Serial.print(US3.getVolume()); Serial.println(F(" liters"));
      }
      if(Recirculation.getOutValve(3)){
        Serial.print(F("Level Solution4: ")); Serial.print(US4.getVolume()); Serial.println(F(" liters"));
      }
      if(Recirculation.getOutValve(4)){
        Serial.print(F("Level Water: ")); Serial.print(US5.getVolume()); Serial.println(F(" liters"));  
      }
      if(Recirculation.getGoValve(SOLUTION_MAKER)){
        Serial.print(F("Level SMaker: ")); Serial.print(US6.getVolume()); Serial.println(F(" liters"));
      }
    }
    if(Recirculation.getSolPump() || Recirculation.getFSolValve()){
      Serial.print(F("Level SMaker: ")); Serial.print(US6.getVolume()); Serial.println(F(" liters"));
    }
    if(IrrigationKegsNutrition.getState() || 
    (Compressor.getValveNut() && Compressor.getState()) || 
    Compressor.getFreeValveNut() || 
    Recirculation.getRSolValve()){
      Serial.print(F("Pressure Kegs_nut: ")); Serial.print(pressureSensorNutrition.getValue()); Serial.println(F(" psi"));
    }
    if(IrrigationKegsH2O.getState() || (Compressor.getValveH2O() && Compressor.getState()) || Compressor.getFreeValveH2O()){
      Serial.print(F("Pressure Kegs_h20: ")); Serial.print(pressureSensorWater.getValue()); Serial.println(F(" psi"));
    }
    /*
    if(Recirculation.getRSolValve() || Recirculation.getRH2OValve()){
      Serial.print(F("debug,Air/Water Recirculation Sensor: "));
      if(checkWaterEvacuation.getState()==WATER_STATE){ Serial.println(F("WATER")); } 
      else { Serial.println(F("AIR")); }
    }
    if(IrrigationKegsH2O.getState() || IrrigationKegsNutrition.getState()){
      Serial.print(F("debug,Air/Water Irrigation Sensor: "));
      if(checkWaterEvacuation.getState()==WATER_STATE){ Serial.println(F("WATER")); } 
      else { Serial.println(F("AIR")); }
    }
    */
  }
}
