/*** Include Libraries ***/
#include <Wire.h>
#include <EEPROM.h>
#include <actuator.h>
#include <day.h>
#include <controllerHVAC.h>
#include <sensor.h>
#include <compressorController.h>
#include <irrigationController.h>
#include <recirculationController.h>
#include <processControl.h>

/*** Multiplexor Definitions ***/
const uint8_t ds = 31; // Data
const uint8_t stcp = 29; // Latch
const uint8_t shcp = 27; // Clock
const uint8_t mR = 33; // Relay

/*** Emergency Stop***/
const uint8_t emergencyUser = 35;
bool emergencyState = false;
bool emergencyButton = false;
unsigned long buttonTime;
const unsigned long debounceTime = 1000;
bool debounce = false;

/*** Pressure sensors definitions ***/
// analogSensor object(analogPin, type)
analogSensor pressureSensorNutrition(A15, 0);
analogSensor pressureSensorTank(A14, 1);
analogSensor pressureSensorWater(A13, 2);

/*** UltraSonic Sensors ***/
// UltraSonic object(pin, type)
UltraSonic US0(34, 40, 0);
UltraSonic US1(26, 48, 1);
UltraSonic US2(28, 46, 2);
UltraSonic US3(30, 44, 3);
UltraSonic US4(32, 42, 4);
UltraSonic US5(22, 52, 5);
UltraSonic US6(36, 38, 6);

/*** Water Sensors ***/
// waterSensor object(pin, type)
waterSensor checkWaterIrrigation(23, 0);
waterSensor checkWaterEvacuation(25, 1);
bool checkRecirculation = true;
bool enableWaterSensor = false;

/*** HVAC Controller object ***/
// controllerHVAC object(Mode, Fan Mode)
controllerHVAC HVAC(OFF_MODE , AUTO_FAN);

/*** Compressor Controller object ***/
// controllerHVAC object(tankLogic, nutritionLogic, waterLogic)
// Parameters are the logic of the solenoids where LOW is normally closed
compressorController Compressor(LOW, LOW, LOW);

/*** Recirculation Controller ***/
recirculationController Recirculation;

/*** Irrigation Controller ***/
// irrigationController(uint8_t cyclesPerDay, uint8_t initialHour, uint8_t ord1, uint8_t ord2, uint8_t ord3, uint8_t ord4, uint8_t per1, uint8_t per2, uint8_t per3, uint8_t per4 )
irrigationController Irrigation(4, 0, 0, 1, 2, 3, 25, 25, 25, 25);

/*** Actuators ***/
// solenoidValve object
// LED object(floor, section)
// Actuator object(type, floor, time_On, timeOff);
// asyncActuator(type)

// 1st floor
solenoidValve EV1A1; solenoidValve EV1A2; solenoidValve EV1A3; solenoidValve EV1A4;
solenoidValve EV1B1; solenoidValve EV1B2; solenoidValve EV1B3; solenoidValve EV1B4;
LED_Mod L1S1(0, 0); LED_Mod L1S2(0, 1); LED_Mod L1S3(0, 2); LED_Mod L1S4(0, 3);
Actuator IFan1 (0, 0, 290, 300); Actuator OFan1 (1, 0, 100, 250); Actuator VFan1 (2, 0, 120, 180);
Actuator VHum1 (3, 0, 5, 1200);

// 2nd floor
solenoidValve EV2A1; solenoidValve EV2A2; solenoidValve EV2A3; solenoidValve EV2A4;
solenoidValve EV2B1; solenoidValve EV2B2; solenoidValve EV2B3; solenoidValve EV2B4;
LED_Mod L2S1(1, 0); LED_Mod L2S2(1, 1); LED_Mod L2S3(1, 2); LED_Mod L2S4(1, 3);
Actuator IFan2 (0, 1, 290, 300); Actuator OFan2 (1, 1, 160, 250); Actuator VFan2 (2, 1, 120, 180);
Actuator VHum2 (3, 1, 5, 1200);

