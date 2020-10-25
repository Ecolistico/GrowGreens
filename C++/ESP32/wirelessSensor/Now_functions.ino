void parseBytes(const char* str, char sep, byte* bytes, int maxBytes, int base) {
    for (int i = 0; i < maxBytes; i++) {
        bytes[i] = strtoul(str, NULL, base);  // Convert byte
        str = strchr(str, sep);               // Find next separator
        if (str == NULL || *str == '\0') {
            break;                            // No more separators, exit
        }
        str++;                                // Point to next character after separator
    }
}
/*DEBUG*/
void saveConfig(uint8_t ip[4], String type, String id, String mac_add, const uint8_t *mac){
  IPAddress auxIp(ip[0], ip[1], ip[2], ip[3]);
  if (addr.fromString(auxIp.toString()) && (type=="front" || type=="center" || type=="back") && testContainerId(id) && testMAC(mac_add)) {
    mqttBrokerIp = auxIp.toString();
    esp32Type = type;
    container_ID = id;
    if(MACstr!=mac_add){
      
      MACstr = mac_add;
      registerPeer(MACstr); // Register the new MAC address
    }
    // The entered value is owned by AutoConnectAux of /settings
    // To retrieve the elements of /settings, it is necessary to get
    // the AutoConnectAux object of /settings.
    File param = SPIFFS.open(PARAM_FILE, "w");
    Portal.aux(AUX_SETTING)->saveElement(param, { "ip_mqttServer", "containerID", "esp32Type", "mac_address" });
    param.close();
    idData.container_ID = container_ID;
    idData.esp32Type = esp32Type;
    parseBytes(mqttBrokerIp.c_str(), '.', idData.mqttBrokerIp, 4, 10); // Convert IP string to uint8_t array
    idData.help = false;
    idData.active = true;
    debugNowSend(esp_now_send(mac, (uint8_t *) &idData, sizeof(idData)));
    Serial.println(F("[ESP-NOW] new ID data sent to Master"));
  } 
  else Serial.println(F("[ESP-NOW] Error: Settings recieved are wrong, ignoring data"));
}
/*DEBUG*/
void clearMAC(const uint8_t *mac) {
  Serial.println(F("[ESP-NOW] Forgeting actual Master MAC direction..."));
  MACstr = "";
  // The entered value is owned by AutoConnectAux of /settings
  // To retrieve the elements of /settings, it is necessary to get
  // the AutoConnectAux object of /settings.
  File param = SPIFFS.open(PARAM_FILE, "w");
  Portal.aux(AUX_SETTING)->saveElement(param, { "ip_mqttServer", "containerID", "esp32Type", "mac_address" });
  param.close();
  Serial.println(F("[ESP-NOW] Master MAC address was erased"));
  idData.container_ID = container_ID;
  idData.esp32Type = esp32Type;
  parseBytes(mqttBrokerIp.c_str(), '.', idData.mqttBrokerIp, 4, 10); // Convert IP string to uint8_t array
  idData.help = true;
  idData.active = true;
  debugNowSend(esp_now_send(mac, (uint8_t *) &idData, sizeof(idData)));
  Serial.println(F("[ESP-NOW] Asking for configuration id data to Master"));
}

