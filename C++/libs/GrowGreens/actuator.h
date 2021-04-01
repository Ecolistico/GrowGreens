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

// actuator.h
#ifndef actuator_h
#define actuator_h

#include <stdlib.h>
#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#include <wiring.h>
#endif

#define MAX_ACTUATORS           32 // Max number of Actuators or Solenoid Valves
#define DEFAULT_TIME_ON         5 // 5 seconds
#define DEFAULT_CYCLE_TIME      600 // 10 minutes = 600 seconds
#define flowSensor              2 // Define flowSensor pin

/*
class solenoidValve
  {  private:
        static bool __EnableGroup; // Group Enable variable
        static String __Group; // Group Name
        static uint8_t __ActualNumber; // Actual Number working into the array
        static unsigned long __CycleTime; // Cycle time
        static unsigned long __ActionTime; // Effective action time
        static float __K; // flowSensor constant
        static volatile long __NumPulses; // count flowSensorpulses
        static float __H2Ow; // Potential Water Wasted. Flow measure when no solenoids are active.

        static void countPulses(); // Interrupt Caudalimeter

         bool __State, __Enable;// Actuator State and Enable for each solenoid
         uint8_t __Number, __Floor, __Region; // Actuator place into array, floor and region
         unsigned long __TimeOn, __Actual_time;
         float __H2OVolume; // Water consumption for each solenoidValve

         void setTime();
         void resetTime();
         bool changeOrderNumber(uint8_t new_number); // Change the order number. Return 0 if succesful
         void getWasteH2O(bool print = true); // Get Water Consumption  when no solenoids are active
         void getConsumptionH2O(); // Get Water Consumption for each Region
         void solenoidPrint(String act, uint8_t level=0); // Print an action for the solenoid
         void solenoidPrint(String act1, String act2, String act3, uint8_t level=0); // Print an action for the solenoid
         void groupPrint(String act, uint8_t level=0); // Print an action for the group
         void groupPrint(String act1, String act2, String act3, uint8_t level=0); // Print an action for the group

     public:
          static uint8_t __TotalActuators;
          static solenoidValve *ptr[MAX_ACTUATORS]; // List of pointers to each object

          static void flowSensorBegin();

          solenoidValve (); // Constructor

          unsigned long getTime(bool zero);

          unsigned long getCurrentTime(); // Returns the time into the cycle for the particular actuator
          bool setTimeOn(unsigned long t_on); // Return true if succesful
          unsigned long getTimeOn();
          bool getState(); // Returns actuator state
          uint8_t getOrder(); // Returns the order number into the group of the actuator
          void turnOn(bool rst_time); // Turn Off State & RST Time
          void turnOff(bool rst_time); // Turn On & RST Time
          bool reOrder(uint8_t number); // Return true if succesful
          void enable(bool en); // Enable the actuator
          bool isEnable(); // Returns true if the array of the actuator is enable
          uint8_t getFloor(); // Returns the floor of the actuator
          uint8_t getRegion(); // Return the region of the actuator
          bool reOrder_byFloor(uint8_t fl); // Send to the last positions the
          void restartH2O(bool print); // Set Volume in zero
          void restartAllH2O(bool print); // Set all Volumes in zero
          float getH2O(); // Return Volume of H2O
          void printH2O(); // Print on serial the acummulate water consumption
          void printAllH2O(bool printAll = true); // Print on serial all the acummulates of water
          // Variables of time in seconds
          uint8_t begin(uint8_t fl, uint8_t reg); // Return true if succesful
          bool run();
          static unsigned long getActionTime();
          static unsigned long getCycleTime();
          static bool setCycleTime (unsigned long t_cycle); // Return true if succesful
          static uint8_t getActualNumber(); // Returns the actual actuator number active into the array
          static bool reOrderAll(bool sequence, bool order); // True if succesful
          static bool defaultOrder(uint8_t night_floor); // Clarisa Favorite Order
          static void enableGroup(bool en); // Enable the group
          static bool isEnableGroup(); // Returns true if the array of the actuator is enable
          static float getWaterAll(); // Returns the total of the water consumption
          // Returns the total of the water consumption by floor
          static float getWaterByFloor(uint8_t fl);
   };
*/

// Class to control all the Actuators that are not synchronize or have predefine cycles.
/* Types:
 * 0 - "EV-KegsH2O"
 * 1 - "EV-KegsNutrition"
*/
class Actuator
  {  private:
         bool __State, __Enable;
         uint8_t __Type, __Counter;

         void printAction(String act, uint8_t level=0);

     public:
         // Constructor
         asyncActuator(uint8_t type=250);
         void turnOn();
         void turnOff();
         bool getState(); // Returns asyncActuator State
         void enable (bool en); // Enable the asyncActuator
         bool isEnable(); // Returns true if asyncActuator is enable
   };

  #endif
