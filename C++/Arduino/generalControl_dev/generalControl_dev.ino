/* PENDIENTE:
 *  1) EMERGENCY STOP
 *    1.1) Programar paro de emergencia con multiplexors enable/disable
 *    
 *  2) TROUBLESHOOTING
 *    2.1) Mapeo de multiplexores correcto
 *    2.2) Declaración pines sensores es correcto
 *    
 *    BUGS:
 *    * Unexpected changes of variables before calling EEPROM functions in serial communications
 *        solenoid,setTimeOn,7,2,5,11 usually failed
 *        fan,setTimeOn,4,50 usually failed
 *    * EEPROM write(pos, value) write 0 at pos+1
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
#ifdef TEST
bool test = true;
#else
bool test = false;
#endif

// Boot variables
bool firstHourUpdate = false;     // Update first time we get the time
bool bootParameters = false;      // Update first time we get initial parameters
// Aux variables
float h2oConsumption;             // Store water constumption per irrigation cycle
float initialWeight;              // Get the difference in weight to know water consumption
// Emergency Variables
bool emergencyFlag = false;       // Flag to set an emergency
bool emergencyPrint = false;      // Flag to know when emergency message is printed
bool emergencyButtonFlag = false;// Flag to know when emergency button is pressed
// Reboot variables
bool rebootFlag = false;          // Flag to know when we need to reboot
bool rebootPrint = false;         // Flag to know when reboot message is printed
unsigned long rebootTimer;        // Timer start counting when rebootFlag true

void emergencyButtonPressed() {
  if(!mySensors->_mySwitches[0]->getState() && !emergencyButtonFlag) {
    emergencyButtonFlag = true;
    for(int i=0; i<bconfig.mux; i++) myMux->_myMux[i]->enable(false);
  }
  else if(mySensors->_mySwitches[0]->getState() && emergencyButtonFlag){
    emergencyButtonFlag = false;
    for(int i=0; i<bconfig.mux; i++) myMux->_myMux[i]->enable(true);
  }
  
}
void setup() {
  // Initialize serial
  Serial.begin(115200);

  if(test){
    bool nothing;
    /*
    myMem.begin(bconfig, pconfig, sconfig, lconfig); // Begin EEPROM
    //myMem.write(170, 255);
    //for(int i = 126; i<170; i++) myMem.write(i, 255);
    //myMem.clean(); // Set all bytes at 255 in EEPROM
    myMem.print();
    */
  }
  
  else{
    Serial.println(F("info,Setting up Device..."));

    // Initialize timers
    rebootTimer = millis();
  
    // Initialize dynamic memory
    memoryReady = myMem.begin(bconfig, pconfig, sconfig, lconfig);
    
    // Charge eeprom parameters for each fan and load it
    // Turn on each fan with delay (to avoid EMI)
    if(memoryReady){  
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
            int orderA = myMem.read_stateOrder(0, i*bconfig.solenoids*bconfig.regions + j);
            int orderB = myMem.read_stateOrder(0, i*bconfig.solenoids*bconfig.regions + j + bconfig.solenoids);
            myMux->_myMux[0]->addState(myValves->_floor[i]->_regA[j]->_State, orderA);
            myMux->_myMux[0]->addState(myValves->_floor[i]->_regB[j]->_State, orderB);
         }
      }
      // Add 4 Extra Solenoids
      int readPos = bconfig.floors*bconfig.solenoids*bconfig.regions;
      for(int i = 0; i<AUX_ACTUATORS; i++){
        int order = myMem.read_stateOrder(0, readPos+i);
        if(i==0) myMux->_myMux[0]->addState(myIrrigation->_VAirL, order);
        else if(i==1) myMux->_myMux[0]->addState(myIrrigation->_VConnectedL, order);
        else if(i==2) myMux->_myMux[0]->addState(myIrrigation->_VFreeL, order);
        else if(i==3) myMux->_myMux[0]->addState(myIrrigation->_PumpL, order);
        else Serial.println(F("error,main setup(): Cannot add extra solenoid"));
      }

      // Configuration fo AC Multiplexers (only fans)
      for (int i = 0; i<bconfig.floors; i++) {
          int order1 = myMem.read_stateOrder(1, i);
          int order2 = myMem.read_stateOrder(1, i+bconfig.floors);
          myMux->_myMux[1]->addState(myFans->_fan[i]->_State, order1);
          myMux->_myMux[1]->addState(myFans->_fan[i]->_State, order2);
      }

      for(int i = 0; i<bconfig.mux; i++ ) myMux->_myMux[i]->orderMux();

      /*** DEBUG ***/
      ///*
      myMux->_myMux[0]->enable(true);
      myMux->_myMux[1]->enable(true);
      myValves->invertOrder(false); // Start irrigation from floor 8
      myValves->enable(true); // This has to be call when 
      //*/
      /*** DEBUG ***/
      Serial.println(F("info,Device is ready"));
    }
    else Serial.println(F("critical,Memory: Please provide the memory configuration missed to be able to start"));
  }

  // Test Big Pump
  myIrrigation->turnOnPump();
}

void loop() {
  if(memoryReady){
    emergencyButtonPressed(); // Check if emergency button is pressed
    
    // Update objects and variables
    myFans->run();
    myValves ->run();
    myIrrigation->update();
    mySensors->read(); // Scale timeOut killing other process

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
