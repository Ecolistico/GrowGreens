void saveNewSlave(const uint8_t *mac, bool force  /*= false*/){
  if((container_ID==idData.container_ID && container_ID!="") || force){ // DEBUG
    memcpy(slavesInfo[slavesNumber].peerInfo.peer_addr, mac, 6);
    slavesInfo[slavesNumber].peerInfo.channel = 0; // Default channel
    slavesInfo[slavesNumber].peerInfo.encrypt = 0; // No encryption
    slavesInfo[slavesNumber].container_ID = idData.container_ID;
    slavesInfo[slavesNumber].esp32Type = idData.esp32Type;
    memcpy(slavesInfo[slavesNumber].mqttBrokerIp, idData.mqttBrokerIp, 4);
    slavesNumber++;
    memory.putBytes("slavesNumber", &slavesNumber, sizeof(slavesNumber));
    memory.putBytes("slavesInfo", &slavesInfo, sizeof(slavesInfo)); 
    if(force) {
      Serial.println(F("warning,[MEMORY] New slave saved in memory (forced)"));
      debugNow(esp_now_add_peer(&slavesInfo[slavesNumber-1].peerInfo), "Add peer");
    } else {
      Serial.println(F("[MEMORY] New slave saved in memory")); 
      debugNow(esp_now_add_peer(&slavesInfo[slavesNumber-1].peerInfo), "Add peer");
    }
  } else Serial.println(F("error,[MEMORY] Attemp to create new slave failed - ID´s does not match")); 
}

void chargeSlaves() {
  memory.begin("slavesNumber");
  memory.begin("slavesInfo");
 
  memory.getBytes("slavesNumber", &slavesNumber, sizeof(slavesNumber));

  if (slavesNumber != 0){
    Serial.println(F("[MEMORY] There are slaves info saved in memory"));
    size_t slavesLen = memory.getBytesLength("slavesInfo");
    char buffer[slavesLen]; // prepare a buffer for the data
    memory.getBytes("slavesInfo", buffer, slavesLen);
    if (slavesLen % sizeof(peerSaved)) { // simple check that data fits
      log_e("Data is not correct size!");
      return;
    }
    peerSaved *slaves = (peerSaved *) buffer; // cast the bytes into a struct ptr
    for (int i=0; i<slavesNumber;i++){
      slavesInfo[i]= slaves[i];
      slaveActive[i] = false;
      Serial.print(F("[MEMORY] Slave ")); Serial.print(i+1); Serial.print(F(" credentials: "));
      Serial.print(slavesInfo[i].container_ID+' '); Serial.print(slavesInfo[i].esp32Type + ' '); 
      IPAddress auxIp(slavesInfo[i].mqttBrokerIp[0], slavesInfo[i].mqttBrokerIp[1], slavesInfo[i].mqttBrokerIp[2], slavesInfo[i].mqttBrokerIp[3]);
      Serial.print(auxIp.toString() + ' ');
      char macStr[18]; // Aux variable to store the incoming MAC
      snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           slavesInfo[i].peerInfo.peer_addr[0], slavesInfo[i].peerInfo.peer_addr[1], slavesInfo[i].peerInfo.peer_addr[2], slavesInfo[i].peerInfo.peer_addr[3], slavesInfo[i].peerInfo.peer_addr[4], slavesInfo[i].peerInfo.peer_addr[5]);
      Serial.println(macStr);
      if (esp_now_add_peer(&slavesInfo[i].peerInfo) != ESP_OK) Serial.println(F("[ESP-NOW] Failed to add peer"));
      else Serial.println(F("[ESP-NOW] Slave peer registered"));
    }
  } else Serial.println(F("[MEMORY] There are not slaves info saved"));
}

void memoryCleanSlaves(bool reb /*= true*/){
  //memory.clear(); // Clean NVS and preferences
  memory.remove("slavesNumber");
  memory.remove("slavesInfo");
  Serial.println(F("warning,[MEMORY] slaves info erased"));
  if (reb) ESP.restart();
  delay(1000);
}

void saveID() {
  memory.putBytes("containerID", &container_ID, sizeof(container_ID));
  Serial.print(F("[MEMORY] new container ID saved: ")); Serial.println(container_ID);
}

void chargeID(){
  memory.begin("containerID");
  memory.getBytes("containerID", &container_ID, sizeof(container_ID));
  if(container_ID.length()==container_ID_length) {
    Serial.print(F("[MEMORY] Container ID charged: ")); 
    Serial.println(container_ID);
  } else Serial.println(F("[MEMORY] There is not id info saved")); 
}

void memoryCleanID(){
  memory.remove("containerID");
  Serial.println(F("warning,[MEMORY] id info erased"));
  memoryCleanSlaves();
}

void saveNewNonSlave(const uint8_t *mac){
  // If container id´s are different from the original 
  if(container_ID!=idData.container_ID && container_ID!=""){ // DEBUG
    memcpy(notSlavesInfo[notSlavesNumber].peerInfo.peer_addr, mac, 6);
    notSlavesInfo[notSlavesNumber].peerInfo.channel = 0; // Default channel
    notSlavesInfo[notSlavesNumber].peerInfo.encrypt = 0; // No encryption
    notSlavesInfo[notSlavesNumber].container_ID = idData.container_ID;
    notSlavesInfo[notSlavesNumber].esp32Type = idData.esp32Type;
    memcpy(notSlavesInfo[notSlavesNumber].mqttBrokerIp, idData.mqttBrokerIp, 4);
    notSlavesNumber++;
    memory.putBytes("notSlavesNumber", &notSlavesNumber, sizeof(notSlavesNumber));
    memory.putBytes("notSlavesInfo", &notSlavesInfo, sizeof(notSlavesInfo)); 
    Serial.println(F("[MEMORY] New nonSlave saved in memory")); 
  } else Serial.println(F("error,[MEMORY] Attemp to create new nonSlave failed - ID´s does not match")); 
}

void chargeNonSlaves() {
  memory.begin("notSlavesNumber");
  memory.begin("notSlavesInfo");
 
  memory.getBytes("notSlavesNumber", &notSlavesNumber, sizeof(notSlavesNumber));

  if (notSlavesNumber != 0){
    Serial.println(F("[MEMORY] There are nonSlaves info saved in memory"));
    size_t slavesLen = memory.getBytesLength("notSlavesInfo");
    char buffer[slavesLen]; // prepare a buffer for the data
    memory.getBytes("notSlavesInfo", buffer, slavesLen);
    if (slavesLen % sizeof(peerSaved)) { // simple check that data fits
      log_e("Data is not correct size!");
      return;
    }
    peerSaved *slaves = (peerSaved *) buffer; // cast the bytes into a struct ptr
    for (int i=0; i<notSlavesNumber;i++) notSlavesInfo[i]= slaves[i];
  } else Serial.println(F("[MEMORY] There are not nonSlaves info saved"));
}

void memoryCleanNonSlaves(bool reb /*= true*/){
  memory.remove("notSlavesNumber");
  memory.remove("notSlavesInfo");
  Serial.println(F("warning,[MEMORY] nonSlaves info erased"));
  if (reb) ESP.restart();
  delay(1000);
}
