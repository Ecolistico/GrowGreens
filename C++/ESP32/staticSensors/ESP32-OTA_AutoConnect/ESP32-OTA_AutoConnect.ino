/* Tested with AutoConnect 1.1.7 */

/*** Include Libraries ***/
#include <DHTesp.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <FS.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <AutoConnect.h>
#include <AutoConnectCredential.h>
#include <Preferences.h>
#include <SPI.h>
#include <EthernetENC.h>

/*** Include files ***/
// All files are stored in flash memory
#define PARAM_FILE "/param.json"
#define AUX_MQTTSETTING "/mqtt_settings"
#define AUX_MQTTSETTING_CLEAR "/mqtt_settings_clear"
#define AUX_MQTTSAVE "/mqtt_save"
#define AUX_MQTTCLEAR "/mqtt_clear"

/*** Redifine existing functions ***/
#define GET_CHIPID()  ((uint16_t)(ESP.getEfuseMac()>>32))

/*** Create memory object ***/
Preferences memory;

/*** Initialization of object and variables ***/
AutoConnect Portal;
AutoConnectConfig Config;
WiFiClient esp32Client;
EthernetClient ethClient;
PubSubClient mqttClient(ethClient);
station_config_t AC_credential_config;
station_config_t *AC_credential = &AC_credential_config;
IPAddress addr;
String mqttBrokerIp;
String container_ID;
String esp32Type;
byte container_ID_length = 10;
byte mac[] = { random(2), random(2), random(2), random(2), random(2), random(2) };

/***** Sensors Definitions *****/
byte dht_1R_pin = 13;
byte dht_1L_pin = 21; // D12 cannot be used. Flash error
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

// Filter Settings
uint8_t filter; // = 0; // Set filter to use: 0=none, 1=exponential, 2=kalman
uint8_t exp_alpha; // = 40; // Smooth Constant in Exponencial Filter. uint8_8 divided by 100
uint8_t kalman_noise; // = 50; // Noise in Kalman Filter. uint8_8 divided by 100
float kalman_err; // = 1; // Error in Kalman Filter. uint8_8 divided by 100

// Temporal variables
unsigned long update_time;
uint8_t update_constant; // Update time every X seconds

/*** Name functions ***/
// AutoConnect Functions
void loadSettings(bool rst);
String loadParams(AutoConnectAux& aux, PageArgument& args);
String clearParams(AutoConnectAux& aux, PageArgument& args);
String saveParams(AutoConnectAux& aux, PageArgument& args);
bool loadAux(const String auxName); // OK
void setup_AutoConnect(AutoConnect &Portal, AutoConnectConfig &Config);
bool testContainerId(String ID);
// Memory
void memorySetup();
void memorySave(uint8_t par);
void memoryGet(uint8_t par);
// Sensors
bool setExponentialFilter(uint8_t alpha);
float exponential_filter(uint8_t alpha, float t, float t_1);
bool setKalmanFilter(uint8_t noise);
float kalman_filter(float t, float t_1);
TempAndHumidity getData(DHTesp &dht, TempAndHumidity &input_data);
void updateData();
void setupSensors();
// MQTT
bool mqttConnect();
void mqttPublish(String top, String msg);
void callback(char* topic, byte* message, unsigned int length);
void sendData();
void resetCredentials();

void setup() {
  Serial.begin(115200);
  Serial.println(F("Initial setup"));
  Serial.print(F("Please wait"));
  for(int i=0;i<30;i++){
    Serial.print(".");
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
  
  if (addr.fromString(mqttBrokerIp) && (esp32Type=="front" || esp32Type=="center" || esp32Type=="back") && container_ID.length()==container_ID_length ){
    Serial.println("Parameters are ok");
    mqttClient.setServer(mqttBrokerIp.c_str(), 1883);
    mqttClient.setCallback(callback); // Function to execute actions with entries of mqtt messages
  }
  else {
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
  }
  
  Ethernet.begin(mac);
  // Allow the hardware to sort itself out
  delay(1500);
  
  Serial.println("TEST");
  memorySetup();
  setupSensors();
  update_time = millis();  
}

void loop() {
    Portal.handleClient();
    
    if (!mqttClient.connected()) {
      mqttConnect();
    }
    
    if(millis()-update_time>update_constant*1000){
      updateData();
      update_time = millis();
    }
    
    mqttClient.loop();
}
