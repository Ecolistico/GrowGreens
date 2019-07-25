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

#define MAX_ACTUATORS           50 // Max number of Actuators or Solenoid Valves
#define DEFAULT_TIME_ON         5 // 5 seconds
#define DEFAULT_CYCLE_TIME      600 // 10 minutes = 600 seconds
#define MAX_TYPES_ACTUATOR      4 // Right now we have just 4 types
#define MAX_FLOOR               4 // Number of floors
#define MAX_IRRIGATION_REGIONS  8 // Number of region in each floor
#define flowSensor              2 // Define flowSensor pin

// Class to manage an actuator with defined time cycles.
/*
Definitions
  Types:
    Input Fan = 0
    Output Fan = 1
    Vent Fan = 2
    Humidity Valve = 3
  Floor: RealFloor-1 [0-3]
Limitation Note: This class is limited by definition to 50
  actuators this limitations can be changed but, you need to keep
  in mind your procesator memory consumption
*/
class Actuator
 {  private:
        bool __State , __Enable;
        uint8_t __Type, __Floor; // What type of actuator and in what floor is it.
        unsigned long __TimeOn , __CycleTime, __Actual_time;

        void set_time();
        void resetTime();
        void printAction(String act);
        void turnOn();
        void turnOff();
        bool setTimeOn(unsigned long t_on); // Returns true if succesful
        bool setCycleTime(unsigned long t_cycle); // Returns true if succesful
        bool setTime(unsigned long t_on, unsigned long t_cycle); // Return true if succesful
        unsigned long getTimeOn(); // Returns TimeOn
        unsigned long getCycleTime(); // Returns TimeOff
        unsigned long getTime(); // Returns the time into the cycle from [0,timeOff+timeOn].
        void enable (bool en); // Enable the actuator
        bool isEnable(); // Returns true if actuator is enable
        bool isType(uint8_t type); // Return true if is the same type
        bool isFloor(uint8_t floor); // Return true if is the same floor
        void begin(); // Start the actuator
        void run(); // Run the actuator

    public:
         static byte __TotalActuators;
         static Actuator *ptr[MAX_ACTUATORS]; // List of pointers to each object

         Actuator( // Constructor
           uint8_t type,
           uint8_t floor,
           unsigned long t_on,
           unsigned long t_cycle
         );

         bool getState(); // Returns Actuator State

         // Looks for the correct actuator and execute setTimeOn
         static void setTimeOnAll(
           uint8_t type,
           uint8_t floor,
           unsigned long timeOn
         );
         // Looks for the correct actuator and execute setCycleTime
         static void setCycleTimeAll(
           uint8_t type,
           uint8_t floor,
           unsigned long cycleTime
         );
         // Looks for the correct actuator and execute setTime
         static void setTimeAll(
           uint8_t type,
           uint8_t floor,
           unsigned long timeOn,
           unsigned long cycleTime
         );
         // Looks for the correct actuator and execute setTimeOn
         static unsigned long getTimeOnAll(
           uint8_t type,
           uint8_t floor
         );
         // Looks for the correct actuator and execute setCycleTime
         static unsigned long getCycleTimeAll(
           uint8_t type,
           uint8_t floor
         );
         // Looks for the correct actuator and execute setTime
         static unsigned long getTimeAll(
           uint8_t type,
           uint8_t floor
         );
         // Looks for the correct actuator and execute enable
         static void enableAll(
           uint8_t type,
           uint8_t floor,
           bool en
         );
         static void beginAll(); // Starts all the actuators
         static void runAll(); // Run all the actuators
  } ;

