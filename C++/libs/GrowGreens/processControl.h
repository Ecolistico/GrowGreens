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

// processControl.h

#ifndef processControl_h
#define processControl_h

#include <stdlib.h>
#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#include <wiring.h>
#endif

class processControl
  {
    private:
      void printAction(String act);

    public:
      uint8_t _state; // State of the process
      uint8_t _lastState; // Previous state
      unsigned long _actualTime; // Time Control for the process
      // Auxiliar variable to transfer parameters to different states in the same process
      float _parameter;
      uint8_t _type;

      processControl(uint8_t type = 250); // Constructor
      void setState(uint8_t st, float par = 0); // Function to update the process state
  };

#endif
