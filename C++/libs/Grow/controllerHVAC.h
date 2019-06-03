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

// controllerHVAC.h

#ifndef controllerHVAC_h
#define controllerHVAC_h

#include <stdlib.h>
#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#include <wiring.h>
#endif

#define WAIT_TIME 1.5*60*1000 // Time in ms of warm/wait for the Compressor Contactor
#define AUTO_FAN LOW // Auto fan as low
#define ON_FAN HIGH // On fan as high
#define OFF_MODE 0 // Off mode as 0
#define COOL_MODE 1 // Cool mode as 1
#define HEAT_MODE 2 // Heat mode as 2

// Class to reply an HVAC controller.
/* Limitation Note: This class is defined to reply Honeywell controller
behaivor. Model: TH1110DV1009
Cables Wiring:
  * RC - Power 24VAC
       |
  * R  - Power 24VAC
  (For sistems with 2 signals/tranfosrmers in this case is connect to RC with a jumper)
  * C  - Common 24VAC
  * W  - Heat Relay
  * Y  - Compressor Contactor
  * G  - Fan Relay
  * O  - Changeover valve energized in cooling
  * B  - Changeover valve energized in heating
*/

class controllerHVAC
 {  private:
        bool __W , __Y, __G, __O, __B ;
        bool __Warn, __Work, __Fan;
        byte __Mode, __PrevMode;
        unsigned long __Actual_time ;

        void resetTime() ;

    public:
         controllerHVAC ( ) ; // Constructor
         controllerHVAC ( byte Mode ) ; // Constructor
         controllerHVAC ( byte Mode, bool fan ) ; // Constructor

         bool getHR_State() ; // Returns Heat Relay State
         bool getCC_State() ; // Returns Compressor Contactor State
         bool getFR_State() ; // Returns Fan Relay State
         bool getCVC_State() ; // Returns Changeover Valve Cooling State
         bool getCVH_State() ; // Returns Changeover Valve Heating State
         bool getWarn_State() ; // Returns Warn State. On when is preparing
         bool getWork_State() ; // Returns Work State. On when is working
         bool changeMode(byte Mode); // Change the operation mode. Returns true if succesful
         void changeFan(bool fan); // Change fan mode

         void run() ;
  } ;

  #endif
