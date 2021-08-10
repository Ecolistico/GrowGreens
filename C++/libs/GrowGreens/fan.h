/*
Library for Grow software package
Written by : José Manuel Casillas Martín
email: jmcasimar@sippys.com.mx
Date : 2021-03-31
Version : 2.0

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

// fan.h

#ifndef fan_h
#define fan_h

#include <stdlib.h>
#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#include <wiring.h>
#endif

#include <commonStructures.h> // Shared structures bewtween differents classes

class fan
 {  private:
        bool _State, _Enable;
        uint8_t _Floor;
        uint8_t _TimeOn; // In percentage
        unsigned long _CycleTime, _ActualTime;

        void setTime();
        void resetTime();
        void fanPrint(String act, uint8_t level=0);
        void fanPrint(String act1, String act2, String act3, uint8_t level=0);

    public:
         static uint8_t _numberFan;

         fan(uint8_t floor, unsigned long t_cycle, uint8_t t_on); // Constructor

         unsigned long getTime(); // Get the time for the fan
         void setTimeOn(uint8_t t_on);
         void setCycleTime(unsigned long t_cycle);
         uint8_t getTimeOn(); // Returns TimeOn in percentage
         unsigned long getCycleTime(); // Returns TimeOff
         uint8_t getFloor(); // Returns floor variable
         void turnOn(bool rst_time); // Turn Off State & RST Time
         void turnOff(bool rst_time);  // Turn On & RST Time
         void enable (bool en); // Enable the actuator
         bool isEnable(); // Returns true if actuator is enable
         bool getState(); // Returns Actuator State

         void run(); // Run the fan
  };

  class systemFan
    { private:
        uint8_t _floorNumber;

        void systemPrint(String act1, String act2, String act3, uint8_t level=0); // Print an action for entire system

      public:
        fan *_fan[MAX_FLOOR_NUMBER]; // List of pointers to each fan control

        systemFan(uint8_t floorNum); // Constructor

        void enable(bool en); // Enable the system
        void run();

    };

#endif
