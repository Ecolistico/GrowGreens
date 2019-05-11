/*
Library for GrowGreens software package
Written by : José Manuel Casillas Martín
Date : 2019-*-*
Version : 0.1 (testing version)

This file is part of GrowGreens.

GrowGreens is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

GrowGreens is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GrowGreens.  If not, see <https://www.gnu.org/licenses/>.
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

// Class to manage day-night cycles
class Day
  {  private:
         bool __isItDay ;
         float __dayBeginHour, __nightBeginHour;

     public:
          Day ( ) ; // Constructor
          Day ( float dayTime_Hour, float nightTime_Hour ) ; // Constructor
          Day ( byte dayTime_Hour, byte nightTime_Hour, byte dayTime_Minute, byte nightTime_Minute) ; // Constructor
          bool changeDayHour (float dayTime_Hour ); // Returns true if succesful
          bool changeDayHour (byte dayTime_Hour, byte dayTime_Minute); // Returns true if succesful
          bool changeNightHour (float nightTime_Hour); // Returns true if succesful
          bool changeNightHour (byte nightTime_Hour, byte nightTime_Minute); // Returns true if succesful
          bool changeHours (float dayTime_Hour, float nightTime_Hour ); // Returns true if succesful
          bool changeHours ( byte dayTime_Hour, byte nightTime_Hour, byte dayTime_Minute, byte nightTime_Minute) ; // Returns true if succesful
          bool run ( byte HOUR, byte MINUTE ); // Returns true if day and false if night
   } ;

// Class to manage multiples day-night cycles in 24 hours
class MultiDay
 {  private:
        bool __isItDay ;
        byte __daysPerDay, __initProblem ;
        float __hoursDay, __hoursNight ;
        float __beginHour[24] ; // Max 24 cycles per day

    public:
         MultiDay ( ) ; // Constructor by default 1 day with 16 hours of light starting at 6a.m.
         MultiDay ( byte cyclesPerDay, float percentageLight, float initialHour ) ; // Constructor
         bool getState(); // Returns __isItDay
         bool redefine( byte cyclesPerDay, float percentageLight, float initialHour ) ; // Returns true if succesful
         bool run ( byte HOUR, byte MINUTE ); // Returns true if day and false if night
  } ;


// Class to control LED´s or actuators with large time cycles of work like day-night cycles
class LED
  {  private:
         bool __State , __Enable ;
         String __Name;

         void printState();

     public:
          LED ( String name ) ; // Constructor
          LED ( bool state, String name ) ; // Constructor
          void changeState() ;
          void setState(bool state) ;
          bool getState() ;
          void enable(bool en) ;
          bool isEnable() ;
          void turnOn() ;
          void turnOff() ;
   } ;

  #endif