// Class to manage and synchronize multiples Solenoids Valves
/*
Limitation Note: This class is limited by definition to
maximum 50 actuators this limitations can be changed but,
you need to keep in mind your procesator memory consumption

Others definitions:
  Floor. It can be a whole number from 0 to MAX_FLOOR-1
  Region. It can be a whole nuber from 0 to MAX_FLOOR-1
  In alpha version each region is:
    * 0 = Germination A
    * 1 = Stage1 A
    * 2 = Stage2 A
    * 3 = Stage3 A
    * 4 = Germination B
    * 5 = Stage1 B
    * 6 = Stage2 B
    * 7 = Stage3 B

  Function reOrder_all(bool, bool) works as follow:
  LL - Ga, 1a, 2a, 3a, Gb, 1b, 2b, b3
  LH - 3b, 2b, 1b, Gb, 3a, 2a, 1a, Ga
  HH - 3a, 3b, 2a, 2b, 1a, 1b, Ga, Gb
  HL - Gb, Ga, 1b, 1a, 2b, 2a, 3b, 3a
*/
class solenoidValve
  {  private:
         bool __State, __Enable;// Actuator State and Enable for each solenoid
         byte __Number , __Floor , __Region ; // Actuator place into array, floor and region
         unsigned long __TimeOn , __Actual_time ;
         String __Name;
         float __H2OVolume; // Water consumption for each solenoidValve

         static bool __Enable_group; // Group Enable variable
         static String __Group ; // Group Name
         static byte __ActualNumber ; // Actual Number working into the array
         static unsigned long __CycleTime; // Cycle time
         static unsigned long __ActionTime; // Effective action time
         static float __K; // flowSensor constant
         static volatile long __NumPulses ; // count flowSensorpulses
         static void countPulses(); // Interrupt Caudalimeter
         static float __H2Ow; // Potential Water Waster. Flow measure when no solenoids are active.

         void set_time() ;
         void resetTime() ;
         bool changeOrderNumber(byte new_number); // Change the order number. Return 0 if succesful
         void getWasteH2O(bool print = HIGH) ; // Get Water Consumption  when no solenoids are active
         void getConsumptionH2O() ; // Get Water Consumption for each Region

     public:
          static byte __TotalActuators;
          static solenoidValve *ptr[MAX_ACTUATORS] ; // List of pointers to each object

          static void flowSensorBegin();

          solenoidValve (String name) ; // Constructor
          unsigned long getTime() ; // Returns the time into the cycle for the particular actuator
          // Variables of time in microseconds
          bool setTimeOn ( unsigned long t_on ) ; // Return true if succesful
          bool setCycleTime ( unsigned long t_cycle ) ; // Return true if succesful
          unsigned long getTimeOn() ;
          unsigned long getCycleTime() ;
          bool getState() ; // Returns actuator state
          void changeState() ; // Change actual actuator state
          void turnOn( ) ;
          void turnOff( ) ;
          void changeState(bool rst_time) ; // Change State & RST Time
          void turnOn(bool rst_time) ; // Turn Off State & RST Time
          void turnOff(bool rst_time) ; // Turn On & RST Time
          bool reOrder(byte number); // Return true if succesful
          byte getOrder(); // Returns the order number into the group of the actuator
          byte getActualNumber(); // Returns the actual actuator number active into the array
          void enable (bool en) ; // Enable the actuator
          bool isEnable() ; // Returns true if the array of the actuator is enable
          static void enableGroup(bool en); // Enable the group
          static bool isEnableGroup(); // Returns true if the array of the actuator is enable
          byte getFloor(); // Returns the floor of the actuator
          byte getRegion(); // Return the region of the actuator
          bool reOrderAll(bool sequence, bool order); // True if succesful
          bool reOrder_byFloor(byte fl); // Send to the last positions the
          bool defaultOrder(byte night_floor); // Clarisa Favorite Order
          void restartH2O(); // Set Volume in zero
          void restartAllH2O(); // Set all Volumes in zero
          float getH2O(); // Return Volume of H2O
          void printH2O(); // Print on serial the acummulate water consumption
          void printAllH2O(bool printAll = HIGH); // Print on serial all the acummulates of water
          String getName(); // Returns the name of the solenoid
          // Variables of time in seconds
          byte begin( byte fl, byte reg, unsigned long t_on, unsigned long cycleTime ) ; // Return true if succesful
          bool run() ;
          static void runAll();
   } ;

  #endif