// Callback when data is sent
void OnDataSent(const uint8_t *mac, esp_now_send_status_t status) {
  Serial.print(F("[ESP-NOW] Last Packet Send Status:\t"));
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// callback when data is recv from Master
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  uint8_t mac1[] = {0X00, 0X00, 0X00, 0X00, 0X00, 0X00}; // Aux variable to compare MAC
  char macStr[18]; // Aux variable to store the incoming MAC
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
           
  Serial.print(F("[ESP-NOW] Last Packet recieve from: ")); Serial.println(macStr);
  
  if (MACstr.length()>0){ // If I know the direction of my master device
    parseBytes(MACstr.c_str(), ':', mac1, 6, 16); // Convert MAC string to uint8_t array
    uint8_t count = 0;
    for (int i=0; i<6; i++) if (mac[i]==mac1[i]) count++;
    if (count==6) { // If MAC is equal to master
      if (len == sizeof(idData)){ // If configuration info is recieved
        memcpy(&idData, incomingData, sizeof(idData)); 
        if(idData.active){ // If master ask if we are active on ESP-NOW
          
        } else {
          if (idData.help) saveConfig(idData.mqttBrokerIp, idData.container_ID, idData.esp32Type, String(macStr), mac); // Validate the info and save it
          else { // Send the actual configuration
            idData.container_ID = container_ID;
            idData.esp32Type = esp32Type;
            parseBytes(mqttBrokerIp.c_str(), '.', idData.mqttBrokerIp, 4, 10); // Convert IP string to uint8_t array
            idData.help = false;
            idData.active = true;
            debugNowSend(esp_now_send(mac, (uint8_t *) &idData, sizeof(idData)));
            Serial.println(F("[ESP-NOW] ID data sent to Master"));
          }
        }   
      } else if (len == sizeof(incData)){
        memcpy(&incData, incomingData, sizeof(incData));
        if (incData.cmd==SEND_DATA){
          debugNowSend(esp_now_send(mac, (uint8_t *) &envData, sizeof(envData)));
          Serial.println(F("[ESP-NOW] sensor data sent to Master"));
        } else if (incData.cmd==SEND_DOOR){
          // updateDoor data
          debugNowSend(esp_now_send(mac, (uint8_t *) &doorData, sizeof(doorData)));
          Serial.println(F("[ESP-NOW] door data sent to Master"));
        } else if (incData.cmd==SEND_ID){
          idData.container_ID = container_ID;
          idData.esp32Type = esp32Type;
          parseBytes(mqttBrokerIp.c_str(), '.', idData.mqttBrokerIp, 4, 10); // Convert IP string to uint8_t array
          idData.help = false;
          idData.active = true;
          debugNowSend(esp_now_send(mac, (uint8_t *) &idData, sizeof(idData)));
          Serial.println(F("[ESP-NOW] ID data sent to Master"));
        } else if (incData.cmd==UPDATE_CONSTANT){
          if(incData.param>=2){
            update_constant = incData.param;
            memorySave(3);
            Serial.print(F("[ESP-NOW] Attempt to change update time constant succeed, Taking measurements every ")); Serial.print(incData.param); Serial.println(F(" s"));
          }
          else Serial.println(F("[ESP-NOW] Attempt to change update time constant failed, Parameter has to be at least 2 s"));
        } else if (incData.cmd==NOT_FILTER){
          filter = 0;
          memorySave(0);
          Serial.println(F("[ESP-NOW] Attempt to change -Not Filter- Configuration succeeded"));
        } else if (incData.cmd==SET_EXPONENTIAL_FILTER){
          if(setExponentialFilter(incData.param)){
            memorySave(1);
            Serial.print(F("[ESP-NOW] Attempt to set Exponential Filter succeeded, New Alpha = ")); Serial.println(incData.param);
          }
          else Serial.println(F("[ESP-NOW] Attempt to set Exponential Filter failed,Alpha parameter is wrong"));
        } else if (incData.cmd==SET_KALMAN_FILTER){
          if(setKalmanFilter(incData.param)){
            memorySave(2);
            Serial.print(F("[ESP-NOW] Attempt to set Kalman Filter succeeded, New Kalman Noise = ")); Serial.println(incData.param);
          }
          else Serial.println(F("[ESP-NOW] Attemp to set Kalman Filter failed,Kalman Noise parameter is wrong"));
        } else if (incData.cmd==FORGET_MAC) {
          clearMAC(mac); // DEBUG FORGET_MAC clearMAC()
        } else if (incData.cmd==REBOOT){
          DeinitESPNow();
          Serial.println(F("[ESP-NOW] Rebooting..."));
          ESP.restart();
          delay(1000);
        } else if (incData.cmd==HARD_RESET){
          DeinitESPNow();
          Serial.println(F("[ESP-NOW] Deleting WiFi credentials and rebooting..."));
          resetCredentials();
        } else Serial.println(F("[ESP-NOW] Error: Command not recognized"));
      }
    }
    else { // If MAC addresses not match
      if (len==sizeof(incData)){ // Only listen to FORGET_MAC command
        memcpy(&incData, incomingData, sizeof(incData));
        if (incData.cmd==FORGET_MAC){
          clearMAC(mac); // DEBUG FORGET_MAC clearMAC()
        } else Serial.println(F("[ESP-NOW] Error: MAC address does not match with master"));
      } else Serial.println(F("[ESP-NOW] Error: MAC address does not match with master"));
    }
  } else { // If I do not know the direction then
    if (len == sizeof(idData)){ // If configuration info is recieved
      memcpy(&idData, incomingData, sizeof(idData)); 
      if (idData.help) saveConfig(idData.mqttBrokerIp, idData.container_ID, idData.esp32Type, String(macStr), mac); // Validate the info and save it
      else if(MACstr == ""){ // If MAC is empty then send the info to device that is asking. It Could be the master.
        bool createPeer = false;
        idData.container_ID = container_ID;
        idData.esp32Type = esp32Type;
        parseBytes(mqttBrokerIp.c_str(), '.', idData.mqttBrokerIp, 4, 10); // Convert IP string to uint8_t array
        idData.help = false;
        idData.active = true;
        if(!esp_now_is_peer_exist(mac)) createPeer = true;
        if(createPeer) {
          for(int i=0; i<6;i++) auxPeer.peer_addr[i] = mac[i];
          auxPeer.channel = 0;
          auxPeer.encrypt = 0;
          debugNow(esp_now_add_peer(&auxPeer), "Add temporary peer");
        }
        debugNowSend(esp_now_send(mac, (uint8_t *) &idData, sizeof(idData))); // Send and debug
        if(createPeer) debugNow(esp_now_del_peer(mac), "Delete temporary peer");
      } else Serial.println(F("[ESP-NOW] Error: Cannot send credentials because MAC address does not match with master"));
    }
  }
}

