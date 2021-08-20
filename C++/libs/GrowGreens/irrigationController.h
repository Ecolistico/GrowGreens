/*
Library for Grow software package
Written by : José Manuel Casillas Martín
email: jmcasimar@sippys.com.mx
Date : 2021-08-10
Version : 0.2 (testing version)

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

// irrigationController.h

#ifndef irrigationController_h
#define irrigationController_h

#include <stdlib.h>
#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#include <wiring.h>
#endif

#include <commonStructures.h> // Shared structures bewtween differents classes

// Class to control the air compressor
class irrigationController
 {  private:
        // States
        bool _Pump; // Water Pump State
        bool _VAir; // Valve - air tank
        bool _VConnected; // Valve - Connect tanks
        bool _VFree; // Valve - Free pressure in water tank
        // Logic of valves
        bool _VAir_InvertedLogic; // Logic for __Vnut
        bool _VConnected_InvertedLogic; // Logic for __Vtank
        bool _VFree_InvertedLogic; // Logic for __Vh20
        // Aux variables
        bool _KeepConnected; // Aux variable to connect air and water tanks
        uint8_t _Mode;
        bool _Enable;

        // Compressor functions
        void turnOffAll(); // Set all LOW, using logic
        void pressurize(); // Settings to pressurize
        void despressurize(); // Settings to despressurize
        void setMode(uint8_t mode); // Set the actual mode of operation

        // Printing functions
        void printAct(String act, uint8_t level=0);

    public:
         // Constructor
         irrigationController(logicConfig lconfig);

         // States
         bool getPump(); // Return State of Water Pump
         bool getVAir(); // Return Valve Air State
         bool getVconnected(); // Return Valve Connecting tanks State
         bool getVfree(); // Return Free Pressure Valve State

         // Compressor
         void Off(bool connected); // Mode 0/1
         void pressurizeAll(); // Mode 2
         void pressurizeAir(); // Mode 3
         void pressurize_depressurize(); // Mode 4
         void depressurizeWater(); // Mode 5
         void depressurizeAll(); // Mode 6

         void keepConnected(bool con); // Do we have to keep connected both tanks?

          // Recirculation
          void turnOnPump();
          void turnOffPump();

          // Control
          void enable(bool en);
  };

  #endif
