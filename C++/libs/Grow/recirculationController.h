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

// recirculationController.h

#ifndef recirculationController_h
#define recirculationController_h

#include <stdlib.h>
#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#include <wiring.h>
#endif

#include <sensor.h>

#define PIN_LEVEL_0 25
#define PIN_LEVEL_1 26
#define PIN_LEVEL_2 27
#define PIN_LEVEL_3 28
#define PIN_LEVEL_4 29
#define PIN_LEVEL_5 30
#define NAME_LEVEL_0 "Recirculation Level"
#define NAME_LEVEL_1 "Solution A Level"
#define NAME_LEVEL_2 "Solution B Level"
#define NAME_LEVEL_3 "Solution C Level"
#define NAME_LEVEL_4 "Solution D Level"
#define NAME_LEVEL_5 "Solution E Level"
#define MAX_NUMBER_US_SENSOR 6
#define PIN_LEVEL_SWITCH 31
#define RADIO_TANK 2.65 // Radio for solution Tank in dm (decimeters)
#define WIDTH_TANK 4 // Width for recirculation Tank in dm (decimeters)
#define LENGTH_TANK 4 // Length for recirculation Tank in dm (decimeters)

// Class to control the recirculation of nutrient and water
/*
*/
class recirculationController
 {  private:
        byte __In, __Out; // Solution coming in and coming out
        bool __InPump, __OutPump; // Water Pumps
        bool __In1, __In2, __In3, __In4, __In5; // Solenoids Valves
        bool __Out1, __Out2, __Out3, __Out4, __Out5; // Solenoids Valves
        bool __In_LogInv; // Solenoids Valves In ¿Logic Inverted)
        bool __Out_LogInv; // Solenoids Valves Out ¿Logic Inverted)
        byte __Switch_Pin; // Level switch Pin
        bool __Switch_State; // Level Switch State
        int __Switch_Counter; // Counter for Switch Level
        UltraSonic *__Level[MAX_NUMBER_US_SENSOR]; // UltraSonic Sensors
        unsigned long __ActualTime;

        void turnOn(bool &state, bool inverted_logic = LOW);
        void turnOff(bool &state, bool inverted_logic = LOW);

    public:
         recirculationController(); // Constructor (Not use logic inverted)
         recirculationController(bool InInverted, bool OutInverted); // Constructor

         bool getState_In1() ; // Returns Solenoid In 1 State
         bool getState_In2() ; // Returns Solenoid In 2 State
         bool getState_In3() ; // Returns Solenoid In 3 State
         bool getState_In4() ; // Returns Solenoid In 4 State
         bool getState_In5() ; // Returns Solenoid In 5 State
         bool getState_Out1() ; // Returns Solenoid Out 1 State
         bool getState_Out2() ; // Returns Solenoid Out 2 State
         bool getState_Out3() ; // Returns Solenoid Out 3 State
         bool getState_Out4() ; // Returns Solenoid Out 4 State
         bool getState_Out5() ; // Returns Solenoid Out 5 State
         bool getState_InPump() ; // Returns In Pump State
         bool getState_OutPump() ; // Returns Out Pump State

         float getLiters(); // Returns the liters in recirculation Tank
         float getLiters1(); // Returns the liters in Solution Tank 1
         float getLiters2(); // Returns the liters in Solution Tank 2
         float getLiters3(); // Returns the liters in Solution Tank 3
         float getLiters4(); // Returns the liters in Solution Tank 4
         float getLiters5(); // Returns the liters in Solution Tank 5

         void setIn(byte nutrient); // Allow to save what nutrient is coming in
         void setOut(byte nutrient); // Allow to save what nutrient is coming out

         void turnOn_OutPump(byte valve); // Always turn On with some Output Valve
         void turnOn_InPump(byte valve); // Always turn On with some Input Valve

         void read_LevelSwitch(); // Read what level switch is sensing
          // Start level switch and assign UltraSonic ptrs
         void begin(byte pin, UltraSonic &level0, UltraSonic &level1, UltraSonic &level2,
           UltraSonic &level3, UltraSonic &level4, UltraSonic &level5);
         void run(); // Get measurements and execute action
  } ;


  #endif
