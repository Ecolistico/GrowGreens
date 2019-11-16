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
// analogSensor object(analogPin, name)
analogSensor pressureSensorNutrition(A15, "Nutrition Pressure");
analogSensor pressureSensorTank(A14, "Tank Pressure");
analogSensor pressureSensorWater(A13, "Water Pressure");

/*** UltraSonic Sensors ***/
// UltraSonic object(pin, name)
UltraSonic US0(34, 40, "Recirculation Level");
UltraSonic US1(26, 48, "Solution 1 Level");
UltraSonic US2(28, 46, "Solution 2 Level");
UltraSonic US3(30, 44, "Solution 3 Level");
UltraSonic US4(32, 42, "Solution 4 Level");
UltraSonic US5(24, 50, "Water Level");
UltraSonic US6(36, 38, "Solution Maker Level");

/*** Water Sensors ***/
// waterSensor object(pin, name)
waterSensor checkWaterIrrigation(23, "Water Irrigation Sensor");
waterSensor checkWaterEvacuation(25, "Water Evacuation Sensor");
bool checkRecirculation = true;

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
// solenoidValve object(name)
// LED object(name, floor, section)
// Actuator object(type, floor, time_On, timeOff);
// asyncActuator(name)

// 1st floor
solenoidValve EV1A1("1A1"); solenoidValve EV1A2("1A2"); solenoidValve EV1A3("1A3"); solenoidValve EV1A4("1A4");
solenoidValve EV1B1("1B1"); solenoidValve EV1B2("1B2"); solenoidValve EV1B3("1B3"); solenoidValve EV1B4("1B4");
LED_Mod L1S1("L1S1", 0, 0); LED_Mod L1S2("L1S2", 0, 1); LED_Mod L1S3("L1S3", 0, 2); LED_Mod L1S4("L1S4", 0, 3);
Actuator IFan1 (0, 0, 30, 150); Actuator OFan1 (1, 0, 30, 150); Actuator VFan1 (2, 0, 30, 150);
Actuator VHum1 (3, 0, 2, 300);

// 2nd floor
solenoidValve EV2A1("2A1"); solenoidValve EV2A2("2A2"); solenoidValve EV2A3("2A3"); solenoidValve EV2A4("2A4");
solenoidValve EV2B1("2B1"); solenoidValve EV2B2("2B2"); solenoidValve EV2B3("2B3"); solenoidValve EV2B4("2B4");
LED_Mod L2S1("L2S1", 1, 0); LED_Mod L2S2("L2S2", 1, 1); LED_Mod L2S3("L2S3", 1, 2); LED_Mod L2S4("L2S4", 1, 3);
Actuator IFan2 (0, 1, 30, 150); Actuator OFan2 (1, 1, 30, 150); Actuator VFan2 (2, 1, 30, 150);
Actuator VHum2 (3, 1, 2, 300);

// 3rd floor
solenoidValve EV3A1("3A1"); solenoidValve EV3A2("3A2"); solenoidValve EV3A3("3A3"); solenoidValve EV3A4("3A4");
solenoidValve EV3B1("3B1"); solenoidValve EV3B2("3B2"); solenoidValve EV3B3("3B3"); solenoidValve EV3B4("3B4");
LED_Mod L3S1("L3S1", 2, 0); LED_Mod L3S2("L3S2", 2, 1); LED_Mod L3S3("L3S3", 2, 2); LED_Mod L3S4("L3S4", 2, 3);
Actuator IFan3 (0, 2, 30, 150); Actuator OFan3 (1, 2, 30, 150); Actuator VFan3 (2, 2, 30, 150);
Actuator VHum3 (3, 2, 2, 300);

// 4th floor
solenoidValve EV4A1("4A1"); solenoidValve EV4A2("4A2"); solenoidValve EV4A3("4A3"); solenoidValve EV4A4("4A4");
solenoidValve EV4B1("4B1"); solenoidValve EV4B2("4B2"); solenoidValve EV4B3("4B3"); solenoidValve EV4B4("4B4");
LED_Mod L4S1("L4S1", 3, 0); LED_Mod L4S2("L4S2", 3, 1); LED_Mod L4S3("L4S3", 3, 2); LED_Mod L4S4("L4S4", 3, 3);
Actuator IFan4 (0, 3, 30, 150); Actuator OFan4 (1, 3, 30, 150); Actuator VFan4 (2, 3, 30, 150);
Actuator VHum4 (3, 3, 2, 300);

// asyncActuator
asyncActuator IrrigationKegsH2O("EV-KegsH2O"); // eV to irrigate H2O
asyncActuator IrrigationKegsNutrition("EV-KegsNutrition"); // eV to irrigate Nutrition

/*** Day Objects ***/
// MultiDay object(daysPerDay, light%, initHour)
MultiDay day1(3, 75, 0);
MultiDay day2(3, 75, 2);
MultiDay day3(3, 75, 4);
MultiDay day4(3, 75, 6);

/*** Process Control ***/
processControl IPC; // Initial Preconditions Control
processControl MPC; // Middle Preconditions Control
processControl CC; // Comunication Control

/*** Auxiliar Variables ***/
// Control irrigation process
uint8_t irrigationStage = 0; // Defautl state

// Control when update solutionConsumption and h2oConsumption
bool emergency = false;

// Control pressure
float max_pressure = 100; // Default Maximum Pressure in the system (psi)
float min_pressure = 80; // Minimun Pressure in the system to start a new irrigation cycle (psi)
float critical_pressure = 60; // Default Critical Pressure in the system (psi)

// Control Solution and refill process
/* The next variables need to be register in raspberry in case that arduino turn off */
uint8_t lastSolution = 250; // Last solution to be irrigated, by default value of 250 is setted but this value does not match with a solution
float solutionConsumption = 50; // Default consumption in stage 1 of irrigation
float h2oConsumption = 50; // Default consumption in stage 4 of irrigation
// __VolKnut, __VolKh2o in recirculationControllerClass has to be init at boot
/* The previous variables need to be register in raspberry in case that arduino turn off */

// Control ReOrder when day/night change
uint8_t night = 0;

// Control action that has to be executed at least once when booting/rebooting
bool firstHourUpdate = false;
bool bootParameters = false;

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
void updateIrrigationState();
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
// Aux Functions
void emergencyStop();
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
  // Initialize recController
  Recirculation.begin(US0, US1, US2, US3, US4, US5, US6);
  
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
  
  // Finished
  Serial.println(F("Device Ready"));

  // Request boot info to raspberry
  Serial.println(F("?boot"));

  // Enable Relays
  digitalWrite(mR, !HIGH); // Turn on multiplexors
  
  // Testing settings 
  //solenoidValve::enableGroup(true);
}

void loop() {
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
