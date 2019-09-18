/* Add:
*/

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
PubSubClient mqttClient(esp32Client);
station_config AC_credential_config;
station_config *AC_credential = &AC_credential_config;
IPAddress addr;
String mqttBrokerIp;
String container_ID;
String esp32Type;
byte container_ID_length = 10;

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
