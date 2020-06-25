void saveNewSlave(const uint8_t *mac) {
  memcpy(slavesInfo[slavesNumber].peerInfo.peer_addr, mac, 6);
  slavesInfo[slavesNumber].peerInfo.channel = 0; // Default channel
  slavesInfo[slavesNumber].peerInfo.encrypt = 0; // No encryption
  slavesInfo[slavesNumber].container_ID = idData.container_ID;
  slavesInfo[slavesNumber].esp32Type = idData.esp32Type;
  memcpy(slavesInfo[slavesNumber].mqttBrokerIp, idData.mqttBrokerIp, 4);
  slavesNumber++;
  memory.putBytes("slavesNumber", &slavesNumber, sizeof(slavesNumber));
  memory.putBytes("slavesInfo", &slavesInfo, sizeof(slavesInfo)); 
  Serial.println(F("[MEMORY] New slave saved in memory"));
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
  } else Serial.println(F("[MEMORY] There are not slave info saved in memory"));
}

void clealMemory(){
  memory.clear(); // Clean NVS and preferences // Not working properly
  chargeSlaves();
  //memory.putBytes("slavesNumber", 0, sizeof(slavesNumber));
  //memory.remove("slavesNumber");
  //memory.remove("slavesInfo");
}
