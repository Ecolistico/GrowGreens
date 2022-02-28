/* Tested with AutoConnect 1.1.7 */

/*** Include Libraries ***/
#include <WiFi.h>
#include <SPIFFS.h>
#include <FS.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <AutoConnect.h>
#include <AutoConnectCredential.h>
#include <SPI.h>
#include <EthernetENC.h>
#include <Arduino.h>
#include <midea_ir.h>

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
EthernetClient ethClient;
PubSubClient mqttClient(ethClient);
station_config_t AC_credential_config;
station_config_t *AC_credential = &AC_credential_config;
IPAddress addr;
//IPAddress ipEnt;
String mqttBrokerIp;
String container_ID;
String esp32Type; // Principal or return
byte container_ID_length = 10;
//byte mac[] = {0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02};
byte mac[6];
//byte mac[] = { random(2), random(2), random(2), random(2), random(2), random(2) };
uint8_t mqttAttempt = 0;

/***** Hardware IR Definitions *****/
const uint8_t MIDEA_RMT_CHANNEL = 0;
const uint8_t MIDEA_TX_PIN = 4;
// Ir Controller
MideaIR ir;

// Portal aux variable handler
bool portalAux = false;
bool startPortalAux = false;

/*** Name functions ***/
// AutoConnect Functions
void loadSettings(bool rst);
String loadParams(AutoConnectAux& aux, PageArgument& args);
String clearParams(AutoConnectAux& aux, PageArgument& args);
void deleteParams();
String saveParams(AutoConnectAux& aux, PageArgument& args);
bool loadAux(const String auxName); // OK
void setup_AutoConnect(AutoConnect &Portal, AutoConnectConfig &Config);
bool testContainerId(String ID);
// MQTT
bool mqttConnect();
void mqttPublish(String top, String msg);
void callback(char* topic, byte* message, unsigned int length);
void resetCredentials();
bool startCP(IPAddress ip);

void setup() {
  WiFi.mode(WIFI_MODE_STA);
  WiFi.macAddress(mac);
  Serial.begin(115200);
  Serial.println(F("Initial setup"));
  Serial.print(F("Please wait"));
  midea_ir_init(&ir, MIDEA_RMT_CHANNEL, MIDEA_TX_PIN); // init IR Controller
  for(int i=0;i<30;i++){
    Serial.print(F("."));
    delay(1000);  
  }
  Serial.println();
  
  Serial.println(F("Trying connection"));
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
    Serial.println(F("aux. load error"));
  }
  
  Serial.print(F("WiFi "));

  loadSettings(false);
  
  if (addr.fromString(mqttBrokerIp) && (esp32Type=="AirReturn" || esp32Type=="AirPrincipal") && container_ID.length()==container_ID_length){
    Serial.println(F("Parameters are ok"));
    mqttClient.setServer(mqttBrokerIp.c_str(), 1883);
    mqttClient.setCallback(callback); // Function to execute actions with entries of mqtt messages
  }
  else {
    if (Portal.begin()) {
      Serial.print(F("connected:")); Serial.println(WiFi.SSID());
      Serial.print(F("IP:")); Serial.println(WiFi.localIP().toString());
      // Delete credentials
      //resetCredentials();
    } else {
      Serial.print(F("connection failed:" )); Serial.println(String(WiFi.status()));
      while (1) {
        delay(100);
        yield();
      }
    }
  }
  
  if(Ethernet.begin(mac)) Serial.println(F("Ethernet begin")); 
  else Serial.println(F("Ethernet failed"));
  
  // Allow the hardware to sort itself out
  delay(1500); 
}

void loop() {  
    if (!mqttClient.connected() && portalAux==false) { // If MQTT disconnected retry connection 
      mqttConnect(); 
    } else if (portalAux==false) mqttClient.loop();   
      
    Portal.handleClient(); // Handle Portal
    Ethernet.maintain();

}
