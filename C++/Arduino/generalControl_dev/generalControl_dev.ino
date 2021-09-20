/* PENDIENTE:
 *  1) Multiplexores:
 *    1.1 Funciones serial para guardar orden de los estados
 *
 *  8) TROUBLESHOOTING
 *    8.1) dynamicMem Congruencia de punteros e índices
 *    8.2) Comunicación Serial
 *    8.3) Mapeo de multiplexores correcto
 */

/*** Include Libraries ***/
#include <commonStructures.h>
#include <dynamicMemory.h>
#include <fan.h>
#include <processControl.h>
#include <irrigationController.h>
#include <sensor.h>
#include <solenoid.h>
#include <mux.h>

//#define TEST true   // Define test mode to integrate or test new functions/classes

/*** Special Functions ***/
//void(* resetFunc) (void) = 0; //declare reset function @ address 0

/*** Objects ***/
dynamicMem myMem;                     // Dynamic Memory
systemFan * myFans;                   // Fan Control pointer
irrigationController * myIrrigation;  // Compressor and Recirculation Control pointer
systemValves * myValves;              // Solenoid control pointer
sensorController * mySensors;         // Sensor Control pointer
muxController * myMux;                // Multiplexer Control Pointer
processControl irrigationState;       // Indicate irrigation stage
processControl controlState;          // Indicate actions to be taken to reach desire operation coditions

// Structures
basicConfig bconfig;    // Basic config
pressureConfig pconfig; // Pressure config
sensorConfig sconfig;   // Sensor config
logicConfig lconfig;    // Logic config
dateTime dTime;         // Time info

// auxVariables
bool memoryReady = false;

// Serial comunication
String inputstring = "";
bool input_string_complete = false;

// Dev & Testing
#ifndef TEST
bool test = true;
#else
bool test = false;
#endif

/* REDEFINE */
// Control action that has to be executed at least once when booting/rebooting
bool firstHourUpdate = false;     // Update first time we get the time
bool bootParameters = false;      // Update first time we get initial parameters
float h2oConsumption;             // Store water constumption per irrigation cycle
float initialWeight;              // Get the difference in weight to know water consumption
bool emergencyFlag = false;       // Flag to set an emergency
bool emergencyPrint = false;      // Flag to know when emergency message is printed
bool rebootFlag = false;          // Flag to know when we need to reboot
bool rebootPrint = false;         // Flag to know when reboot message is printed
unsigned long rebootTimer;        // Timer start counting when rebootFlag true

void setup() {
  // Initialize serial
  Serial.begin(115200);

  if(test){
    bool nothing;
  }
  
  else{
    Serial.println(F("info,Setting up Device..."));
  
    // Initialize timers
    rebootTimer = millis();
  
    // Initialize dynamic memory
    memoryReady = myMem.begin();
  
    // Charge eeprom parameters for each fan and load it
    // Turn on each fan with delay (to avoid EMI)
    if(memoryReady){
      bconfig = myMem.getConfig_basic();    // Get basic config
      pconfig = myMem.getConfig_pressure(); // Get pressure config
      sconfig = myMem.getConfig_sensors();  // Get sensor config
      lconfig = myMem.getConfig_logic();    // Get logic config
  
      // Initialize fan control
      myFans = new systemFan(bconfig.floors, myMem);
  
      // Initialize irrigation control
      myIrrigation = new irrigationController(lconfig);
  
      // Initialize sensorController
      mySensors = new sensorController(sconfig, myMem);
  
      // Initialize solenoid control
      myValves = new systemValves(bconfig, myMem);

      // Initialize multiplexer control
      myMux = new muxController(bconfig.mux, myMem); 
      
      // Configuration for DC Multiplexers (only solenodis)
      for (int i = 0; i<bconfig.floors; i++) {
         for(int j=0; j<bconfig.solenoids; j++){
            int orderA = myMem.read_stateOrder(1, i*bconfig.solenoids*bconfig.regions + j);
            int orderB = myMem.read_stateOrder(1, i*bconfig.solenoids*bconfig.regions + j + bconfig.solenoids);
            myMux->_myMux[0]->addState(myValves->_floor[i]->_regA[j]->_State, orderA);
            myMux->_myMux[0]->addState(myValves->_floor[i]->_regB[j]->_State, orderB);
         }
      }
      // Add 4 Extra Solenoids
      int readPos = bconfig.floors*bconfig.solenoids*bconfig.regions;
      for(int i = 0; i<AUX_ACTUATORS; i++){
        int order = myMem.read_stateOrder(1, readPos+i);
        if(i==0) myMux->_myMux[0]->addState(myIrrigation->_VAirL, order);
        else if(i==1) myMux->_myMux[0]->addState(myIrrigation->_VConnectedL, order);
        else if(i==2) myMux->_myMux[0]->addState(myIrrigation->_VFreeL, order);
        else if(i==3) myMux->_myMux[0]->addState(myIrrigation->_PumpL, order);
        else Serial.println(F("error,main setup(): Cannot add extra solenoid"));
      }
      
      myMux->_myMux[0]->orderMux();

      // Configuration fo AC Multiplexers (only fans)
      for (int i = 0; i<bconfig.floors; i++) {
          int order1 = myMem.read_stateOrder(2, i);
          int order2 = myMem.read_stateOrder(2, i+bconfig.floors);
          myMux->_myMux[1]->addState(myFans->_fan[i]->_State, order1);
          myMux->_myMux[1]->addState(myFans->_fan[i]->_State, order2);
      }

      myMux->_myMux[1]->orderMux();
      
      Serial.println(F("info,Device is ready"));
    }
    else Serial.println(F("critical,Memory: Please provide the memory configuration missed to be able to start"));
  }
}

void loop() {
  if(memoryReady){
    // Update objects and variables
    myFans->run();
    myValves ->run();
    myIrrigation->update();
    mySensors->read();

    // Run main control
    mainControl();

    // Update outputs
    myMux->update();
  }

  // Reboot manage
  if(!rebootPrint && rebootFlag){
    rebootPrint = true;
    Serial.println(F("warning,System will reboot in 5 minutes to recharge modified parameters"));
  }
  
  if(rebootFlag && millis()-rebootTimer>REBOOT_TIME){
    rebootTimer = millis();
    Serial.println(F("warning,REBOOTING SYSTEM"));
  }

  // Emergency manage
  if(emergencyPrint!=emergencyFlag){
    if(emergencyFlag) Serial.println(F("warning,System in emergency mode"));
    else Serial.println(F("warning,System leaving in emergency mode"));
    emergencyPrint = emergencyFlag;
  }
}
