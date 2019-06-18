int countNightFloors(){
  int night_floors = 0;
  if(day1.getState()==LOW){night_floors++;}
  if(day2.getState()==LOW){night_floors++;}
  if(day3.getState()==LOW){night_floors++;}
  if(day4.getState()==LOW){night_floors++;}

  return night_floors;
}

void pressureConditions(float pressureCompressor, float pressureNutrition, float pressureWater, float minPressure, float maxPressure){
  if(pressureCompressor<minPressure && pressureNutrition<minPressure && pressureWater<minPressure && pressureDecision==0){ // All pressures low
    solenoidValve::enableGroup(LOW); // Disable solenoidValves
    // Decision taken
    pressureDecision = Compressor.setMode(8); // Fill everything with pressure 
  }

  else if(pressureCompressor<minPressure && pressureNutrition<minPressure && pressureDecision==0){ // Compressor and Nutrition Low
    int night_floors = countNightFloors(); // Count how many floors are in night stage

    // Day floors recieved irrigation first, so if we have Nutrition Pressure Low we have to check if irrigating nutrition or not
    if(solenoidValve::ptr[0]->getActualNumber()<(MAX_IRRIGATION_REGIONS*(MAX_FLOOR-night_floors)) ){ // If it is not finished with nutrition
      solenoidValve::enableGroup(LOW); // Disable solenoidValves
    }
    // Decision taken
    pressureDecision = Compressor.setMode(6); // Fill just Nutrition/Compressor with pressure 
  }

  else if(pressureCompressor<minPressure && pressureWater<minPressure && pressureDecision==0){ // Compressor and Water Low
    int night_floors = countNightFloors(); // Count how many floors are in night stage
    
    // Day floors recieved irrigation first, so if we have Water Pressure Low we have to check if irrigating nutrition or water
    if(solenoidValve::ptr[0]->getActualNumber()>=(MAX_IRRIGATION_REGIONS*(MAX_FLOOR-night_floors)) ){
      solenoidValve::enableGroup(LOW); // Disable solenoidValves
    }
    // Decision taken
    pressureDecision = Compressor.setMode(7); // Fill just water. This is going to force that we have just compressor pressure Low 
  }
  
  else if(pressureNutrition<minPressure && pressureWater<minPressure && pressureDecision==0){ // Nutrition and Water Low
    solenoidValve::enableGroup(LOW); // Disable solenoidValves
    // Decision taken
    pressureDecision = Compressor.setMode(8); // Fill everything with pressure
  }

  else if(pressureCompressor<minPressure && pressureDecision==0){  // Compressor Low
    // Decision taken
    pressureDecision = Compressor.setMode(5); // Fill compressor with pressure
  }
  
  else if(pressureNutrition<minPressure && pressureDecision==0){ // Nutrition Low
    int night_floors = countNightFloors(); // Count how many floors are in night stage

    // Day floors recieved irrigation first, so if we have Nutrition Pressure Low we have to check if irrigating nutrition or not
    if(solenoidValve::ptr[0]->getActualNumber()<(MAX_IRRIGATION_REGIONS*(MAX_FLOOR-night_floors)) ){ // If it is not finished with nutrition
      solenoidValve::enableGroup(LOW); // Disable solenoidValves
    }
    // Decision taken
    pressureDecision = Compressor.setMode(6); // Fill just Nutrition/Compressor with pressure
  }
  
  else if(pressureWater<minPressure && pressureDecision==0){ // Water Low
    int night_floors = countNightFloors(); // Count how many floors are in night stage
    
    // Day floors recieved irrigation first, so if we have Water Pressure Low we have to check if irrigating nutrition or water
    if(solenoidValve::ptr[0]->getActualNumber()>=(MAX_IRRIGATION_REGIONS*(MAX_FLOOR-night_floors)) ){
      solenoidValve::enableGroup(LOW); // Disable solenoidValves
    }
    // Decision taken
    pressureDecision = Compressor.setMode(7); // Fill just water. This is going to force that we have just compressor pressure Low 
  }

  /*
   * Agregar condiciones especiales para poder cambiar de decisión
   */
   if(pressureDecision == 5 && pressureCompressor>=maxPressure){ // If the compressor get the maxPressure
    pressureDecision = Compressor.setMode(0); // Do nothing and prepare for a new decision
   }
   else if(pressureDecision == 6 && pressureCompressor>=maxPressure && pressureNutrition>=maxPressure){ // If the compressor and nutrition get the maxPressure
    pressureDecision = Compressor.setMode(0); // Do nothing and prepare for a new decision
   }
   else if(pressureDecision == 7 && pressureWater>=maxPressure ){ // If the water get the maxPressure
    pressureDecision = Compressor.setMode(0); // Do nothing and prepare for a new decision
   }
   else if(pressureDecision == 8 && pressureCompressor>=maxPressure && pressureNutrition>=maxPressure && pressureWater>=maxPressure){ // If all the pressure get the maxPressure
    pressureDecision = Compressor.setMode(0); // Do nothing and prepare for a new decision
   }
  
}
