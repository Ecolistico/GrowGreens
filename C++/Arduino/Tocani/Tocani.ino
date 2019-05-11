/***** Incluir librerías *****/
#include "AccelStepper.h"

/***** Definir pines *****/
// Motores Piso 1
const byte X11motorDirPin = 22;
const byte X11motorStepPin = 23;
const byte X12motorDirPin = 24;
const byte X12motorStepPin = 25;
const byte Y1motorDirPin = 26;
const byte Y1motorStepPin = 27;
// Motores Piso 2
const byte X21motorDirPin = 28;
const byte X21motorStepPin = 29;
const byte X22motorDirPin = 30;
const byte X22motorStepPin = 31;
const byte Y2motorDirPin = 32;
const byte Y2motorStepPin = 33;
// Motores Piso 3
const byte X31motorDirPin = 34;
const byte X31motorStepPin = 35;
const byte X32motorDirPin = 36;
const byte X32motorStepPin = 37;
const byte Y3motorDirPin = 38;
const byte Y3motorStepPin = 39;
// Motores Piso 4
const byte X41motorDirPin = 40;
const byte X41motorStepPin = 41;
const byte X42motorDirPin = 42;
const byte X42motorStepPin = 43;
const byte Y4motorDirPin = 44;
const byte Y4motorStepPin = 45;
// Fines de carrera Piso 1
const byte x11_home = 46;
const byte x12_home = 47;
const byte y1_home = 48;
bool x11_home_State;
bool x12_home_State;
bool y1_home_State;
// Fines de carrera Piso 2
const byte x21_home = 49;
const byte x22_home = 50;
const byte y2_home = 51;
bool x21_home_State;
bool x22_home_State;
bool y2_home_State;
// Fines de carrera Piso 3
const byte x31_home = 4;
const byte x32_home = 5;
const byte y3_home = 6;
bool x31_home_State;
bool x32_home_State;
bool y3_home_State;
// Fines de carrera Piso 4
const byte x41_home = 7;
const byte x42_home = 8;
const byte y4_home = 9;
bool x41_home_State;
bool x42_home_State;
bool y4_home_State;

/***** Definir objetos *****/
// Configurar instancia de AccelStep
// El "1" nos indica que usaremos un driver
// Primer Piso
AccelStepper stepperX11(1, X11motorStepPin, X11motorDirPin);
AccelStepper stepperX12(1, X12motorStepPin, X12motorDirPin);
AccelStepper stepperY1(1, Y1motorStepPin, Y1motorDirPin);
// Segundo Piso
AccelStepper stepperX21(1, X21motorStepPin, X21motorDirPin);
AccelStepper stepperX22(1, X22motorStepPin, X22motorDirPin);
AccelStepper stepperY2(1, Y2motorStepPin, Y2motorDirPin);
// Tercer Piso
AccelStepper stepperX31(1, X31motorStepPin, X31motorDirPin);
AccelStepper stepperX32(1, X32motorStepPin, X32motorDirPin);
AccelStepper stepperY3(1, Y3motorStepPin, Y3motorDirPin);
// Cuarto Piso
AccelStepper stepperX41(1, X41motorStepPin, X41motorDirPin);
AccelStepper stepperX42(1, X42motorStepPin, X42motorDirPin);
AccelStepper stepperY4(1, Y4motorStepPin, Y4motorDirPin);

/***** Configuración inicial/Variables auxiliares *****/
// Configurar velocidad y aceleración máxima
int motorSpeed = 10000; // Máximos pasos/segundo (Aproximadamente 3rps/ con microsteps de 1/16)
int motorAccel = motorSpeed/4; // Pasos/segundo^2 para acelerar

// Auxiliares para inf. serial
char control;
char serialAux;
float Travel;  // Usada para guardar la información introducida por el usuario para su movimiento

