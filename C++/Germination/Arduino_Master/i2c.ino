void receive(){
  if(Wire.available()){
    x = Wire.read();
    // Time Info
    if(x == 'a'){
      myTime.longBytes[0] = Wire.read();
      myTime.longBytes[1] = Wire.read();
      myTime.longBytes[2] = Wire.read();
      myTime.longBytes[3] = Wire.read();
    }
    // Temperature Info
    else if(x == 'b'){
      temp.longBytes[0] = Wire.read();
      temp.longBytes[1] = Wire.read();
      temp.longBytes[2] = Wire.read();
      temp.longBytes[3] = Wire.read();
    }
    // Humidity Info
    else if(x == 'c'){
      hum.longBytes[0] = Wire.read();
      hum.longBytes[1] = Wire.read();
      hum.longBytes[2] = Wire.read();
      hum.longBytes[3] = Wire.read();
    }
    // CO2 Info
    else if(x == 'd'){
      co2.longBytes[0] = Wire.read();
      co2.longBytes[1] = Wire.read();
      co2.longBytes[2] = Wire.read();
      co2.longBytes[3] = Wire.read();
    }
    // Floor 1 settings
    else if(x == '1'){
      f1.hour_begin = Wire.read();
      f1.minute_begin = Wire.read();
      f1.hour_end = Wire.read();
      f1.minute_end = Wire.read();
    }
    // Floor 2 settings
    else if(x == '2'){
      f2.hour_begin = Wire.read();
      f2.minute_begin = Wire.read();
      f2.hour_end = Wire.read();
      f2.minute_end = Wire.read();
    }
    // Floor 3 settings
    else if(x == '3'){
      f3.hour_begin = Wire.read();
      f3.minute_begin = Wire.read();
      f3.hour_end = Wire.read();
      f3.minute_end = Wire.read();
    }
    // Floor 4 settings
    else if(x == '4'){
      f4.hour_begin = Wire.read();
      f4.minute_begin = Wire.read();
      f4.hour_end = Wire.read();
      f4.minute_end = Wire.read();
    }
    // Floor 5 settings
    else if(x == '5'){
      f5.hour_begin = Wire.read();
      f5.minute_begin = Wire.read();
      f5.hour_end = Wire.read();
      f5.minute_end = Wire.read();
    }
    // Floor 6 settings
    else if(x == '6'){
      f6.hour_begin = Wire.read();
      f6.minute_begin = Wire.read();
      f6.hour_end = Wire.read();
      f6.minute_end = Wire.read();
    }
    // Floor 7 settings
    else if(x == '7'){
      f7.hour_begin = Wire.read();
      f7.minute_begin = Wire.read();
      f7.hour_end = Wire.read();
      f7.minute_end = Wire.read();
    }
    // Floor 8 settings
    else if(x == '8'){
      f8.hour_begin = Wire.read();
      f8.minute_begin = Wire.read();
      f8.hour_end = Wire.read();
      f8.minute_end = Wire.read();
    }
    flag = true;
  }
}

void request() {
  Wire.write(f1.hour_begin); Wire.write(f1.minute_begin); Wire.write(f1.hour_end); Wire.write(f1.minute_end);
  Wire.write(f2.hour_begin); Wire.write(f2.minute_begin); Wire.write(f2.hour_end); Wire.write(f2.minute_end);
  Wire.write(f3.hour_begin); Wire.write(f3.minute_begin); Wire.write(f3.hour_end); Wire.write(f3.minute_end);
  Wire.write(f4.hour_begin); Wire.write(f4.minute_begin); Wire.write(f4.hour_end); Wire.write(f4.minute_end);
  Wire.write(f5.hour_begin); Wire.write(f5.minute_begin); Wire.write(f5.hour_end); Wire.write(f5.minute_end);
  Wire.write(f6.hour_begin); Wire.write(f6.minute_begin); Wire.write(f6.hour_end); Wire.write(f6.minute_end);
  Wire.write(f7.hour_begin); Wire.write(f7.minute_begin); Wire.write(f7.hour_end); Wire.write(f7.minute_end);
  Wire.write(f8.hour_begin); Wire.write(f8.minute_begin); Wire.write(f8.hour_end); Wire.write(f8.minute_end);
  digitalWrite(updatePin, LOW);
}

void i2c_debug() {
  if(flag){
    if(x == 'a') {
     Serial.print(F("EPOCH = ")); Serial.println(myTime.epoch); 
    }
    else if(x == 'b') {
     Serial.print(F("Temp = ")); Serial.println(temp.number); 
    }
    else if(x == 'c') {
     Serial.print(F("Hum =  ")); Serial.println(hum.number); 
    }
    else if(x == 'd') {
     Serial.print(F("CO2 = ")); Serial.println(co2.number); 
    }
    else if(x == '1') {
     Serial.print(F("Floor 1. Turn on at ")); Serial.print(f1.hour_begin); Serial.print(F(":")); Serial.print(f1.minute_begin); Serial.print(F("\tTurn off at ")); Serial.print(f1.hour_end); Serial.print(F(":")); Serial.println(f1.minute_end);
    }
    else if(x == '2') {
     Serial.print(F("Floor 2. Turn on at ")); Serial.print(f2.hour_begin); Serial.print(F(":")); Serial.print(f2.minute_begin); Serial.print(F("\tTurn off at ")); Serial.print(f2.hour_end); Serial.print(F(":")); Serial.println(f2.minute_end);
    }
    else if(x == '3') {
     Serial.print(F("Floor 3. Turn on at ")); Serial.print(f3.hour_begin); Serial.print(F(":")); Serial.print(f3.minute_begin); Serial.print(F("\tTurn off at ")); Serial.print(f3.hour_end); Serial.print(F(":")); Serial.println(f3.minute_end);
    }
    else if(x == '4') {
     Serial.print(F("Floor 4. Turn on at ")); Serial.print(f4.hour_begin); Serial.print(F(":")); Serial.print(f4.minute_begin); Serial.print(F("\tTurn off at ")); Serial.print(f4.hour_end); Serial.print(F(":")); Serial.println(f4.minute_end);
    }
    else if(x == '5') {
     Serial.print(F("Floor 5. Turn on at ")); Serial.print(f5.hour_begin); Serial.print(F(":")); Serial.print(f5.minute_begin); Serial.print(F("\tTurn off at ")); Serial.print(f5.hour_end); Serial.print(F(":")); Serial.println(f5.minute_end);
    }
    else if(x == '6') {
     Serial.print(F("Floor 6. Turn on at ")); Serial.print(f6.hour_begin); Serial.print(F(":")); Serial.print(f6.minute_begin); Serial.print(F("\tTurn off at ")); Serial.print(f6.hour_end); Serial.print(F(":")); Serial.println(f6.minute_end);
    }
    else if(x == '7') {
     Serial.print(F("Floor 7. Turn on at ")); Serial.print(f7.hour_begin); Serial.print(F(":")); Serial.print(f7.minute_begin); Serial.print(F("\tTurn off at ")); Serial.print(f7.hour_end); Serial.print(F(":")); Serial.println(f7.minute_end);
    }
    else if(x == '8') {
     Serial.print(F("Floor 8. Turn on at ")); Serial.print(f8.hour_begin); Serial.print(F(":")); Serial.print(f8.minute_begin); Serial.print(F("\tTurn off at ")); Serial.print(f8.hour_end); Serial.print(F(":")); Serial.println(f8.minute_end);
    }
    flag = false;
    x = 'z';
  }
}
