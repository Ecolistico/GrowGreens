/*** Include Libraries ***/
#include <Wire.h>
#include <DHT.h>
#include <EEPROM.h>
#include <actuator.h>
#include <day.h>
#include <controllerHVAC.h>
#include <sensor.h>
#include <compressorController.h>
#include <recirculationController.h>

/*** Temp-Hum Sensors(DHT-22) Definitions ***/ 
// DHT object(pin, type)
DHT dht22_ext(5, DHT22);

/*** Multiplexor Definitions ***/
const uint8_t ds = 22; // Data
const uint8_t stcp = 23; // Latch
const uint8_t shcp = 24; // Clock

/*** Pressure sensors definitions ***/
// analogSensor object(analogPin, name)
analogSensor pressureSensorNutrition(A0, "Nutrition Pressure");
analogSensor pressureSensorTank(A1, "Tank Pressure");
analogSensor pressureSensorWater(A2, "Water Pressure");

/*** UltraSonic Sensors ***/
// UltraSonic object(pin, name)
UltraSonic US0(25, "Water Level");
UltraSonic US1(26, "Solution 1 Level");
UltraSonic US2(27, "Solution 2 Level");
UltraSonic US3(28, "Solution 3 Level");
UltraSonic US4(29, "Solution 4 Level");
UltraSonic US5(30, "Recirculation Level");
UltraSonic US6(31, "Solution Maker Level");

/*** Water Sensors ***/
// waterSensor object(pin, name)
waterSensor checkWaterIrrigation(32, "Water Irrigation Sensor");
waterSensor checkWaterEvacuation(33, "Water Evacuation Sensor");
bool checkRecirculation = true;

/*** Auxiliar Variables ***/
// To check change between solution/water irrigation for day/night floor according to routine
uint8_t irrigationStage = 0; // Defautl state

// To control pressure
float max_pressure = 160; // Default Maximum Pressure in the system (psi)
float min_pressure = 150; // Minimun Pressure in the system to start a new irrigation cycle (psi)
float critical_pressure = 90; // Default Critical Pressure in the system (psi)

// To control Solution
/* The next variables need to be register in raspberry in case that arduino turn off */
float solutionConsumption = 50; // Default consumption in stage 1 of irrigation
float h2oConsumption = 20; // Default consumption in stage 4 of irrigation
// __VolKnut, __VolKh2o in recirculationControllerClass has to be init at boot
/* The previous variables need to be register in raspberry in case that arduino turn off */

// To control ReOrder when day/night change
uint8_t night = 0;

// To control action that have to be executed at least once when boot
bool firstHourUpdate = false;
bool updatedVolumenKegs = false;

// Serial comunication
String inputstring = "";
bool input_string_complete = false;

// DateTime Info
uint8_t dateHour;
uint8_t dateMinute;

// Multiplexer timer
unsigned long multiplexerTime;

// To coordinate async functions
uint8_t IPC = 0; // Irrigation Precondition Control
uint8_t lastIPC = 0; // IPC control for retry some action
unsigned long IPC_Time = 0; // IPC Time Control
float IPC_Parameter = 0; // IPC auxiliars parameters
uint8_t IPC_Central_Request = 0; // Request to the central computer

/*** HVAC Controller object ***/
// controllerHVAC object(Mode, Fan Mode)
controllerHVAC HVAC(OFF_MODE , AUTO_FAN);

/*** Compressor Controller object ***/
// controllerHVAC object(tankLogic, nutritionLogic, waterLogic)
// Parameters are the logic of the solenoids where LOW is normally closed
compressorController Compressor(LOW, LOW, LOW);

/*** Recirculation Controller ***/
recirculationController Recirculation;

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
Actuator VHum1 (3, 0, 1, 300);

// 2nd floor
solenoidValve EV2A1("2A1"); solenoidValve EV2A2("2A2"); solenoidValve EV2A3("2A3"); solenoidValve EV2A4("2A4");
solenoidValve EV2B1("2B1"); solenoidValve EV2B2("2B2"); solenoidValve EV2B3("2B3"); solenoidValve EV2B4("2B4");
LED_Mod L2S1("L2S1", 1, 0); LED_Mod L2S2("L2S2", 1, 1); LED_Mod L2S3("L2S3", 1, 2); LED_Mod L2S4("L2S4", 1, 3);
Actuator IFan2 (0, 1, 30, 150); Actuator OFan2 (1, 1, 30, 150); Actuator VFan2 (2, 1, 30, 150);
Actuator VHum2 (3, 1, 1, 300);

// 3rd floor
solenoidValve EV3A1("3A1"); solenoidValve EV3A2("3A2"); solenoidValve EV3A3("3A3"); solenoidValve EV3A4("3A4");
solenoidValve EV3B1("3B1"); solenoidValve EV3B2("3B2"); solenoidValve EV3B3("3B3"); solenoidValve EV3B4("3B4");
LED_Mod L3S1("L3S1", 2, 0); LED_Mod L3S2("L3S2", 2, 1); LED_Mod L3S3("L3S3", 2, 2); LED_Mod L3S4("L3S4", 2, 3);
Actuator IFan3 (0, 2, 30, 150); Actuator OFan3 (1, 2, 30, 150); Actuator VFan3 (2, 2, 30, 150);
Actuator VHum3 (3, 2, 1, 300);

// 4th floor
solenoidValve EV4A1("4A1"); solenoidValve EV4A2("4A2"); solenoidValve EV4A3("4A3"); solenoidValve EV4A4("4A4");
solenoidValve EV4B1("4B1"); solenoidValve EV4B2("4B2"); solenoidValve EV4B3("4B3"); solenoidValve EV4B4("4B4");
LED_Mod L4S1("L4S1", 3, 0); LED_Mod L4S2("L4S2", 3, 1); LED_Mod L4S3("L4S3", 3, 2); LED_Mod L4S4("L4S4", 3, 3);
Actuator IFan4 (0, 3, 30, 150); Actuator OFan4 (1, 3, 30, 150); Actuator VFan4 (2, 3, 30, 150);
Actuator VHum4 (3, 3, 1, 300);

// asyncActuator
asyncActuator IrrigationKegsH2O("EV-KegsH2O"); // eV to irrigate H2O
asyncActuator IrrigationKegsNutrition("EV-KegsNutrition"); // eV to irrigate Nutrition

/*** Day Objects ***/
// MultiDay object(daysPerDay, light%, initHour)
MultiDay day1(3, 75, 0);
MultiDay day2(3, 75, 2);
MultiDay day3(3, 75, 4);
MultiDay day4(3, 75, 6);

void setup() {
  // Initialize Serial
  Serial.begin(115200); Serial.println(F("Setting up Device..."));
  
  // Define INPUTS&OUTPUTS
  solenoidValve::flowSensorBegin();
  recirculationController::flowSensorBegin();
  
  pinMode(stcp, OUTPUT);
  pinMode(shcp, OUTPUT);
  pinMode(ds, OUTPUT);
  
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
  chargePressureParameter(); // For pressure control

  // Set initial state
  inputstring.reserve(30); // Reserve 30 bytes for serial strings
  codification_Multiplexer(); // Initialize Multiplexers
  multiplexerTime = millis(); // Initialize counters

  // Finished
  Serial.println(F("Device Ready"));
  
  // Testing settings 
  //solenoidValve::enableGroup(true);
}

void loop() {
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
  
  if(millis()-multiplexerTime>100){ // Send states to multiplexors 10 times/second
    multiplexerTime = millis();
    codification_Multiplexer();
  }
}
