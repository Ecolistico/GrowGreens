void loadSettings(bool rst){
  File file = SPIFFS.open(PARAM_FILE, "r");
  
  size_t size = file.size();
  std::unique_ptr<char[]> buf (new char[size]);
  file.readBytes(buf.get(), size);
  const size_t capacity = JSON_ARRAY_SIZE(4) + 4*JSON_OBJECT_SIZE(6) + 430;
  DynamicJsonDocument jsonBuffer(capacity);
  
  //JsonArray& root = jsonBuffer.parseArray(buf.get());
  DeserializationError err = deserializeJson(jsonBuffer, buf.get());
  if (err) {
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(err.c_str());
  } else {
    JsonObject root_0 = jsonBuffer[0];
    JsonObject root_1 = jsonBuffer[1];
    JsonObject root_2 = jsonBuffer[2];
    JsonObject root_3 = jsonBuffer[3];

    mqttBrokerIp = root_0["value"].as<String>();
    container_ID = root_1["value"].as<String>();
    esp32Type = root_2["value"].as<String>();
    esp32Floor = root_3["value"].as<String>();

    file.close();
    if (addr.fromString(mqttBrokerIp) && (esp32Type=="front" || esp32Type=="center" || esp32Type=="back") && container_ID.length()==container_ID_length && esp32Floor.toInt()<10){
      Serial.println(F("Uploading Settings..."));
      Serial.print(F("MQTT Broker Ip: ")); Serial.println(mqttBrokerIp);
      Serial.print(F("Container ID: ")); Serial.println(container_ID);
      Serial.print(F("ESP32 Type: ")); Serial.println(esp32Type);
      Serial.print(F("ESP32 Floor: ")); Serial.println(esp32Floor);
    }else{
      if(rst){
        Serial.println(F("Settings are wrong\nReseting credentials and rebooting..."));
        resetCredentials();  
      }
    }
    delay(2000); 
  }
}

String loadParams(AutoConnectAux& aux, PageArgument& args) {
  (void)(args);
  File param = SPIFFS.open(PARAM_FILE, "r");
  if (param) {
    aux.loadElement(param);
    param.close();
  }
  else
    Serial.println(PARAM_FILE " open failed");
  return String("");
}

String clearParams(AutoConnectAux& aux, PageArgument& args) {
  mqttBrokerIp = "";
  container_ID = "";
  esp32Type = "";
  esp32Floor = "";
  
  // The entered value is owned by AutoConnectAux of /mqtt_setting.
  // To retrieve the elements of /mqtt_setting, it is necessary to get
  // the AutoConnectAux object of /mqtt_setting.
  File param1 = SPIFFS.open(PARAM_FILE, "w");
  Portal.aux("/mqtt_settings_clear")->saveElement(param1, { "ip_mqttServer", "containerID", "esp32Type","esp32Floor" });
  param1.close();

  // Echo back saved parameters to AutoConnectAux page.
  AutoConnectText&  echo = aux.getElement<AutoConnectText>("parameters");
  echo.value = "MQTT Broker IP: " + mqttBrokerIp + "<br>";
  echo.value += "Container ID: " + container_ID + "<br>";
  echo.value += "ESP32 Type: " + esp32Type + "<br>";
  echo.value += "ESP32 Floor: " + esp32Floor + "<br>";
  
  return String("");
}

void deleteParams() {
  mqttBrokerIp = "";
  container_ID = "";
  esp32Type = "";
  esp32Floor = "";
  
  // The entered value is owned by AutoConnectAux of /mqtt_setting.
  // To retrieve the elements of /mqtt_setting, it is necessary to get
  // the AutoConnectAux object of /mqtt_setting.
  File param1 = SPIFFS.open(PARAM_FILE, "w");
  Portal.aux("/mqtt_settings_clear")->saveElement(param1, { "ip_mqttServer", "containerID", "esp32Type","esp32Floor" });
  param1.close();
}

