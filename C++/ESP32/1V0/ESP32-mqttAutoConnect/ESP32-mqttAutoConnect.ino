/*** Include Libraries ***/
#include <DHTesp.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <FS.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <AutoConnect.h>
#include <AutoConnectCredential.h>
#include <EEPROM.h>

/*** Include files ***/
// All files are stored in flash memory
#define PARAM_FILE "/param.json"
#define AUX_MQTTSETTING "/mqtt_settings"
#define AUX_MQTTSETTING_CLEAR "/mqtt_settings_clear"
#define AUX_MQTTSAVE "/mqtt_save"
#define AUX_MQTTCLEAR "/mqtt_clear"
/*** Redifine existing functions ***/
#define GET_CHIPID()  ((uint16_t)(ESP.getEfuseMac()>>32))

/*** Initialization of object and variables ***/
AutoConnect Portal;
AutoConnectConfig Config;
WiFiClient esp32Client;
PubSubClient mqttClient(esp32Client);
station_config AC_credential_config;
station_config *AC_credential = &AC_credential_config;
IPAddress addr;
String mqttBrokerIp;
String container_ID;
String esp32Type;
byte container_ID_length = 12;

/***** Sensors Definitions *****/
byte dht_1R_pin = 13;
byte dht_1L_pin = 12;
byte dht_2R_pin = 14;
byte dht_2L_pin = 27;
byte dht_3R_pin = 26;
byte dht_3L_pin = 25;
byte dht_4R_pin = 33;
byte dht_4L_pin = 32;

/***** Sensors objects *****/
DHTesp dht_1R;
DHTesp dht_1L;
DHTesp dht_2R;
DHTesp dht_2L;
DHTesp dht_3R;
DHTesp dht_3L;
DHTesp dht_4R;
DHTesp dht_4L;

/***** Sensors auxiliar variables *****/
TempAndHumidity data_1R;
TempAndHumidity data_1L;
TempAndHumidity data_2R;
TempAndHumidity data_2L;
TempAndHumidity data_3R;
TempAndHumidity data_3L;
TempAndHumidity data_4R;
TempAndHumidity data_4L;

// Smooth Constant for exponencial filter
float alpha = 0.4;

// Temporal variables
unsigned long update_time;
unsigned long update_constant = 5000; // Actualizar información cada 5 segundos.

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

void loadSettings(){
  File file = SPIFFS.open(PARAM_FILE, "r");
  
  size_t size = file.size();
  std::unique_ptr<char[]> buf (new char[size]);
  file.readBytes(buf.get(), size);
  const size_t capacity = JSON_ARRAY_SIZE(3) + 3*JSON_OBJECT_SIZE(6) + 430;
  DynamicJsonBuffer jsonBuffer(capacity);
  JsonArray& root = jsonBuffer.parseArray(buf.get());
  JsonObject& root_0 = root[0];
  JsonObject& root_1 = root[1];
  JsonObject& root_2 = root[2];

  mqttBrokerIp = root_0["value"].asString();
  container_ID = root_1["value"].asString();
  esp32Type = root_2["value"].asString();

  file.close();
  if (addr.fromString(mqttBrokerIp) && (esp32Type=="front" || esp32Type=="center" || esp32Type=="back") && container_ID.length()==container_ID_length ){
    Serial.println("Uploading Settings...");
    Serial.print("MQTT Broker Ip: "); Serial.println(mqttBrokerIp);
    Serial.print("Container ID: "); Serial.println(container_ID);
    Serial.print("ESP32 Type: "); Serial.println(esp32Type);
  }else{
    Serial.println("Settings are wrong\nReseting credentials and rebooting...");
    resetCredentials();
  }
  
  delay(2000);
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
  
  // The entered value is owned by AutoConnectAux of /mqtt_setting.
  // To retrieve the elements of /mqtt_setting, it is necessary to get
  // the AutoConnectAux object of /mqtt_setting.
  File param1 = SPIFFS.open(PARAM_FILE, "w");
  Portal.aux("/mqtt_settings_clear")->saveElement(param1, { "ip_mqttServer", "containerID", "esp32Type" });
  param1.close();

  // Echo back saved parameters to AutoConnectAux page.
  AutoConnectText&  echo = aux.getElement<AutoConnectText>("parameters");
  echo.value = "MQTT Broker IP: " + mqttBrokerIp + "<br>";
  echo.value += "Container ID: " + container_ID + "<br>";
  echo.value += "ESP32 Type: " + esp32Type + "<br>";
  
  return String("");
}

