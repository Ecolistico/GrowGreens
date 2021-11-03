/*
 * To do:
 * 1- Change EEPROM to externalEEPROM and all its functions
 *
 * 2- Add scale calibration procedure
 * 2.1- Get offset
 * 2.2- Get scale
 * 2.3- Save it in EEPROM
 * 2.4 Possible troubles with interrupts and flowmeters sensors.
 * 2.5 Possible troubles with cable length. It could possible tried to change the Shift In function to make delayMicroseconds about 15-20us and see how it works
 *
 * 3- Add analog sensor calibration procedure
 * 3.1- Ask user for the type of curve (constant, linear, cuadratic, etc.)
 * 3.2- Get the number of meditions necesary to calculate the equation
 * 3.3- Save the parameters get it
 *
 * 4- Add flowmters sensor calibration procedura
 * 4.1 Set interrupts and open the valve for x seconds/minutes
 * 4.2 Ask user for the number of liters that crossed the flowmeter
 * 4.3 Get and save in EEPROM the factor K
 *
 * 5- Create MUX/DEMUX Class to manage the commutation towers
 *
 * 6- Reestructure EEPROM with all this information
 *
 * 7- Redo flow chart of each process
*/
// This is a test
/*** Include Libraries ***/
#include <Wire.h>
#include <External_EEPROM.h> // Click here to get the library: http://librarymanager/All#SparkFun_External_EEPROM
/* You need to define the buffer lengths in SparkFun_External_EEPROM.h for
generic arduino mega 2560 board. For more details ask Eleazar Dominguez(eleazardg@sippys.com.mx). */
#include <solenoid.h>
#include <sensor.h>
/*** EEPROM object ***/
ExternalEEPROM myEEPROM;

/*** Config Variables ***/
byte floors = 0;
byte solenoids = 0;
byte minutes = 0;
byte analogSensors = 0;
byte flowMeters = 0;
byte scales = 0;
byte ultrasonicSensors = 0;
byte switches = 0;
bool configFinished = false;

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

// Control when update solutionConsumption and h2oConsumption
bool emergency = false;

// Control pressure
float max_pressure = 100; // Default Maximum Pressure in the system (psi)
float min_pressure = 80; // Minimun Pressure in the system to start a new irrigation cycle (psi)
float critical_pressure = 60; // Default Critical Pressure in the system (psi)
float free_pressure = 10; // Set pressure to free air in kegs

// Control Actions that has to be taken the first time an irrigation valve is activated
bool firstValve = HIGH;

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
void begin_EEPROM();
void write_EEPROM(int pos, byte val);
byte read_byte(int pos);
void write_EEPROM(int pos, int val);
int read_int(int pos);
void write_EEPROM(int pos, float val);
float read_float(int pos);
void write_EEPROM(int pos, long val);
long read_long(int pos);
void write_EEPROM(int pos, unsigned long val);
unsigned long read_ulong(int pos);
void clean_EEPROM(bool truncate = true);
void print_EEPROM(bool truncate = true)
void configEEPROM(byte floors, byte solenoids, byte minutes, byte analogSensors, byte flowMeters, byte scales, byte ultrasonicSensors, byte switches);
void getConfig();
bool checkConfig();
void saveRegion(int Floor, bool side, int region);
void saveSolenoidTimeOn(int Floor, int region, byte timeOn);
void saveCycleTime(int val);
void saveAnalog(byte pin, float A, float B, float C);
/*
void analogSaveFilter(int Type, int filt, float filterParam);
void analogSaveModel(int Type, float a, float b, float c);
bool chargeAnalogParameters(int Type);
void ultrasonicSaveFilter(int Type, int filt, float filterParam);
void ultrasonicSaveModel(int Type, int model, float modelParam, float height);
bool chargeUltrasonicParameters(int Type);
void pressureSave(int Type, float Press);
void chargePressureParameter();
*/
// async
/*
void solenoidValverunAll();
void async();
void asyncIrrigation();
void initialPreconditions(void (*ptr2function)());
void middlePreconditions(void (*ptr2function)());
void doNothing();
void startIrrigation();
void startMiddleIrrigation();
void irrigationEmergency();
*/
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

  // Begin EEPROM
  begin_EEPROM();
  getConfig();
  // Check EEPROM Config
  configFinished = checkConfig();

  if(configFinished) {
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
  else {
    Serial.println(F("critical,System does not have basic configuration please provide it."));
  }
}

void loop() {
  if(configFinished){
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
  }

  // Testing code

}
