/*
Library for Grow software package
Written by : José Manuel Casillas Martín
email: jmcasimar@sippys.com.mx
Date : 2021-09-16
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

// mux.h

#ifndef mux_h
#define mux_h

#include <stdlib.h>
#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#include <wiring.h>
#endif

#include <dynamicMemory.h>
#include <commonStructures.h> // Shared structures bewtween differents classes

// Class to manage multiplexers
class MUX
 {  private:
      static uint8_t _total;                     // Total MUX objects created
      uint8_t _number;                           // Actual number of the MUX object
      unsigned long _timer, _printTimer;         // Internal timer to send output
      bool _en;                                  // To know if multiplexers should be enable or disable
      bool _stcpEn;                              // To know if Latch Pin Must Be On or Off
      bool _false;                               // Save false to assign it to a pointer
      bool _off[OUT_PER_PCB];                    // To set al states as false when need it
      MuxOut _muxOut[MAX_MODULES];               // Output States per single PCB
      MuxOut _demuxOut[MAX_MODULES];             // Input States per single PCB
      MuxState _mux[MAX_MODULES*OUT_PER_PCB];    // List of all outputs
      MuxState _demux[MAX_MODULES*OUT_PER_PCB];  // List of all inputs

      void muxPrint(String act, uint8_t level=0);
      void muxPrint(String act1, String act2, String act3, String act4, uint8_t level=0);

    public:
      uint8_t _numStates;      // Counter for states added
      Mux _config;             // Config Variables

      MUX(Mux config); // Constructor for the class

      void begin(); // Initialize pins and states
      void addState(bool & state, uint8_t order); // Add a new output value
      void printState(uint8_t quotient, uint8_t reminder); // Debug function
      void orderMux();   // Order Output States in array of 8
      void codificationMultiplexer(); // Write State to PCB´s
      void enable(bool en);      // Set all states off
      void update();  // Rewrite states
  };

// Class to manage all the multiplexers at once
class muxController
  { private:
       uint8_t _muxNumber;   // Number of Mux systems

     public:
       MUX * _myMux[MAX_MUX_SYSTEMS];                        // Multiplexer pointer

       muxController(uint8_t muxNumber, dynamicMem & myMem); // Constructor
       void update();                    // Update states for all multiplexors systems

  };
  #endif
