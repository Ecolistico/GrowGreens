/*
Library from Grow software package
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
#define MAX_EQUATION_DEGREE   2   // Max degree of equation for calibration
#define MAX_ULTRASONIC        12  // Max number of sensors
#define MAX_DISTANCE          300 // Maximum distance(cm) we want to ping
#define MIN_SECUTIRY_DISTANCE 10  // Minimum distance(cm) expected
#define MAX_SECUTIRY_DISTANCE 100 // Maximum distance(cm) expected

// Class to declarate Analog Sensor and applied different filters
class analogSensor
  { private:
        byte __Pin , __Degree ;
        float __A , __B , __C , __Value , __PreValue ;
        String __Name;

        // Constants for the filters
        byte __Filter ;
        float __Alpha , __Noise , __Err;
        // Exponential Filter: __Alpha = smoothing variable
        // Kalman Filter: __Noise = Enviromental Noise, __Err = Error

        float model(float val);
        float filter(float val);
        float exponential_filter(float alpha, float t, float t_1);
        float kalman_filter(float t, float t_1);

    public:
        analogSensor ( byte pin, String name ) ; // Constructor
        void begin() ; // Start sensor
        bool setModel(byte degree, float a = 0, float b = 0, float c = 0) ;
        void notFilter() ;
        void defaultFilter() ; // Set Kalman with Noise = 0.5
        bool setExponentialFilter(float alpha = 0.5) ;
        bool setKalmanFilter(float noise) ;
        float getValue() ;
        float getPreValue() ;
        void printModel() ;
        void printFilter() ;
        float read() ;
  } ;

// Class to work with multiples UltraSonic Sensors
class UltraSonic
  { private:
        byte __Pin, __State ;
        String __Name;
        NewPing *__Sonar;
        int __minDist, __maxDist, __countState;
        float __Value , __PreValue ;
        unsigned long __ActualTime; // Time Counter

        // Constants for the filters
        byte __Filter ;
        float __Alpha , __Noise , __Err;
        // Exponential Filter: __Alpha = smoothing variable
        // Kalman Filter: __Noise = Enviromental Noise, __Err = Error

        float filter(float val);
        float exponential_filter(float alpha, float t, float t_1);
        float kalman_filter(float t, float t_1);

    public:
        static byte __TotalActuators;
        static UltraSonic *ptr[MAX_ULTRASONIC] ; // List of pointers to each object

        UltraSonic ( byte pin, String name, int minDist = MIN_SECUTIRY_DISTANCE, int maxDist = MAX_SECUTIRY_DISTANCE ) ; // Constructor
        void begin() ; // Start sensor
        void beginAll() ; // Starts all sensors at once
        void notFilter() ;
        void defaultFilter() ; // Set Kalman with Noise = 0.5
        bool setExponentialFilter(float alpha = 0.2) ;
        bool setKalmanFilter(float noise) ;
        float getValue() ;
        float getPreValue() ;
        byte getState() ;
        bool changeMinDist(int minDist) ;
        bool changeMaxDist(int maxDist) ;
        void printFilter() ;
        void readAndPrint() ;
        float read() ;
        byte run() ;
        void runAll() ;

  } ;

  #endif
