// callback when data is sent from Master to Slave
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print(F("[ESP-NOW] Last Packet Sent to: ")); Serial.println(macStr);
  Serial.print(F("[ESP-NOW] Last Packet Send Status: ")); Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// callback when data is recv from Master
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  char macStr[18]; // Aux variable to store the incoming MAC
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  Serial.print(F("[ESP-NOW] Last Packet recieve from: ")); Serial.println(macStr);
    
  if(sizeof(idData) == len){
    memcpy(&idData, incomingData, sizeof(idData));
    if(idData.active){
      bool isSlave = slaveExist(mac);
      if(idData.help && isSlave) sendID(mac, true, true);  // If device is asking for its credentials and slave exists then send the data
      else if(!isSlave) saveNewSlave(mac);  // Check if the MAC addreess already exist, if does not exist save it
      else if(!isSlave) Serial.println(F("error,[ESP-NOW] idData does not match any function"));
    } else deactiveSlave(mac); // Device informed that is turning down ESP-NOW
  }
  else Serial.println(F("[ESP-NOW] Data size does not matched"));
}

// Init ESP Now with fallback
void InitESPNow() {
  WiFi.disconnect();
  if (esp_now_init() == ESP_OK) Serial.println(F("[ESP-NOW] Init Success"));
  else {
    Serial.println(F("[ESP-NOW] Init Failed"));
    ESP.restart();
  }
  
  esp_now_register_send_cb(OnDataSent); // Register for a callback function that will be called when data is sent
  esp_now_register_recv_cb(OnDataRecv); // Register for a callback function that will be called when data is received

  chargeSlaves();   // Charge slaves from NVS with preferences
  //clealMemory();    // Erases all the NVS
}

void sendID(const uint8_t *mac, bool help, bool active) {
  bool macExist = false;
  uint8_t index = 200;
  
  for(int i=0; i<slavesNumber;i++){
    if(slavesInfo[i].peerInfo.peer_addr[0]==mac[0] && slavesInfo[i].peerInfo.peer_addr[1]==mac[1] && slavesInfo[i].peerInfo.peer_addr[2]==mac[2] && 
       slavesInfo[i].peerInfo.peer_addr[3]==mac[3] && slavesInfo[i].peerInfo.peer_addr[4]==mac[4] && slavesInfo[i].peerInfo.peer_addr[6]==mac[6]) {
      macExist = true;
      index = i;
      break;
     }
  }

  if (macExist){
    idData.container_ID = slavesInfo[index].container_ID;
    idData.esp32Type = slavesInfo[index].esp32Type;
    memcpy(idData.mqttBrokerIp, slavesInfo[index].mqttBrokerIp, 4);
    idData.help = help;
    idData.active = active;
    esp_err_t result = esp_now_send(mac, (uint8_t *) &idData, sizeof(idData));
    debugNowSend(result);
    Serial.println(F("[ESP-NOW] Credentials sent to slave"));
  } else Serial.println(F("error,[ESP-NOW] Cannot send credentials to slave because they are not saved"));
}

void sendID(uint8_t index, bool help, bool active){
  uint8_t mac[] = {0X00, 0X00, 0X00, 0X00, 0X00, 0X00}; // Aux variable to save MAC
  memcpy(mac, slavesInfo[index].peerInfo.peer_addr, 6);
  idData.container_ID = slavesInfo[index].container_ID;
  idData.esp32Type = slavesInfo[index].esp32Type;
  memcpy(idData.mqttBrokerIp, slavesInfo[index].mqttBrokerIp, 4);
  idData.help = help;
  idData.active = active;
  char macStr[18]; // Aux variable to store the incoming MAC
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  esp_err_t result = esp_now_send(mac, (uint8_t *) &idData, sizeof(idData));
  debugNowSend(result);
}

bool slaveExist(const uint8_t *mac) {
  for(int i=0; i<slavesNumber;i++){
    if(slavesInfo[i].peerInfo.peer_addr[0]==mac[0] && slavesInfo[i].peerInfo.peer_addr[1]==mac[1] && slavesInfo[i].peerInfo.peer_addr[2]==mac[2] && 
       slavesInfo[i].peerInfo.peer_addr[3]==mac[3] && slavesInfo[i].peerInfo.peer_addr[4]==mac[4] && slavesInfo[i].peerInfo.peer_addr[6]==mac[6]) {
      slaveActive[i] = true;
      Serial.println(F("[ESP-NOW] The slave already exists"));
      return true;  
     }
  }
  return false;
}

void deactiveSlave(const uint8_t *mac) {
  for(int i=0; i<slavesNumber; i++){
    if(slavesInfo[i].peerInfo.peer_addr[0]==mac[0] && slavesInfo[i].peerInfo.peer_addr[1]==mac[1] && slavesInfo[i].peerInfo.peer_addr[2]==mac[2] && 
       slavesInfo[i].peerInfo.peer_addr[3]==mac[3] && slavesInfo[i].peerInfo.peer_addr[4]==mac[4] && slavesInfo[i].peerInfo.peer_addr[6]==mac[6]) {
      slaveActive[i] = false;
      char macStr[18]; // Aux variable to store the incoming MAC
      snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
      Serial.print(F("[ESP-NOW] The slave with address ")); Serial.print(macStr); Serial.print(F(" and credentials "));
      Serial.print(slavesInfo[i].container_ID+' '); Serial.print(slavesInfo[i].esp32Type + ' '); 
      IPAddress auxIp(slavesInfo[i].mqttBrokerIp[0], slavesInfo[i].mqttBrokerIp[1], slavesInfo[i].mqttBrokerIp[2], slavesInfo[i].mqttBrokerIp[3]);
      Serial.print(auxIp.toString()); Serial.println(F("was deactivated"));
     }
  }
}

void debugNowSend(esp_err_t result) {
  Serial.print(F("[ESP-NOW] Send Status: "));
  if (result == ESP_OK) {
    Serial.println(F("Success"));
  } else if (result == ESP_ERR_ESPNOW_NOT_INIT) {
    // How did we get so far!!
    Serial.println(F("ESPNOW not Init."));
  } else if (result == ESP_ERR_ESPNOW_ARG) {
    Serial.println(F("Invalid Argument"));
  } else if (result == ESP_ERR_ESPNOW_INTERNAL) {
    Serial.println(F("Internal Error"));
  } else if (result == ESP_ERR_ESPNOW_NO_MEM) {
    Serial.println(F("ESP_ERR_ESPNOW_NO_MEM"));
  } else if (result == ESP_ERR_ESPNOW_NOT_FOUND) {
    Serial.println(F("Peer not found."));
  } else {
    Serial.println(F("Not sure what happened"));
  }
}
