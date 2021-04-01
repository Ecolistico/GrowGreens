void begin_EEPROM(){
  Wire.begin();
  if (myEEPROM.begin() == false)
  {
    Serial.println("No memory detected. Freezing.");
    while (1);
  }
  Serial.println("Memory detected!");
}

void write_EEPROM(int pos, byte val){
  byte auxVal = myEEPROM.read(pos);  
  if(val!=auxVal){
    Serial.print(F("debug,Saving in EEPROM on position("));
    Serial.print(pos);
    Serial.print(F(") Value="));
    Serial.println(val);
    myEEPROM.write(pos, val);
  }
  else{ 
    Serial.print(F("warning,EEPROM on position("));
    Serial.print(pos);
    Serial.print(F(") Value="));
    Serial.print(val);
    Serial.println(F(" already saved"));
  }   
}

byte read_byte(int pos){
   Serial.print(F("debug,EEPROM - pos("));
   byte Value = myEEPROM.read(pos);
   Serial.print(pos); Serial.print(F("): ")); Serial.println(Value);
   return Value;
}

void write_EEPROM(int pos, int val){
  int auxVal = 0;
  myEEPROM.get(pos, auxVal);
  if(val!=auxVal){
    Serial.print(F("info,Saving in EEPROM on position("));
    Serial.print(pos);
    Serial.print(F(") Value="));
    Serial.println(val);
    myEEPROM.put(pos, val);
  }
  else{ 
    Serial.print(F("warning,EEPROM on position("));
    Serial.print(pos);
    Serial.print(F(") Value="));
    Serial.print(val);
    Serial.println(F(" already saved"));
  }  
}

int read_int(int pos){
   int Value = 0;
   Serial.print(F("debug,EEPROM - pos("));
   myEEPROM.get(pos, Value);
   Serial.print(pos); Serial.print(F("): ")); Serial.println(Value);
   return Value;
}


void write_EEPROM(int pos, float val){
  float auxVal = 0;
  myEEPROM.get(pos, auxVal);
  if(val!=auxVal){
    Serial.print(F("debug,Saving in EEPROM on position("));
    Serial.print(pos);
    Serial.print(F(") Value="));
    Serial.println(val);
    myEEPROM.put(pos, val);
  }
  else{ 
    Serial.print(F("warning,EEPROM on position("));
    Serial.print(pos);
    Serial.print(F(") Value="));
    Serial.print(val);
    Serial.println(F(" already saved"));
  }   
}

float read_float(int pos){
   float Value = 0;
   Serial.print(F("debug,EEPROM - pos("));
   myEEPROM.get(pos, Value);
   Serial.print(pos); Serial.print(F("): ")); Serial.println(Value);
   return Value;
}

void write_EEPROM(int pos, long val) {
  long auxVal = 0;
  myEEPROM.get(pos, auxVal);
  if(val!=auxVal){
    Serial.print(F("info,Saving in EEPROM on position("));
    Serial.print(pos);
    Serial.print(F(") Value="));
    Serial.println(val);
    myEEPROM.put(pos, val);
  }
  else{ 
    Serial.print(F("warning,EEPROM on position("));
    Serial.print(pos);
    Serial.print(F(") Value="));
    Serial.print(val);
    Serial.println(F(" already saved"));
  }  
}

long read_long(int pos){
   long Value = 0;
   Serial.print(F("debug,EEPROM - pos("));
   myEEPROM.get(pos, Value);
   Serial.print(pos); Serial.print(F("): ")); Serial.println(Value);
   return Value;
}

void write_EEPROM(int pos, unsigned long val) {
  unsigned long auxVal = 0;
  myEEPROM.get(pos, auxVal);
  if(val!=auxVal){
    Serial.print(F("info,Saving in EEPROM on position("));
    Serial.print(pos);
    Serial.print(F(") Value="));
    Serial.println(val);
    myEEPROM.put(pos, val);
  }
  else{ 
    Serial.print(F("warning,EEPROM on position("));
    Serial.print(pos);
    Serial.print(F(") Value="));
    Serial.print(val);
    Serial.println(F(" already saved"));
  }  
}

