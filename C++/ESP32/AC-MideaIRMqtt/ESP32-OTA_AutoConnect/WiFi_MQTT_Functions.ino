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
      String subscribeTopic = container_ID + "/esp32" + esp32Type; 
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
  String topicName = container_ID + "/esp32" + esp32Type;
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    messageTemp += (char)message[i];
  }
  
  if(String(topic) == topicName){
    if(messageTemp == "hardReset"){ // Erase WiFi Credentials and reboot ESP32
      Serial.println(F("Deleting WiFi credentials and rebooting..."));
      mqttPublish(container_ID+"/esp32"+esp32Type+"/log", "Deleting WiFi credentials and rebooting...");
      deleteParams();
      resetCredentials();
    }
  
    else if(messageTemp == "reboot"){ // Reboot ESP32
      Serial.println(F("Rebooting..."));
      mqttPublish(container_ID+"/esp32"+esp32Type+"/log", "Rebooting...");
      ESP.restart();
      delay(1000);
    }
    
    else if(messageTemp == "AcOff"){ // AC OFF
    // set state to disabled
    ir.enabled = false;

    // send the IR signal which will turn the A/C off
    midea_ir_send(&ir);
    Serial.println(F("AcOFF"));
    mqttPublish(container_ID+"/esp32"+esp32Type+"/log", "AcOFF");      
    }

    else if(messageTemp.startsWith("AcOn")){
      String parameter[12];
      int k = 0;
      int l = 0;
      for (int j = 0; j<messageTemp.length(); j++){
        if (messageTemp.substring(j, j+1) == ",") {
          parameter[k] = messageTemp.substring(l,j); 
          k++;
          l = j+1;
        }
        else if(j==messageTemp.length()-1){
          parameter[k] = messageTemp.substring(l,j+1);
        } 
      }

    if(parameter[0]==F("AcOn")){ // Functions executed in Actuator class
       int Temp = parameter[1].toInt();
       int Fan = parameter[2].toInt();
      // set mode, temperature and fan level and internal state to enabled
      ir.enabled = true;
      ir.mode = MODE_COOL;
      ir.fan_level = Fan;
      ir.temperature = Temp;
      // send the IR signal with the previously set properties which will switch the A/C on
      midea_ir_send(&ir);
      Serial.print(F("Attempt to change AC set mode, temperature and fan level")); Serial.print(Temp); Serial.println(F(",")); Serial.print(Fan);
      mqttPublish(container_ID+"/esp32"+esp32Type+"/log", "Attempt to change AC set mode, temperature and fan level "+String(Temp)+ "," + String(Fan));       
       
    }
     else{Serial.println(F("error,AcOn(): Parameter[1-2] wrong"));} 
  }
}
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
  ESP.restart();
  delay(1000);
}