// Auxiliares de movimiento en 2 ejes
float mmSteps_X = 3200.00/40;
float mmSteps_Y = 3200.00/60;
// Primer Piso
float X1; // Guardar la posición X
float Y1; // Guardar la posición Y
float max_X1position = 11268; // Posición máxima X mm
float max_Y1position = 1693; // Posición máxima Y mm
int sweep1_State; // Variable para saber si está un barrido en curso y en qué etapa del barrido vamos
bool sweep1_Side; // Variable para saber de que lado empieza el barrido
bool sweep1_aux; // Variable auxiliar para el barrido
int x1_sign; // Variable entera para definir dirección del barrido en X
int y1_sign; // Variable entera para definir dirección del barrido en Y
int x1_sweep = 15; // Divisiones para hacer el barrido en X
int y1_sweep = 3; // Divisiones para hacer el barrido en Y
// Segundo Piso
float X2; // Guardar la posición X
float Y2; // Guardar la posición Y
float max_X2position = 11268; // Posición máxima X mm
float max_Y2position = 1693; // Posición máxima Y mm
int sweep2_State; // Variable para saber si está un barrido en curso y en qué etapa del barrido vamos
bool sweep2_Side; // Variable para saber de que lado empieza el barrido
bool sweep2_aux; // Variable auxiliar para el barrido
int x2_sign; // Variable entera para definir dirección del barrido en X
int y2_sign; // Variable entera para definir dirección del barrido en Y
int x2_sweep = 15; // Divisiones para hacer el barrido en X
int y2_sweep = 3; // Divisiones para hacer el barrido en Y
// Tercer Piso
float X3; // Guardar la posición X
float Y3; // Guardar la posición Y
float max_X3position = 11268; // Posición máxima X mm
float max_Y3position = 1693; // Posición máxima Y mm
int sweep3_State; // Variable para saber si está un barrido en curso y en qué etapa del barrido vamos
bool sweep3_Side; // Variable para saber de que lado empieza el barrido
bool sweep3_aux; // Variable auxiliar para el barrido
int x3_sign; // Variable entera para definir dirección del barrido en X
int y3_sign; // Variable entera para definir dirección del barrido en Y
int x3_sweep = 15; // Divisiones para hacer el barrido en X
int y3_sweep = 3; // Divisiones para hacer el barrido en Y
// Cuarto Piso
float X4; // Guardar la posición X
float Y4; // Guardar la posición Y
float max_X4position = 11268; // Posición máxima X mm
float max_Y4position = 1693; // Posición máxima Y mm
int sweep4_State; // Variable para saber si está un barrido en curso y en qué etapa del barrido vamos
bool sweep4_Side; // Variable para saber de que lado empieza el barrido
bool sweep4_aux; // Variable auxiliar para el barrido
int x4_sign; // Variable entera para definir dirección del barrido en X
int y4_sign; // Variable entera para definir dirección del barrido en Y
int x4_sweep = 15; // Divisiones para hacer el barrido en X
int y4_sweep = 3; // Divisiones para hacer el barrido en Y

