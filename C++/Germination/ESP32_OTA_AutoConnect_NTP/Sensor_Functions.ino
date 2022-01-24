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
  getData(dht_1, data_1);
  getData(dht_2, data_2);
  getData(dht_3, data_3);
  getData(dht_4, data_4);
  getData(dht_5, data_5);
  getData(dht_6, data_6);
  
  Serial.print(data_1.temperature); Serial.print(F(",")); Serial.print(data_1.humidity); Serial.print(F(",")); Serial.print(data_2.temperature); Serial.print(F(",")); Serial.print(data_2.humidity); Serial.print(F(","));
  Serial.print(data_3.temperature); Serial.print(F(",")); Serial.print(data_3.humidity); Serial.print(F(",")); Serial.print(data_4.temperature); Serial.print(F(",")); Serial.print(data_4.humidity); Serial.print(F(","));
  Serial.print(data_5.temperature); Serial.print(F(",")); Serial.print(data_5.humidity); Serial.print(F(",")); Serial.print(data_6.temperature); Serial.print(F(",")); Serial.println(data_6.humidity); //Serial.print(F(","));
}

void setupSensors(){
  dht_1.setup(dht_1_pin, DHTesp::DHT22);
  dht_2.setup(dht_2_pin, DHTesp::DHT22);
  dht_3.setup(dht_3_pin, DHTesp::DHT22);
  dht_4.setup(dht_4_pin, DHTesp::DHT22);
  dht_5.setup(dht_5_pin, DHTesp::DHT22);
  dht_6.setup(dht_6_pin, DHTesp::DHT22);

  pinMode(update_pin, INPUT); // Detect when request for new LED´s configuration
}
