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

// Print Log from sensors when the information is relevant
void logSens() {
  if(millis()-logSensTime>5000){
    logSensTime = millis();
    if(Recirculation.getInPump()){
      Serial.print("warning,Level Recirculation Tank: "); Serial.print(US0.getVolume()); Serial.println(" liters");
      if(Recirculation.getOutValve(0)){
        Serial.print("warning,Level Water: "); Serial.print(US5.getVolume()); Serial.println(" liters");  
      }
      if(Recirculation.getOutValve(1)){
        Serial.print("warning,Level Solution1: "); Serial.print(US1.getVolume()); Serial.println(" liters");
      }
      if(Recirculation.getOutValve(2)){
        Serial.print("warning,Level Solution2: "); Serial.print(US2.getVolume()); Serial.println(" liters");
      }
      if(Recirculation.getOutValve(3)){
        Serial.print("warning,Level Solution3: "); Serial.print(US3.getVolume()); Serial.println(" liters");
      }
      if(Recirculation.getOutValve(4)){
        Serial.print("warning,Level Solution4: "); Serial.print(US4.getVolume()); Serial.println(" liters");
      }
    }
    if(Recirculation.getOutPump()){
      if(Recirculation.getOutValve(0)){
        Serial.print("warning,Level Water: "); Serial.print(US5.getVolume()); Serial.println(" liters");  
      }
      if(Recirculation.getOutValve(1)){
        Serial.print("warning,Level Solution1: "); Serial.print(US1.getVolume()); Serial.println(" liters");
      }
      if(Recirculation.getOutValve(2)){
        Serial.print("warning,Level Solution2: "); Serial.print(US2.getVolume()); Serial.println(" liters");
      }
      if(Recirculation.getOutValve(3)){
        Serial.print("warning,Level Solution3: "); Serial.print(US3.getVolume()); Serial.println(" liters");
      }
      if(Recirculation.getOutValve(4)){
        Serial.print("warning,Level Solution4: "); Serial.print(US4.getVolume()); Serial.println(" liters");
      }
      if(Recirculation.getGoValve(SOLUTION_MAKER)){
        Serial.print("warning,Level SMaker: "); Serial.print(US6.getVolume()); Serial.println(" liters");
      }
    }
    if(Recirculation.getSolPump()){
      Serial.print("warning,Level SMaker: "); Serial.print(US6.getVolume()); Serial.println(" liters");
    }
    if(IrrigationKegsNutrition.getState() || Compressor.getValveNut() || Compressor.getFreeValveNut()){
      Serial.print("warning,Pressure Kegs_nut: "); Serial.print(pressureSensorNutrition.getValue()); Serial.println(" psi");
    }
    if(Compressor.getValveTank()){
      Serial.print("warning,Pressure air_Tank: "); Serial.print(pressureSensorTank.getValue()); Serial.println(" psi");
    }
    if(IrrigationKegsH2O.getState() || Compressor.getValveH2O() || Compressor.getFreeValveH2O()){
      Serial.print("warning,Pressure Kegs_h20: "); Serial.print(pressureSensorWater.getValue()); Serial.println(" psi");
    }
    if(Recirculation.getRSolValve() || Recirculation.getRH2OValve()){
      Serial.print("warning,Air/Water Recirculation Sensor: ");
      if(checkWaterEvacuation.getState()==WATER_STATE){ Serial.println("WATER"); } 
      else { Serial.println("AIR"); }
    }
    if(IrrigationKegsH2O.getState() || IrrigationKegsNutrition.getState()){
      Serial.print("warning,Air/Water Irrigation Sensor: ");
      if(checkWaterEvacuation.getState()==WATER_STATE){ Serial.println("WATER"); } 
      else { Serial.println("AIR"); }
    }
  }
}
