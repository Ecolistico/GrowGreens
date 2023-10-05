/*
Library for Grow software package
Written by : José Manuel Casillas Martín
email: jmcasimar@sippys.com.mx
Date : 2021-01-22
Version : 2.1 (Cancun Optimized Version)

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

// dynamicMemory.h

// Class to create an object to manage memory dynamically to allow different system parameters using the same memory structure

#ifndef dynamicMemory_h
#define dynamicMemory_h

#include <stdlib.h>
#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#include <wiring.h>
#endif

#include <commonStructures.h> // Shared structures bewtween differents classes
#include <Wire.h>
#include <External_EEPROM.h> // Click here to get the library: http://librarymanager/All#SparkFun_External_EEPROM
/* Notes:
 1- You need to define the buffer lengths in SparkFun_External_EEPROM.h for
generic arduino mega 2560 board. For further information and details contact Eleazar Dominguez(eleazardg@sippys.com.mx). */

class dynamicMem
  { public:
      ExternalEEPROM *myMem; // Pointer to I2C memory

      dynamicMem(); // Constructor

      /* General Functions */
      bool begin(basicConfig &bconfig, pressureConfig &pconfig, sensorConfig &sconfig, logicConfig &lconfig);
      void clean(bool truncate = true);
      void print(bool truncate = true);
      void config_basic(basicConfig config);
      basicConfig getConfig_basic();
      void config_pressure(pressureConfig config);
      pressureConfig getConfig_pressure();
      void config_sensors(sensorConfig config);
      sensorConfig getConfig_sensors();
      void config_logic(logicConfig config);
      logicConfig getConfig_logic();

      /* Checking functions */
      bool check_float(float fl);
      bool check_basicConfig();
      bool check_pressureConfig();
      bool check_sensorConfig();
      bool check_logicConfig();
      bool check_dynamicMem(); // Check for solenoids and fans  missing

      /* Write-Read Functions */
      void write(int pos, byte val);
      uint8_t read_byte(int pos);
      void write(int pos, int val);
      int read_int(int pos);
      void write(int pos, float val);
      float read_float(int pos);
      void write_long(int pos, long val);
      long read_long(int pos);
      void write(int pos, unsigned long val);
      unsigned long read_ulong(int pos);

      /* Dynamic Region Saving Functions*/
      void save_solenoidParameters(uint8_t floor, uint8_t region, uint8_t number, solenoid_memory solenoidM);
      void save_fanParameters(uint8_t floor, fan_memory fanM);
      void save_analog(uint8_t num, analogSensor analog);
      void save_flowmeter(uint8_t num, flowmeter fm);
      void save_scale(uint8_t num, scale sc);
      void save_switch(uint8_t num, switchSensor sw);
      void save_ultrasonic(uint8_t num, ultrasonicSensor ultra);
      void save_mux(uint8_t num, Mux muxConfig);
      void save_stateOrder(uint8_t mx, uint8_t num, uint8_t order);

      /* Dynamic Region Returning Functions */
      solenoid_memory read_solenoidParameters(uint8_t floor, uint8_t region, uint8_t number);
      fan_memory read_fan(uint8_t floor);
      analogSensor read_analog(uint8_t num);
      flowmeter read_flowmeter(uint8_t num);
      scale read_scale(uint8_t num);
      switchSensor read_switch(uint8_t num);
      ultrasonicSensor read_ultrasonic(uint8_t num);
      Mux read_mux(uint8_t num);
      uint8_t read_stateOrder(uint8_t mx, uint8_t num);

      /* Dynamic Region checking functions */
      int MaxIrrigationPos();
      int MaxFanPos();
      int MaxAnalogPos();
      int MaxFlowmeterPos();
      int MaxScalePos();
      int MaxSwitchPos();
      int MaxUltrasonicPos();
      int MaxMuxPos();
      int OffStateOrderPos(uint8_t mx);
      int MaxStateOrderPos();
  };

#endif
