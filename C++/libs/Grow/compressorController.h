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
        bool __State, __Enable;
        bool __Valve_Compressor, __Valve_Nutrition, __Valve_Water;
        bool __Compressor_invertedLogic;
        bool __Nutrition_invertedLogic;
        bool __Water_invertedLogic;

        void turnOn(); // Turn on the compressor
        void turnOff(); // Turn off the compressor
        void doNothing(); // Turn off everything
        void openCompressor(); // Settings to equal pressures in compressor kegs
        void openNutrition(); // Settings to equal pressures in nutrition kegs
        void openWater(); // Settings to equal pressures in water kegs
        void openEverything(); // Settings to equal all pressures
        void fillCompressor(); // Settings to fill the compressor kegs
        void fillNutrition(); // Settings to fill the nutrition kegs
        void fillWater(); // Settings to fill the water kegs
        void fillEverything(); // Settings to fill everything

        void turnOn_Valve(bool &state, bool inverted_logic);
        void turnOff_Valve(bool &state, bool inverted_logic);

    public:
         compressorController(bool comprLogic, bool nutrLogic, bool waterLogic); // Constructor

         bool getState() ; // Returns Compressor State
         bool getValveCompressorState(); // Return Valve Compressor State
         bool getValveNutritionState(); // Return Valve Nutrition State
         bool getValveWaterState(); // Return Valve Water State
         void close_ValveNutrition(); // Close the Nutrition Valve (Usefull for free pressure)
         void close_ValveWater(); // CLose the Water Valve (Usefull for free pressure)
         void enable (bool en) ; // Enable the Compressor
         bool isEnable() ; // Returns true if Compressor is enable
         byte setMode(byte mode) ;
  } ;

  #endif
