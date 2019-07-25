/* Por probar:
 *  a) pressure_Conditions
 *  b) recirculationController
 *    
 * Falta: 
 * 1: Libería sensor.h
 *    a) Crear comando para recalibrar sensores por serial
 *    b) Crear estructura para guardar datos de calibración de sensores y parametros de filtros en EEPROM
 * 
 * 2: serial_Communication functions to link with day.h
 *    a) Crear comando para reprogramar días por serial
 *    b) Crear estructura para guardar parámetros de días en EEPROM
 * 
 * 3: compressorController.h and recirculationController.h
 *    a) Condiciones excepcionales para cambiar de decisión -> pressure_Conditions
 * 
 * 4: controllerHVAC.h
 *    a) Falta comprobar funcionamiento de HVAC Controller y revisar si es necesario el tiempo de preparación en modo-HEAT
 *    b) Verificar que no sea necesario apagar el sistema después de cierto tiempo de trabajo continuo.
 * 
 * 5: EEPROM_Function:
 *    a)  Probar a fondo funciones saveParamters_EEPROM(), print_EEPROM() y clean_EEPROM()
 *    b) Falta desarrollar funciones daySave_EEPROM() y sensorSave_EEPROM()
 * 
 * 6: set_up:
 *    a) Give enough time to charge all the paremeters and get multiples lectures from sensor to estabilizar lectures
 *    
 * 7: recirculationController.h:
 *    a) Desecho cuando todo esta saturado
 *    b) Salida del recirculado - entrada kegs/Solucion mezclas
 *    
*/
 
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
#define DHTTYPE22 DHT22
const byte DHT22_EXT = 25;
DHT dht22_ext(DHT22_EXT, DHTTYPE22);

/*** Multiplexor Definitions ***/
const byte ds = 22; // Data
const byte stcp = 23; // Latch
const byte shcp = 24; // Clock

/*** Water sensors definitions ***/
// Pressure
analogSensor pressureSensorNutrition(A0, "Nutrition Pressure");
analogSensor pressureSensorWater(A1, "Water Pressure");
analogSensor pressureSensorCompressor(A2, "Compressor Pressure");
float max_pressure = 160; // Maximum Pressure in the system (psi)
float min_pressure = 100; // Minimum Pressure in the system (psi)
float min_cycle_pressure = 145; // Minimun Pressure in the system when it is going to start a new irrigation cycle (psi)
byte pressureDecision = 0; // Variable for control on decisions taken

/*** UltraSonic Sensors ***/
UltraSonic US0(PIN_LEVEL_0, NAME_LEVEL_0);
UltraSonic US1(PIN_LEVEL_1, NAME_LEVEL_1);
UltraSonic US2(PIN_LEVEL_2, NAME_LEVEL_2);
UltraSonic US3(PIN_LEVEL_3, NAME_LEVEL_3);
UltraSonic US4(PIN_LEVEL_4, NAME_LEVEL_4);
UltraSonic US5(PIN_LEVEL_5, NAME_LEVEL_5);

/*** HVAC Controller object ***/
controllerHVAC HVAC(OFF_MODE , AUTO_FAN);

/*** Compressor Controller object ***/
// Parameters are the logic of the solenoids where LOW is normally closed : Compressor_invertedLogic, Nutrition_invertedLogic and Water_invertedLogic
compressorController Compressor(LOW, HIGH, LOW);

/*** Recirculation Controller ***/
recirculationController Recirculation(LOW, LOW); // recirculationController without solenoids normally open

/*** Actuators ***/
// solenoidValve object(name)
// LED object(name, floor, section)
// Actuator object(type, floor, time_On, timeOff);

// 1st floor
solenoidValve EV1A1("1A1"); solenoidValve EV1A2("1A2"); solenoidValve EV1A3("1A3"); solenoidValve EV1A4("1A4");
solenoidValve EV1B1("1B1"); solenoidValve EV1B2("1B2"); solenoidValve EV1B3("1B3"); solenoidValve EV1B4("1B4");
LED L1S1("L1S1", 1, 1); LED L1S2("L1S2", 1, 2); LED L1S3("L1S3", 1, 3); LED L1S4("L1S4", 1, 4);
Actuator IFan1 (0, 0, 30, 150); Actuator OFan1 (1, 0, 30, 150); Actuator VFan1 (2, 0, 30, 150);
Actuator VHum1 (3, 0, 1, 300);

