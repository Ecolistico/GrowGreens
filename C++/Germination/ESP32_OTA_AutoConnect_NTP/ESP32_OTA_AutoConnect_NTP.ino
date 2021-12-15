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
#include <EthernetUdp.h>
#include <Wire.h>

/*** Include files ***/
// All files are stored in flash memory
#define PARAM_FILE "/param.json"
#define AUX_MQTTSETTING "/mqtt_settings"
#define AUX_MQTTSETTING_CLEAR "/mqtt_settings_clear"
#define AUX_MQTTSAVE "/mqtt_save"
#define AUX_MQTTCLEAR "/mqtt_clear"

/*** Redifine existing functions ***/
#define GET_CHIPID()  ((uint16_t)(ESP.getEfuseMac()>>32))

// Max buffer size
#define UDP_TX_PACKET_MAX_SIZE 350

/*** Create memory object ***/
Preferences memory;

/*** Initialization of object and variables ***/
AutoConnect Portal;
AutoConnectConfig Config;
WiFiClient esp32Client;
EthernetClient ethClient;
PubSubClient mqttClient(ethClient);
EthernetUDP udp;
station_config_t AC_credential_config;
station_config_t *AC_credential = &AC_credential_config;
IPAddress addr;

//IPAddress ipEnt;
String mqttBrokerIp;
String container_ID;
String esp32Type;
String esp32Floor;
byte container_ID_length = 10;
byte mac[6];
uint8_t mqttAttempt = 0;

/***** Sensors Definitions *****/
byte dht_1_pin = 14;
byte dht_2_pin = 33;
byte dht_3_pin = 27;
byte dht_4_pin = 32;
byte dht_5_pin = 26;
byte dht_6_pin = 25;

/***** Sensors objects *****/
DHTesp dht_1;
DHTesp dht_2;
DHTesp dht_3;
DHTesp dht_4;
DHTesp dht_5;
DHTesp dht_6;

/***** Auxiliar pins *****/
byte update_pin = 13; // Magnetic pin 1 on shield

/* Cozir */
byte pinRX = 16;
byte pinTX = 17;
String inputstring = "";

/***** Sensors auxiliar variables *****/
// Data Structure
typedef struct {
  uint8_t minute_begin;
  uint8_t hour_begin;
  uint8_t minute_end;
  uint8_t hour_end;
} led_config;

union datetime
{ unsigned long epoch;
  byte longBytes[4];
};
datetime myTime;

union floatNumber
{ float number;
  byte longBytes[4];
};
floatNumber temp1;
floatNumber hum1;
floatNumber temp2;
floatNumber hum2;
floatNumber temp3;
floatNumber hum3;
floatNumber temp4;
floatNumber hum4;
floatNumber temp5;
floatNumber hum5;
floatNumber temp6;
floatNumber hum6;

// Sensor and epoch data
TempAndHumidity data_1;
TempAndHumidity data_2;
TempAndHumidity data_3;
TempAndHumidity data_4;
TempAndHumidity data_5;
TempAndHumidity data_6;
unsigned long epoch;
  
// Filter Settings
uint8_t filter; // = 0; // Set filter to use: 0=none, 1=exponential, 2=kalman
uint8_t exp_alpha; // = 40; // Smooth Constant in Exponencial Filter. uint8_8 divided by 100
uint8_t kalman_noise; // = 50; // Noise in Kalman Filter. uint8_8 divided by 100
float kalman_err; // = 1; // Error in Kalman Filter. uint8_8 divided by 100

// LED´s setting
led_config f1;
led_config f2;
led_config f3;
led_config f4;
led_config f5;
led_config f6;
led_config f7;
led_config f8;

// Portal aux variable handler
bool portalAux = false;
bool startPortalAux = false;

// Temporal variables
unsigned long update_time;
uint8_t update_constant; // Update time every X seconds

// NTP Client Variables
const char NTP_REMOTEHOST[] PROGMEM = "ntp.bit.nl";  // NTP server name
const unsigned int NTP_REMOTEPORT = 123;             // NTP requests are to port 123
const unsigned int NTP_LOCALPORT = 8888;             // Local UDP port to use
const unsigned int NTP_PACKET_SIZE = 48;             // NTP time stamp is in the first 48 bytes of the message
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];           // buffer to hold incoming packet,
bool NTP_flag = false;

// I2C
const int i2c_dir = 77; // 0x4D
unsigned long i2c_timer = millis();

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
// Memory
void memorySetup();
void memorySave(uint8_t par);
void memoryGet(uint8_t par);
// NTP
void udpRead();
void udpWrite();
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
bool startCP(IPAddress ip);
// I2C
void i2c_updateData();
void i2c_updateParameters();
void i2c_write4Byte(byte longBytes[4]);
void i2c_requestData();
void i2c_updateConfigData();

void setup() {
  WiFi.mode(WIFI_MODE_STA);
  WiFi.macAddress(mac);
  Serial.begin(115200);
  Wire.begin();
  Serial.println(F("Initial setup"));
  Serial.print(F("Please wait"));
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
  
  if (addr.fromString(mqttBrokerIp) && esp32Type=="germination" && container_ID.length()==container_ID_length && esp32Floor.toInt()<10){
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
  udp.begin(NTP_LOCALPORT);
  
  // Allow the hardware to sort itself out
  delay(1500);
  memorySetup();
  setupSensors();
  update_time = millis();  
  i2c_timer = millis();
  i2c_updateConfigData();
  udpWrite();
}

void loop() {  
    udpRead();
    
    if (!mqttClient.connected() && portalAux==false) { // If MQTT disconnected retry connection 
      mqttConnect(); 
    } else if (portalAux==false) mqttClient.loop();   
   
    if(millis()-update_time>update_constant*1000){ // Update sensor data
      updateData();
      update_time = millis();
    }

    i2c_updateData();
    i2c_requestData();
    
    Portal.handleClient(); // Handle Portal
    Ethernet.maintain();
}
