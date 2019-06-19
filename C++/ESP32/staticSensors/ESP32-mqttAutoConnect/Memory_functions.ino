void memorySetup(){
  memory.begin("filter");
  memory.begin("alpha");
  memory.begin("k_noise");
  memory.begin("update");

  for(int i=0; i<4; i++){
    memoryGet(i);
  }
  
  if(exp_alpha == 0){
    exp_alpha = 40;
    memorySave(1);
  }
  if(kalman_noise == 0){
    kalman_noise = 50;
    memorySave(2);
  }
  if(update_constant == 0){
    update_constant = 2;
    memorySave(3);
  }
  
  Serial.print(F("Filter = ")); Serial.println(filter);
  Serial.print(F("Alpha = ")); Serial.println(exp_alpha);
  Serial.print(F("Kalman Noise = ")); Serial.println(kalman_noise);
  Serial.print(F("Update = ")); Serial.println(update_constant);
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
  }
}
