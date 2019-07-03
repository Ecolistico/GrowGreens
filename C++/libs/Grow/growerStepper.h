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

// growerStepper.h

#ifndef growerStepper_h
#define growerStepper_h

#include <stdlib.h>
#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#include <wiring.h>
#endif

#include "AccelStepper.h" // Include AccelStepper library

#define MOTOR_SPEED 10000 // Maximum Steps Per Second
#define MOTOR_ACCEL 2500 // Steps/second/Second of acceleration
#define DEFAULT_MICROSTEP 8 // By default we use configuration of 1/8 microSteps
#define DEFAULT_PULLEY_TEETH 20 // By default we use pulley of 20 teeth
#define DEFAULT_X_MM_TOOTH 2 // By default we use toothed tape of 2mm in X
#define DEFAULT_Y_MM_TOOTH 3 // By default we use toothed tape of 3mm in Y
#define MOTOR_STEP_PER_REV 200 // By default we use motors of 200 steps/rev
#define DEFAULT_MAX_X_DISTANCE_MM 11000 // By default the max distance are 11m
#define DEFAULT_MAX_Y_DISTANCE_MM 2100 // By  default the max distance are 2.1m
#define WAIT_TIME_FOR_GO_HOME 1800000 // By default 30min the value is given in ms

// Class to control the motors of a Grower
class growerStepper
  {
    private:
        uint8_t        __DirX1, __StepX1; // Pins required to the stepper motor X1
        uint8_t        __DirX2, __StepX2; // Pins required to the stepper motor X2
        uint8_t        __DirY, __StepY; // Pins required to the stepper motor Y
        uint8_t        __HomeX1, __HomeX2, __HomeY; // Pins required to home
        uint8_t        __Enable; // Pin to enable all the motors
        bool           __IsEnable; // Let´s know if the motor are enable/disable
        unsigned long  __ActualTime; // The time that the motor has stopped

        // Inverse of the configurated microSteps in the driver
        uint8_t        __MicroSteps;
        // Teeth of the pulley, Xmm per tooth, Ymm per tooth
        uint8_t        __PulleyTeeth, __Xmm, __Ymm;
        uint8_t        __StepPerRev;

        AccelStepper *stepperX1;
        AccelStepper *stepperX2;
        AccelStepper *stepperY;

        bool __Available; // Variables to know if the growerStepepr is working
        bool __Home, __IsAtHome; // Variable to know if right now is in Home Position
        uint8_t __Calibration; // Aux variable for calibration algorithm

        long __MaxX, __MaxY; // Maximun security distances
        bool __OutHomeX1, __OutHomeX2, __OutHomeY; // Aux variables to allow move out of the limits switch

        //static uint8_t __steppersRunning;

        long StepsToMM_X(long steps); // Return the number of mm that equals the steps parameter in X
        long MMToSteps_X(long dist_mm); // Return the number of steps that equals the mm parameter in X
        long StepsToMM_Y(long steps); // Return the number of mm that equals the steps parameter in Y
        long MMToSteps_Y(long dist_mm); // Return the number of steps that equals the mm  parameter in Y
        void resetTime(); // Reset the time since the last time the motors were used
        bool isTimeToGoHome(); // Return true when it´s been a while without moving the motors and it´s not at home

    public:
        // Constructor. Dir, Step and Home Pins for all the motors
        growerStepper(
          uint8_t dirX1,
          uint8_t stepX1,
          uint8_t dirX2,
          uint8_t stepX2,
          uint8_t dirY,
          uint8_t stepY,
          uint8_t homeX1,
          uint8_t homeX2,
          uint8_t homeY,
          uint8_t en
        );

         // Init the group of motors with default configuration and send grower to home
        void begin(
          uint8_t steps_per_rev = MOTOR_STEP_PER_REV,
          uint8_t microStep = DEFAULT_MICROSTEP,
          uint8_t pulleyTeeth = DEFAULT_PULLEY_TEETH,
          uint8_t Xmm = DEFAULT_X_MM_TOOTH,
          uint8_t Ymm = DEFAULT_Y_MM_TOOTH,
          bool goHome = HIGH
        );

        long getXPosition(); // Returns the position in mm. It assumes that home was reached
        long getYPosition(); // Returns the position in mm. It assumes that home was reached
        void setMaxDistanceX(long maxDist); // Set the max security distance that can be run in x
        void setMaxDistanceY(long maxDist); // Set the max security distance that can be run in Y
        long getMaxDistanceX(); // Get the max security distance in X
        long getMaxDistanceY(); // Get the max security distance in Y
        bool moveX(long some_mm); // Move X some_mm relative to its actual position
        bool moveY(long some_mm); // Move Y some_mm relative to its actual position
        bool moveXTo(long some_mm); // Move X to absolut position some_mm
        bool moveYTo(long some_mm); // Move Y to absolut position some_mm
        void calibration(); // Runs the calibration mode to stablish the maximun security distance in X and Y
        void enable(); // Enable the steppers
        void disable(); // Disable the steppers
        bool isEnable(); // Returns true if enable
        bool isAvailable(); // Returns true if available
        bool home(); // Go to Home
        void run(); // Run all the steppers with conditions
  };

#endif