// Init ESP Now with fallback
void InitESPNow() {
  if(!activeESPNOW){
    WiFi.disconnect();
    if (esp_now_init() == ESP_OK) {
      Serial.println(F("[ESP-NOW] Init Success"));
      activeESPNOW = true;
      loadSettings(false); // Just charge parameters without checking format
      esp_now_register_send_cb(OnDataSent); // Register for a callback function that will be called when data is sent
      esp_now_register_recv_cb(OnDataRecv); // Register for a callback function that will be called when data is received
      registerPeer(MACstr); // Register master peer
    }
    else {
      Serial.println(F("[ESP-NOW] Error: Init Failed"));
      ESP.restart();
    }
  } else Serial.println(F("[ESP-NOW] Error: Protocol has been initialized before"));
  
}

void DeinitESPNow() { // Turn off ESP-NOW
  if (activeESPNOW){
    if (MACstr.length()>0){ // If we know the master address then inform that we are disconnecting the protocol
      uint8_t mac[] = {0X00, 0X00, 0X00, 0X00, 0X00, 0X00}; // Aux variable to save MAC
      parseBytes(MACstr.c_str(), ':', mac, 6, 16); // Convert MAC string to uint8_t array
      idData.container_ID = container_ID;
      idData.esp32Type = esp32Type;
      parseBytes(mqttBrokerIp.c_str(), '.', idData.mqttBrokerIp, 4, 10); // Convert IP string to uint8_t array
      idData.help = false;
      idData.active = false;
      debugNowSend(esp_now_send(mac, (uint8_t *) &idData, sizeof(idData)));
    }
    esp_now_unregister_recv_cb();
    esp_now_unregister_send_cb();
    if (esp_now_deinit()==0) Serial.println(F("[ESP-NOW] Turn off conection"));
    else Serial.println(F("[ESP-NOW] Error: Cannot turn of conection"));
  } else Serial.println(F("[ESP-NOW] Error: Protocol has not been  initialized before"));
}

void registerPeer(String MAC) {
  // Add peer if we have the MAC address
  if(MAC.length()>0){
    uint8_t mac[] = {0X00, 0X00, 0X00, 0X00, 0X00, 0X00}; // Aux variable to compare MAC
    parseBytes(MAC.c_str(), ':', mac, 6, 16); // Convert MAC string to uint8_t array
    // Register peer
    memcpy(masterInfo.peer_addr, mac, 6);
    masterInfo.channel = 0;  
    masterInfo.encrypt = false;  
    debugNow(esp_now_add_peer(&masterInfo), "Add peer");
    if(esp_now_is_peer_exist(masterInfo.peer_addr)) {
      Serial.println(F("[ESP-NOW] Master peer was registered"));
      idData.container_ID = container_ID;
      idData.esp32Type = esp32Type;
      parseBytes(mqttBrokerIp.c_str(), '.', idData.mqttBrokerIp, 4, 10); // Convert IP string to uint8_t array
      idData.help = false;
      idData.active = true;
      debugNowSend(esp_now_send(mac, (uint8_t *) &idData, sizeof(idData)));
    }
  }
  else Serial.println(F("[ESP-NOW] MAC address from master has not been registered yet"));  
}

void unregisterPeer(String MAC) {
  // Unregister peer if we have the MAC address
  if(MAC.length()>0){
    uint8_t mac[] = {0X00, 0X00, 0X00, 0X00, 0X00, 0X00}; // Aux variable to compare MAC
    parseBytes(MAC.c_str(), ':', mac, 6, 16); // Convert MAC string to uint8_t array
    debugNow(esp_now_del_peer(mac), "Unregister peer");
  } else Serial.println(F("[ESP-NOW] MAC address format is wrong"));  
}

void debugNowSend(esp_err_t result) { debugNow(result, "Send"); }

void debugNow(esp_err_t result, String action) {
  Serial.print(F("[ESP-NOW] ")); Serial.print(action); Serial.print(F(" Status: "));
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