// Función para ir a Home
void Home1(bool side){
  int sign;
  
  if(side==true){ // Home principal
    Serial.println("Buscando Home...");
    sign = 1;
  }
  else if(side==false){ // Home secundario
    Serial.println("Buscando Posición de Inicio...");
    sign = -1;
  }

  Serial.print("Moviendo en X: "); Serial.print(-max_X1position*mmSteps_X*sign); Serial.println(" pasos.");
  stepperX11.move(-max_X1position*mmSteps_X*sign);
  stepperX12.move(-max_X1position*mmSteps_X*sign);
  
  if(side==true){
    while( (digitalRead(x11_home)==HIGH || digitalRead(x12_home)==HIGH) && (stepperX1.targetPosition()!=stepperX1.currentPosition() || stepperX2.targetPosition()!=stepperX2.currentPosition()) ){
      stepperX1.run();
      stepperX2.run();
      if (digitalRead(x1_home)==LOW){
        stepperX1.moveTo(stepperX1.currentPosition());
        Serial.println("Fin X1 Home Detectado");
      }
      if (digitalRead(x2_home)==LOW){
        stepperX2.moveTo(stepperX2.currentPosition());
        Serial.println("Fin X2 Home Detectado");
      }
    }
  }
  else if(side==false){
    while(digitalRead(x1_end)==HIGH && (stepperX1.targetPosition()!=stepperX1.currentPosition() || stepperX2.targetPosition()!=stepperX2.currentPosition()) ){
      stepperX1.run();
      stepperX2.run();
      if(digitalRead(x1_end)==LOW){
        stepperX1.moveTo(stepperX1.currentPosition());
        stepperX2.moveTo(stepperX2.currentPosition());
        Serial.println("Fin X1 End Detectado");
      }
      
   }
  }

  if(side==true){
    Serial.println("Home en eje X encontrado");
  }
  else if(side==false){
    Serial.println("Posición en eje X encontrada");
  }

  Serial.print("Moviendo en Y: "); Serial.print(-max_Yposition*mmSteps_Y*sign); Serial.println(" pasos.");
  stepperY.move(-max_Yposition*mmSteps_Y*sign);

  if(side==true){
    while(digitalRead(y_home)==HIGH && stepperY.targetPosition()!=stepperY.currentPosition()){
      stepperY.run();
      if (digitalRead(y_home)==LOW){
        stepperY.moveTo(stepperY.currentPosition());
        Serial.println("Fin Y Home Detectado");
      }
    }
  }
  else if(side==false){
    while(digitalRead(y_end)==HIGH && stepperY.targetPosition()!=stepperY.currentPosition() ){
      stepperY.run();
      if(digitalRead(y_end)==LOW){
        stepperY.moveTo(stepperY.currentPosition());
        Serial.println("Fin Y End Detectado");
      }
   }
  }

  if(side==true){
    Serial.println("Home en eje Y encontrado");
    stepperX1.setCurrentPosition(0);
    stepperX2.setCurrentPosition(0);
    stepperY.setCurrentPosition(0);
    X=0;
    Y=0;
    sweep_Side = true;
    x_sign = 1;
    y_sign = 1;
  }
  else if(side==false){
    Serial.println("Posición en eje Y encontrada");
    stepperX1.setCurrentPosition(max_Xposition*mmSteps_X);
    stepperX2.setCurrentPosition(max_Xposition*mmSteps_X);
    stepperY.setCurrentPosition(max_Yposition*mmSteps_Y);
    X=max_Xposition;
    Y=max_Yposition;
    sweep_Side = false;
    x_sign = -1;
    y_sign = -1;
  }
  sweep_State=0;
}

bool isFinished(int Xdiv, int Ydiv, int state){
  if( (Xdiv*Ydiv) == (state)){
    return true;
  }
  else{
    return false;
  }
}

bool XorY(int Ydiv, int state){
  if((state)%Ydiv==0){
    return true;
  }
  else{
    return false;
  }
}

void sweep_XY(int x_div, int y_div){
  float x_steps_div = float(float(max_Xposition-50)*mmSteps_X/(x_div-1));
  float y_steps_div = float(float(max_Yposition-50)*mmSteps_Y/(y_div-1));
  
  if(XorY(y_div, sweep_State)==true && sweep_aux==0){
    Serial.print("En X se dan saltos de "); Serial.print(x_steps_div/mmSteps_X*x_sign); Serial.println(" mm");
    stepperX1.move(x_steps_div*x_sign);
    stepperX2.move(x_steps_div*x_sign);
    while(stepperX1.targetPosition()!=stepperX1.currentPosition() || stepperX2.targetPosition()!=stepperX2.currentPosition()){
      stepperX1.run();
      stepperX2.run();
    }
    X+=x_steps_div/mmSteps_X*x_sign;
    sweep_aux=1;
    y_sign = -y_sign;
  }else if(XorY(x_div, sweep_State)==false && sweep_aux==0){
    Serial.print("En Y se dan saltos de "); Serial.print(y_steps_div/mmSteps_Y*y_sign); Serial.println(" mm");
    stepperY.move(y_steps_div*y_sign);
    while(stepperY.targetPosition()!=stepperY.currentPosition()){
      stepperY.run();
    }
    Y+=y_steps_div/mmSteps_Y*y_sign;
    sweep_aux=1;
  }
  else if(sweep_aux==1 && sendData()==true){
    sweep_State+=1;
    sweep_aux = 0;
  }
}

