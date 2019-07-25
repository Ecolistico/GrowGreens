#include "solutionMaker.h"
#include <EEPROM.h>

/* Pendiente:
 *  - Motor licuadora (Relevador) hardware/software
 *  - Sensores phMetro, conductimetro guardar direcciones i2c en memoria eeprom además del resto de funciones
 */
solutionMaker sMaker(
          47, // Dir1
          45, // Step1
          41, // Dir2
          39, // Step2
          35, // Dir3
          33, // Step3
          29, // Dir4
          27, // Step4
          49, // En1
          43, // En2
          37, // En3
          31, // En4
          51, // Motor1A
          53, // Motor1B
          23, // Motor2A
          25, // Motor2B
          5, // Motor1En
          6, // Motor2En
          46, // LedS1
          48, // LedS2
          50, // LedS3
          52, // LedS4
          44, // LedA1
          42, // LedA2
          38, // LedReady
          40, // LedWorking
          22, // Temperature sensor
          24, // LCD button
          9 // Relay (motor or actuator to get the solution disolve)
        );

// Serial comunication
String inputstring = "";
bool input_string_complete = false;

void setup() {
  Serial.begin(115200);
  sMaker.begin();
  read_EEPROM(HIGH); // Charge calibration parameters
}

void loop() {
  sMaker.run();  
}
