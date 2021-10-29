/*
Library from Grow software package
Written by : José Manuel Casillas Martín
email: jmcasimar@sippys.com.mx
Date : 2021-08-12
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

// sensor.h

#ifndef sensor_h
#define sensor_h

#include <stdlib.h>
#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#include <wiring.h>
#endif

#include <NewPing.h>
#include <HX711.h>
#include <dynamicMemory.h>
#include <commonStructures.h> // Shared structures bewtween differents classes

// Class for Analog Sensors
class analogSens
  { private:
        uint8_t _Pin, _Degree, _number;
        bool _FirstRead;
        float _A, _B, _C, _Value, _PreValue;

        // Constants for the filters
        uint8_t _Filter;
        float _Alpha, _Noise, _Err;
        // Exponential Filter: _Alpha = smoothing variable
        // Kalman Filter: _Noise = Enviromental Noise, _Err = Error

        float model(float val);
        float filter(float val);
        float exponential_filter(float alpha, float t, float t_1);
        float kalman_filter(float t, float t_1);
        void printModel();
        void printFilter();

    public:
        analogSens(uint8_t pin, uint8_t num); // Constructor

        void setModel(float a = 0, float b = 0, float c = 0);
        void notFilter();
        void defaultFilter(); // Set Kalman with Noise = 0.5
        void setExponentialFilter(float alpha = 0.5);
        void setKalmanFilter(float noise);
        void setFilter(uint8_t filter, float param);
        float getValue();
        float getPreValue();
        void begin(); // Start sensor
        void read();
        void printRead(); // Print in serial last read
  };

// Class for flowmeters
class Flowmeter
  { private:
      uint8_t _pin, _number;
      float _K, _water;

    public:
      Flowmeter(uint8_t pin, uint8_t num, float k); // Constructor

      static volatile long _numPulses1, _numPulses2;
      static void countPulses1(); // Interrupt Routine 1
      static void countPulses2(); // Interrupt Routine 2

      void begin();
      void restartWater();
      float getWater();
      void read();
      void printRead(); // Print in serial last read
  };

// Class for scale
class ScaleSens
  { private:
      bool _en;
      unsigned long _printTimer;
      uint8_t _pin1, _pin2, _number;
      float _weight, _minWeight, _maxWeight;

    public:
      HX711 *_sc;

      ScaleSens(uint8_t pin1, uint8_t pin2, uint8_t num);
      void begin(long offset, float scale, float min_weight, float max_weight);
      void read();
      float getWeight();
      float getMinWeight();
      float getMaxWeight();
      void setMinWeight(float weight);
      void setMaxWeight(float weight);
      void print(float value);
      void print(long value);
      void printRead(); // Print in serial last read
      void printRead_notScale(); // Print in serial read with scale = 1
      void printRead_notOffset(); // Print in serial read with scale = 1 and offset = 0
  };

// Class for Switch
class SwitchSens
  { private:
      uint8_t _pin, _number, _counter;
      bool _logicInverted, _state, _readState;

    public:
      SwitchSens(uint8_t pin, uint8_t num, bool logic = true); // Constructor
      void begin();
      void setLogic(bool logic);
      bool getState();
      void read();
      void printRead(); // Print in serial last read
  };

// Class for UltraSonic Sensors
class UltraSonic
  { private:
        uint8_t _Pin1, _Pin2, _State, _number;
        bool _FirstRead;
        NewPing *_Sonar;
        int _minDist, _maxDist, _countState;
        float _Distance, _PreDistance;

        // Constant for the model
        uint8_t _Model;
        float _Param, _Height;

        // Constants for the filters
        uint8_t _Filter;
        float _Alpha, _Noise, _Err;
        // Exponential Filter: _Alpha = smoothing variable
        // Kalman Filter: _Noise = Enviromental Noise, _Err = Error

        float model(float val);
        float cilinder(float val);
        float rectangularPrism(float val);
        float filter(float val);
        float exponential_filter(float alpha, float t, float t_1);
        float kalman_filter(float t, float t_1);
        void printModel();
        void printFilter();
        float read(); // Take a measurement

    public:
        // Constructor
        UltraSonic( uint8_t pin1, uint8_t pin2, uint8_t num, int minDist = MIN_SECURITY_DISTANCE, int maxDist = MAX_SECURITY_DISTANCE);

        void setModel(uint8_t model, float param, float height);
        void notFilter();
        void defaultFilter(); // Set Kalman with Noise = 0.5
        void setExponentialFilter(float alpha = 0.2);
        void setKalmanFilter(float noise);
        int getMinDist();
        int getMaxDist();
        float getDistance(); // Returns more actual distance measurement
        float getPreDistance(); // Returns previous distance measurement
        float getVolume(); // Returns volumen measure
        float getMinVolume(); // Returns min volumen with the model saved
        float getMaxVolume(); // Returns max volumen with the model saved
        uint8_t getState(); // Returns state (High/Ok/Low)
        bool changeMinDist(int minDist);
        bool changeMaxDist(int maxDist);
        uint8_t updateState();
  };

// Class to manage all the sensor at once
class sensorController
   {  private:
        unsigned long _sensorTimer;
        sensorConfig _sconfig;

      public:
        // Sensors
        analogSens * _myAnalogs[MAX_ANALOG_SENSOR];      // Analog sensors pointer
        Flowmeter * _myFlowmeters[MAX_FLOWMETER_SENSOR]; // Flowmeter sensors pointer
        ScaleSens * _myScales[MAX_SCALES_SENSOR];        // Scale sensors pointer
        SwitchSens * _mySwitches[MAX_SWITCHES_SENSOR];   // Switch sensors pointer
        UltraSonic * _myUltrasonics[MAX_ULTRASONIC];     // Ultrasonic sensors pointer

        sensorController(sensorConfig sconfig, dynamicMem & myMem); // Constructor
        void read();
   };

#endif
