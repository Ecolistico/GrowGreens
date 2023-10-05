/*
Library for Grow software package
Written by : José Manuel Casillas Martín & Santiago Saracho Gonzalez
email: jmcasimar@sippys.com.mx & sasago@sippys.com.mx
Date : 2021-*-*
Version : 0.1 (testing version)

This file is part of Grow Software Package and is intended for use with the
SMaker ECOLISTICO shield.

Grow is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Grow is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Grow.  If not, see <https://www.gnu.org/licenses/>.
*/

// solutionMaker.h

#ifndef sMakerCunTFT2_h
#define sMakerCunTFT2_h

#include <stdlib.h>
#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#include <wiring.h>
#endif

#include "AccelStepper.h" // Include AccelStepper library
#include <Wire.h> // Necessary for I2C communication
//#include <LiquidCrystal_I2C.h> // For LCD screen
#include <OneWire.h> // Necessary for 1-wire communication
#include <DallasTemperature.h> // For temperature sensor
#include <EZO.h> // For Atlas Scientific sensors
#include <math.h> // For ph/conductivity equations
#include <DHT.h> // For DHT22 use
#include <HX711.h> //For Loadcell use
#include "SparkFun_External_EEPROM.h" // Click here to get the library: http://librarymanager/All#SparkFun_External_EEPROM
#include <Wire.h>


//Stepper motor definitions
#define MAX_STEPPERS 8 // Max number of stepper motors in current version
#define MOTOR_SPEED 3000 // Maximum Steps Per Second
#define MOTOR_ACCEL 12000 // Steps/second/Second of acceleration
#define DEFAULT_MICROSTEP 16 // By default we use configuration of 1/4 microSteps, if any number beside 4, 8, 16 is chosen 4 will be used
#define MOTOR_STEP_PER_REV 200 // By default we use motors of 200 steps/rev

//Dispensers
#define SOLUTIONS 8 //Liquid salt dispensers
#define MAX_PUMPS_NUMBER 2 // The max number of different pumps
#define saltDist 90 //Minimum separation between salt cartridges (used for car movement)
#define PUMP_VELOCITY 255 // Value pwm -> 255 = 100%, 155 = 60% of the maximun velocity.

//Relay actuators
#define RELAY_NUMBER 4 //Relay quantity
#define RELAY_ACTION_TIME 60000 // 60 seconds by default

//Sensors
#define DHTTYPE DHT22 //Define DHT22 type sensor
#define LIMIT_SWITCHES 5 //Define limit switch ammount
#define loadCellCalibration -400
#define LoadCellOffset 0

//Runnning system
#define DEVIATION 0.05 //Deviation required in EC to run
#define PreTime 30000 //30 seconds for testing

/* Pin definitions
    When using SMaker ECOLISTICO V1.1 Shield al pins are already defined
*/
#define dirS1 34
#define stepS1 35
#define dirS2 36
#define stepS2 37
#define dirS3 48
#define stepS3 49
#define dirS4 47
#define stepS4 46
#define dirS5 52
#define stepS5 53
#define dirS6 51
#define stepS6 50
#define dirS7 A11
#define stepS7 A10
#define dirS8 A8
#define stepS8 A9
#define en 45

#define pumpA1 39
#define pumpA2 40
#define pumpB1 42
#define pumpB2 41
#define pumpEnA 38
#define pumpEnB 43

#define tempSens 31
#define DHTSens 30
#define relay1 6
#define relay2 5
#define relay3 4
#define relay4 3
#define limitS1 11
#define limitS2 10
#define limitS3 9
#define limitS4 8
#define limitS5 7
#define loadCellDT A5
#define loadCellSCK A6
#define Servo1 19
#define Servo2 2
//#define EZO_PH_OFF 13 //Define ezo off pin
#define EZO_EC_OFF 12 //Define ezo off pin

//EEPROM base constants
#define BaseEEPROM 10

struct EEPROMstruct{
  uint8_t Solutions;
  float   loadCellCal;
  uint8_t cycle;
  uint8_t Relays;
  int     __ECtarget;
  int     __ECmax;
  float   __Calibration[SOLUTIONS];
  float   __CalibrationLiquids[SOLUTIONS];
  uint8_t __RelayTime[RELAY_NUMBER];
  uint8_t __saltOrders[SOLUTIONS];
};

