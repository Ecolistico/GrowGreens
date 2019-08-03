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

// compressorController.h

#ifndef compressorController_h
#define compressorController_h

#include <stdlib.h>
#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#include <wiring.h>
#endif


// Class to control the air compressor
class compressorController
 {  private:
        bool __State;
        bool __Vnut; // Valve - nutrition kegs
        bool __Vtank; // Valve - air tank
        bool __Vh20; // Valve - H2O kegs
        bool __Fnut; // Valve - free pressure nutrition kegs
        bool __Fh2o; // Valve - free pressure H2O kegs
        bool __nutInvertedLogic; // Logic for __Vnut
        bool __tankInvertedLogic; // Logic for __Vtank
        bool __h20InvertedLogic; // Logic for __Vh20
        bool __KeepConnected; // Aux variable

        uint8_t __Mode;

        void turnOn(); // Turn on the compressor
        void turnOff(); // Turn off the compressor

        void doNothing(); // Set all LOW, logic does not matter
        void turnOffAll(); // Set all LOW, using logic
        void openTank(); // Setting pressure of the tank = compressor
        void openNut(); // Setting pressure of the nutrition kegs = compressor
        void openH2O(); // Setting pressure of the H2O kegs = compressor
        void openAll(); // Setting all pressures equal
        void fillTank(); // Settings to fill the tank
        void fillNut(); // Settings to fill the nutrition kegs
        void fillH2O(); // Settings to fill the H2O kegs
        void fillAll(); // Settings to fill everything

        void printAct(String act);

    public:
         compressorController( // Constructor
           bool nutLogic,
           bool tankLogic,
           bool h2oLogic
         );

         bool getState(); // Returns Compressor State
         bool getValveTank(); // Return Valve Compressor State
         bool getValveNut(); // Return Valve Nutrition State
         bool getValveH2O(); // Return Valve Water State
         bool getFreeValveNut(); // Returns Free Valve Nutrition state
         bool getFValveH2O(); // Returns Free Valve H2O state

         void openFreeNut(); // Free pressure of nutrition kegs
         void closeFreeNut(); // Close free pressure valve nutrition kegs
         void openFreeH2O(); // Free pressure of H2O kegs
         void closeFreeH2O(); // Close free pressure valve H2O kegs

         void keepConnected(bool con); // Do we have to keep connected nut with tank?

         uint8_t getMode(); // Return the actual mode of operation
         bool setMode(uint8_t mode); // Set the actual mode of operation
  };

  #endif
