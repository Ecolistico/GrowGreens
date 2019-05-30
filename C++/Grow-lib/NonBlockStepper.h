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

// NonBlockStepper.h

#ifndef NonBlockStepper_h
#define NonBlockStepper_h

#include <stdlib.h>
#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#include <wiring.h>
#endif


class NonBlockingStepper
  {
    private:
        uint8_t        __Dir, __Step; // Pins required to the stepper motor
        long           __CurrentPos;    // Current Absolut Position in Steps
        long           __TargetPos;     // Target Position in Steps
        float          __Speed;         // Motor Speed in Steps per second
        float          __MaxSpeed;      // Maximun Speed in Steps per Second
        float          __Acceleration; // Acceleration in Steps per Second per Second
        unsigned long  __StepInterval; // Interval between steps in miliseconds
        unsigned long  __LastStepTime; // The last step time in miliseconds

    protected:
        void computeNewSpeed(); // Compute and set new __Speed
        virtual void step(); // Take one step
        virtual float desiredSpeed(); // Compute and return desired speed

    public:
        AccelStepper(uint8_t dir = 2, uint8_t step = 3); // Constructor. Dir and Step pins needed
        void moveTo(long absolute); // Move to Absolute Position
        void move(long relative); // Move to Relative Position
        boolean run();  // Return true if the motos is at the target position. Else compute the algorithm
        boolean runSpeed(); // Return true if the motor was stepped. Moves at constant speed.
        void setMaxSpeed(float speed);  // Set __MaxSpeed. More than 1000 steps per second are unreliable
        void setAcceleration(float acceleration); // Set Acceleration Parameter > 0
        void setSpeed(float speed); // Set Constant Speed for runSpeed()
        float speed(); // Returns __Speed
        long distanceToGo(); // Distance from the current Position to Target Position
        long targetPosition(); // Returns __TargetPos
        long currentPosition(); // Returns __CurrentPos
        void setCurrentPosition(long position); // Set __CurrentPos
        void disableOutputs();
        void enableOutputs();

  };

#endif
