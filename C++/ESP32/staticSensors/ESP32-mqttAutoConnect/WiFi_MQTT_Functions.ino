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

void callback(char* topic, byte* message, unsigned int length) {
  String topicName = container_ID + "/esp32" + esp32Type;
  String messageTemp;
  for (int i = 0; i < length; i++) {
    messageTemp += (char)message[i];
  }
  
  if (String(topic) == topicName && messageTemp == "sendData") {
    Serial.println("Sending sensor data to MQTT Broker");
    sendData();
  }

  if(String(topic) == topicName && messageTemp == "hardRestart"){
    resetCredentials();
  }

  /*
 * Agregar función para que cambie el parámetro alfa
 * Agregar función para que modifique parámetro entre toma de mediciones (update_constant)
 * Agregar función para que modifique parámetro longitud ID (container_ID_length)
 */
  
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