bool sendData(){
  if(sendData_aux==0){
    coz_hum = 0;
    coz_temp = 0;
    coz_co2 = 0;
    sendData_aux = 1;
    Serial3.println("Q"); // Enviar petición a Cozir
    return false;
  }
  else if(sendData_aux==1 && (coz_hum<=0 || coz_temp<=0 || coz_co2<=0) ){
    sendData_aux = 0;
    return false;
  }
  //else if(sendData_aux==1 && coz_hum!=0 && coz_temp!=0 && coz_co2!=0){
  else if(sendData_aux==1 && coz_hum>0 && coz_temp>0 && coz_co2>0){
    Serial.println(sensorstring); // Revisar cadenas entrantes
    Serial.println("Sweep");
    Serial.println(X); Serial.println(Y);
    Serial.println(coz_temp); Serial.println(coz_hum); Serial.println(coz_co2);
    sendData_aux = 0;
    return true;
  }
  return false;
}

void sweep(int x_div, int y_div){
  if(isFinished(x_div, y_div, sweep_State)){
    sweep_State=-1;
    sweep_Side = !sweep_Side;
    control='M';
    Serial.println("Toma de datos terminada");
  }
  else if(!isFinished(x_div, y_div, sweep_State) && sweep_State==0){
    if(sendData()==true){
        sweep_State+=1; 
    }
  }
  else if(!isFinished(x_div, y_div, sweep_State) && sweep_State>0){
    sweep_XY(x_div, y_div);
  }
}


void endOfCareer(){
  if(digitalRead(x1_home)==LOW && x1_home_State==LOW){
    Serial.println("Fin de carrera X1 Home Detectado");
    stepperX1.moveTo(stepperX1.currentPosition());
    x1_home_State = HIGH;
  }
  if(digitalRead(x1_home)==HIGH && x1_home_State==HIGH){
    x1_home_State = LOW;
  }
  if(digitalRead(x2_home)==LOW && x2_home_State==LOW){
    Serial.println("Fin de carrera X2 Home Detectado");
    stepperX2.moveTo(stepperX2.currentPosition());
    x2_home_State = HIGH;
  }
  if(digitalRead(x2_home)==HIGH && x2_home_State==HIGH){
    x2_home_State = LOW;
  }
  if(digitalRead(x1_end)==LOW && x1_end_State==LOW){
    Serial.println("Fin de carrera X End Detectado");
    stepperX1.moveTo(stepperX1.currentPosition());
    stepperX2.moveTo(stepperX2.currentPosition());
    x1_end_State = HIGH;
  }
  if(digitalRead(x1_end)==HIGH && x1_end_State==HIGH){
    x1_end_State = LOW;
  }
  if(digitalRead(y_home)==LOW && y_home_State==LOW){
    Serial.println("Fin de carrera Y Home Detectado");
    stepperY.moveTo(stepperY.currentPosition());
    y_home_State = HIGH;
  }
  if(digitalRead(y_home)==HIGH && y_home_State==HIGH){
    y_home_State = LOW;
  }
  if(digitalRead(y_end)==LOW && y_end_State==LOW){
    Serial.println("Fin de carrera Y End Detectado");
    stepperY.moveTo(stepperY.currentPosition());
    y_end_State=HIGH;
  }
  if(digitalRead(y_end)==HIGH && y_end_State==HIGH){
    y_end_State=LOW;
  }
}

/****************  Funcion para COZIR  ****************/
void serialEvent3() {
  sensorstring = Serial3.readStringUntil(13); // Leer cadena hasta salto de línea
  sensor_string_complete = true;
}

void setup(){
 Serial.begin(9600);
 Serial3.begin(9600);
 delay(2000);
 Serial.println("Modo Tocani");
 Serial.println("Escribe -- I# -- para ir a Home, I0-->Fondo\tI1-->Puerta");
 Serial.println("Escribe -- X# -- para mover el eje X en milimetros");
 Serial.println("Escribe -- Y# -- para mover el eje Y en milimetros");
 Serial.println("Escribe -- Cozir -- para recibir Temp/Hum/CO2");
 Serial.println("Escribe -- S -- para comenzar un barrido para recolectar datos");
 
 control = 'M';

 stepperX1.setMaxSpeed(motorSpeed);
 stepperX1.setSpeed(0);
 stepperX1.setAcceleration(motorAccel);
 stepperX2.setMaxSpeed(motorSpeed);
 stepperX2.setSpeed(0);
 stepperX2.setAcceleration(motorAccel);
 stepperY.setMaxSpeed(motorSpeed);
 stepperY.setSpeed(0);
 stepperY.setAcceleration(motorAccel);
 stepperX1.setPinsInverted(true,false,false);
 stepperX2.setPinsInverted(true,false,false);
 
 pinMode(x1_home, INPUT_PULLUP);
 pinMode(x1_end, INPUT_PULLUP);
 pinMode(x2_home, INPUT_PULLUP);
 pinMode(y_home, INPUT_PULLUP);
 pinMode(y_end, INPUT_PULLUP);
  
  //Home(true); // Ir al Home Principal
}

