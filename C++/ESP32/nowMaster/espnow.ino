// callback when data is sent from Master to Slave
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print(F("debug,[ESP-NOW] Last Packet Sent to: ")); Serial.println(macStr);
  Serial.print(F("debug,[ESP-NOW] Last Packet Send Status: ")); Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// callback when data is recv from Master
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  char macStr[18]; // Aux variable to store the incoming MAC
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  Serial.print(F("debug,[ESP-NOW] Last Packet recieve from: ")); Serial.println(macStr);
    
  if(sizeof(idData) == len){ // If the data we recieve has the id structure
    memcpy(&idData, incomingData, sizeof(idData));
    bool isSlave = slaveExist(mac);
    bool isNonSlave = nonSlaveExist(mac);
    if(isSlave){
      if (isNonSlave) Serial.println(F("critical,[ESP-NOW] Device is slave and non slave at the same time"));
      if(idData.active){
        if(idData.help && isSlave) sendID(mac, true, false);  // If device is asking for its credentials and slave exists then send the data
        else Serial.println(F("error,[ESP-NOW] idData does not match any function"));  
      } else deactiveSlave(mac); // Device informed that is turning down ESP-NOW, Master marks this device like disconnected
    } 
    else if(container_ID==idData.container_ID && container_ID!="") saveNewSlave(mac);  // Check if the MAC addreess already exist, if does not exist save it
    else if (!isNonSlave) saveNewNonSlave(mac); // Check if the MAC addreess already exist, if does not exist save it
  } else if (sizeof(envData) == len) { // If the data we recieve has the environmental structure
    memcpy(&envData, incomingData, sizeof(envData));
    Serial.print(F("[ESP-NOW] Environmental variables are: ")); 
    Serial.print(envData.R1.temperature); Serial.print(F(",")); Serial.print(envData.R1.humidity); Serial.print(F(","));
    Serial.print(envData.L1.temperature); Serial.print(F(",")); Serial.print(envData.L1.humidity); Serial.print(F(","));
    Serial.print(envData.R2.temperature); Serial.print(F(",")); Serial.print(envData.R2.humidity); Serial.print(F(","));
    Serial.print(envData.L2.temperature); Serial.print(F(",")); Serial.print(envData.L2.humidity); Serial.print(F(","));
    Serial.print(envData.R3.temperature); Serial.print(F(",")); Serial.print(envData.R3.humidity); Serial.print(F(","));
    Serial.print(envData.L3.temperature); Serial.print(F(",")); Serial.print(envData.L3.humidity); Serial.print(F(","));
    Serial.print(envData.R4.temperature); Serial.print(F(",")); Serial.print(envData.R4.humidity); Serial.print(F(","));
    Serial.print(envData.L4.temperature); Serial.print(F(",")); Serial.println(envData.L4.humidity);
  } else if (sizeof(doorData) == len) { // If the data we recieve has the door structure
    memcpy(&doorData, incomingData, sizeof(doorData));
    Serial.print(F("[ESP-NOW] Door variables are: ")); 
    Serial.print(doorData.R1); Serial.print(F(",")); Serial.print(doorData.L1); Serial.print(F(","));
    Serial.print(doorData.R2); Serial.print(F(",")); Serial.print(doorData.L2); Serial.print(F(","));
    Serial.print(doorData.R3); Serial.print(F(",")); Serial.print(doorData.L3); Serial.print(F(","));
    Serial.print(doorData.R4); Serial.print(F(",")); Serial.println(doorData.L4);
  }
  else Serial.println(F("error,[ESP-NOW] Data size does not matched"));
}

// Init ESP Now with fallback
void InitESPNow() {
  WiFi.disconnect();
  if (esp_now_init() == ESP_OK) Serial.println(F("[ESP-NOW] Init Success"));
  else {
    Serial.println(F("error,[ESP-NOW] Init Failed"));
    ESP.restart();
  }
  
  esp_now_register_send_cb(OnDataSent); // Register for a callback function that will be called when data is sent
  esp_now_register_recv_cb(OnDataRecv); // Register for a callback function that will be called when data is received

  chargeSlaves();           // Charge slaves from NVS with preferences
  chargeNonSlaves();        // Charge non slaves from NVS with preferences
}