String saveParams(AutoConnectAux& aux, PageArgument& args) {
  mqttBrokerIp = args.arg("ip_mqttServer");
  mqttBrokerIp.trim();
  
  container_ID = args.arg("containerID");
  container_ID.trim();
  
  esp32Type = args.arg("esp32Type");
  esp32Type.trim();


  if (addr.fromString(mqttBrokerIp) && (esp32Type=="front" || esp32Type=="center" || esp32Type=="back") && container_ID.length()==container_ID_length ) {
    // The entered value is owned by AutoConnectAux of /mqtt_setting.
    // To retrieve the elements of /mqtt_setting, it is necessary to get
    // the AutoConnectAux object of /mqtt_setting.
    File param = SPIFFS.open(PARAM_FILE, "w");
    Portal.aux("/mqtt_settings")->saveElement(param, { "ip_mqttServer", "containerID", "esp32Type" });
    param.close();
  }
  
  // Echo back saved parameters to AutoConnectAux page.
  AutoConnectText&  echo = aux.getElement<AutoConnectText>("parameters");
  if (addr.fromString(mqttBrokerIp) && (esp32Type=="front" || esp32Type=="center" || esp32Type=="back") && container_ID.length()==container_ID_length ) {
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
  if(container_ID.length()==container_ID_length){
    echo.value += "Container ID: " + container_ID + "<br>";
  }else{
    echo.value += "Container ID: <p style='color:red;'>The ID does not have the correct size</p><br>";
  }
  if(esp32Type=="front" || esp32Type=="center" || esp32Type=="back"){
    echo.value += "ESP32 Type: " + esp32Type + "<br>";
  }else{
    echo.value += "ESP32 Type: <p style='color:red;'>It has to be 'front', 'center' or 'back'</p><br>";
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
  else
    Serial.println("SPIFFS open failed: " + fn);
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
  Config.title = "SIPPYS Access Point";
  Portal.config(Config);                        // Configure AutoConnect
}

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

// Exponencial filter
float exponencial_filter(float alpha, float t, float t_1){
  if(isnan(alpha) || isnan(t) || isnan(t_1)){
    return t;
  }
  else{
    return (t*alpha+(1-alpha)*t_1);  
  }
}

// Get sensor data
  TempAndHumidity getData(DHTesp &dht, TempAndHumidity &input_data){
  TempAndHumidity previous_data = input_data;
  input_data = dht.getTempAndHumidity();
  // Apply filters
  input_data.temperature = exponencial_filter(alpha, input_data.temperature, previous_data.temperature);
  input_data.humidity = exponencial_filter(alpha, input_data.humidity, previous_data.humidity);
  return input_data;
}

// Updata sensor data
void updateData(){
  getData(dht_1R, data_1R);
  getData(dht_1L, data_1L);
  getData(dht_2R, data_2R);
  getData(dht_2L, data_2L);
  getData(dht_3R, data_3R);
  getData(dht_3L, data_3L);
  getData(dht_4R, data_4R);
  getData(dht_4L, data_4L);
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

void setup() {
  Serial.begin(115200);
  Serial.println("Initial setup");
  Serial.print("Please wait");
  for(int i=0;i<30;i++){
    Serial.print(".");
    delay(1000);  
  }
  Serial.println();
  
  Serial.println("Trying connection");
  SPIFFS.begin();
  
  loadAux(AUX_MQTTSETTING);
  loadAux(AUX_MQTTSETTING_CLEAR);
  loadAux(AUX_MQTTSAVE);
  loadAux(AUX_MQTTCLEAR);

  AutoConnectAux* setting = Portal.aux(AUX_MQTTSETTING);
  if (setting) {
    PageArgument  args;
    loadParams(*setting, args);
    
    setup_AutoConnect(Portal, Config);
    
    Portal.on(AUX_MQTTSETTING, loadParams);
    Portal.on(AUX_MQTTCLEAR, clearParams);
    Portal.on(AUX_MQTTSAVE, saveParams);
  }
  else{
    Serial.println("aux. load error");
  }
  
  Serial.print("WiFi ");
  if (Portal.begin()) {
    Serial.println("connected:" + WiFi.SSID());
    Serial.println("IP:" + WiFi.localIP().toString());
    // Delete credentials
    //resetCredentials();
  } else {
    Serial.println("connection failed:" + String(WiFi.status()));
    while (1) {
      delay(100);
      yield();
    }
  }
  update_time = millis();  
}

void loop() {
    Portal.handleClient();

    if (!mqttClient.connected()) {
      mqttConnect();
    }

    if(millis()-update_time>update_constant){
      updateData();
      update_time = millis();
    }

    mqttClient.loop();
}
