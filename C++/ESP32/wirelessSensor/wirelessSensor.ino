/* Add:
 *  - Check NVS data storage does not get corrupted by OTA
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
#include <esp_now.h>

/*** Include files ***/
// All files are stored in flash memory
#define PARAM_FILE "/param.json"
#define AUX_SETTING "/settings"
#define AUX_SETTING_CLEAR "/settings_clear"
#define AUX_SAVE "/param_save"
#define AUX_CLEAR "/param_clear"

/*** Redifine existing functions ***/
#define GET_CHIPID()  ((uint16_t)(ESP.getEfuseMac()>>32))

/*** Create memory object ***/
Preferences memory;

/*** Initialization of object and variables ***/
AutoConnect Portal;
AutoConnectConfig Config;
WiFiClient esp32Client;
PubSubClient mqttClient(esp32Client);
station_config_t AC_credential_config;
station_config_t *AC_credential = &AC_credential_config;
IPAddress addr;
String mqttBrokerIp;
String container_ID;
String esp32Type;
String MACstr; // MAC in string format
const uint8_t container_ID_length = 10;
uint8_t mqttAttempt = 0;

/***** Sensors Definitions *****/
uint8_t dht_1R_pin = 13;
uint8_t dht_1L_pin = 21; // D12 cannot be used. Flash error
uint8_t dht_2R_pin = 14;
uint8_t dht_2L_pin = 27;
uint8_t dht_3R_pin = 26;
uint8_t dht_3L_pin = 25;
uint8_t dht_4R_pin = 33;
uint8_t dht_4L_pin = 32;

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

/****** Variables to use esp-now communication ******/
// It is necesary that all the structures used to transmit info via ESP-NOW have differentes sizes
// Environmental structure (64 bytes)
typedef struct env_message {
    TempAndHumidity R1;
    TempAndHumidity L1;
    TempAndHumidity R2;
    TempAndHumidity L2;
    TempAndHumidity R3;
    TempAndHumidity L3;
    TempAndHumidity R4;
    TempAndHumidity L4;
} env_message;

// Door structure (8 bytes)
typedef struct door_message {
    bool R1;
    bool L1;
    bool R2;
    bool L2;
    bool R3;
    bool L3;
    bool R4;
    bool L4;
} door_message;

// Identifier structure (32 bytes)
typedef struct identifier_message {
  String container_ID;
  String esp32Type;
  uint8_t mqttBrokerIp[4];
  bool help;
  bool active;
} identifier_message;

// ESP-NOW commands
typedef enum nowState{
  SEND_DATA,                // Send sensor data 
  SEND_DOOR,                // Send door state data
  SEND_ID,                  // Send MQTT configuration
  UPDATE_CONSTANT,          // Update time constant
  NOT_FILTER,               // Set not filter for sensor data
  SET_EXPONENTIAL_FILTER,   // Set an exponential filter for sensor data
  SET_KALMAN_FILTER,        // Set a Kalman filter got sensor data
  FORGET_MAC,               // Forget MAC address
  REBOOT,                   // Reboot esp
  HARD_RESET                // Delete all WiFi credential then restart esp
};

// Incoming message (12 bytes)
typedef struct incoming_message {
    nowState cmd;
    uint8_t param;
    float notNeeded;
} incoming_message;

env_message envData;            // To send
door_message doorData;          // To send
identifier_message idData;      // To send/recieve
incoming_message incData;       // To recieve
bool activeESPNOW = false;      // Variable to know if ESPNOW is active
esp_now_peer_info_t masterInfo; // Peer info
esp_now_peer_info_t auxPeer;    // Aux peer to send info to potential Masters

// Filter Settings
uint8_t filter; // = 0; // Set filter to use: 0=none, 1=exponential, 2=kalman
uint8_t exp_alpha; // = 40; // Smooth Constant in Exponencial Filter. uint8_8 divided by 100
uint8_t kalman_noise; // = 50; // Noise in Kalman Filter. uint8_8 divided by 100
float kalman_err; // = 1; // Error in Kalman Filter. uint8_8 divided by 100

