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

#define MOTOR_SPEED 4000 // Maximum Steps Per Second
#define MOTOR_ACCEL 1000 // Steps/second/Second of acceleration
#define DEFAULT_MICROSTEP 8 // By default we use configuration of 1/8 microSteps
#define DEFAULT_PULLEY_TEETH 20 // By default we use pulley of 20 teeth
#define DEFAULT_X_MM_TOOTH 2 // By default we use toothed tape of 2mm in X
#define DEFAULT_Y_MM_TOOTH 3 // By default we use toothed tape of 3mm in Y
#define MOTOR_STEP_PER_REV 200 // By default we use motors of 200 steps/rev
#define DEFAULT_MAX_X_DISTANCE_MM 11573 // By default the max distance are 11.785m
#define DEFAULT_MAX_Y_DISTANCE_MM 1914 // By  default the max distance are 1.914m
#define X_HOME_DISTANCE_MM 12500 // The max distance to get home in x
#define Y_HOME_DISTANCE_MM 2500 // The max distance to get home in y
#define WAIT_TIME_FOR_GO_HOME 1800000 // By default 30min the value is given in ms
#define MIN_LIMIT_SECURITY_DISTANCE 15 // The min distance when moving to not touch the limit switch

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

        int __Sequence; // Variable to know in what part of the sequence is the grower
        bool __SequenceStop; // Variable to stop the sequence until sequenceContinue is executed
        bool __SequenceStageFinished; // Variable to inform that sequence movements has finished
        int __SequenceMovements; // Variable to know the max number of movements
        int __SequenceXMoves, __SequenceYMoves; // Variable to know the number of moves in x and y
        int __SequenceDir;
        long __SequenceXmm, __SequenceYmm;

        long __MaxX, __MaxY; // Maximun security distances
        bool __OutHomeX1, __OutHomeX2, __OutHomeY; // Aux variables to allow move out of the limits switch
        bool __MoveX1, __MoveX2, __MoveY; // Aux variable to detect when some motors stops moving

        uint8_t __Floor;
        bool __Stop;

        static uint8_t __steppersRunning;

        long StepsToMM_X(long steps); // Return the number of mm that equals the steps parameter in X
        long MMToSteps_X(long dist_mm); // Return the number of steps that equals the mm parameter in X
        long StepsToMM_Y(long steps); // Return the number of mm that equals the steps parameter in Y
        long MMToSteps_Y(long dist_mm); // Return the number of steps that equals the mm  parameter in Y
        void resetTime(); // Reset the time since the last time the motors were used
        bool isTimeToGoHome(); // Return true when it´s been a while without moving the motors and it´s not at home
        void printAction(String act); // Print in serial an action executed with the correct format for each grower
        void printAction(String act, uint8_t level); // Print an action with format and log level
        void setMaxAccel(uint8_t stepper); // Allows stop the motors to fast
        void setNormalAccel(); // Allows move normally the motors

    public:
        // Constructor. Dir, Step and Home Pins for all the motors
        growerStepper(
          uint8_t fl,
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
          bool goHome,
          uint8_t steps_per_rev = MOTOR_STEP_PER_REV,
          uint8_t microStep = DEFAULT_MICROSTEP,
          uint8_t pulleyTeeth = DEFAULT_PULLEY_TEETH,
          uint8_t Xmm = DEFAULT_X_MM_TOOTH,
          uint8_t Ymm = DEFAULT_Y_MM_TOOTH
        );

        long getXPosition(); // Returns the position in mm. It assumes that home was reached
        long getYPosition(); // Returns the position in mm. It assumes that home was reached
        void setMaxDistanceX(long maxDist); // Set the max security distance that can be run in x
        void setMaxDistanceY(long maxDist); // Set the max security distance that can be run in Y
        long getMaxDistanceX(); // Get the max security distance in X
        long getMaxDistanceY(); // Get the max security distance in Y
        bool moveX(long some_mm, bool seq = false); // Move X some_mm relative to its actual position
        bool moveY(long some_mm, bool seq = false); // Move Y some_mm relative to its actual position
        bool moveXTo(long some_mm, bool seq = false); // Move X to absolut position some_mm
        bool moveYTo(long some_mm, bool seq = false); // Move Y to absolut position some_mm
        bool calibration(); // Runs the calibration mode to stablish the maximun security distance in X and Y
        void enable(); // Enable the steppers
        void disable(); // Disable the steppers
        bool isEnable(); // Returns true if enable
        bool isAvailable(); // Returns true if available
        bool isInCalibration(); // Returns true if is running a calibration sequence
        bool continueSequence(); // Allow the grower to continue if is in sequence
        void stopSequence(); // Stop a sequence if there is one running
        // Check if it is possible start a sequence and calculate some parameters
        bool sequence(long mm_X, long mm_Y);
        bool home(); // Go to Home
        void stop(); // Stops all the motors
        void stop(uint8_t st); // Stops just 1 motor
        void run(); // Run all the steppers with conditions
  };

#endif
