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

void saveConfig(uint8_t ip[4], String type, String id, String mac_add, const uint8_t *mac){
  IPAddress auxIp(ip[0], ip[1], ip[2], ip[3]);
  if (addr.fromString(auxIp.toString()) && (type=="front" || type=="center" || type=="back") && testContainerId(id) && testMAC(mac_add)) {
    mqttBrokerIp = auxIp.toString();
    esp32Type = type;
    container_ID = id;
    MACstr = mac_add;
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
    esp_err_t result = esp_now_send(mac, (uint8_t *) &idData, sizeof(idData));
    debugNowSend(result);
    Serial.println(F("[ESP-NOW] new ID data sent to Master"));
  } 
  else Serial.println(F("[ESP-NOW] Error: Settings recieved are wrong, ignoring data"));
}

void clearMAC() {
  MACstr = "";
  // The entered value is owned by AutoConnectAux of /settings
  // To retrieve the elements of /settings, it is necessary to get
  // the AutoConnectAux object of /settings.
  File param = SPIFFS.open(PARAM_FILE, "w");
  Portal.aux(AUX_SETTING)->saveElement(param, { "ip_mqttServer", "containerID", "esp32Type", "mac_address" });
  param.close();
  Serial.println(F("[ESP-NOW] Master MAC address was erased"));
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
            esp_err_t result = esp_now_send(mac, (uint8_t *) &idData, sizeof(idData));
            debugNowSend(result);
            Serial.println(F("[ESP-NOW] ID data sent to Master"));
          }
        }   
      } else if (len == sizeof(incData)){
        memcpy(&incData, incomingData, sizeof(incData));
        if (incData.cmd==SEND_DATA){
          esp_err_t result = esp_now_send(mac, (uint8_t *) &envData, sizeof(envData));
          debugNowSend(result);
          Serial.println(F("[ESP-NOW] sensor data sent to Master"));
        } else if (incData.cmd==SEND_DOOR){
          // updateDoor data
          esp_err_t result = esp_now_send(mac, (uint8_t *) &doorData, sizeof(doorData));
          debugNowSend(result);
          Serial.println(F("[ESP-NOW] door data sent to Master"));
        } else if (incData.cmd==SEND_ID){
          idData.container_ID = container_ID;
          idData.esp32Type = esp32Type;
          parseBytes(mqttBrokerIp.c_str(), '.', idData.mqttBrokerIp, 4, 10); // Convert IP string to uint8_t array
          idData.help = false;
          idData.active = true;
          esp_err_t result = esp_now_send(mac, (uint8_t *) &idData, sizeof(idData));
          debugNowSend(result);
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
        } else if (incData.cmd==FORGET_MAC){
          Serial.println(F("[ESP-NOW] Forgeting actual Master MAC direction..."));
          MACstr = "";
          // DEBUG FORGET_MAC
          clearMAC();
          idData.container_ID = container_ID;
          idData.esp32Type = esp32Type;
          parseBytes(mqttBrokerIp.c_str(), '.', idData.mqttBrokerIp, 4, 10); // Convert IP string to uint8_t array
          idData.help = true;
          idData.active = true;
          esp_err_t result = esp_now_send(mac, (uint8_t *) &idData, sizeof(idData));
          debugNowSend(result);
          Serial.println(F("[ESP-NOW] Asking for configuration id data to Master"));
        } else if (incData.cmd==REBOOT){
          Serial.println(F("[ESP-NOW] Rebooting..."));
          ESP.restart();
          delay(1000);
        } else if (incData.cmd==HARD_RESET){
          Serial.println(F("[ESP-NOW] Deleting WiFi credentials and rebooting..."));
          resetCredentials();
        } else Serial.println(F("[ESP-NOW] Error: Command not recognized"));
      }
    }
    else { // If MAC addresses not match
      if (len==sizeof(incData)){ // Only listen to FORGET_MAC command
        memcpy(&incData, incomingData, sizeof(incData));
        if (incData.cmd==FORGET_MAC){
          Serial.println(F("[ESP-NOW] Forgeting actual Master MAC direction..."));
          MACstr = "";
          // DEBUG FORGET_MAC save new config
          clearMAC();
          idData.container_ID = container_ID;
          idData.esp32Type = esp32Type;
          parseBytes(mqttBrokerIp.c_str(), '.', idData.mqttBrokerIp, 4, 10); // Convert IP string to uint8_t array
          idData.help = true;
          idData.active = true;
          esp_err_t result = esp_now_send(mac, (uint8_t *) &idData, sizeof(idData));
          debugNowSend(result);
          Serial.println(F("[ESP-NOW] Asking for configuration id data to Master"));
        } else Serial.println(F("[ESP-NOW] Error: MAC address does not match with master"));
      } else Serial.println(F("[ESP-NOW] Error: MAC address does not match with master"));
    }
  } else { // If I do not know the direction then
    if (len == sizeof(idData)){ // If configuration info is recieved
      memcpy(&idData, incomingData, sizeof(idData)); 
      if (idData.help) saveConfig(idData.mqttBrokerIp, idData.container_ID, idData.esp32Type, String(macStr), mac); // Validate the info and save it
      else Serial.println(F("[ESP-NOW] Error: Cannot send credentials because MAC address does not match with master"));
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
      // Retry InitESPNow, add a counte and then restart?
      // InitESPNow();
      // or Simply Restart
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
      esp_err_t result = esp_now_send(mac, (uint8_t *) &idData, sizeof(idData));
      debugNowSend(result);
    }
    if (esp_now_deinit()==0) {
      esp_now_unregister_recv_cb();
      esp_now_unregister_send_cb();
      Serial.println(F("[ESP-NOW] Turn off conection"));
    }
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
    if (esp_now_add_peer(&masterInfo) != ESP_OK){
      Serial.println(F("[ESP-NOW] Failed to add peer"));
      return;
    } else {
      Serial.println(F("[ESP-NOW] Master peer registered"));
      idData.container_ID = container_ID;
      idData.esp32Type = esp32Type;
      parseBytes(mqttBrokerIp.c_str(), '.', idData.mqttBrokerIp, 4, 10); // Convert IP string to uint8_t array
      idData.help = false;
      idData.active = true;
      esp_err_t result = esp_now_send(mac, (uint8_t *) &idData, sizeof(idData));
      debugNowSend(result);
    }
  }
  else Serial.println(F("[ESP-NOW] MAC address from master has not been registered yet"));  
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
    Serial.println(F("[ESP-NOW] Not sure what happened"));
  }
}