// Temporal variables
unsigned long update_time;
uint8_t update_constant; // Update time every X seconds

// Portal aux variable handler
bool portalAux = false;
bool startPortalAux = false;

/*** Name functions ***/
// AutoConnect Functions
void loadSettings(bool check);
String loadParams(AutoConnectAux& aux, PageArgument& args);
String clearParams(AutoConnectAux& aux, PageArgument& args);
String saveParams(AutoConnectAux& aux, PageArgument& args);
bool loadAux(const String auxName); // OK
void setup_AutoConnect(AutoConnect &Portal, AutoConnectConfig &Config);
bool testContainerId(String ID);
bool testMAC(String MAC);
bool startCP(IPAddress ip);
void startPortal(bool start);
// Memory
void memorySetup();
void memorySave(uint8_t par);
void memoryGet(uint8_t par);
// ESP-NOW
void parseBytes(const char* str, char sep, byte* bytes, int maxBytes, int base);
void saveConfig(uint8_t ip[4], String type, String id, String mac_add, const uint8_t *mac); //DEBUG function
void clearMAC(const uint8_t *mac); // DEBUG Delete MAC address from filesystem
void OnDataSent(const uint8_t *mac, esp_now_send_status_t status);
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len);
void InitESPNow();
void DeinitESPNow();
void registerPeer(String MAC);
void unregisterPeer(String MAC);
void debugNowSend(esp_err_t result);
void debugNow(esp_err_t result, String action);
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
  
  loadAux(AUX_SETTING);
  loadAux(AUX_SETTING_CLEAR);
  loadAux(AUX_SAVE);
  loadAux(AUX_CLEAR);

  setup_AutoConnect(Portal, Config);
  
  AutoConnectAux* setting = Portal.aux(AUX_SETTING);
  if (setting) {
    PageArgument  args;
    loadParams(*setting, args);
    
    Portal.on(AUX_SETTING, loadParams);
    Portal.on(AUX_CLEAR, clearParams);
    Portal.on(AUX_SAVE, saveParams);
  }
  else{
    Serial.println(F("aux. load error"));
  }
  
  if (Portal.begin()) {
    Serial.print(F("WiFi "));
    Serial.println("connected:" + WiFi.SSID());
    Serial.println("IP:" + WiFi.localIP().toString());
  }
  else {
    Serial.print(F("WiFi "));
    Serial.println("connection failed:" + String(WiFi.status()));
  }
  
  memorySetup();
  setupSensors();
  update_time = millis();  
}

void loop() {
    if (WiFi.status() == WL_CONNECTED) { // If connected to WiFi
      if (startPortalAux){ // If Portal is enable
        portalAux = false;
        startPortalAux = false;
        DeinitESPNow();
        Serial.println(F("Disable Portal"));
        WiFi.disconnect(true);
      }else { // If portal disable manage MQTT 
        if (mqttAttempt<=3){
          if (!mqttClient.connected()) { // If MQTT disconnected retry connection 
            mqttAttempt++;
            mqttConnect(); 
          }
          mqttClient.loop();  
        } else { // If MQTT cannot connect to broker then initialize AP
          portalAux = true;
          Serial.println(F("Attempts to connect MQTT exceed"));
          Serial.println(F("Enable Portal"));
          WiFi.disconnect(true);
        }
      }
    }
    else { // If WiFi disconnected then start Portal or disable the portal
      if (!startPortalAux){
        Serial.println(F("Trying connection"));
        startPortal(portalAux);
        if (portalAux) { portalAux = false; }  
      }
    }
    
    if(millis()-update_time>update_constant*1000){ // Update sensor data
      updateData();
      update_time = millis();
    }
    
    Portal.handleClient(); // Handle Portal
}