unsigned long read_ulong(int pos){
   unsigned long Value = 0;
   Serial.print(F("debug,EEPROM - pos("));
   myEEPROM.get(pos, Value);
   Serial.print(pos); Serial.print(F("): ")); Serial.println(Value);
   return Value;
}

void clean_EEPROM(bool truncate /*= true*/){
  int sizeLength = 0;
  if(truncate) sizeLength=1000;
  else sizeLength=myEEPROM.getMemorySize();
  Serial.println(F("warning,EEPROM: Deleting Memory..."));
  for(int i=0; i<sizeLength; i++){
    write_EEPROM(i, 0);
  }
  Serial.println(F("warning,EEPROM: Memory Deleted"));
}

void print_EEPROM(bool truncate /*= true*/){
  int sizeLength = 0;
  if(truncate) sizeLength=1000;
  else sizeLength=myEEPROM.getMemorySize();
  Serial.println(F("info,EEPROM: Printing Memory Info"));
  for(int i=0; i<sizeLength; i++){
    Serial.print(i); Serial.print(F(": ")); Serial.println(myEEPROM.read(i));
  }
}

void configEEPROM(byte floors, byte solenoids, byte minutes, byte analogSensors, byte flowMeters, byte scales, byte ultrasonicSensors, byte switches){
  write_EEPROM(0, floors);
  Serial.println(F("Floors Number written in position 0"));
  write_EEPROM(1, solenoids);
  Serial.println(F("Solenoids number written in position 1"));
  write_EEPROM(2, analogSensors);
  Serial.println(F("Analog sensors number written in position 2"));
  write_EEPROM(3, flowMeters);
  Serial.println(F("Flowmeter sensors number written in position 3"));
  write_EEPROM(4, scales);
  Serial.println(F("Weight sensors number written in position 4"));
  write_EEPROM(5, ultrasonicSensors);
  Serial.println(F("Ultrasonic sensors number written in position 5"));
  write_EEPROM(6, switches);
  Serial.println(F("Switch sensors number written in position 6"));    
  write_EEPROM(21, minutes);
  Serial.println(F("Minutes written in position 21"));
  Serial.println(F("EEPROM Configured successfully!"));
}

void getConfig(){
  floors = read_byte(0);
  solenoids = read_byte(1);
  analogSensors = read_byte(2);
  flowMeters = read_byte(3);
  scales = read_byte(4);
  ultrasonicSensors = read_byte(5);
  switches = read_byte(6);
  minutes = read_byte(21);
}

bool checkConfig() {
  if (floors==0 || solenoids==0 || minutes==0) return false;
  else return true;
}


void saveRegion(int Floor, bool side, int region){
  int NumEv = myEEPROM.read(1);
  if (region>=0 && region<=NumEv) {
    int NewPos = 21 + 1 +((Floor-1)*2)+int(side);
    write_EEPROM(NewPos, region);
  }
  else{Serial.println(F("error,Region Save EEPROM Function: parameter region incorrect"));}
}

void saveSolenoidTimeOn(int Floor, int region, byte timeOn){
  int NumFloor = myEEPROM.read(0);
  int NumEv = myEEPROM.read(1);
  int posWrite = 21 + 1 + NumFloor*2 + ((Floor-1)*NumEv)+(region-1);
  myEEPROM.write(posWrite, timeOn); 
}

void saveCycleTime(int val){
  if(val>=0 && val<=254){
    int pos = solenoidValve::__TotalActuators*5;
    save_EEPROM(pos, val);
  }
  else{ Serial.println(F("error,Solenoid Save Cycle Time (): value parameter incorrect")); }
}

