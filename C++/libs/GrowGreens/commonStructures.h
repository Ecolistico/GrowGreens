// This file define some common structure shared between different classes

#ifndef commonStructures_h
#define commonStructures_h

// Common
#define MAX_FLOOR_NUMBER        12      // 12 floors
#define REBOOT_TIME             300000  // 5 minutes

// Solenoids
#define DEFAULT_TIME_ON         4  // 4 seconds
#define DEFAULT_CYCLE_TIME      30 // 30 minutes
#define MAX_VALVES_PER_REGION   10 // 10 solenoids

// Fan
#define MAX_ACTUATORS           32    // Max number of fans object
#define DEFAULT_TIME_ON_F         50  // 50 percent for fans
#define DEFAULT_CYCLE_TIME_F      10  // 10 minutes for fans

// Dynamic Memory
#define DYNAMIC_START 30          // Start Dynamic Memory in position 30

// Sensors
#define READ_SENSOR_TIME      100 // Take readings each 100 ms
#define MAX_ANALOG_SENSOR     5   // Max number of analog sensors
#define MAX_EQUATION_DEGREE   3   // Max degree of equation for calibration
#define MAX_ULTRASONIC        8   // Max number of ultrasonic sensors
#define MAX_DISTANCE          300 // Maximum distance(cm) we want to ping
#define MIN_SECURITY_DISTANCE 10  // Minimum distance(cm) expected
#define MAX_SECURITY_DISTANCE 82  // Maximum distance(cm) expected
#define PI 3.1416
#define MAX_FLOWMETER_SENSOR  2   // Maximun number of flowmters allowed
#define MAX_SCALES_SENSOR     2   // Maximun number of scales allowed
#define MAX_SWITCHES_SENSOR   10  // Maximun number of switches allowed

typedef struct {
  uint8_t floors;
  uint8_t solenoids;
  uint8_t regions;
  uint8_t cycleTime;
} basicConfig;

typedef struct {
  uint8_t max_pressure;
  uint8_t min_pressure;
  uint8_t critical_pressure;
  uint8_t free_pressure;
} pressureConfig;

typedef struct {
  uint8_t analogs;
  uint8_t flowmeters;
  uint8_t scales;
  uint8_t switches;
  uint8_t ultrasonics;
} sensorConfig;

typedef struct {
  bool Vair_Logic;
  bool Vconnected_Logic;
  bool VFree_Logic;
} logicConfig;

typedef struct {
  uint8_t timeOn;
  unsigned long cycleTime;
} fan_memory;

typedef struct {
  uint8_t pin;
  float A;
  float B;
  float C;
  uint8_t filter;
  float filterParam;
} analogSensor;

typedef struct {
  uint8_t pin;
  float K;
} flowmeter;

typedef struct {
  uint8_t pin1;
  uint8_t pin2;
  long offset;
  float scale;
} scale;

typedef struct {
  uint8_t pin;
  bool logic;
} switchSensor;

typedef struct {
  uint8_t pin1;
  uint8_t pin2;
  uint8_t model;
  float param;
  float height;
} ultrasonicSensor;

typedef struct {
  uint8_t hour;
  uint8_t minute;
} dateTime;

#endif