// 3rd floor
solenoidValve EV3A1; solenoidValve EV3A2; solenoidValve EV3A3; solenoidValve EV3A4;
solenoidValve EV3B1; solenoidValve EV3B2; solenoidValve EV3B3; solenoidValve EV3B4;
LED_Mod L3S1(2, 0); LED_Mod L3S2(2, 1); LED_Mod L3S3(2, 2); LED_Mod L3S4(2, 3);
Actuator IFan3 (0, 2, 290, 300); Actuator OFan3 (1, 2, 190, 250); Actuator VFan3 (2, 2, 120, 180);
Actuator VHum3 (3, 2, 5, 1200);

// 4th floor
solenoidValve EV4A1; solenoidValve EV4A2; solenoidValve EV4A3; solenoidValve EV4A4;
solenoidValve EV4B1; solenoidValve EV4B2; solenoidValve EV4B3; solenoidValve EV4B4;
LED_Mod L4S1(3, 0); LED_Mod L4S2(3, 1); LED_Mod L4S3(3, 2); LED_Mod L4S4(3, 3);
Actuator IFan4 (0, 3, 290, 300); Actuator OFan4 (1, 3, 220, 250); Actuator VFan4 (2, 3, 120, 180);
Actuator VHum4 (3, 3, 5, 1200);

// asyncActuator
asyncActuator IrrigationKegsH2O(0); // eV to irrigate H2O
asyncActuator IrrigationKegsNutrition(1); // eV to irrigate Nutrition

/*** Day Objects ***/
// MultiDay object(daysPerDay, light%, initHour)
MultiDay day1(3, 75, 0);
MultiDay day2(3, 75, 2);
MultiDay day3(3, 75, 4);
MultiDay day4(3, 75, 6);
fixDay fixD; // Fix Day (Provisional)

/*** Process Control ***/
processControl IPC(0); // Initial Preconditions Control
processControl MPC(1); // Middle Preconditions Control
processControl CC(2); // Comunication Control

/*** Auxiliar Variables ***/
// Control irrigation process
uint8_t irrigationStage = 0; // Defautl state

// Control when update solutionConsumption and h2oConsumption
bool emergency = false;

// Control pressure
float max_pressure = 100; // Default Maximum Pressure in the system (psi)
float min_pressure = 80; // Minimun Pressure in the system to start a new irrigation cycle (psi)
float critical_pressure = 60; // Default Critical Pressure in the system (psi)
float free_pressure = 15; // Set pressure to free air in kegs

// Control Solution and refill process
/* The next variables need to be register in raspberry in case that arduino turn off */
uint8_t lastSolution = 250; // Last solution to be irrigated, by default value of 250 is setted but this value does not match with a solution
float solutionConsumption = 50; // Default consumption in stage 1 of irrigation
float h2oConsumption = 50; // Default consumption in stage 4 of irrigation
// __VolKnut, __VolKh2o in recirculationControllerClass has to be init at boot
/* The previous variables need to be register in raspberry in case that arduino turn off */

// Control Actions that has to be taken the first time an irrigation valve is activated
bool firstValve = HIGH;

// Control ReOrder when day/night change
uint8_t night = 0;

// Control action that has to be executed at least once when booting/rebooting
bool firstHourUpdate = false;
bool bootParameters = false;
unsigned long bootTimer;

// Serial comunication
String inputstring = "";
bool input_string_complete = false;

// DateTime Info
uint8_t dateHour;
uint8_t dateMinute;

// Multiplexer timer
unsigned long multiplexerTime;

// logSens timer
unsigned long logSensTime;

