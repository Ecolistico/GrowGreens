/*
 * Falta: Agregar 
 */
 
/*** Include Libraries ***/
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <DHT.h>
#include <IRremote.h>
#include <actuator.h>
#include <day.h>
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

/*** Actuators ***/
// Define initial time
#define electrovalve_timeOn 1 // 5 seconds
#define electrovalve_timeOff 300 // 60 seconds
#define fan_timeOn 30 // 3 seconds
#define fan_timeOff 60 // 60 seconds

// 1st floor
// Actuator object(time_On, timeOff);
solenoidValve EV1A1("1A1"); solenoidValve EV1A2("1A2"); solenoidValve EV1A3("1A3"); solenoidValve EV1A4("1A4");
solenoidValve EV1B1("1B1"); solenoidValve EV1B2("1B2"); solenoidValve EV1B3("1B3"); solenoidValve EV1B4("1B4");
LED L1S1("L1S1"); LED L1S2("L1S2"); LED L1S3("L1S3"); LED L1S4("L1S4");
Actuator IFan1 (fan_timeOn, fan_timeOff); Actuator OFan1 (fan_timeOn, fan_timeOff); Actuator VFan1 (fan_timeOn, fan_timeOff);
Actuator humidityValve1 (electrovalve_timeOn, electrovalve_timeOff);
// 2nd floor
solenoidValve EV2A1("2A1"); solenoidValve EV2A2("2A2"); solenoidValve EV2A3("2A3"); solenoidValve EV2A4("2A4");
solenoidValve EV2B1("2B1"); solenoidValve EV2B2("2B2"); solenoidValve EV2B3("2B3"); solenoidValve EV2B4("2B4");
LED L2S1("L2S1"); LED L2S2("L2S2"); LED L2S3("L2S3"); LED L2S4("L2S4");
Actuator IFan2 (fan_timeOn, fan_timeOff); Actuator OFan2 (fan_timeOn, fan_timeOff); Actuator VFan2 (fan_timeOn, fan_timeOff);
Actuator humidityValve2 (electrovalve_timeOn, electrovalve_timeOff);
// 3rd floor
solenoidValve EV3A1("3A1"); solenoidValve EV3A2("3A2"); solenoidValve EV3A3("3A3"); solenoidValve EV3A4("3A4");
solenoidValve EV3B1("3B1"); solenoidValve EV3B2("3B2"); solenoidValve EV3B3("3B3"); solenoidValve EV3B4("3B4");
LED L3S1("L3S1"); LED L3S2("L3S2"); LED L3S3("L3S3"); LED L3S4("L3S4");
Actuator IFan3 (fan_timeOn, fan_timeOff); Actuator OFan3 (fan_timeOn, fan_timeOff); Actuator VFan3 (fan_timeOn, fan_timeOff);
Actuator humidityValve3 (electrovalve_timeOn, electrovalve_timeOff);
// 4th floor
solenoidValve EV4A1("4A1"); solenoidValve EV4A2("4A2"); solenoidValve EV4A3("4A3"); solenoidValve EV4A4("4A4");
solenoidValve EV4B1("4B1"); solenoidValve EV4B2("4B2"); solenoidValve EV4B3("4B3"); solenoidValve EV4B4("4B4");
LED L4S1("L4S1"); LED L4S2("L4S2"); LED L4S3("L4S3"); LED L4S4("L4S4");
Actuator IFan4 (fan_timeOn, fan_timeOff); Actuator OFan4 (fan_timeOn, fan_timeOff); Actuator VFan4 (fan_timeOn, fan_timeOff);
Actuator humidityValve4 (electrovalve_timeOn, electrovalve_timeOff);

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
  pressureSensor.begin();
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
  
}
