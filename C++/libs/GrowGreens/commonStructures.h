// This file define some common structure shared between different classes

#ifndef commonStructures_h
#define commonStructures_h

// Common
#define MAX_FLOOR_NUMBER        12      // 12 floors
#define REBOOT_TIME             300000  // 5 minutes

// Solenoids
#define DEFAULT_TIME_ON         4  // 4 seconds
#define DEFAULT_CYCLES          1  // 1 cycle
#define DEFAULT_CYCLE_TIME      30 // 30 minutes
#define MAX_VALVES_PER_REGION   10 // 10 solenoids

// Actuators
#define AUX_ACTUATORS           4  // 4 actuators extra

// Fan
#define MAX_ACTUATORS           32  // Max number of fans object
#define DEFAULT_TIME_ON_F       50  // 50 percent for fans
#define DEFAULT_CYCLE_TIME_F    10  // 10 minutes for fans

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
#ifndef PI
#define PI 3.1416
#endif
#define MAX_FLOWMETER_SENSOR  2   // Maximun number of flowmters allowed
#define MAX_SCALES_SENSOR     2   // Maximun number of scales allowed
#define MAX_SWITCHES_SENSOR   10  // Maximun number of switches allowed

// MUX
#define MAX_MUX_SYSTEMS 4 // Max number of multiplexer systems
#define MAX_MODULES 20  // Max number of PCB in a single commutation tower
#define OUT_PER_PCB 8   // Outputs per PCB

typedef struct {
  uint8_t floors;
  uint8_t solenoids;
  uint8_t regions;
  uint8_t cycleTime;
  uint8_t mux;
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
  uint8_t cycleTime;
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
  float min_weight;
  float max_weight;
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

typedef struct {
  uint8_t pcb_mounted;
  uint8_t ds;           // Data MUX
  uint8_t stcp;         // Latch MUX
  uint8_t shcp;         // Clock MUX
  uint8_t ds1;          // Data DEMUX
  uint8_t stcp1;        // Latch DEMUX
  uint8_t shcp1;        // Clock DEMUX
} Mux;

typedef struct {
  bool *state;
  uint8_t number;
} MuxState;

typedef struct {
  bool *st[OUT_PER_PCB];
} MuxOut;

typedef struct {
  uint8_t timeOnS;
  uint8_t cycles;
} solenoid_memory;

#endif
