/***** MQTT Functions *****/
// Attemp MQTT connection
bool mqttConnect() {
  static const char alphanum[] = "0123456789"
                                 "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                 "abcdefghijklmnopqrstuvwxyz";  // For random generation of client ID.
  char    clientId[9];

  uint8_t retry = 3;
  while (!mqttClient.connected()) {
    if (mqttBrokerIp.length() <= 0){
      loadSettings();
      break;
    }
    
    mqttClient.setServer(mqttBrokerIp.c_str(), 1883);
    Serial.println(String("Attempting MQTT broker: ") + mqttBrokerIp);
    mqttClient.setCallback(callback); // Function to execute actions with entries of mqtt messages

    for (uint8_t i = 0; i < 8; i++) {
      clientId[i] = alphanum[random(62)];
    }
    clientId[8] = '\0';

    if (mqttClient.connect(clientId)) {
      Serial.println("Established: " + String(clientId));
      String subscribeTopic = container_ID + "/esp32" + esp32Type; 
      char charTopic[subscribeTopic.length()+1];
      subscribeTopic.toCharArray(charTopic,subscribeTopic.length()+1);
      mqttClient.subscribe(charTopic);
      return true;
    } else {
      Serial.println("Connection failed: " + String(mqttClient.state()));
      if (!--retry)
        break;
      delay(3000);
    }
  }
  return false;
}

void mqttPublish(String top, String msg){
  char textData[msg.length()+1];
  msg.toCharArray(textData,msg.length()+1);

  char topicData[top.length()+1];
  top.toCharArray(topicData,top.length()+1);

  mqttClient.publish(topicData, textData);  
}

void callback(char* topic, byte* message, unsigned int length) {
  String topicName = container_ID + "/esp32" + esp32Type;
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    messageTemp += (char)message[i];
  }
  
  if(String(topic) == topicName){
    if (messageTemp == "sendData") { // Send sensor data to Broker
      sendData();
      Serial.println(F("Sending sensor data to MQTT Broker"));
      mqttPublish(container_ID+"/esp32"+esp32Type+"/log", "Sensor data sended");
    }
  
    else if(messageTemp == "hardRestart"){ // Erase WiFi Credentials and reboot ESP32
      Serial.println(F("Deleting WiFi credentials and rebooting..."));
      mqttPublish(container_ID+"/esp32"+esp32Type+"/log", "Deleting WiFi credentials and rebooting...");
      resetCredentials();
    }
  
    /*** Not tested yet ***/
    else if(messageTemp == "reboot"){ // Reboot ESP32
      Serial.println(F("Rebooting..."));
      mqttPublish(container_ID+"/esp32"+esp32Type+"/log", "Rebooting...");
      ESP.restart();
      delay(1000);
    }
    
    else if(messageTemp.startsWith("updateConstant")){ // Change update constant to get measurements from the sensor at different frequency
      // Msg command structure= "updateConstant,int" where int is the number of seconds
      int newValue;
      
      for (int i = 0; i < messageTemp.length(); i++) {
        if (messageTemp.substring(i, i+1) == ",") {
          newValue = messageTemp.substring(i+1).toInt();
          break;
        }
      }
      
      Serial.print(F("New Update Constant Value = ")); Serial.println(newValue); // If this line works change for -> 
      /*  if(newValue>=2){
       *    update_constant = newValue;
       *    memorySave(3);
       *    Serial.print(F("Attemp to change update time constant succeed,Taking measurements every ")); Serial.print(newValue); Serial.println(F(" s"));
       *    mqttPublish(container_ID+"/esp32"+esp32Type+"/log", "Attemp to change update time constant succeed,Taking measurements every "+String(newValue)+" s");
       *  }
       *  else{
       *    Serial.println(F("Attemp to change update time constant failed,Parameter has to be at least 2 s"));
       *    mqttPublish(container_ID+"/esp32"+esp32Type+"/error", "Attemp to change update time constant failed,Parameter has to be at least 2 s");
       *  }
      */
      // test after that
    }

    else if(messageTemp == "notFilter"){
      filter = 0;
      Serial.println(F("Attemp to change -Not Filter- Configuration succed"));
      
      /*memorySave(0);
      mqttPublish(container_ID+"/esp32"+esp32Type+"/log", "Attemp to change -Not Filter- Configuration succed");*/
    }
    
    else if(messageTemp.startsWith("setExponentialFilter")){
      int newValue;
        
      for (int i = 0; i < messageTemp.length(); i++) {
        if (messageTemp.substring(i, i+1) == ",") {
          newValue = messageTemp.substring(i+1).toInt();
          break;
        }
      }
      Serial.print(F("Exponential Filter selected, Alpha = ")); Serial.println(newValue); // If this line works change for -> 
      /*  if(setExponentialFilter(newValue)){
       *    memorySave(1);
       *    Serial.print(F(""Attemp to set Exponential Filter succed,New Alpha = ")); Serial.println(newValue);
       *    mqttPublish(container_ID+"/esp32"+esp32Type+"/log",  "Attemp to set Exponential Filter succed,New Alpha = "+String(newValue));
       *  }
       *  else{
       *    Serial.println(F("Attemp to set Exponential Filter failed,Alpha parameter is wrong"));
       *    mqttPublish(container_ID+"/esp32"+esp32Type+"/error", "Attemp to set Exponential Filter failed,Alpha parameter is wrong");
       *  }
      */ 
      // And test after that
    }

    else if(messageTemp.startsWith("setKalmanFilter")){
      int newValue;
        
      for (int i = 0; i < messageTemp.length(); i++) {
        if (messageTemp.substring(i, i+1) == ",") {
          newValue = messageTemp.substring(i+1).toInt();
          break;
        }
      }
      Serial.print(F("Kalman Filter selected, Noise = ")); Serial.println(newValue); // If this line works change for ->
    /*  if(setKalmanFilter(newValue)){
     *    memorySave(2);
     *    Serial.print(F(""Attemp to set Kalman Filter succed,New Kalman Noise = ")); Serial.println(newValue);
     *    mqttPublish(container_ID+"/esp32"+esp32Type+"/log",  "Attemp to set Kalman Filter succed,New Kalman Noise = "+String(newValue));
     *  }
     *  else{
     *    Serial.println(F("Attemp to set Kalman Filter failed,Kalman Noise parameter is wrong"));
     *    mqttPublish(container_ID+"/esp32"+esp32Type+"/error", "Attemp to set Kalman Filter failed,Kalman Noise parameter is wrong");
     *  }
     */ 
      // And test after that
    
    /*
    * a) Probar nuevas funciones agregadas
    * b) Agregar grabar y borrar parametros EEPROM, revisar que no interfiera con el guardado de las credenciales del WiFi. Se tiene que guardar al menos: 
    * uint8_t filter
    * uint8_t exp_alpha
    * uint8_t kalman_noise
    * uint8_t update_constant
    */
    /*** Not tested yet ***/ 
    }  
  }
}

