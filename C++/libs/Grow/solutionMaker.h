/*
Library for Grow software package
Written by : José Manuel Casillas Martín
email: jmcasimar@sippys.com.mx
Date : 2019-*-*
Version : 0.1 (testing version)

This file is part of Grow Software Package.

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

#ifndef solutionMaker_h
#define solutionMaker_h

#include <stdlib.h>
#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#include <wiring.h>
#endif

#include "AccelStepper.h" // Include AccelStepper library
#include <Wire.h> // Necessary for I2C communication
#include <LiquidCrystal_I2C.h> // For LCD screen
#include <OneWire.h> // Necessary for 1-wire communication
#include <DallasTemperature.h> // For temperature sensor

#define MAX_SOLUTIONS_NUMBER 4 // The max number of motors that can dispense
#define MAX_PUMPS_NUMBER 2 // The max number of peristaltic pumps
#define MOTOR_SPEED 4000 // Maximum Steps Per Second
#define MOTOR_ACCEL 1000 // Steps/second/Second of acceleration
#define DEFAULT_MICROSTEP 8 // By default we use configuration of 1/8 microSteps
#define MOTOR_STEP_PER_REV 200 // By default we use motors of 200 steps/rev
#define PUMP_VELOCITY 155 // Value pwm -> 255 = 100%, 155 = 60% of the maximun velocity.
// Less than 155(60%) may not work
#define LCD_I2C_DIR 0x27 // Direction for screen
#define LCD_COLUMNS 20 // Screen columns number
#define LCD_ROWS 4 // Screen rows number

#define PH_I2C_ADDRESS 99 // default I2C ID number for EZO pH Circuit.
#define SHORT_PH_DELAY 300 // Short delay for some ph responses
#define LONG_PH_DELAY 900 // Long delay for some ph responses

// Class to control the Solution Maker
/*
Note: All the motors need same configuration and direction
*/
class solutionMaker
  {
    private:
        uint8_t        __DirS[MAX_SOLUTIONS_NUMBER], __StepS[MAX_SOLUTIONS_NUMBER]; // Pins required for all the motors
        uint8_t        __Motor[MAX_PUMPS_NUMBER*2]; // Pins to control all the peristaltic pumps
        uint8_t        __En[MAX_SOLUTIONS_NUMBER+MAX_PUMPS_NUMBER]; // Pin to enable all the motors and pumps
        bool           __IsEnable[MAX_SOLUTIONS_NUMBER+MAX_PUMPS_NUMBER]; // Let´s know if the motor/pump isenable/disable
        bool           __Available[MAX_SOLUTIONS_NUMBER+MAX_PUMPS_NUMBER]; // Let´s know if the motor/pump available/unavailable
        uint8_t        __Calibration[MAX_SOLUTIONS_NUMBER+MAX_PUMPS_NUMBER]; // Calibration parameter for motor/pumps

        uint8_t        __MicroSteps; // Inverse of the configurated microSteps in the driver
        uint8_t        __StepPerRev; // Steps per rev

        uint8_t        __PumpVelocity; // pwm value
        bool           __InvertPump; // Invert the direction of all the pumps
        unsigned long  __PumpTime[MAX_PUMPS_NUMBER]; // Time counter for pumps working
        unsigned long  __PumpOnTime[MAX_PUMPS_NUMBER]; // The time that the pumps will be working
        unsigned long __ReadTime, __LCDTime; // Auxiliars counter for tempReads and LCD turn off light
        
        uint8_t        __Led[MAX_SOLUTIONS_NUMBER+MAX_PUMPS_NUMBER+2]; // Pins for LED´s
        // One led for each solution, one for each acid and two for status
        bool           __Work; // Variable to know if is working the Solution Maker
        bool           __StatusLCD; // Variable to know if the screen is on/off
        bool           __LCDLightOn; // Variable to turn off the LCD light

        // Atlas Scientific Sensors variables
        byte __PHcode;  // Variable to hold the I2C response code from phMeter
        char __PHdata[20]; // Variable to read the incoming data form the sensor
        unsigned long __PHtime; // Variable to wait for the ph response
        float __PHvalue; // Variable to hold the phMeter value

        // Temperature sensor
        float __Temp; // Variable to hold the temperature
        float __LCDTemp; // Variable to change the Temp Value on LCD

        // Constants for the filters
        byte __Filter ;
        float __Alpha , __Noise , __Err;
        // Exponential Filter: __Alpha = smoothing variable
        // Kalman Filter: __Noise = Enviromental Noise, __Err = Error

        // Pointers
        AccelStepper *stepperS[MAX_SOLUTIONS_NUMBER]; // Pointer for each stepper motor
        LiquidCrystal_I2C *lcd; // Pointer to control LCD screen
        OneWire *ourWire; // Pointer to Wire Object
        DallasTemperature *tempSensor; // Pointer to temperature sensor

        bool isWorking(); // Returns true if some actuator is unavailable, else false
        void enable(uint8_t actuator); // Enable/Disable the actuator
        void disable(uint8_t actuator); // Disable the actuator
        void turnOnPump(unsigned long time1, uint8_t pump); // Turn on pump
        void turnOffPump(uint8_t pump); // Turn off pump
        /* Converts time [s] to volume [ml] for the pumps
            By default use calibration param of the first pump assuming that all are equals
        */
        unsigned long TimeToML(float seconds, uint8_t pump);
        /* Converts volume [ml] to time [s] for the pumps
            By default use calibration param of the first pump assuming that all are equals
        */
        unsigned long MLToTime(float mililiters, uint8_t pump);
        // Return the number of grams that equals the revs parameter in some stepper
        long RevToGrams(long rev, uint8_t st);
        // Return the number of revs that equals the grams parameter in some stepper
        long GramsToRev(long grams, uint8_t st);
        long RevToSteps(long rev); // Returns the number of steps that are require to move one rev
        long StepsToRev(long st); // Returns the number of rev that are some steps
        void moveStepper(long steps, uint8_t st);
        void resetPosition(uint8_t st); // reset the position to zero of some stepper
        // Print in serial an action executed with the correct format
        void printAction(String act, uint8_t actuator);
        void printLCD(String main, String subAction = ""); // Print something in LCD Screen
        float filter(float val, float preVal);
        float exponential_filter(float alpha, float t, float t_1);
        float kalman_filter(float t, float t_1);
        void readTemperature(); // Take a single reading of the temperature

    public:
        // Constructor. Dir, Step and Enable Pins for all the motors
        solutionMaker(
          uint8_t dirS1,
          uint8_t stepS1,
          uint8_t dirS2,
          uint8_t stepS2,
          uint8_t dirS3,
          uint8_t stepS3,
          uint8_t dirS4,
          uint8_t stepS4,
          uint8_t enable1,
          uint8_t enable2,
          uint8_t enable3,
          uint8_t enable4,
          uint8_t motor1A,
          uint8_t motor1B,
          uint8_t motor2A,
          uint8_t motor2B,
          uint8_t en1,
          uint8_t en2,
          uint8_t led1,
          uint8_t led2,
          uint8_t led3,
          uint8_t led4,
          uint8_t led5,
          uint8_t led6,
          uint8_t led7,
          uint8_t led8,
          uint8_t tempSens
        );

         // Init the object with default configuration
        void begin(
          uint8_t steps_per_rev = MOTOR_STEP_PER_REV,
          uint8_t microStep = DEFAULT_MICROSTEP,
          uint8_t pump_velocity = PUMP_VELOCITY,
          bool invertPump = false
        );

        bool dispense(long some_grams, uint8_t st); // Move some stepper to dispense some_grams
        /* Move some revs in any stepper.
        It allows to measure the grams dispense to get the calibration parameter */
        void stepperCalibration(long rev, uint8_t st);
        /* Move some miliseconds any pump.
        It allows to measure the grams/ml dispense to get the calibration parameter */
        bool dispenseAcid(float some_ml, uint8_t pump); // Add some ml of acid with a pump
        /* Turn on a pump.
        It allows to measure the ml dispense to get the calibration parameter */
        void pumpCalibration(float time1, uint8_t pump);
        void setCalibrationParameter(uint8_t param, uint8_t actuator);
        bool isEnable(uint8_t actuator); // Returns true if actuator is enable, else false
        bool isAvailable(uint8_t actuator); // Returns true if actuator is available, else false
        long getGrams(uint8_t st); // Returns the grams that were dispense
        void setPHtime(); // Set the timer to wait for phMeter response
        void stop(uint8_t st); // Stops some motor/pump
        void notFilter() ;
        void defaultFilter() ; // Set Kalman with Noise = 0.5
        bool setExponentialFilter(float alpha = 0.5) ;
        bool setKalmanFilter(float noise) ;
        void printFilter();
        void eventLCD(); // Read Solution Maker Status and print it on the screen
        void run(); // Run all the actuator with conditions

  };

#endif
