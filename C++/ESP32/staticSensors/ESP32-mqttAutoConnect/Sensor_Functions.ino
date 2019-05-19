/***** Filters Functions *****/
// Exponencial filter
bool setExponentialFilter(float alpha = 0.5){
  if(alpha>0 && alpha<1){
      filter = 1; 
      exp_alpha = alpha;
      return true;
    }
  else{return false;}
}

float exponential_filter(float alpha, float t, float t_1){
  if(isnan(alpha) || isnan(t) || isnan(t_1)){
    return t;
  }
  else{
    return (t*alpha+(1-alpha)*t_1);  
  }
}

// Kalman Filter 
bool setKalmanFilter(float noise){
  if(noise!=0){
    filter = 2;
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
    float kalman_gain = kalman_err/(kalman_err+kalman_noise);
    kalman_err = (1-kalman_gain)*kalman_err;
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
}