void loop(){
  while (Serial.available()>0) { // Revisar si hay valores disponibles en el Buffer Serial
    control = Serial.read();
    Travel= Serial.parseFloat();  // Put numeric value from buffer in TravelX variable
    serialAux = Serial.read();
    serialAux = Serial.read();
    if (control == 'P'){
      Serial.println("");
      Serial.println("*******************************");
      Serial.print("   Posición X es: ");
      Serial.println((X));
      Serial.print("   Posición Y es: ");
      Serial.println((Y));
      Serial.println("*******************************");
      Serial.println("");
    }
    if (control == 'I'){
      if (Travel == 1){
        Home(false); // Enviar a la puerta
      }
      if (Travel == 0){
        Home(true); // Enviar al fondo
      }
    }
    
    if (control == 'X'){
      stepperX1.setCurrentPosition(stepperX1.currentPosition());
      stepperX2.setCurrentPosition(stepperX2.currentPosition());
      stepperX1.setMaxSpeed(motorSpeed);
      stepperX1.setSpeed(0);
      stepperX1.setAcceleration(motorAccel);
      stepperX2.setMaxSpeed(motorSpeed);
      stepperX2.setSpeed(0);
      stepperX2.setAcceleration(motorAccel);
      stepperX1.setPinsInverted(true,false,false);
      stepperX2.setPinsInverted(true,false,false);
      
      float TravelX=Travel*mmSteps_X;
      Serial.print("Moviendo X ");
      Serial.print(Travel);
      Serial.println(" milimetros");
      stepperX1.move(TravelX);  // Mover a la posición indicada
      stepperX2.move(TravelX);
      Serial.print("Posición actual X: ");
      Serial.println(stepperX1.currentPosition()/mmSteps_X);
      Serial.print("Posición objetivo X: ");
      Serial.println(stepperX1.targetPosition()/mmSteps_X);
      X+=Travel;
    }
    if (control == 'Y'){
      stepperY.setCurrentPosition(stepperY.currentPosition());
      stepperY.setMaxSpeed(motorSpeed);
      stepperY.setSpeed(0);
      stepperY.setAcceleration(motorAccel);
      
      float TravelY=Travel*mmSteps_Y;
      Serial.print("Moviendo Y ");
      Serial.print(Travel);
      Serial.println(" milimetros");
      stepperY.move(TravelY);
      Y+=Travel;
      Serial.print("Posición actual Y: ");
      Serial.println(stepperY.currentPosition()/mmSteps_Y);
      Serial.print("Posición objetivo Y: ");
      Serial.println(stepperY.targetPosition()/mmSteps_Y);
    }
    
    if(control == 'S'){
      Serial.println("Iniciando toma de datos");
      Home(sweep_Side);
      if(sweep_Side==true){
        Travel = 25;
      }else{
        Travel = -25;
      }
      
      float TravelX=Travel*mmSteps_X;
      float TravelY=Travel*mmSteps_Y;
      
      stepperX1.move(Travel*mmSteps_X);
      stepperX2.move(Travel*mmSteps_X);
      while(stepperX1.targetPosition()!=stepperX1.currentPosition() || stepperX2.targetPosition()!=stepperX2.currentPosition()){
        stepperX1.run();
        stepperX2.run();
      }
      X+=Travel;
      stepperY.move(Travel*mmSteps_Y);
      while(stepperY.targetPosition()!=stepperY.currentPosition()){
        stepperY.run();
      }
      Y+=Travel;
      
      sweep_State=0;
    }
    
    delay(10);  // Esperar un poco antes de mover los motores
 }

 if (control == 'X'){ 
  stepperX1.run();
  stepperX2.run();
 }
 if (control == 'Y'){ 
  stepperY.run();
 }

 if (control == 'S'){
  sweep(x_sweep, y_sweep);
 }
 
  endOfCareer();
}
