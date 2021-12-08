/*
Library for Atlas Scientific EZO Sensors
Written by : José Manuel Casillas Martín
email: jmcasimar@hotmail.com.mx
Date : 2019-*-*
Version : 0.1 (testing version)

This file is part of jmcasimar_AS_EZO library Software Package.

jmcasimar_EZO library is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

jmcasimar_EZO library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with jmcasimar_EZO library.  If not, see <https://www.gnu.org/licenses/>.
*/

// EZO.h
#ifndef EZO_h
#define EZO_h

#include <stdlib.h>
#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#include <wiring.h>
#endif

#include <Wire.h> // Necessary for I2C communication

#define EZO_PH 0 // Type of sensor pHmeter
#define EZO_EC 1 // Type of sensor conductimeter
#define PH_I2C_ADDRESS 99 // default I2C ID number for EZO pH Circuit
#define EC_I2C_ADDRESS 100 // default I2C ID number for EZO EC Circuit
#define LARGEST_STRING 48 // Largest string response array
#define MAX_PARAM 6 // Max String Param that we can get

// Class to control Atlas Scientific EZO Sensors
/* To fix:
 *  - changeI2Caddress is not working as expected
 * To add:
 *  - add changeUART, is consider unnecesary because you have to modify the
 *    the wire in order to make it work
 * Limitant:
 *  - Cannot export calibration from two sensors at the same time
 *  - You have to wait some seconds after import some parameters to a sensor or it will fail
 *  - After reboot have to wait some time before ask for another action
*/
class EZO
  {
    private:
        uint8_t __Type; // Variable for the type of sensor
        uint8_t __I2CAddress; // The I2C address for each sensor
        float __Value; // The value readed
        char __SendData[LARGEST_STRING]; // Variable to send data to the sensor
        char __RecieveData[LARGEST_STRING]; // Variable to read the incoming data from the sensor
        String __ParamResponse[MAX_PARAM]; // Variable to hold the split response of the sensor
        unsigned long __Time; // Variable to wait for the response
        unsigned long __ActualTime; // Time counter
        bool __Request; // Variable to know when we ask something to the sensor
        bool __Read; // Variable to know when we are waiting for a read lecture

        /* Export calibration Routine Variables */
        byte __CalibrationCount;
        byte __CalibrationLines;
        byte __CalibrationBytes;

        void printAction(String act);
        void sendCmd(String command);
        void requestResponse();
        void fillRecieveDataArray();
        void getParamResponse();
        void decodeResults();
        void exportCalibration(bool act); // Exports calibration

    public:
        EZO(uint8_t type); // Constructor

        /* Both sensors */
        void init(); // Set up the sensor
        bool isEnable(); // Returns false when we are awaiting for some response, else true
        float getValue(); // Returns the last value readed
        //void UART(unsigned long baud); Right now is not necessary // Change EZO sensor to UART Mode
        void calibration(byte act, float value); // Performs Calibration
        void exportCal(); // Run Export Calibration Sequence
        void importCalibration(String parameters); // Imports Calibration
        void reboot(); // Reboot device
        void find(); // Find device with blinking white led
        void information(); // Returns device information
        /* Right now not working changeI2Caddress */
        void changeI2Caddress(int newAddress);  // Change the address for the EZO Sensors
        void led(byte act); // Enable/Disable/Status LED
        void plock(byte act); // Enable/Disable protocol lock
        void read(); // returns a single reading
        void readWithTempCompensation(float temp); // returns a single reading compensating temperature
        void sleep(); // enter sensor into sleep mode/low power consumption
        void infoStatus(); // Returns information Status
        void tempCompensation(bool act, float temp = 0); // temperature compensation

        /* Just phMeter */
        void slope(); // Returns slope for the equation

        /* Just conductimeter */
        void setProbe(bool act, float probeType); // Set probe type
        void enableParameters(bool en, byte parameter); // Enable/Disable parameters

        void run(); // Runs in loop and wait for a response to get it
  };

#endif