String saveParams(AutoConnectAux& aux, PageArgument& args) {
  bool esp32FloorFlag = true;
  mqttBrokerIp = args.arg("ip_mqttServer");
  mqttBrokerIp.trim();
  
  container_ID = args.arg("containerID");
  container_ID.trim();
  
  esp32Type = args.arg("esp32Type");
  esp32Type.trim();

  esp32Floor = args.arg("esp32Floor");
  esp32Floor.trim();

  bool testContID = testContainerId(container_ID);
  if (addr.fromString(mqttBrokerIp) && (esp32Type=="front" || esp32Type=="center" || esp32Type=="back") && testContID && esp32Floor.toInt()<10) {
    // The entered value is owned by AutoConnectAux of /mqtt_setting.
    // To retrieve the elements of /mqtt_setting, it is necessary to get
    // the AutoConnectAux object of /mqtt_setting.
    File param = SPIFFS.open(PARAM_FILE, "w");
    Portal.aux("/mqtt_settings")->saveElement(param, { "ip_mqttServer", "containerID", "esp32Type", "esp32Floor" });
    param.close();
  }
  
  // Echo back saved parameters to AutoConnectAux page.
  AutoConnectText&  echo = aux.getElement<AutoConnectText>("parameters");
  if (addr.fromString(mqttBrokerIp) && (esp32Type=="front" || esp32Type=="center" || esp32Type=="back") && testContID && esp32Floor.toInt()<10) {
    echo.value = "<p style='color:green;'>Parameters were saved correcty!</p><br>";
  }
  else{
    echo.value = "<p style='color:red;'>Parameters were not saved because at least one of them is incorrect</p><br>";
  }
  if (addr.fromString(mqttBrokerIp)) {
    echo.value += "MQTT Broker IP: " + mqttBrokerIp + "<br>";
  }else{
    echo.value += "MQTT Broker IP: <p style='color:red;'>You did not provide a correct IP Address</p><br>";
  }
  if(testContID){
    echo.value += "Container ID: " + container_ID + "<br>";
  }else{
    echo.value += "Container ID: <p style='color:red;'>The ID does not have the correct size/form</p><br>";
  }
  if(esp32Type=="front" || esp32Type=="center" || esp32Type=="back"){
    echo.value += "ESP32 Type: " + esp32Type + "<br>";
  }else{
    echo.value += "ESP32 Type: <p style='color:red;'>It has to be 'front', 'center' or 'back'</p><br>";
  }
  if(esp32Floor.toInt()<10){
    echo.value += "ESP32 Floor " + esp32Floor + "<br>";
  }else{
    echo.value += "ESP32 Floor: <p style='color:red;'>The ESP does not have the correct size/form</p><br>";
  }
  
  return String("");
}

// Load AutoConnectAux JSON from SPIFFS.
bool loadAux(const String auxName) {
  bool  rc = false;
  String  fn = auxName + ".json";
  File fs = SPIFFS.open(fn.c_str(), "r");
  if (fs) {
    rc = Portal.load(fs);
    fs.close();
  }
  else { Serial.print(F("SPIFFS open failed: ")); Serial.println(fn); }
  
  return rc;
}

void setup_AutoConnect(AutoConnect &Portal, AutoConnectConfig &Config){  
  Config.apid = "ESP32-" +  String(GET_CHIPID(), HEX);// Sets SoftAP Name
  Config.psk = "Kale5elak."; // Sets SoftAP Password
  Config.apip = IPAddress(192,168,10,1);      // Sets SoftAP IP address
  Config.gateway = IPAddress(192,168,10,1);     // Sets WLAN router IP address
  Config.netmask = IPAddress(255,255,255,0);    // Sets WLAN scope
  //Config.autoReconnect = true;                  // Enable auto-reconnect
  Config.autoReconnect = false;                  // Disable auto-reconnect (prevents save credentials even if they are erased)
  Config.homeUri = "/_ac";  // Sets home path
  Config.bootUri = AC_ONBOOTURI_HOME; // Reboot path
  Config.title = "GrowGreens Access Point by Ecolistico";
  Config.ota = AC_OTA_BUILTIN; // Enable OTA Feature
  Portal.config(Config);                        // Configure AutoConnect
}

bool testContainerId(String ID){
  bool resp = false;
  int cont = 0;

  if(ID.length()==container_ID_length){
    for(int i=0; i<container_ID_length; i++){
      int test = -1;
      int prevTest = -1;
      int prevTest1 = -1;
      
      if(i==1){
        test = int(ID[i])-48;
        prevTest = int(ID[i-1])-48;
        if(prevTest==0){
          if(test>0 && test<=9){cont++;}
        }
        else if(test>=0 && test<=9){cont++;}
      }
      else if(i==5 || i==9){
        test = int(ID[i])-48;
        prevTest = int(ID[i-1])-48;
        prevTest1 = int(ID[i-2])-48;
        if(prevTest==0 && prevTest1==0){
          if(test>0 && test<=9){cont++;}
        }
        else if(test>=0 && test<=9){cont++;}
      }
      else if(i==2 || i==6){
        if(ID[i]=='-'){cont++;}
      }
      else{
        test = int(ID[i])-48;
        if(test>=0 && test<=9){cont++;}
      }
    }
  }

  if(cont==container_ID_length){resp=true;}
  
  return resp;
}

bool startCP(IPAddress ip) {
  Serial.println("C.P. started, IP:" + WiFi.localIP().toString());
  startPortalAux = true;
  //InitESPNow();
  return true;
}


void startPortal(bool start) {
  Config.immediateStart = start;
  Portal.config(Config); // Configure AutoConnect
  if (Portal.begin()) {
    Serial.print(F("WiFi "));
    Serial.println("connected:" + WiFi.SSID());
    Serial.println("IP:" + WiFi.localIP().toString());
  } else {
    Serial.print(F("WiFi "));
    Serial.println("connection failed:" + String(WiFi.status()));
  }
}
