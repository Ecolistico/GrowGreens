/***** Filters Functions *****/
// Exponencial filter
bool setExponentialFilter(uint8_t alpha = 50){
  if(alpha>0 && alpha<100){
      filter = 1;
      memorySave(0);
      exp_alpha = alpha;
      return true;
    }
  else{return false;}
}

float exponential_filter(uint8_t alpha, float t, float t_1){
  if(isnan(alpha) || isnan(t) || isnan(t_1)){
    return t;
  }
  else{
    float a = float(alpha)/100;
    return (t*a+(1-a)*t_1);  
  }
}

// Kalman Filter 
bool setKalmanFilter(uint8_t noise){
  if(noise>0){
    filter = 2;
    memorySave(0);
    kalman_noise = noise; // Enviromental Noise
    kalman_err = 1; // Initial Error
    return true;
  }
  else{return false;}
}

float kalman_filter(float t, float t_1){
  if( kalman_err==0 || isnan(t) || isnan(t_1) ){
    if(kalman_err==0){
      kalman_err = 0.1;
    }
    return t;
  }
  else{
    float kal_noise = float(kalman_noise)/100; // Convert uint8_t to float
    float kalman_gain = (kalman_err)/(kalman_err+kal_noise); // Calculate Kalman Gain
    kalman_err = (1-kalman_gain)*kalman_err; // Calculate Kalma Error
    return (t_1+kalman_gain*(t-t_1));
  }  
}

/***** Sensors Functions *****/
// Get sensor data
TempAndHumidity getData(DHTesp &dht, TempAndHumidity &input_data){
  TempAndHumidity previous_data = input_data;
  input_data = dht.getTempAndHumidity();
  // Apply filters
  if(filter==1){
    input_data.temperature = exponential_filter(exp_alpha, input_data.temperature, previous_data.temperature);
    input_data.humidity = exponential_filter(exp_alpha, input_data.humidity, previous_data.humidity);
  }
  else if(filter==2){
    input_data.temperature = kalman_filter(input_data.temperature, previous_data.temperature);
    input_data.humidity = kalman_filter(input_data.humidity, previous_data.humidity);
  }
  return input_data;
}

// Updata sensor data
void updateData(){
  getData(dht_1R, data_1R);
  getData(dht_1L, data_1L);
  getData(dht_2R, data_2R);
  getData(dht_2L, data_2L);
  getData(dht_3R, data_3R);
  getData(dht_3L, data_3L);
  getData(dht_4R, data_4R);
  getData(dht_4L, data_4L);
  Puerta1 = PuertaEstado(pinM1);
  Puerta2 = PuertaEstado(pinM2);
  Puerta3 = PuertaEstado(pinM3);
  Puerta4 = PuertaEstado(pinM4);
  Serial.print(data_1R.temperature); Serial.print(F(",")); Serial.print(data_1R.humidity); Serial.print(F(",")); Serial.print(data_1L.temperature); Serial.print(F(",")); Serial.print(data_1L.humidity); Serial.print(F(","));
  Serial.print(data_2R.temperature); Serial.print(F(",")); Serial.print(data_2R.humidity); Serial.print(F(",")); Serial.print(data_2L.temperature); Serial.print(F(",")); Serial.print(data_2L.humidity); Serial.print(F(","));
  Serial.print(data_3R.temperature); Serial.print(F(",")); Serial.print(data_3R.humidity); Serial.print(F(",")); Serial.print(data_3L.temperature); Serial.print(F(",")); Serial.print(data_3L.humidity); Serial.print(F(","));
  Serial.print(data_4R.temperature); Serial.print(F(",")); Serial.print(data_4R.humidity); Serial.print(F(",")); Serial.print(data_4L.temperature); Serial.print(F(",")); Serial.println(data_4L.humidity); Serial.print(F(","));
  Serial.print(Puerta1); Serial.print(F(",")); Serial.print(Puerta2); Serial.print(F(",")); 
  Serial.print(Puerta3); Serial.print(F(",")); Serial.println(Puerta4);
}

void setupSensors(){
  dht_1R.setup(dht_1R_pin, DHTesp::DHT22);
  dht_1L.setup(dht_1L_pin, DHTesp::DHT22);
  dht_2R.setup(dht_2R_pin, DHTesp::DHT22);
  dht_2L.setup(dht_2L_pin, DHTesp::DHT22);
  dht_3R.setup(dht_3R_pin, DHTesp::DHT22);
  dht_3L.setup(dht_3L_pin, DHTesp::DHT22);
  dht_4R.setup(dht_4R_pin, DHTesp::DHT22);
  dht_4L.setup(dht_4L_pin, DHTesp::DHT22);
  pinMode(pinM1, INPUT_PULLUP);
  pinMode(pinM2, INPUT_PULLUP);
  pinMode(pinM3, INPUT_PULLUP);
  pinMode(pinM4, INPUT_PULLUP);
}

bool PuertaEstado(byte PinM){
  bool PuertaE;
   if(digitalRead(PinM)==HIGH)
  {
    PuertaE = true;    
  }
  else
  {
    PuertaE = false;
  }
  return PuertaE;
}

