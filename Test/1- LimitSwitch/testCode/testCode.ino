/* Check Readme for further information */

const byte LS_1X1 = 27;
const byte LS_1X2 = 29;
const byte LS_1Y = 31;
const byte LS_2X1 = 33;
const byte LS_2X2 = 35;
const byte LS_2Y = 37;
const byte LS_3X1 = 39; 
const byte LS_3X2 = 41;
const byte LS_3Y = 43;
const byte LS_4X1 = 45; 
const byte LS_4X2 = 47;
const byte LS_4Y = 49;

void setup() {
  Serial.begin(9600);
  pinMode(LS_1X1, INPUT_PULLUP);
  pinMode(LS_1X2, INPUT_PULLUP);
  pinMode(LS_1Y, INPUT_PULLUP);
  pinMode(LS_2X1, INPUT_PULLUP);
  pinMode(LS_2X2, INPUT_PULLUP);
  pinMode(LS_2Y, INPUT_PULLUP);
  pinMode(LS_3X1, INPUT_PULLUP);
  pinMode(LS_3X2, INPUT_PULLUP);
  pinMode(LS_3Y, INPUT_PULLUP);
  pinMode(LS_4X1, INPUT_PULLUP);
  pinMode(LS_4X2, INPUT_PULLUP);
  pinMode(LS_4Y, INPUT_PULLUP);
  Serial.println("Starting test");
}

void loop() {
  if(digitalRead(LS_1X1)==LOW){ Serial.println("1X1 Pressed"); }
  if(digitalRead(LS_1X2)==LOW){ Serial.println("1X2 Pressed"); }
  if(digitalRead(LS_1Y)==LOW){ Serial.println("1Y Pressed"); }
  if(digitalRead(LS_2X1)==LOW){ Serial.println("2X1 Pressed"); }
  if(digitalRead(LS_2X2)==LOW){ Serial.println("2X2 Pressed"); }
  if(digitalRead(LS_2Y)==LOW){ Serial.println("2Y Pressed"); }
  if(digitalRead(LS_3X1)==LOW){ Serial.println("3X1 Pressed"); }
  if(digitalRead(LS_3X2)==LOW){ Serial.println("3X2 Pressed"); }
  if(digitalRead(LS_3Y)==LOW){ Serial.println("3Y Pressed"); }
  if(digitalRead(LS_4X1)==LOW){ Serial.println("4X1 Pressed"); }
  if(digitalRead(LS_4X2)==LOW){ Serial.println("4X2 Pressed"); }
  if(digitalRead(LS_4Y)==LOW){ Serial.println("4Y Pressed"); }
  delay(1000);
}
