/* PENDIENTE:
 *  1) Librería multiplexores: 
 *    1.1 Guardar pines EEPROM
 *    1.2 Inicializar estados (numero placas = salidas)
 *    1.3 Dictionario estados -> Posicion física dentro de la placa
 *    1.4 Funciones para modificar diccionario
 *  
 *  3) Parámetros de control báscula
 *    3.1 Guardar EEPROM
 *    3.2 Añadir estructura a dynamicMem
 *    
 *  6) Condiciones BOOT
 *    6.1 Parametros necesarios para arrancar
 *    6.2 Funciones de conversión de parametros de comunicación a establecer estado inicial del sistema
 *  
 *  7) Flujo de programa
 *  
 *  8) TROUBLESHOOTING
 *    8.1) dynamicMem Congruencia de punteros e índices
 *    8.2) Comunicación Serial
 */

/*** Include Libraries ***/
#include <commonStructures.h>
#include <dynamicMemory.h>
#include <fan.h>
#include <irrigationController.h>
#include <sensor.h>
#include <solenoid.h>

/*** Special Functions ***/
//void(* resetFunc) (void) = 0; //declare reset function @ address 0

/*** Objects ***/
dynamicMem myMem;                     // Dynamic Memory
systemFan * myFans;                   // Fan Control pointer
irrigationController * myIrrigation;  // Compressor and Recirculation Control pointer
systemValves * myValves;              // Solenoid control pointer
sensorController * mySensors;         // Sensor Control pointer

/* PENDIENTE: PROCESS CONTROL */
// processControl PC1;
// processControl PC2;

// Structures
basicConfig bconfig;    // Basic config
pressureConfig pconfig; // Pressure config
sensorConfig sconfig;   // Sensor config
logicConfig lconfig;    // Logic config
dateTime dTime;         // Time info

// auxVariables
bool memoryReady;

// Serial comunication
String inputstring = "";
bool input_string_complete = false;

/* REDEFINE */
// Control action that has to be executed at least once when booting/rebooting
bool firstHourUpdate = false;
bool bootParameters = false;
unsigned long bootTimer;
float h2oConsumption;
bool rebootFlag = false;
bool rebootPrint = false;
unsigned long rebootTimer;

void setup() {
  // Initialize serial
  Serial.begin(115200);
  Serial.println(F("info,Setting up Device..."));

  // Initialize timers
  rebootTimer = millis();
  
  // Initialize dynamic memory
  memoryReady = myMem.begin();
  
  // Charge eeprom parameters for each fan and load it
  // Turn on each fan with delay (to avoid EMI)
  if(memoryReady){
    bconfig = myMem.getConfig_basic();    // Get basic config
    pconfig = myMem.getConfig_pressure(); // Get pressure config
    sconfig = myMem.getConfig_sensors();  // Get sensor config
    lconfig = myMem.getConfig_logic();    // Get logic config
    
    // Initialize fan control
    myFans = new systemFan(bconfig.floors, myMem);
    
    // Initialize irrigation control
    myIrrigation = new irrigationController(lconfig);

    // Initialize sensorController
    mySensors = new sensorController(sconfig, myMem);
    
    // Initialize solenoid control
    myValves = new systemValves(bconfig, myMem);
    
    Serial.println(F("info,Device is ready"));
  }
  else Serial.println(F("critical,Memory: Please provide the memory configuration missed to be able to start"));
  
}

void loop() {
  if(memoryReady){
    myFans->run();
    myValves ->run(); 
    mySensors->read();
  }

  if(!rebootPrint && rebootFlag){
    rebootPrint = true;
    Serial.println(F("warning,System will reboot in 5 minutes to recharge modified parameters"));
  }
  
  if(rebootFlag && millis()-rebootTimer>REBOOT_TIME){
    rebootTimer = millis();
    Serial.println(F("REBOOTING SYSTEM"));
  }
}
