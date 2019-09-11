/*** Tested on Arduino Uno ***/

// Include libraries
#include <SPI.h>
#include <MFRC522.h>
#include <EEPROM.h>

// Define Pins
#define RST_PIN  9    // Pin 9 reset RC522
#define SS_PIN  10   // Pin 10 SS (SPI) RC522
#define red_led 8 // Pin 8 LED red RC522
#define access_led A4 // Pin A4 LED green/Access
#define error_led A5 // Pin A5 LED red/Error
#define button A3 // Pin A3 Touch Button
#define access A2 // Pin A2 open Access (solenoid)

// Define objects
MFRC522 mfrc522(SS_PIN, RST_PIN); //Creamos el objeto para el RC522

/*** Aux variables ***/
// Access state & red_led
uint8_t access_state = 0;
uint8_t count_state1 = 0;
uint8_t count_state2 = 0;
unsigned long count_time1 = millis();
unsigned long count_time2 = millis();
bool red_led_state = false;
unsigned long led_time = millis();
// Button
bool button_state = LOW;
unsigned long button_time = millis();
// ID control
uint8_t actualID[4];

void blink_red_led(){
  // State 2 [Save new ID] - LED Blinking
  if(access_state == 1){
    if(millis()-led_time>500){
      led_time = millis();
      if(red_led_state){
        red_led_state = LOW;
        digitalWrite(red_led, red_led_state);
      }
      else{
        red_led_state = HIGH;
        digitalWrite(red_led, red_led_state);
      }
    } 
  }
  // Else - LED Off
  else if(access_state==0 && red_led_state){
    red_led_state = LOW;
    digitalWrite(red_led, red_led_state);
  }
}

void detectButton(){
  if(!digitalRead(button) && !button_state){
    Serial.println(F("Button pressed"));
    button_state = HIGH;
    button_time = millis();
    delay(100);
  }
  else if(digitalRead(button) && button_state){
    button_state = LOW;
    if(access_state!=0 && millis()-button_time>1000){
      access_state = 0;
      count_state1 = 0;
      count_state2 = 0;
      Serial.println(F("Come back to normal state"));
      digitalWrite(access_led, HIGH);
      delay(2000);
      digitalWrite(access_led, LOW);
    }
    else if(millis()-button_time>4000){
      count_state1++;
      count_state2 = 0;
      count_time1 = millis();
      if(count_state1>=3){
        digitalWrite(red_led, HIGH);
        cleanID();
        digitalWrite(red_led, LOW);
        count_state1 = 0;
        digitalWrite(access_led, HIGH);
        digitalWrite(error_led, HIGH);
        delay(2000);
        digitalWrite(access_led, LOW);
        digitalWrite(error_led, LOW);
      }
    }
    else if(millis()-button_time>1000 && millis()-button_time<2500){
      count_state2++;
      count_state1 = 0;
      count_time2 = millis();
      if(count_state2>=3){
        access_state = 1;
        Serial.println(F("Save new ID mode")); 
        digitalWrite(access_led, HIGH);
        delay(2000);
        digitalWrite(access_led, LOW);
      }
    }
  }
}

void reset_count(){
  if(count_state1!=0 && millis()-count_time1>10000){
    count_state1 = 0;
  }
  if(count_state2!=0 && millis()-count_time2>10000){
    count_state2 = 0;
  }
}

void cleanID(){
  int numID = EEPROM.read(0);
  for(int i=1; i<numID*4+1; i++){
    EEPROM.update(i, 0);
  }
  EEPROM.update(0,0);
  Serial.println("All ID´s deleted");
}

void printEEPROM(){
  Serial.println("Printing EEPROM");
  for(int i=0; i<10*4+1; i++){
    Serial.print(i); Serial.print(F(": ")); Serial.println(EEPROM.read(i));
  }
}

bool ID_exists(uint8_t id[4]){
  int numID = EEPROM.read(0);
  bool ex = false;
  for(int i=0; i<numID; i++){
    int byte1 = EEPROM.read(1+i*4);
    int byte2 = EEPROM.read(2+i*4);
    int byte3 = EEPROM.read(3+i*4);
    int byte4 = EEPROM.read(4+i*4);
    if(byte1==id[0] && byte2==id[1] && byte3==id[2] && byte4==id[3]){
      ex = true;
      break;
    }
  }
  if(ex){ 
    digitalWrite(error_led,HIGH);
    Serial.println(F("ID already has access"));
    delay(1000);
    digitalWrite(error_led,LOW);
    
  }
  return ex;
}

void saveID(uint8_t id[4]){
  int numID = EEPROM.read(0);
  EEPROM.update(1+numID*4, id[0]);
  EEPROM.update(2+numID*4, id[1]);
  EEPROM.update(3+numID*4, id[2]);
  EEPROM.update(4+numID*4, id[3]);
  EEPROM.update(0, numID+1);
  Serial.println(F("New ID Saved"));
  Serial.print(F("Now you have "));
  Serial.print(numID+1);
  Serial.println(F(" access ID´s"));
  digitalWrite(access_led, HIGH);
  delay(500);
  digitalWrite(access_led, LOW);
  delay(500);
  digitalWrite(access_led, HIGH);
  delay(500);
  digitalWrite(access_led, LOW);
}

void check_Access(uint8_t id[4]){
  int numID = EEPROM.read(0);
  bool ac = false;
  for(int i=0; i<numID; i++){
    int byte1 = EEPROM.read(1+i*4);
    int byte2 = EEPROM.read(2+i*4);
    int byte3 = EEPROM.read(3+i*4);
    int byte4 = EEPROM.read(4+i*4);
    if(byte1==id[0] && byte2==id[1] && byte3==id[2] && byte4==id[3]){
      ac = true;
      break;
    }
  }
  if(ac){
    digitalWrite(access_led, HIGH);
    digitalWrite(access,HIGH);
    Serial.println("Access Granted");
    delay(3000);
    digitalWrite(access_led, LOW);
    digitalWrite(access,LOW);
  }
  else{
    digitalWrite(error_led,HIGH);
    Serial.println("Access Denied");
    delay(3000);
    digitalWrite(error_led,LOW);
  }
}

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  
  pinMode(red_led, OUTPUT);
  pinMode(access_led, OUTPUT); 
  pinMode(error_led, OUTPUT); 
  pinMode(access, OUTPUT);
  pinMode(button,INPUT_PULLUP);

  Serial.println(F("Ecolistico Access Control.\nLocation: Valle de Bravo, Edo. de México"));

  digitalWrite(access_led, HIGH);
  digitalWrite(error_led, HIGH);
  delay(1000);
  digitalWrite(access_led, LOW);
  digitalWrite(error_led, LOW);
  
  /*** Debug ***/
  //printEEPROM();
  //digitalWrite(access, HIGH);
  
}

void loop() {
  blink_red_led();
  detectButton();
  reset_count();
  
  // Check for new RFID cards
  if(mfrc522.PICC_IsNewCardPresent()) {  
    // Select one card
    if(mfrc522.PICC_ReadCardSerial()) {
      // Send card Unique-ID
      Serial.println("Card Detected");
      for (byte i = 0; i < mfrc522.uid.size; i++) { actualID[i] = mfrc522.uid.uidByte[i]; } 
      // Normal operation
      if(access_state==0){ check_Access(actualID); }
      // Saving new´s ID
      else if(access_state==1){
        if(!ID_exists(actualID)){
          saveID(actualID);
        }
      }
      // Finish lecture
      mfrc522.PICC_HaltA();         
    }      
  } 
}