void sendID(const uint8_t *mac, bool help, bool active) {
  bool macExist = false;
  uint8_t index = 200;
  
  for(int i=0; i<slavesNumber;i++){
    if(slavesInfo[i].peerInfo.peer_addr[0]==mac[0] && slavesInfo[i].peerInfo.peer_addr[1]==mac[1] && slavesInfo[i].peerInfo.peer_addr[2]==mac[2] && 
       slavesInfo[i].peerInfo.peer_addr[3]==mac[3] && slavesInfo[i].peerInfo.peer_addr[4]==mac[4] && slavesInfo[i].peerInfo.peer_addr[5]==mac[5]) {
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
    debugNowSend(esp_now_send(mac, (uint8_t *) &idData, sizeof(idData))); // Send and debug
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
  debugNowSend(esp_now_send(mac, (uint8_t *) &idData, sizeof(idData))); // Send and debug
}

bool slaveExist(const uint8_t *mac) {
  for(int i=0; i<slavesNumber;i++){
    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             slavesInfo[i].peerInfo.peer_addr[0], slavesInfo[i].peerInfo.peer_addr[1], slavesInfo[i].peerInfo.peer_addr[2], slavesInfo[i].peerInfo.peer_addr[3], slavesInfo[i].peerInfo.peer_addr[4], slavesInfo[i].peerInfo.peer_addr[5]);
    char macStr1[18];
    snprintf(macStr1, sizeof(macStr1), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
             
    Serial.print(F("debug,[ESP-NOW] Compare MAC recieved: ")); Serial.println(macStr1); 
    Serial.print(F("debug,[ESP-NOW] Compare  MAC  saved: ")); Serial.println(macStr);
    if(slavesInfo[i].peerInfo.peer_addr[0]==mac[0] && slavesInfo[i].peerInfo.peer_addr[1]==mac[1] && slavesInfo[i].peerInfo.peer_addr[2]==mac[2] && 
       slavesInfo[i].peerInfo.peer_addr[3]==mac[3] && slavesInfo[i].peerInfo.peer_addr[4]==mac[4] && slavesInfo[i].peerInfo.peer_addr[5]==mac[5]) {
      slaveActive[i] = true;
      if (!esp_now_is_peer_exist(mac)) debugNow(esp_now_add_peer(&slavesInfo[i].peerInfo), "Add peer");// Register peer if not exist
      Serial.println(F("[ESP-NOW] The slave already exists"));
      return true;  
     }
  }
  return false;
}

bool nonSlaveExist(const uint8_t *mac) {
  for(int i=0; i<notSlavesNumber;i++){
    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             notSlavesInfo[i].peerInfo.peer_addr[0], notSlavesInfo[i].peerInfo.peer_addr[1], notSlavesInfo[i].peerInfo.peer_addr[2], notSlavesInfo[i].peerInfo.peer_addr[3], notSlavesInfo[i].peerInfo.peer_addr[4], notSlavesInfo[i].peerInfo.peer_addr[5]);
    char macStr1[18];
    snprintf(macStr1, sizeof(macStr1), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
             
    Serial.print(F("debug,[ESP-NOW] (nonSlaveExist) Compare MAC recieved: ")); Serial.println(macStr1); 
    Serial.print(F("debug,[ESP-NOW] (nonSlaveExist) Compare  MAC  saved: ")); Serial.println(macStr);
    if(notSlavesInfo[i].peerInfo.peer_addr[0]==mac[0] && notSlavesInfo[i].peerInfo.peer_addr[1]==mac[1] && notSlavesInfo[i].peerInfo.peer_addr[2]==mac[2] && 
       notSlavesInfo[i].peerInfo.peer_addr[3]==mac[3] && notSlavesInfo[i].peerInfo.peer_addr[4]==mac[4] && notSlavesInfo[i].peerInfo.peer_addr[5]==mac[5]) {
      Serial.println(F("[ESP-NOW] The non-slave already exists"));
      return true;  
     }
  }
  return false;
}

void deactiveSlave(const uint8_t *mac) {
  for(int i=0; i<slavesNumber; i++){
    if(slavesInfo[i].peerInfo.peer_addr[0]==mac[0] && slavesInfo[i].peerInfo.peer_addr[1]==mac[1] && slavesInfo[i].peerInfo.peer_addr[2]==mac[2] && 
       slavesInfo[i].peerInfo.peer_addr[3]==mac[3] && slavesInfo[i].peerInfo.peer_addr[4]==mac[4] && slavesInfo[i].peerInfo.peer_addr[5]==mac[5]) {
      slaveActive[i] = false;
      char macStr[18]; // Aux variable to store the incoming MAC
      snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
      Serial.print(F("[ESP-NOW] The slave with address ")); Serial.print(macStr); Serial.print(F(" and credentials "));
      Serial.print(slavesInfo[i].container_ID+' '); Serial.print(slavesInfo[i].esp32Type + ' '); 
      IPAddress auxIp(slavesInfo[i].mqttBrokerIp[0], slavesInfo[i].mqttBrokerIp[1], slavesInfo[i].mqttBrokerIp[2], slavesInfo[i].mqttBrokerIp[3]);
      Serial.print(auxIp.toString()); Serial.println(F("was deactivated"));
      debugNow(esp_now_del_peer(slavesInfo[i].peerInfo.peer_addr), "Slave Delete");
     }
  }
}

// Scan for slaves in AP mode
void ScanForSlave() {
  Serial.println(F("debug,[WI-FI] Scanning..."));
  int8_t scanResults = WiFi.scanNetworks();
  bool potentialSlave = false;
  
  if (scanResults != 0) {
    Serial.print(F("debug,[WI-FI] Found ")); Serial.print(scanResults); Serial.println(F(" devices "));
    for (int i = 0; i < scanResults; ++i) {
      // Print SSID and RSSI for each device found
      String SSID = WiFi.SSID(i);
      int32_t RSSI = WiFi.RSSI(i); // Upgrade give RSSI condition to avoid try to talk with every AP
      uint8_t *BSSID = WiFi.BSSID(i);  
      
      if (SSID.indexOf("ESP32") == 0) { // Check if the current device starts with `ESP32`
        uint8_t mac[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        for (int i=0; i<6; i++) {
          if (i==5) mac[i] = (*(BSSID+i))-1;
          else mac[i] = *(BSSID+i);
        }
        if (!slaveExist(mac) && !nonSlaveExist(mac)) {
          bool createPeer = false;
          potentialSlave = true; // SSID of interest
          Serial.println(F("debug,[WI-FI] Potential slave found"));
          Serial.print(F("debug,[WI-FI] ")); Serial.print(i + 1); Serial.print(F(": ")); Serial.print(SSID); Serial.print(F(" [")); 
          for (int i=0; i<6;i++) {
            Serial.print(*(mac+i),HEX); if(i!=5) Serial.print(F(":"));
          }
          Serial.print(F("]")); Serial.print(F(" (")); Serial.print(RSSI); Serial.println(F(")"));
          // Ask for its credentials
          idData.help = false;
          idData.active = false;
          if(!esp_now_is_peer_exist(BSSID)) createPeer = true;
          if(createPeer) {
            for(int i=0; i<6;i++) auxPeer.peer_addr[i] = *(BSSID+i);
            auxPeer.channel = 0;
            auxPeer.encrypt = 0;
            debugNow(esp_now_add_peer(&auxPeer), "Add temporary peer");
          }
          debugNowSend(esp_now_send(BSSID, (uint8_t *) &idData, sizeof(idData))); // Send and debug
          if(createPeer) debugNow(esp_now_del_peer(BSSID), "Delete temporary peer");
        }
      }
    }
  }
  
  if (!potentialSlave) Serial.println(F("debug,[WI-FI] Slave not found"));

  WiFi.scanDelete();  // clean up ram
}

void checkActiveSlave() {
  for(int i=0; i<slavesNumber; i++){
    if(slaveActive[i] == false){
      // Ask if it´s active
      idData.help = false;
      idData.active = true;
      debugNowSend(esp_now_send(slavesInfo[i].peerInfo.peer_addr, (uint8_t *) &idData, sizeof(idData))); // Send and debug
    }
  }
}

void askSlaves() {
  for(int i=0; i<slavesNumber; i++){
    if(slaveActive[i]){ // Ask for sensor info
      outData.cmd = SEND_DATA;  // 8 bytes
      debugNowSend(esp_now_send(slavesInfo[i].peerInfo.peer_addr, (uint8_t *) &outData, sizeof(outData))); // Send and debug
      outData.cmd = SEND_DOOR;  // 8 bytes
      debugNowSend(esp_now_send(slavesInfo[i].peerInfo.peer_addr, (uint8_t *) &outData, sizeof(outData)));
    }
  }
}

void debugNowSend(esp_err_t result) { debugNow(result, "Send"); }

void debugNow(esp_err_t result, String action) {
  Serial.print(F("debug,[ESP-NOW] ")); Serial.print(action); Serial.print(F(" Status: "));
  if (result == ESP_OK) Serial.println(F("Success"));
  else if (result == ESP_ERR_ESPNOW_NOT_INIT) Serial.println(F("ESPNOW not Init"));  // How did we get so far!!
  else if (result == ESP_ERR_ESPNOW_ARG) Serial.println(F("Invalid Argument")); 
  else if (result == ESP_ERR_ESPNOW_INTERNAL) Serial.println(F("Internal Error")); 
  else if (result == ESP_ERR_ESPNOW_NO_MEM) Serial.println(F("Out of memory"));
  else if (result == ESP_ERR_ESPNOW_NOT_FOUND) Serial.println(F("Peer not found"));
  else if (result == ESP_ERR_ESPNOW_FULL) Serial.println(F("Peer list is full"));
  else if (result == ESP_ERR_ESPNOW_EXIST) Serial.println(F("Peer has existed"));
  else Serial.println(F("Not sure what happened"));
}
