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

#define MAX_SOLUTIONS_NUMBER 4 // The max number of solutions

// Class to control irrigation routine
/*
Definitions:
  Solution 1 = KNO3
  Solution 2 = NH4H2PO04
  Solution 3 = CA(NO3)2
  Solution 4 = MGSO4 + Micros
*/

class irrigationController
  { private:
      uint8_t __solution;
      uint8_t __actualCycle;
      uint8_t __cyclesPerDay;
      uint8_t __order[MAX_SOLUTIONS_NUMBER];
      uint8_t __percentage[MAX_SOLUTIONS_NUMBER];
      uint16_t __ec[MAX_SOLUTIONS_NUMBER];
      float __ph[MAX_SOLUTIONS_NUMBER];
      uint8_t __hoursPerCycle;
      uint8_t __beginHour[24]; // Max 24 cycles per day

    public:
      // Constructor by default 1 irrigationCycle, order = 1,2,3,4 and 25% each solution
      irrigationController();
      // Constructor with complete flexibility
      irrigationController(uint8_t cyclesPerDay,
                        uint8_t initialHour,
                        uint8_t ord1,
                        uint8_t ord2,
                        uint8_t ord3,
                        uint8_t ord4,
                        uint8_t per1,
                        uint8_t per2,
                        uint8_t per3,
                        uint8_t per4 );
      uint8_t getSolution(); // Returns __solution
      uint8_t getCycle(); // Returns __actualCycle
      // Returns true if succesful
      void redefine(uint8_t cyclesPerDay,
                    uint8_t initialHour,
                    uint8_t ord1,
                    uint8_t ord2,
                    uint8_t ord3,
                    uint8_t ord4,
                    uint8_t per1,
                    uint8_t per2,
                    uint8_t per3,
                    uint8_t per4 );
      // Set ph and ec for each solution
      void setEC(uint8_t sol, uint16_t ec);
      void setEC(uint16_t ec1, uint16_t ec2, uint16_t ec3, uint16_t ec4);
      void setPH(uint8_t sol, float ph);
      void setPH(float ph1, float ph2, float ph3, float ph4);
      // Get ph and ec for each solution
      uint16_t getEC(uint8_t sol);
      float getPH(uint8_t sol);
      // Update and return the solution with the given hour
      uint8_t whatSolution(uint8_t HOUR, uint8_t MINUTE);
      // Return how many minutes are left to the next solution change
      int min2Change(uint8_t HOUR, uint8_t MINUTE);
  };

  #endif
