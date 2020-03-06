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

// day.h

#ifndef day_h
#define day_h

#include <stdlib.h>
#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#include <wiring.h>
#endif

#define MAX_LEDS    20 // Max LEDs Objects
#define MAX_FLOOR   4 // Number of floors
#define MAX_REGION  4 // Number of regions

// Class to manage multiples day-night cycles in 24 hours
class MultiDay
 {  private:
        bool __isItDay;
        uint8_t __daysPerDay;
        float __hoursDay, __hoursNight;
        float __beginHour[24]; // Max 24 cycles per day

    public:
         MultiDay(); // Constructor by default 1 day with 16 hours of light starting at 6a.m.
         MultiDay(uint8_t cyclesPerDay, float percentageLight, float initialHour); // Constructor
         bool getState(); // Returns __isItDay
         uint8_t getProblem(); // Returns problem with init or redefine functions
         void redefine(uint8_t cyclesPerDay, float percentageLight, float initialHour); // Returns true if succesful
         bool isDay(uint8_t HOUR, uint8_t MINUTE); // Returns true if day and false if night
  };


// Class to control LED_Module´s or actuators with large time cycles of work like day-night cycles
/*
Definitions:
  Floor. It can be a whole number from 0 to MAX_FLOOR-1
  Region. It can be a whole nuber from 1 to MAX_FLOOR-1
  In alpha version each region is:
    * 0 = Germination
    * 1 = Stage1
    * 2 = Stage2
    * 3 = Stage3
*/
class LED_Mod
  {  private:
         bool __State, __Enable;
         uint8_t __Floor, __Region;

         void printAction(String act, uint8_t level=0);
         void setState(bool state);

     public:
        static uint8_t __TotalLeds;
        static LED_Mod *ptr[MAX_LEDS]; // List of pointers to each object

        LED_Mod (uint8_t floor, uint8_t region); // Constructor
        bool getState();
        uint8_t getFloor();
        uint8_t getRegion();
        void enable(bool en);
        bool isEnable();
        void turnOn();
        void turnOff();
        static void turnOn(uint8_t floor); // Turn on all the floor
        static void turnOff(uint8_t floor); // Turn off all the floor
        // Enable/Disable regions minor than region parameter into a floor
        static void enable(uint8_t floor, uint8_t region);
   };

// Class to reduce power problems reducing LED Time Module to the half of the time
class fixDay {
    private:
        bool __Led1, __Led2, __Led3, __Led4; // One Variable per floor
        void printAction(String act, uint8_t level=0);
        
    public:
        fixDay(); // Constructor
        bool getLed1();
        bool getLed2();
        bool getLed3();
        bool getLed4();
        void updateLedState(uint8_t hour);
};
  #endif