// Send over MQTT sensor data
void sendData(){
  String textString = String(data_1R.temperature) + "," + String(data_1R.humidity) + "," + String(data_1L.temperature)+ "," + String(data_1L.humidity) + ",";
  textString += String(data_2R.temperature) + "," + String(data_2R.humidity) + "," + String(data_2L.temperature) + "," + String(data_2L.humidity) + ",";
  textString += String(data_3R.temperature) + "," + String(data_3R.humidity) + "," + String(data_3L.temperature) + "," + String(data_3L.humidity) + ",";
  textString += String(data_4R.temperature) + "," + String(data_4R.humidity) + "," + String(data_4L.temperature) + "," + String(data_4L.humidity);
  char textData[textString.length()+1];
  textString.toCharArray(textData,textString.length()+1);
  
  String topicString = container_ID + "/esp32" + esp32Type;
  char topicData[topicString.length()+1];
  topicString.toCharArray(topicData,topicString.length()+1);
    
  mqttClient.publish(topicData, textData);  
}

/***** WiFi Functions *****/
void resetCredentials(){
  WiFi.disconnect(true);
  WiFi.begin("0","0");       // adding this effectively seems to erase the previous stored SSID/PW

  int8_t entries1 = AutoConnectCredential().entries();
  if(entries1>0){
    Serial.print("There is(are) "); Serial.print(entries1); Serial.println(" entrie(s) that will be deleted");
    for(int i=entries1-1;i>=0;i--){
      if( AutoConnectCredential().load(i, AC_credential) ){
        Serial.print("Deleting credential: "); Serial.println(reinterpret_cast<char*>(AC_credential->ssid));
        if(AutoConnectCredential().del(reinterpret_cast<char*>(AC_credential->ssid))){
          Serial.println("Deleted with success");
        }
      }
    }
  }
  
  ESP.restart();
  delay(1000);
}