/*** Name functions ***/
// EEPROM
void clean_EEPROM();
void print_EEPROM();
void save_EEPROM(int pos, int val);
void save_EEPROM(int pos, float val);
void solenoidSaveTimeOn(int fl, int reg, int sol, int val);
void solenoidSaveCycleTime(int val);
void chargeSolenoidParameters(int sol);
void multidaySave(int fl, int cyclesNumber, float lightPercentage, float initHour);
void chargeMultidayParameters();
void regionSave(int fl, int reg);
void chargeLedRegion();
void chargeSolenoidRegion();
void irrigationSave(int cyclesPerDay, int initialHour);
void solutionSave(int sol, int order, int percent);
void chargeIrrigationParameters();
void analogSaveFilter(int Type, int filt, float filterParam);
void analogSaveModel(int Type, float a, float b, float c);
bool chargeAnalogParameters(int Type);
void ultrasonicSaveFilter(int Type, int filt, float filterParam);
void ultrasonicSaveModel(int Type, int model, float modelParam, float height);
bool chargeUltrasonicParameters(int Type);
void pressureSave(int Type, float Press);
void chargePressureParameter();
// async
void solenoidValverunAll();
void async();
void asyncIrrigation();
void initialPreconditions(void (*ptr2function)());
void middlePreconditions(void (*ptr2function)());
void doNothing();
void startIrrigation();
void startMiddleIrrigation();
void irrigationEmergency();
// Multiplexers
void allMultiplexerOff();
void codification_Multiplexer();
void multiplexerRun();
// Serial Communication
void requestSolution();
void updateSystemState();
void serialEvent();
// Setup
void solenoid_setup();
void sensors_setup();
void enableSolenoid(int fl, int reg);
void enableLED(int fl, int reg);
bool isDayInThatSolenoid(uint8_t solenoid);
uint8_t inWhatFloorIsNight();
void updateDay();
void substractSolutionConsumption(bool updateConsumption = false);
void substractWaterConsumption(bool updateConsumption = false);
void rememberState(int ipc, int mpc, int pumpIn, float missedNut, float missedH2O);
// Aux Functions
void emergencyStop();
void boot();
float litersRequire();
void logSens();

void setup() {
  // Initialize Serial
  Serial.begin(115200); Serial.println(F("Setting up Device..."));
  
  // Define INPUTS&OUTPUTS
  solenoidValve::flowSensorBegin();
  recirculationController::flowSensorBegin();
  
  pinMode(stcp, OUTPUT);
  pinMode(shcp, OUTPUT);
  pinMode(ds, OUTPUT);
  pinMode(mR, OUTPUT);
  pinMode(emergencyUser, INPUT_PULLUP);
  digitalWrite(mR, !LOW); // Turn off Relays
  allMultiplexerOff(); // Set multiplexors off
  
  // Initialize objects
  sensors_setup(); // Initialize sensors
  Actuator::beginAll(); // Initialize actuators
  solenoid_setup(); // Initialize solenoids valves
  // Initialize recirculationController
  Recirculation.begin(US0, US1, US2, US3, US4, US5, US6);
  Serial.flush();
  
  // Charge EEPROM parameters saved
  chargeMultidayParameters(); // For multiDay
  chargeLedRegion(); // For LED´s
  chargeSolenoidRegion(); // For solenoidValves
  chargeIrrigationParameters(); // For irrigation control
  chargePressureParameter(); // For pressure control
  
  // Set initial state
  inputstring.reserve(30); // Reserve 30 bytes for serial strings
  codification_Multiplexer(); // Initialize Multiplexers
  // Initialize counters
  multiplexerTime = millis();
  logSensTime = millis();
  buttonTime = millis();
  bootTimer = millis();
  
  // Finished
  Serial.println(F("Device Ready"));
  Serial.flush();
  delay(1000);
  
  if(!digitalRead(emergencyUser)){ // Enable Relays
    digitalWrite(mR, !HIGH); // Turn on multiplexors
  }
  
  // Testing settings 
  //solenoidValve::enableGroup(true); delete
}

void loop() {
  /*** Request boot info to raspberry ***/
  boot();
  
  /*** Emergency Conditions ***/
  emergencyStop();
  /*** Sensors ***/
  analogSensor::readAll(); // Take reading from all the pressure sensors every 100ms
  UltraSonic::readAll(); // Take reading from all the ultrasonics sensors every 100ms
  waterSensor::readAll(); // Take reading from all the water sensors every 50ms
  /*** Air Conditioner ***/
  HVAC.run();
  /*** Recirculation System ***/
  Recirculation.run(checkRecirculation, checkWaterEvacuation.getState());
  /*** Actuators ***/
  Actuator::runAll();
  /*** Irrigation Routine ***/
  solenoidValverunAll();
  async(); // Check the states with async Functions
  /*** Multiplexers & Relays states ***/
  multiplexerRun();
  /*** log&debug***/
  logSens();
  
  // Testing code
}