// Class to control the Solution Maker
class solutionMaker
  {
    private:
        uint8_t        __enableMotorA, __enableMotorB;
        uint8_t        __En; // Pin to enable all the motors and pumps
        bool           __SteppersEnabled; //Holds true or false if steppers are enabled or disabled
        uint8_t        __StepPerRev; // Steps per rev
        uint8_t        __MicroSteps; // MicroSteps to be used 4/8/16
        unsigned long  __PumpTime[MAX_PUMPS_NUMBER]; // Time counter for pumps working
        unsigned long  __PumpOnTime[MAX_PUMPS_NUMBER]; // The time that the pumps will be working
        bool           __PumpOn[MAX_PUMPS_NUMBER]; //pumps are active
        unsigned long  __ReadTime, __RelayTime[RELAY_NUMBER]; // Auxiliars counter for sensorReads and relays
        unsigned long  __PreTimer;
        long           homeTimer;
        bool           carEndDetected;
        unsigned long  endTimer;
        bool           prepareTimeState;
        long           prepareTimeAux;
      
        float          PH_Target;
        int            EC_Target;

        float          mlSalts[MAX_STEPPERS];
        


        // Variable to know if some Atlas Scientific Sensor is exporting its calibration parameters
        bool          __ExportEzo;
        bool          __ImportEzo; // Variable to know if we are importing some calibration parameter
        bool          __SleepEzo; // Variable to know if Atlas Scientific Sensors are in sleep mode
        bool          readAux;
        bool          tareAux;
        bool          __stopped;
        long          debugAux;
        long          dispenseDelay;
        bool          dispenseAux;
        long          tareTime;
        float         showMultiplier;
        bool          runStepper;


        // Constants for the filters
        uint8_t       __Filter;
        float         __Alpha , __Noise , __Err;
        // Exponential Filter: __Alpha = smoothing variable
        // Kalman Filter: __Noise = Enviromental Noise, __Err = Error

        //EEPROM DEFAULTS
        byte          CarMotor;
        byte          DispenserMotor;
        byte          CargoMotor;
        byte          ReleaseMotor;
        byte          ExtraMotor;
        byte          carHomeLS; //LS 1 normally closed
        byte          dispenserLS; //LS 4 normally open
        byte          carEndLS; //LS 5 normally closed
        byte          cargoLS; //LS 2 normally closed
        byte          openLS; //LS 3 normally closed
        unsigned long          PREPARE_CYCLE;

        // Pointers
                EZO *phMeter; // Pointer to phMeter (Atlas Scientific sensor)
        EZO *ecMeter; // Pointer to ecMeter (Atlas Scientific sensor)
        DHT *dhtSensor;
        

        //----------Funciones-------------//
        void enable(uint8_t motor); // Enable actuators
        void disable(); // Disable actuators
        
        /* Converts time [s] to volume [ml] for the pumps
            By default use calibration param of the first pump assuming that all are equals
        */
        unsigned long TimeToML(float seconds, uint8_t pump);
        /* Converts volume [ml] to time [s] for the pumps
            By default use calibration param of the first pump assuming that all are equals
        */
        unsigned long MLToTime(float mililiters, uint8_t pump);

        long RevToSteps(float rev); // Returns the number of steps that are require to move one rev
        float StepsToRev(long st); // Returns the number of rev that are some steps
        float mlToSteps(float ml, uint8_t st);
        // Calculate how much mg of powder or liters of H2O do you need to balance conductivity
        float balanceEC(float EC_init, float EC_final, float liters, uint8_t st);
        float balanceLiquidSalts(float EC_init, float EC_final, float liters, uint8_t st);
        // Calculate how much ml of acid or liters of H2O do you need to balance ph
        float balancePH(float PH_init, float PH_final, float liters, uint8_t pump);
        void moveStepper(long steps, uint8_t st);
        void resetPosition(uint8_t st); // reset the position to zero of some stepper
        void moveCar(int dist); //Moves salt car in search for LS of next solution salt
        void home(); //Moves every stepper to home in the right squence
        void checkLS(); //checks the state of all Limit Switches, helps to determine actions
        // Print in serial an action executed with the correct format
        void printAction(String act, uint8_t actuator, uint8_t level=0);
        // Print in serial an action executed with the correct format (Atlas Scientific Sensors)
        void printEZOAction(String act, uint8_t sensorType, uint8_t level=0);
        float filter(float val, float preVal);
        float exponential_filter(float alpha, float t, float t_1);
        float kalman_filter(float t, float t_1);

        void readTemp(); // Read temperature from DSB1820
        void EZOReadRequest(float temp, bool sleep = false); // Request a single read in Atlas Scientific Sensors

        void EZOexportFinished(); // When some export is finished set variable to ask for a new one
        void relayControl(int rel); // Control the actions of the relay




    public:
        solutionMaker();
        // Pointers
        AccelStepper *stepperS[MAX_STEPPERS]; // Pointer for each stepper motor

        EEPROMstruct   __EEPROMstruct;
        uint8_t        __EEPROM[sizeof(__EEPROMstruct)]; //QTY of eeprom Parameters
      //public variables
        uint8_t        __solutions;
        uint8_t        __max_pumps_number;
        uint8_t        __max_steppers;
        uint8_t        __relay_number;
        uint8_t        __DirS[MAX_STEPPERS], __StepS[MAX_STEPPERS];
        long           __MOTOR_SPEED[MAX_STEPPERS], __MOTOR_ACCEL[MAX_STEPPERS];
        uint8_t        __Motor[MAX_PUMPS_NUMBER]; // Pins to control all the peristaltic pumps
        uint8_t        __PumpVelocity[MAX_PUMPS_NUMBER]; // pwm value for pumps
        uint8_t        __Relay[RELAY_NUMBER]; // Pins to control relay
        bool           __RelayState[RELAY_NUMBER]; // Variable to hold the state of the relay
        unsigned long  __relay_action_time[RELAY_NUMBER];
        float         __loadCellCalibration;
        float         __loadCellOffset;
        byte          microsPump;
        byte          acidPump;
    
        float          __pH, __eC, __pHtarget,  __literTarget; // Variables to hold the phMeter and ecMeter values
        int            __ECtarget;
        int            __ECmax;

        float          __Temp, __Hum; // Variable to hold the temperature
        float          __Calibration[MAX_STEPPERS]; // Calibration parameter for ml to revs 
        float          __CalibrationLiquids[MAX_STEPPERS]; // Calibration parameter for conductivity equations ml for EC at 100%
        const float    __CalibrationDEF[MAX_STEPPERS] = {1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0};
        const float    __CalibrationLiquidsDefMed[MAX_STEPPERS] = {68.59,5090.82,2181.28,1807.23,417.10,3494.85,2028.5,37.56}; 
        const float    __CalibrationLiquidsDefBaj[MAX_STEPPERS] = {68.59,5090.82,2181.28,1807.23,417.10,3494.85,2028.5,37.56};
        float          __microsCalibration;
        float          __mgPowder;
        float          __scaleMedition;
        bool           systemRunning, runPeristaltics, __dispensing;
        bool           show, stopShow;
        uint8_t  saltsOrder[SOLUTIONS];

        HX711 *scale;
        ExternalEEPROM *myMem;

        bool SolFinished[RELAY_NUMBER]; // Variable to know if the last Solution request finished
         // Init the object with default configuration
        void begin();
        void readRequest(); // Request a single reading of the temperature, ph and ec
        void dispense(float some_mg); // Move some stepper to dispense some_mg
         /* Move some miliseconds any pump.
        It allows to measure the ml dispense to get the calibration parameter */
        void dispenseAcid(float some_ml, uint8_t pump); // Add some ml of acid with a pump
        void dispensePeristaltic(float some_ml, uint8_t pump);
        void stopStepper(uint8_t st);
        void turnOnPump(unsigned long time1, uint8_t pump); // Turn on pump
        /* Turn on a pump.
        It allows to measure the ml dispense to get the calibration parameter */
        void turnOffPump(uint8_t pump); // Turn off pump
        void pumpCalibration(float time1, uint8_t pump);
        void setCalibrationParameter(float param, uint8_t actuator); // Set parameters for motors/pumps
        void setCalibrationParameter1(float param, uint8_t actuator); // Set parameter for ph/ec equations
        //long getMG(uint8_t saltNumber); // Returns the mg that were dispense
        //void stop(uint8_t actuator); // Stops some motor/pump
        void notFilter();
        void defaultFilter(); // Set Kalman with Noise = 0.5
        bool setExponentialFilter(float alpha = 0.5);
        bool setKalmanFilter(float noise);
        void printFilter();
        bool EZOisEnable(uint8_t sensorType); // Ask to atlas Scientific Sensor if they are enable
        void EZOcalibration(uint8_t sensorType, uint8_t act, float value);
        void EZOexportCal(uint8_t sensorType); // Run Export Calibration Sequence in Atlas Scientific Sensor
        // Import calibration parameter to Atlas Scientific Sensor
        void EZOimportCalibration(uint8_t sensorType, String parameters);
        void EZOimport(bool start); // If start true (start import), else (stop import)
        void EZOsleep(); // Enter in low power mode the Atlas Scientific Sensor
        void EZOawake(); // Awake the Atlas Scientific Sensors if they are in sleep mode
        bool EZOisSleep(); // Returns __Sleep variable
        void run(); // Run all the actuator with conditions
        void prepareSolution(float liters, float ph, float ec);

        void clean_EEPROM();
        void print_EEPROM(int i);
        void read_EEPROM(bool pr);
        void write_EEPROM(unsigned int pos, byte val);
        void default_EEPROM();
        void save_EEPROM();
        void motorOrderCfg(byte id, byte param);
        void pumpOrderCfg(byte id, byte param);
        void limitOrderCfg(byte id, byte param);
  };

#endif
