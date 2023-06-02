/*
Library for Grow software package
Written by : José Manuel Casillas Martín
email: jmcasimar@sippys.com.mx
Date : 2021-01-22
Version : 2.1 (Cancun Optimized Version)

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

// solenoid.h

// Class to redefine an Irrigation Solenoid Valve
// The objective is to minimize the variables needed to optimize the code

#ifndef solenoid_h
#define solenoid_h

#include <stdlib.h>
#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#include <wiring.h>
#endif

#include <sensor.h>
#include <dynamicMemory.h>
#include <commonStructures.h> // Shared structures bewtween differents classes

class solenoid
  {  private:
         bool _Enable, _Region; // Solenoid Enable and Region for each solenoid
         uint8_t _Number, _Floor, _PrintNumber; // Solenoid place into array and Floor
         unsigned long _TimeOn, _ActualTime;
         float _H2OVolume; // Water consumption for each Solenoid
         uint8_t _Cycles;

         void resetTime();
         void solenoidPrint(String act, uint8_t level = 0); // Print an action for the Solenoid
         void solenoidPrint(String act1, String act2, String act3, uint8_t level = 0); // Print an action for the Solenoid
     public:
          bool _State;    // Solenoid State,

          static uint8_t _numberSolenoid; 

          solenoid (uint8_t num, uint8_t printNum, uint8_t floor, bool reg, unsigned long timeOn, uint8_t cycles); // Constructor

          void setTime();
          void changeOrder(uint8_t new_number); // Change the order number
          void addConsumptionH2O(float newVolume); // Add Water Consumption for each Solenoid
          unsigned long getTime(); // Get the time for the Solenoid
          void setTimeOn(unsigned long t_on); // In miliseconds
          void setCyclesNumber(uint8_t c_num); 
          unsigned long getTimeOn(); // Get the time On for the Solenoid
          uint8_t getCyclesNumber(); 
          bool getState(); // Returns actuator state
          uint8_t getNumber(); // Returns the order number assign to the Solenoid
          void turnOn(bool rst_time); // Turn Off State & RST Time
          void turnOff(bool rst_time); // Turn On & RST Time
          void enable(bool en); // Enable the Solenoid
          bool isEnable(); // Return Enable State for the Solenoid
          void restartH2O(bool print); // Set Volume in zero
          float getH2O(); // Return Volume of H2O
          void printH2O(); // Print on serial the acummulate water consumption
   };


class floorValves
  {  private:
          uint8_t _Floor, _valvesPerRegion; // Floor where the solenoids are placed

          void regionPrint(String act1, String act2, String act3, bool reg, uint8_t level = 0); // Print an action for each Region
          void floorPrint(String act1, String act2, String act3, uint8_t level = 0); // Print an action for each Region

     public:
          float _H2O_regA, _H2O_regB; // Water consumption for each side
          static uint8_t _floorNumber;
          solenoid *_regA[MAX_VALVES_PER_REGION]; // List of pointers to solenoids for region A
          solenoid *_regB[MAX_VALVES_PER_REGION]; // List of pointers to solenoids for region B

          floorValves (uint8_t floor, uint8_t valvesPerRegion); // Constructor

          void updateH2O(); // Recover the consumption for each solenoid to get each region consumption
          void restartH2O(bool print); // Restart the values of each H2O Solenoids in the Region
          void printH2O(); // Print on serial the acummulate water consumption of each Solenoid in the floor
          void restartH2O_floor(bool print); // Restart the values of consumption in the floor
          void printH2O_floor(); // Print Consumption of H2O by floor
          void enable(bool en, uint8_t reg); // Enable and disable all the solenoids of an specific region
  };

class systemValves
  { private:
      bool _Enable; // Disable all the system
      unsigned long _ActualTime, _CycleTime; // Cycle time in minutes
      uint8_t _floorNumber, _valvesPerRegion, _actualNumber; // How many floors conforms the system
      float _H2O_Consumption; // Consumption of water
      float _auxH2O; // Auxiliar variable to calculate the consumption of water
      uint8_t _currentCycle;

      void systemPrint(String act1, String act2, String act3, uint8_t level = 0); // Print an action for entire system
      void printAtFirst(); // Function that runs when _actualNumber == 0, print and reset water info

    public:
      floorValves *_floor[MAX_FLOOR_NUMBER]; // List of pointers to each floor control

      systemValves(basicConfig bconfig, dynamicMem & myMem); // Constructor

      void updateH2O(); // Recover the consumption of each floor and gets the total amount of water used
      void restartH2O(bool print); // Restart the values of each H2O Solenoids in the System
      void printH2O(); // Print on serial the acummulate water consumption of each Solenoid in the system
      void restartH2O_system(bool print); // Restart the values of consumption in the system
      void printH2O_system(); // Print Consumption of H2O by system

      unsigned long getActionTime(); // Return total time On of the system
      unsigned long getCycleTime(); // Return Cycle Time
      void setCycleTime (unsigned long t_cycle); // In minutes
      void resetTime(); // Restart the time into the Cycle
      unsigned long getTime(); // Return Time Since Cycle Starts
      uint8_t getActualNumber(); // Returns the actual actuator number active into the array
      void enable(bool en); // Enable the system
      bool isEnable(); // Returns true if the solenoid System is enable
      void incrementCycle();
      uint8_t getCurrentCycle();
      // To Finish
      /* To do:
        1- Add FlowMeter sensor to this class (CANCELLED)
        2- Add Water Consumption functionality with scale sensor
     */
      void invertOrder(bool invert); // Fucntion to order system
      void run(ScaleSens *scale); // Let´s magic happen
  };

#endif
