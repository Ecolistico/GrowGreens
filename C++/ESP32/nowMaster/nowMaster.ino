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
    float notNeeded; // This variable just assure that the structure has different size from the others
} outcoming_message;

// Identifier structure (36 bytes)
typedef struct peerSaved {
  esp_now_peer_info_t peerInfo;
  String container_ID;
  String esp32Type;
  uint8_t mqttBrokerIp[4];
} peerSaved;

// Declarate structures
env_message envData;                        // To recieve
door_message doorData;                      // To recieve
identifier_message idData;                  // To send/recieve
outcoming_message outData;                  // To send
peerSaved slavesInfo[MAX_PEER_NUMBER];      // Peer info (internal use only)
uint8_t slavesNumber = 0;                   // Number of slaves currently saved (internal use only)
bool slaveActive[MAX_PEER_NUMBER];          // Aux variable to know if the slave is actually active
peerSaved notSlavesInfo[MAX_PEER_NUMBER*3]; // Peer info for the non desire AP (internal use only)
uint8_t notSlavesNumber = 0;                // Number of non desire slaves currently saved (internal use only)
esp_now_peer_info_t auxPeer;                // Aux peer to send info to potential Slaves

// Serial comunication
String inputstring = "";
bool input_string_complete = false;

// Own ID
String container_ID;
const uint8_t container_ID_length = 10;

// Timers
unsigned long scanTimmer = 0; // Scann for slaves
unsigned long askTimmer = 0;  // Ask current variables to slaves

// Just first time variable
uint8_t start = false;

/*** Name functions ***/
// ESPNOW Functions
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);     // Callback when data is sent
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len);  // Callback when data is recieved
void InitESPNow();                                                          // Initialize ESP-NOW
void sendID(const uint8_t *mac, bool help, bool active);                    // Send ID data to ESP slave with mac address
void sendID(uint8_t index, bool help, bool active);                         // Send ID data to ESP slave with index 
bool slaveExist(const uint8_t *mac);                                        // Check if slaves already exists
bool nonSlaveExist(const uint8_t *mac);                                     // Check if nonSlave already exists
void deactiveSlave();                                                       // Deactive slave to avoid trying communicating with them
void ScanForSlave();                                                        // Look for AP to find new slaves
void checkActiveSlave();                                                    // Check if some of the slave are active
void askSlaves();                                                           // Ask varaible to the active slaves
void debugNowSend(esp_err_t result);                                        // Print the result of the esp_now_send()
void debugNow(esp_err_t result, String action);                             // Print the result of some esp_now()
// Memory Functions
void saveNewSlave(const uint8_t *mac, bool force = false);                  // Save new slave in NVS
void chargeSlaves();                                                        // Charge slaves from NVS into variables
void memoryCleanSlaves(bool reb = true);                                    // Clean up slaves info from NVS and Preferences
void saveID();                                                              // Save container_ID in NVS
void chargeID();                                                            // Charge container_ID from NVS
void memoryCleanID();                                                       // Clean container_ID from NVS
void saveNewNonSlave(const uint8_t *mac);                                   // Save new non-slave in NVS
void chargeNonSlaves();                                                     // Charge non-slaves from NVS into variables
void memoryCleanNonSlaves(bool reb = true);                                 // Clean up non-slaves info from NVS and Preferences
// serial
void serialEvent();
bool testContainerId(String ID);
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
  chargeID();   // Charge system ID
  if(container_ID.length()!=container_ID_length) Serial.println(F("?,id"));  // Ask computer for the ID
  InitESPNow(); // Init ESPNow with a fallback logic
  scanTimmer = millis();
  askTimmer = millis();
  Serial.println(F("[ESP-NOW] Master initialized"));

  checkActiveSlave(); // Check what of the slaves are connected
}

//uint8_t run1 = 0;

void loop() {
  serialEvent(); // Listen for serial events
  
  if(millis()-scanTimmer>60000 && container_ID!="" && container_ID.length()==container_ID_length){ // If we know our ID and its time
    // Look for new slaves each minute
    scanTimmer = millis();
    ScanForSlave();
  }

  if(millis()-askTimmer>60000){
    // Ask every minute the variables
    askTimmer = millis();
    askSlaves();
  }
  
  /* DEBUG
  uint8_t mac[] = {0X00, 0X00, 0X00, 0X00, 0X00, 0X00}; // Aux variable to save MAC
  memcpy(mac, slavesInfo[0].peerInfo.peer_addr, 6);
  // try to stablish communication just first time, to know what peers are active/deactive
  // sendID();
  // Create a method to look what system are we attached to with extra variables like mqttIP, containerID, in order to stablish a new peer only and only if their ID configuration match with ours
  if(run1==0){
    run1++;
    outData.cmd = SEND_DATA;  // 8 bytes
    debugNowSend(esp_now_send(mac, (uint8_t *) &outData, sizeof(outData))); // Send and debug
  } else if (run1==1){
    run1++;
    outData.cmd = SEND_DOOR;  // 8 bytes
    debugNowSend(esp_now_send(mac, (uint8_t *) &outData, sizeof(outData)));
  } else if (run1==2){
    run1++;
    outData.cmd = SEND_ID;  // 8 bytes
    debugNowSend(esp_now_send(mac, (uint8_t *) &outData, sizeof(outData)));
  } else if (run1==3){
    run1++;
    outData.cmd = UPDATE_CONSTANT; // 8 bytes
    outData.param = 6;
    debugNowSend(esp_now_send(mac, (uint8_t *) &outData, sizeof(outData)));
  } else if (run1==4){
    run1++;
    outData.cmd = NOT_FILTER; // 8 bytes
    debugNowSend(esp_now_send(mac, (uint8_t *) &outData, sizeof(outData)));
  } else if (run1==5){
    run1++;
    outData.cmd = SET_EXPONENTIAL_FILTER; // 8 bytes
    outData.param = 50;
    debugNowSend(esp_now_send(mac, (uint8_t *) &outData, sizeof(outData)));
  } else if (run1==6){
    run1++;
    outData.cmd = SET_KALMAN_FILTER; // 8 bytes
    outData.param = 35;
    debugNowSend(esp_now_send(mac, (uint8_t *) &outData, sizeof(outData)));
  } else if (run1==7){
    run1++;
    outData.cmd = HARD_RESET; // 8 bytes
    debugNowSend(esp_now_send(mac, (uint8_t *) &outData, sizeof(outData)));
  } // Missed FORGET_MAC and HARD_RESET 
  
  if(run1<7) delay(1000);
  */
}
