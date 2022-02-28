/***** MQTT Functions *****/
// Attemp MQTT connection
bool mqttConnect() {
  static const char alphanum[] = "0123456789"
                                 "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                 "abcdefghijklmnopqrstuvwxyz";  // For random generation of client ID.
  char    clientId[9];

  uint8_t retry = 5;
  while (!mqttClient.connected()) {
    if (mqttBrokerIp.length() <= 0){
      loadSettings(true);
      break;
    }
    
    mqttClient.setServer(mqttBrokerIp.c_str(), 1883);
    Serial.print(F("Attempting MQTT broker: ")); Serial.println(mqttBrokerIp);
    mqttClient.setCallback(callback); // Function to execute actions with entries of mqtt messages

    for (uint8_t i = 0; i < 8; i++) {
      clientId[i] = alphanum[random(62)];
    }
    clientId[8] = '\0';

    if (mqttClient.connect(clientId)) {
      Serial.print(F("Established: ")); Serial.println(String(clientId));
      String subscribeTopic = container_ID + "/esp32" + esp32Type + esp32Floor; 
      char charTopic[subscribeTopic.length()+1];
      subscribeTopic.toCharArray(charTopic,subscribeTopic.length()+1);
      mqttClient.subscribe(charTopic);
      return true;
    } else {
      Serial.print(F("Connection failed: ")); Serial.println(String(mqttClient.state()));
      if (!--retry) {
        portalAux = true; /**/
        startPortal(portalAux);
        Serial.println(F("Attempts to connect MQTT exceed"));
        Serial.println(F("Enable Portal"));
        WiFi.disconnect(true);
        break;
        delay(3000);
      }
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
  String topicName = container_ID + "/esp32" + esp32Type + esp32Floor;
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    messageTemp += (char)message[i];
  }
  
  if(String(topic) == topicName){
    if (messageTemp == "sendData") { // Send sensor data to Broker
      sendData();
      Serial.println(F("Sending sensor data to MQTT Broker"));
      mqttPublish(container_ID+"/esp32"+esp32Type+esp32Floor+"/log", "Sensor data sent");
    }
  
    else if(messageTemp == "hardReset"){ // Erase WiFi Credentials and reboot ESP32
      Serial.println(F("Deleting WiFi credentials and rebooting..."));
      mqttPublish(container_ID+"/esp32"+esp32Type+esp32Floor+"/log", "Deleting WiFi credentials and rebooting...");
      deleteParams();
      resetCredentials();
    }
  
    else if(messageTemp == "reboot"){ // Reboot ESP32
      Serial.println(F("Rebooting..."));
      mqttPublish(container_ID+"/esp32"+esp32Type+esp32Floor+"/log", "Rebooting...");
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
    
      if(newValue>=2){
        update_constant = newValue;
        memorySave(3);
        Serial.print(F("Attempt to change update time constant succeed, Taking measurements every ")); Serial.print(newValue); Serial.println(F(" s"));
        mqttPublish(container_ID+"/esp32"+esp32Type+esp32Floor+"/log", "Attempt to change update time constant succeeded, Taking measurements every "+String(newValue)+" s");
      }
      else{
        Serial.println(F("Attempt to change update time constant failed, Parameter has to be at least 2 s"));
        mqttPublish(container_ID+"/esp32"+esp32Type+esp32Floor+"/error", "Attempt to change update time constant failed, Parameter has to be at least 2 s");
      }
    }

    else if(messageTemp == "notFilter"){
      filter = 0;
      memorySave(0);
      Serial.println(F("Attempt to change -Not Filter- Configuration succeeded"));
      mqttPublish(container_ID+"/esp32"+esp32Type+esp32Floor+"/log", "Attempt to change -Not Filter- Configuration succeeded");
    }
    
    else if(messageTemp.startsWith("setExponentialFilter")){
      int newValue;
        
      for (int i = 0; i < messageTemp.length(); i++) {
        if (messageTemp.substring(i, i+1) == ",") {
          newValue = messageTemp.substring(i+1).toInt();
          break;
        }
      }
      
      if(setExponentialFilter(newValue)){
        memorySave(1);
        Serial.print(F("Attempt to set Exponential Filter succeeded, New Alpha = ")); Serial.println(newValue);
        mqttPublish(container_ID+"/esp32"+esp32Type+esp32Floor+"/log",  "Attempt to set Exponential Filter succeeded, New Alpha = "+String(newValue));
      }
      else{
        Serial.println(F("Attempt to set Exponential Filter failed,Alpha parameter is wrong"));
        mqttPublish(container_ID+"/esp32"+esp32Type+esp32Floor+"/error", "Attempt to set Exponential Filter failed,Alpha parameter is wrong");
      }
    }

    else if(messageTemp.startsWith("setKalmanFilter")){
      int newValue;
        
      for (int i = 0; i < messageTemp.length(); i++) {
        if (messageTemp.substring(i, i+1) == ",") {
          newValue = messageTemp.substring(i+1).toInt();
          break;
        }
      }

      if(setKalmanFilter(newValue)){
        memorySave(2);
        Serial.print(F("Attempt to set Kalman Filter succeeded, New Kalman Noise = ")); Serial.println(newValue);
        mqttPublish(container_ID+"/esp32"+esp32Type+esp32Floor+"/log",  "Attempt to set Kalman Filter succeeded, New Kalman Noise = "+String(newValue));
      }
      else{
        Serial.println(F("Attemp to set Kalman Filter failed,Kalman Noise parameter is wrong"));
        mqttPublish(container_ID+"/esp32"+esp32Type+esp32Floor+"/error", "Attemp to set Kalman Filter failed,Kalman Noise parameter is wrong");
      }
    } 
  }
}

// Send over MQTT sensor data
void sendData(){
  String textString = String(data_1.temperature) + "," + String(data_1.humidity) + "," + String(data_2.temperature)+ "," + String(data_2.humidity) + ",";
  textString += String(data_3.temperature) + "," + String(data_3.humidity) + "," + String(data_4.temperature) + "," + String(data_4.humidity) + ",";
  textString += String(data_5.temperature) + "," + String(data_5.humidity) + "," + String(data_6.temperature) + "," + String(data_6.humidity);
  
  char textData[textString.length()+1];
  textString.toCharArray(textData,textString.length()+1);
  
  String topicString = container_ID + "/esp32" + esp32Type + esp32Floor;
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
    Serial.print(F("There is(are) ")); Serial.print(entries1); Serial.println(F(" entrie(s) that will be deleted"));
    for(int i=entries1-1;i>=0;i--){
      if( AutoConnectCredential().load(i, AC_credential) ){
        Serial.print(F("Deleting credential: ")); Serial.println(reinterpret_cast<char*>(AC_credential->ssid));
        if(AutoConnectCredential().del(reinterpret_cast<char*>(AC_credential->ssid))){
          Serial.println(F("Deleted with success"));
        }
      }
    }
  }
  
  filter = 0;
  exp_alpha = 0;
  kalman_noise = 0;
  update_constant = 0;
  for(int i=0; i<4; i++){
    memorySave(i);
  }
  Serial.println(F("Parameters filter, alpha, kalman_noise and update_constant deleted"));
  
  ESP.restart();
  delay(1000);
}
