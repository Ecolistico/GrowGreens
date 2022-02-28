void memorySetup(){
  // Start filter sensor config
  memory.begin("filter");
  memory.begin("alpha");
  memory.begin("k_noise");
  memory.begin("update");
  // Start led config
  memory.begin("f1_set");
  memory.begin("f2_set");
  memory.begin("f3_set");
  memory.begin("f4_set");
  memory.begin("f5_set");
  memory.begin("f6_set");
  memory.begin("f7_set");
  memory.begin("f8_set");

  // Get parameters from the memory
  for(int i=0; i<12; i++){
    memoryGet(i);
  }

  // Default parameters
  if(exp_alpha == 0){
    exp_alpha = 40;
    memorySave(1);
  }
  if(kalman_noise == 0){
    kalman_noise = 30;
    memorySave(2);
  }
  if(update_constant == 0){
    update_constant = 5;
    memorySave(3);
  }
  // Possibly we need security conditions if the memory parameters for led´s are wrong
  Serial.println(F("Filter Settings"));
  Serial.print(F("Filter = ")); Serial.println(filter);
  Serial.print(F("Alpha = ")); Serial.println(exp_alpha);
  Serial.print(F("Kalman Noise = ")); Serial.println(kalman_noise);
  Serial.print(F("Update = ")); Serial.println(update_constant);
  Serial.println(); 

  Serial.println(F("LED´s Settings"));
  Serial.print(F("Floor 1. Turn on at ")); Serial.print(f1.hour_begin); Serial.print(F(":")); Serial.print(f1.minute_begin); Serial.print(F("\tTurn off at ")); Serial.print(f1.hour_end); Serial.print(F(":")); Serial.println(f1.minute_end);
  Serial.print(F("Floor 2. Turn on at ")); Serial.print(f2.hour_begin); Serial.print(F(":")); Serial.print(f2.minute_begin); Serial.print(F("\tTurn off at ")); Serial.print(f2.hour_end); Serial.print(F(":")); Serial.println(f2.minute_end);
  Serial.print(F("Floor 3. Turn on at ")); Serial.print(f3.hour_begin); Serial.print(F(":")); Serial.print(f3.minute_begin); Serial.print(F("\tTurn off at ")); Serial.print(f3.hour_end); Serial.print(F(":")); Serial.println(f3.minute_end);
  Serial.print(F("Floor 4. Turn on at ")); Serial.print(f4.hour_begin); Serial.print(F(":")); Serial.print(f4.minute_begin); Serial.print(F("\tTurn off at ")); Serial.print(f4.hour_end); Serial.print(F(":")); Serial.println(f4.minute_end);
  Serial.print(F("Floor 5. Turn on at ")); Serial.print(f5.hour_begin); Serial.print(F(":")); Serial.print(f5.minute_begin); Serial.print(F("\tTurn off at ")); Serial.print(f5.hour_end); Serial.print(F(":")); Serial.println(f5.minute_end);
  Serial.print(F("Floor 6. Turn on at ")); Serial.print(f6.hour_begin); Serial.print(F(":")); Serial.print(f6.minute_begin); Serial.print(F("\tTurn off at ")); Serial.print(f6.hour_end); Serial.print(F(":")); Serial.println(f6.minute_end);
  Serial.print(F("Floor 7. Turn on at ")); Serial.print(f7.hour_begin); Serial.print(F(":")); Serial.print(f7.minute_begin); Serial.print(F("\tTurn off at ")); Serial.print(f7.hour_end); Serial.print(F(":")); Serial.println(f7.minute_end);
  Serial.print(F("Floor 8. Turn on at ")); Serial.print(f8.hour_begin); Serial.print(F(":")); Serial.print(f8.minute_begin); Serial.print(F("\tTurn off at ")); Serial.print(f8.hour_end); Serial.print(F(":")); Serial.println(f8.minute_end);
  Serial.println(); 

}

void memorySave(uint8_t par){
  switch(par){
    case 0:
      memory.putBytes("filter", &filter, sizeof(filter));
      break;
    case 1:
      memory.putBytes("alpha", &exp_alpha, sizeof(exp_alpha));
      break;
    case 2:
      memory.putBytes("k_noise", &kalman_noise, sizeof(kalman_noise));
      break;
    case 3:
      memory.putBytes("update", &update_constant, sizeof(update_constant));
      break;
    case 4:
      memory.putBytes("f1_set", &f1, sizeof(f1));
      break;
    case 5:
      memory.putBytes("f2_set", &f2, sizeof(f2));
      break;
    case 6:
      memory.putBytes("f3_set", &f3, sizeof(f3));
      break;
    case 7:
      memory.putBytes("f4_set", &f4, sizeof(f4));
      break;
    case 8:
      memory.putBytes("f5_set", &f5, sizeof(f5));
      break;
    case 9:
      memory.putBytes("f6_set", &f6, sizeof(f6));
      break;
    case 10:
      memory.putBytes("f7_set", &f7, sizeof(f7));
      break;
    case 11:
      memory.putBytes("f8_set", &f8, sizeof(f8));
      break;
    default:
      Serial.println(F("memorySave() error. Parameter not found"));
      break;
  }
}

void memoryGet(uint8_t par){
  switch(par){
    case 0:
      memory.getBytes("filter", &filter, sizeof(filter));
      break;
    case 1:
      memory.getBytes("alpha", &exp_alpha, sizeof(exp_alpha));
      break;
    case 2:
      memory.getBytes("k_noise", &kalman_noise, sizeof(kalman_noise));
      break;
    case 3:
      memory.getBytes("update", &update_constant, sizeof(update_constant));
      break;
    case 4:
      memory.getBytes("f1_set", &f1, sizeof(f1));
      break;
    case 5:
      memory.getBytes("f2_set", &f2, sizeof(f2));
      break;
    case 6:
      memory.getBytes("f3_set", &f3, sizeof(f3));
      break;
    case 7:
      memory.getBytes("f4_set", &f4, sizeof(f4));
      break;
    case 8:
      memory.getBytes("f5_set", &f5, sizeof(f5));
      break;
    case 9:
      memory.getBytes("f6_set", &f6, sizeof(f6));
      break;
    case 10:
      memory.getBytes("f7_set", &f7, sizeof(f7));
      break;
    case 11:
      memory.getBytes("f8_set", &f8, sizeof(f8));
      break;
    default:
      Serial.println(F("memoryGet() error. Parameter not found"));
      break;
  }
}