void saveAnalog(byte pin, float A, float B, float C){
  int NumFloor = myEEPROM.read(0);
  int NumEv = myEEPROM.read(1);
  int NumAnalSen = myEEPROM.read(2);
  int posPin = 21 + 1 + NumFloor*2 + (((NumFloor-1)*NumEv)+(1)) + 1;
  myEEPROM.write(posPin, pin);
  int posA = 21 + 1 + NumFloor*2 + (((NumFloor-1)*NumEv)+(1)) + 1 + ((sizeof(pin))*NumAnalSen);
  myEEPROM.put(posA, A);
  int posB = 21 + 1 + NumFloor*2 + (((NumFloor-1)*NumEv)+(1)) + 1 + ((sizeof(pin) + sizeof(A))*NumAnalSen);
  myEEPROM.put(posB, B);
  int posC = 21 + 1 + NumFloor*2 + (((NumFloor-1)*NumEv)+(1)) + 1 + ((sizeof(pin) + sizeof(A) + sizeof(B))*NumAnalSen);
  myEEPROM.put(posC, C);
}
/*
void analogSaveFilter(int Type, int filt, float filterParam){
  if(Type>=0 && Type<=2){
    if(filt>=0 && filt<=2){
      int pos = 512+(1+sizeof(float)*4)*Type;
      save_EEPROM(pos, filt); 
      save_EEPROM(pos+1, filterParam); 
    }
    else{Serial.println(F("error,Analog Sensor Save Filter: Parameter filt incorrect"));}
  }
  else{Serial.println(F("error,Analog Sensor Save Filter: Parameter type incorrect"));}
}

void analogSaveModel(int Type, float a, float b, float c){
  if(Type>=0 && Type<=2){
    int pos = 512+(1+sizeof(float)*4)*Type+1+sizeof(float);
    save_EEPROM(pos, a);
    save_EEPROM(pos+sizeof(float), b);
    save_EEPROM(pos+sizeof(float)*2, c);
  }
  else{Serial.println(F("error,Analog Sensor Save Mode: Parameter type incorrect"));}
}

bool chargeAnalogParameters(int Type){
  if(Type>=0 && Type<=2){
    int pos = 512+(1+sizeof(float)*4)*Type; // Initial Position to save
    uint8_t filt = EEPROM.read(pos); // Filter
    float paramFilter, a, b, c; // Float values
    uint8_t deg = 0;
    EEPROM.get(pos+1, paramFilter);
    EEPROM.get(pos+1+sizeof(float), a);
    EEPROM.get(pos+1+sizeof(float)*2, b);
    EEPROM.get(pos+1+sizeof(float)*3, c);
    
    if(filt==0){analogSensor::ptr[Type]->notFilter();} // Not Filter
    else if(filt==1){ 
      // Exponential Filter
      if(paramFilter>0 && paramFilter<1){analogSensor::ptr[Type]->setExponentialFilter(paramFilter);}
      else{analogSensor::ptr[Type]->defaultFilter();} // Default Filter
    }
    else if(filt==2){
      // Kalman Filter
      if(paramFilter>0){analogSensor::ptr[Type]->setKalmanFilter(paramFilter);}
      else{analogSensor::ptr[Type]->defaultFilter();} // Default Filter
    }

    if(c!=0){deg = 3;}
    else if(b!=0){deg = 2;}
    else if(a!=0){deg = 1;}
    else{deg = 0;}
    if(deg==0 || !analogSensor::ptr[Type]->setModel(deg, a, b, c)){
      analogSensor::ptr[Type]->setModel(2, -21.17, 0.2117);
    }
    return true;
  }
  else{
    Serial.println(F("error,Analog Sensor Charge Parameter: Parameter type incorrect"));
    return false;
  }
}

void ultrasonicSaveFilter(int Type, int filt, float filterParam){
  if(Type>=0 && Type<=6){
    if(filt>=0 && filt<=2){
      int pos = 563+(2+sizeof(float)*3)*Type;
      save_EEPROM(pos, filt);
      save_EEPROM(pos+1, filterParam); 
    }
    else{Serial.println(F("error,Ultrasonic Sensor Save Filter: Parameter filt incorrect"));}
  }
  else{Serial.println(F("error,Ultrasonic Sensor Save Filter: Parameter type incorrect"));}
}

void ultrasonicSaveModel(int Type, int model, float modelParam, float height){
  if(Type>=0 && Type<=6){
    if(model>=0 && model<=2){
      if(modelParam>0){
        if(height>=UltraSonic::ptr[Type]->getMaxDist()){
          int pos = 563+(2+sizeof(float)*3)*Type+1+sizeof(float);
          save_EEPROM(pos, model);
          save_EEPROM(pos+1, modelParam);
          save_EEPROM(pos+1+sizeof(float), height);
        }
        else{Serial.println(F("error,Ultrasonic Sensor Save Mode: Parameter height incorrect"));}
      }
      else{Serial.println(F("error,Ultrasonic Sensor Save Mode: Parameter modelParam incorrect"));}
    }
    else{Serial.println(F("error,Ultrasonic Sensor Save Mode: Parameter model incorrect"));}
  }
  else{Serial.println(F("error,Ultrasonic Sensor Save Mode: Parameter type incorrect"));}
}

bool chargeUltrasonicParameters(int Type){
  if(Type>=0 && Type<=6){
    int pos = 563+(2+sizeof(float)*3)*Type; // Initial Position to save
    uint8_t filt = EEPROM.read(pos); // Filter
    uint8_t model = EEPROM.read(pos+1+sizeof(float)); // Model
    float paramFilter, paramModel, height; // Float values

    EEPROM.get(pos+1, paramFilter);
    EEPROM.get(pos+2+sizeof(float), paramModel);
    EEPROM.get(pos+2+sizeof(float)*2, height);
    
    if(filt==0){UltraSonic::ptr[Type]->notFilter();} // Not Filter
    else if(filt==1){ 
      // Exponential Filter
      if(paramFilter>0 && paramFilter<1){UltraSonic::ptr[Type]->setExponentialFilter(paramFilter);}
      else{UltraSonic::ptr[Type]->defaultFilter();} // Default Filter
    }
    else if(filt==2){
      // Kalman Filter
      if(paramFilter>0){UltraSonic::ptr[Type]->setKalmanFilter(paramFilter);}
      else{UltraSonic::ptr[Type]->defaultFilter();} // Default Filter
    }

    if(model<0 || model>2 || paramModel<=0 || height<UltraSonic::ptr[Type]->getMaxDist() || !UltraSonic::ptr[Type]->setModel(model, paramModel, height)){
      UltraSonic::ptr[Type]->setModel(1, 27, 88);
    }    
    return true;
  }
  else{
    Serial.println(F("error,Ultrasonic Sensor Charge Parameter: Parameter type incorrect"));
    return false;
  }
}

void pressureSave(int Type, float Press){
  if(Type==0){save_EEPROM(256, Press);}
  else if(Type==1){save_EEPROM(256+sizeof(float), Press);}
  else if(Type==2){save_EEPROM(256+sizeof(float)*2, Press);}
}

void chargePressureParameter(){
  float maxPress, minPress, criticalPress; // Float values

    EEPROM.get(256, maxPress);
    EEPROM.get(256+sizeof(float), minPress);
    EEPROM.get(256+sizeof(float)*2, criticalPress);

    if(maxPress>0 && max_pressure!=maxPress && !isnan(maxPress)){
      max_pressure = maxPress;
      Serial.print(F("Max Pressure\t= "));
      Serial.print(max_pressure);
      Serial.println(F(" psi"));
    }
    if(minPress>0 && min_pressure!=minPress && !isnan(minPress)){
      min_pressure = minPress;
      Serial.print(F("Min Pressure\t= "));
      Serial.print(min_pressure);
      Serial.println(F(" psi"));
    }
    if(criticalPress>0 && critical_pressure!=criticalPress && !isnan(criticalPress)){
      critical_pressure = criticalPress;
      Serial.print(F("Critical Pressure\t= "));
      Serial.print(critical_pressure);
      Serial.println(F(" psi"));
    }

    Serial.flush();
    delay(2500);
}
*/
