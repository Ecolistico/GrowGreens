/*
 * Falta: 
 * Crear comando para recalibrar sensores por serial
 * Crear estructura para guardar datos de calibración de sensores y parametros de filtros en EEPROM
 * 
 * Crear comando para reprogramar días por serial
 * Crear estructura para guardar parámetros de días en EEPROM
 * 
 * Crear objetos para controlar el resto de las válvulas solenoides y bombas
 * Programar condiciones de encendido y apagado de bombas y válvulas solenoides
*/
 
/*** Include Libraries ***/
#include <Wire.h>
#include <DHT.h>
#include <EEPROM.h>
#include <actuator.h>
#include <day.h>
#include <controllerHVAC.h>
#include <sensor.h>


/*** Temp-Hum Sensors(DHT-22) Definitions ***/ 
#define DHTTYPE22 DHT22
const byte DHT22_EXT = 25;
DHT dht22_ext(DHT22_EXT, DHTTYPE22);

/*** Multiplexor Definitions ***/
const byte ds = 22;
const byte stcp = 23;
const byte shcp = 24;

/*** Water sensors definitions ***/
// Pressure
analogSensor pressureSensor(A0, "Pressure");

/***** HVAC Controller object*****/
controllerHVAC HVAC(OFF_MODE , AUTO_FAN);

/*** Actuators ***/
// Define initial time
#define solenoid_timeOn 1 // 1 seconds
#define solenoid_timeOff 300 // 300 seconds
#define fan_timeOn 30 // 3 seconds
#define fan_timeOff 150 // 150 seconds

// 1st floor
// Actuator object(time_On, timeOff);
solenoidValve EV1A1("1A1"); solenoidValve EV1A2("1A2"); solenoidValve EV1A3("1A3"); solenoidValve EV1A4("1A4");
solenoidValve EV1B1("1B1"); solenoidValve EV1B2("1B2"); solenoidValve EV1B3("1B3"); solenoidValve EV1B4("1B4");
LED L1S1("L1S1", 1, 1); LED L1S2("L1S2", 1, 2); LED L1S3("L1S3", 1, 3); LED L1S4("L1S4", 1, 4);
Actuator IFan1 (fan_timeOn, fan_timeOff); Actuator OFan1 (fan_timeOn, fan_timeOff); Actuator VFan1 (fan_timeOn, fan_timeOff);
Actuator humidityValve1 (solenoid_timeOn, solenoid_timeOff);
// 2nd floor
solenoidValve EV2A1("2A1"); solenoidValve EV2A2("2A2"); solenoidValve EV2A3("2A3"); solenoidValve EV2A4("2A4");
solenoidValve EV2B1("2B1"); solenoidValve EV2B2("2B2"); solenoidValve EV2B3("2B3"); solenoidValve EV2B4("2B4");
LED L2S1("L2S1", 2, 1); LED L2S2("L2S2", 2, 2); LED L2S3("L2S3", 2, 3); LED L2S4("L2S4", 2, 4);
Actuator IFan2 (fan_timeOn, fan_timeOff); Actuator OFan2 (fan_timeOn, fan_timeOff); Actuator VFan2 (fan_timeOn, fan_timeOff);
Actuator humidityValve2 (solenoid_timeOn, solenoid_timeOff);
// 3rd floor
solenoidValve EV3A1("3A1"); solenoidValve EV3A2("3A2"); solenoidValve EV3A3("3A3"); solenoidValve EV3A4("3A4");
solenoidValve EV3B1("3B1"); solenoidValve EV3B2("3B2"); solenoidValve EV3B3("3B3"); solenoidValve EV3B4("3B4");
LED L3S1("L3S1", 3, 1); LED L3S2("L3S2", 3, 2); LED L3S3("L3S3", 3, 3); LED L3S4("L3S4", 3, 4);
Actuator IFan3 (fan_timeOn, fan_timeOff); Actuator OFan3 (fan_timeOn, fan_timeOff); Actuator VFan3 (fan_timeOn, fan_timeOff);
Actuator humidityValve3 (solenoid_timeOn, solenoid_timeOff);
// 4th floor
solenoidValve EV4A1("4A1"); solenoidValve EV4A2("4A2"); solenoidValve EV4A3("4A3"); solenoidValve EV4A4("4A4");
solenoidValve EV4B1("4B1"); solenoidValve EV4B2("4B2"); solenoidValve EV4B3("4B3"); solenoidValve EV4B4("4B4");
LED L4S1("L4S1", 4, 1); LED L4S2("L4S2", 4, 2); LED L4S3("L4S3", 4, 3); LED L4S4("L4S4", 4, 4);
Actuator IFan4 (fan_timeOn, fan_timeOff); Actuator OFan4 (fan_timeOn, fan_timeOff); Actuator VFan4 (fan_timeOn, fan_timeOff);
Actuator humidityValve4 (solenoid_timeOn, solenoid_timeOff);

/*** Day Objects ***/
// MultiDay object(daysPerDay, light%, initHour)
MultiDay day1( 3, 75, 0);
MultiDay day2( 3, 75, 2);
MultiDay day3( 3, 75, 4);
MultiDay day4( 3, 75, 6);

// Serial comunication
const byte CMDBUFFER_SIZE = 32;
String inputstring = "";
bool input_string_complete = false;
byte serial_Count;
byte serial_State;
// DateTime Info
byte dateHour;
byte dateMinute;

void setup() {
  // Initialize Serial
  Serial.begin(115200); Serial.println(F("Setting up Device..."));
  
  // Define INPUTS&OUTPUTS
  //pressureSensor.begin();
  solenoidValve::flowSensorBegin();
  pinMode(stcp, OUTPUT);
  pinMode(shcp, OUTPUT);
  pinMode(ds, OUTPUT);
  
  // Initialize objects
  dht22_ext.begin();
  Serial.println(F("DHT sensor started correctly"));

  // Initialize sensors
  sensors_setup();  

  // Initialize solenoids valves
  solenoid_setup();
  
  // Reserve 30 bytes for serial strings
  inputstring.reserve(30);
  
  // Initialize Multiplexers
  codification_Multiplexer();
  
  // Initialize counters

  // Finished
  Serial.println(F("Device Ready"));

  
  // Testing settings 
}


void loop() {
  for (int i = 0; i < solenoidValve::__TotalActuators; i++){
    (solenoidValve::ptr[i]->run());
  }
  
  HVAC.run(); // Decide what to do with Air Conditioner
}
