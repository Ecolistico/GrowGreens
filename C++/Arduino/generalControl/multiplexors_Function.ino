/*****   Send state values to Multiplexors   *****/
/*
 * Falta:
 * Agregar tantos estados como haga falta
*/

void codification_Multiplexer(){
  bool ID1[8] = {EV1A1.getState(), EV1A2.getState(), EV1A3.getState(), EV1A4.getState(), 
                 EV1B1.getState(), EV1B2.getState(), EV1B3.getState(), EV1B4.getState()};
  bool ID2[8] = {EV2A1.getState(), EV2A2.getState(), EV2A3.getState(), EV2A4.getState(),
                 EV2B1.getState(), EV2B2.getState(), EV2B3.getState(), EV2B4.getState()};
  bool ID3[8] = {EV3A1.getState(), EV3A2.getState(), EV3A3.getState(), EV3A4.getState(), 
                 EV3B1.getState(), EV3B2.getState(), EV3B3.getState(), EV3B4.getState()};
  bool ID4[8] = {EV4A1.getState(), EV4A2.getState(), EV4A3.getState(), EV4A4.getState(),
                 EV4B1.getState(), EV4B2.getState(), EV4B3.getState(), EV4B4.getState()};
  bool ID5[8] = {L1S1.getState(), L1S2.getState(), L1S3.getState(), L1S4.getState(),
                 L2S1.getState(), L2S2.getState(), L2S3.getState(), L2S4.getState()};
  bool ID6[8] = {L3S1.getState(), L3S2.getState(), L3S3.getState(), L3S4.getState(),
                 L4S1.getState(), L4S2.getState(), L4S3.getState(), L4S4.getState()};
  bool ID7[8] = {IFan1.getState(), OFan1.getState(), VFan1.getState(), VHum1.getState(), 
                 IFan2.getState(), OFan2.getState(), VFan2.getState(), VHum2.getState()};
  bool ID8[8] = {IFan3.getState(), OFan3.getState(), VFan3.getState(), VHum3.getState(), 
                 IFan2.getState(), OFan4.getState(), VFan4.getState(), VHum4.getState()};               
  bool ID9[8] = {HVAC.getHR_State(), HVAC.getCC_State(), HVAC.getFR_State(), HVAC.getCVC_State(), 
                 HVAC.getCVH_State(), HVAC.getWarn_State(), HVAC.getWork_State(), !LOW}; // An extra relay  // getWarn_State && getWork_State are for LED´s
  bool ID10[8] = {Recirculation.getInValve(0), Recirculation.getInValve(1), Recirculation.getInValve(2), Recirculation.getInValve(3), 
                 Recirculation.getInValve(4), Recirculation.getOutValve(0), Recirculation.getOutValve(1), Recirculation.getOutValve(2)};
  bool ID11[8] = {Recirculation.getOutValve(3), Recirculation.getOutValve(4), Recirculation.getInPump(), Recirculation.getOutPump(), 
                 Compressor.getValveTank(), Compressor.getValveNut(), Compressor.getValveH2O(), Compressor.getState()};
  /*
   * bool ID12[8] = { WaterToKegsH2O.getState(), WaterToSolutionMaker.getState(), Compressor.getFreeValveNut(), Compressor.getFValveH2O(), 
   *                  IrrigationKegsH2O.getState(), IrrigationKegsNutrition.getState(), KegsH2OToRecirculation.getState(), KegsNutritionToRecirculation.getState() };
   * bool ID13[8] = { Recirculation.kegsH2O, Recirculation.kegsNutricion, Recirculation.solutionMaker }
   */
                  
  int value_Multiplexer1 = 0; int value_Multiplexer2 = 0; int value_Multiplexer3 = 0; int value_Multiplexer4 = 0;
  int value_Multiplexer5 = 0; int value_Multiplexer6 = 0; int value_Multiplexer7 = 0; int value_Multiplexer8 = 0;
  int value_Multiplexer9 = 0; int value_Multiplexer10 = 0; int value_Multiplexer11 = 0;
  
  for(int i=0; i<8; i++){
   value_Multiplexer1 |= ID1[i] << i; value_Multiplexer2 |= ID2[i] << i;
   value_Multiplexer3 |= ID3[i] << i; value_Multiplexer4 |= ID4[i] << i;
   value_Multiplexer5 |= ID5[i] << i; value_Multiplexer6 |= ID6[i] << i;
   value_Multiplexer7 |= ID7[i] << i; value_Multiplexer8 |= ID8[i] << i;
   value_Multiplexer9 |= ID9[i] << i; value_Multiplexer10 |= ID10[i] << i;
   value_Multiplexer11 |= ID11[i] << i;
  }
  
  digitalWrite(stcp, LOW);

  shiftOut(ds, shcp, MSBFIRST, value_Multiplexer1); shiftOut(ds, shcp, MSBFIRST, value_Multiplexer2);
  shiftOut(ds, shcp, MSBFIRST, value_Multiplexer3); shiftOut(ds, shcp, MSBFIRST, value_Multiplexer4);
  shiftOut(ds, shcp, MSBFIRST, value_Multiplexer5); shiftOut(ds, shcp, MSBFIRST, value_Multiplexer6);
  shiftOut(ds, shcp, MSBFIRST, value_Multiplexer7); shiftOut(ds, shcp, MSBFIRST, value_Multiplexer8);
  shiftOut(ds, shcp, MSBFIRST, value_Multiplexer9); shiftOut(ds, shcp, MSBFIRST, value_Multiplexer10);
  shiftOut(ds, shcp, MSBFIRST, value_Multiplexer11); 
  digitalWrite(stcp, HIGH); 

  if(value_Multiplexer1+value_Multiplexer2+value_Multiplexer3+value_Multiplexer4+value_Multiplexer5+value_Multiplexer6+value_Multiplexer7+value_Multiplexer8+value_Multiplexer9+value_Multiplexer10+value_Multiplexer11==255*11){
  Serial.println(F("Multiplexers were turned off"));
  }
}
