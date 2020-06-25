/*** Include Libraries ***/
#include <DHTesp.h>
#include <esp_now.h>
#include <WiFi.h>
#include <Preferences.h>

/*** General Definitions ***/
#define MAX_PEER_NUMBER 20

/*** Create memory object ***/
Preferences memory;

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

// Outcoming message (12 bytes)
typedef struct outcoming_message {
    nowState cmd;
    uint8_t param;
    float notNeeded;
} outcoming_message;

// Identifier structure (36 bytes)
typedef struct peerSaved {
  esp_now_peer_info_t peerInfo;
  String container_ID;
  String esp32Type;
  uint8_t mqttBrokerIp[4];
} peerSaved;

// Declarate structures
env_message envData;                    // To recieve
door_message doorData;                  // To recieve
identifier_message idData;              // To send/recieve
outcoming_message outData;              // To send
peerSaved slavesInfo[MAX_PEER_NUMBER];  // Peer info (internal use only)
uint8_t slavesNumber = 0;               // Number of slaves currently saved (internal use only)
bool slaveActive[MAX_PEER_NUMBER];      // Aux variable to know if the slave is actually active

/*** Name functions ***/
// ESPNOW Functions
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);     // Callback when data is sent
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len);  // Callback when data is recieved
void InitESPNow();                                                          // Initialize ESP-NOW
void sendID(const uint8_t *mac, bool help, bool active);                    // Send ID data to ESP slave with mac address
void sendID(uint8_t index, bool help, bool active);                         // Send ID data to ESP slave with index 
bool slaveExist(const uint8_t *mac);                                        // Check if slaves already exists
void deactiveSlave();                                                       // Deactive slave to avoid trying communicating with them
void debugNowSend(esp_err_t result);                                        // Print the result of the esp_now_send()
// Memory Functions
void saveNewSlave(const uint8_t *mac);                                      // Save new slave in NVS
void chargeSlaves();                                                        // Charge slaves from NVS into variables
void memoryClean();                                                         // Clean up NVS and Preferences
// nowMaster
void printDataSizes();

void printDataSizes() { // Function to know the size in bytes from some data types and structures
  Serial.println(sizeof(envData));              // 64 bytes
  Serial.println(sizeof(doorData));             // 8  bytes
  Serial.println(sizeof(idData));               // 32 bytes
  Serial.println(sizeof(outData));              // 12  bytes
  Serial.println(sizeof(peerSaved));            // 64 bytes
  Serial.println(sizeof(bool));                 // 1  byte
  Serial.println(sizeof(uint8_t));              // 1  byte
  Serial.println(sizeof(nowState));             // 4 bytes
  Serial.println(sizeof(String));               // 12 bytes
  Serial.println(sizeof(esp_now_peer_info_t));  // 36 bytes
}
  
void setup() {
  Serial.begin(115200);
  //printDataSizes();   // Debug data and structures sizes
  WiFi.mode(WIFI_STA);  // Set device in STA mode to begin with
  Serial.println(F("[ESP-NOW] Master starting..."));
  Serial.print(F("[ESP-NOW] STA MAC: ")); Serial.println(WiFi.macAddress()); // This is the mac address of the Master in Station Mode
  InitESPNow(); // Init ESPNow with a fallback logic
  Serial.println(F("[ESP-NOW] Master initialized"));
}

uint8_t run1 = 0;

void loop() {
  uint8_t mac[] = {0X00, 0X00, 0X00, 0X00, 0X00, 0X00}; // Aux variable to save MAC
  memcpy(mac, slavesInfo[0].peerInfo.peer_addr, 6);
  
  if(run1==0){
    run1++;
    esp_err_t result = esp_now_send(mac, (uint8_t *) &idData, sizeof(idData)); // 32 bytes
    debugNowSend(result);
  } else if (run1==1){
    run1++;
    esp_err_t result = esp_now_send(mac, (uint8_t *) &envData, sizeof(envData)); // 64 bytes
    debugNowSend(result);
  } else if (run1==2){
    run1++;
    esp_err_t result = esp_now_send(mac, (uint8_t *) &doorData, sizeof(doorData)); // 8 bytes
    debugNowSend(result);
  } else if (run1==3){
    run1++;
    esp_err_t result = esp_now_send(mac, (uint8_t *) &outData, sizeof(outData)); // 8 bytes
    debugNowSend(result);
  } 
  delay(1000);
}