// 2nd floor
solenoidValve EV2A1("2A1"); solenoidValve EV2A2("2A2"); solenoidValve EV2A3("2A3"); solenoidValve EV2A4("2A4");
solenoidValve EV2B1("2B1"); solenoidValve EV2B2("2B2"); solenoidValve EV2B3("2B3"); solenoidValve EV2B4("2B4");
LED L2S1("L2S1", 2, 1); LED L2S2("L2S2", 2, 2); LED L2S3("L2S3", 2, 3); LED L2S4("L2S4", 2, 4);
Actuator IFan2 (0, 1, 30, 150); Actuator OFan2 (1, 1, 30, 150); Actuator VFan2 (2, 1, 30, 150);
Actuator VHum2 (3, 1, 1, 300);

// 3rd floor
solenoidValve EV3A1("3A1"); solenoidValve EV3A2("3A2"); solenoidValve EV3A3("3A3"); solenoidValve EV3A4("3A4");
solenoidValve EV3B1("3B1"); solenoidValve EV3B2("3B2"); solenoidValve EV3B3("3B3"); solenoidValve EV3B4("3B4");
LED L3S1("L3S1", 3, 1); LED L3S2("L3S2", 3, 2); LED L3S3("L3S3", 3, 3); LED L3S4("L3S4", 3, 4);
Actuator IFan3 (0, 2, 30, 150); Actuator OFan3 (1, 2, 30, 150); Actuator VFan3 (2, 2, 30, 150);
Actuator VHum3 (3, 2, 1, 300);

// 4th floor
solenoidValve EV4A1("4A1"); solenoidValve EV4A2("4A2"); solenoidValve EV4A3("4A3"); solenoidValve EV4A4("4A4");
solenoidValve EV4B1("4B1"); solenoidValve EV4B2("4B2"); solenoidValve EV4B3("4B3"); solenoidValve EV4B4("4B4");
LED L4S1("L4S1", 4, 1); LED L4S2("L4S2", 4, 2); LED L4S3("L4S3", 4, 3); LED L4S4("L4S4", 4, 4);
Actuator IFan4 (0, 3, 30, 150); Actuator OFan4 (1, 3, 30, 150); Actuator VFan4 (2, 3, 30, 150);
Actuator VHum4 (3, 3, 1, 300);

/*** Day Objects ***/
// MultiDay object(daysPerDay, light%, initHour)
MultiDay day1( 3, 75, 0);
MultiDay day2( 3, 75, 2);
MultiDay day3( 3, 75, 4);
MultiDay day4( 3, 75, 6);

// Serial comunication
String inputstring = "";
bool input_string_complete = false;

// DateTime Info
byte dateHour;
byte dateMinute;

// Multiplexer timer
unsigned long multiplexerTime;

void setup() {
  // Initialize Serial
  Serial.begin(115200); Serial.println(F("Setting up Device..."));
  
  // Define INPUTS&OUTPUTS
  solenoidValve::flowSensorBegin();
  pinMode(stcp, OUTPUT);
  pinMode(shcp, OUTPUT);
  pinMode(ds, OUTPUT);
  
  // Initialize objects
  dht22_ext.begin();
  Serial.println(F("DHT sensor started correctly"));

  // Initialize sensors
  sensors_setup();  

  // Initialize actuators
  Actuator::beginAll();
  
  // Initialize solenoids valves
  solenoid_setup();
  
  // Reserve 30 bytes for serial strings
  inputstring.reserve(30);
  
  // Initialize Multiplexers
  codification_Multiplexer();

  // Initialize recController
  Recirculation.begin(PIN_LEVEL_SWITCH, US0, US1, US2, US3, US4, US5);
  
  // Initialize counters
  multiplexerTime = millis();
  
  // Finished
  Serial.println(F("Device Ready"));
  
  // Testing settings 
}

void loop() {
  Actuator::runAll(); // Run all the actuators
  //solenoidValve::runAll(); // Run irrigation routine
  //HVAC.run(); // Decide what to do with Air Conditioner
  //UltraSonic::runAll(); // Run all the UltraSonic Sensors measurements

  if(millis()-multiplexerTime>100){
    multiplexerTime = millis();
    codification_Multiplexer();
  }
}
