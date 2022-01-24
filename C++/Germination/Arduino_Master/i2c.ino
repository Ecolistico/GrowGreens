void receive(){
  if(Wire.available()){
    x = Wire.read();
    if(x == 'a'){
      myTime.longBytes[0] = Wire.read();
      myTime.longBytes[1] = Wire.read();
      myTime.longBytes[2] = Wire.read();
      myTime.longBytes[3] = Wire.read();
    }
    else if(x == 'b'){
      temp1.longBytes[0] = Wire.read();
      temp1.longBytes[1] = Wire.read();
      temp1.longBytes[2] = Wire.read();
      temp1.longBytes[3] = Wire.read();
    }
    else if(x == 'c'){
      hum1.longBytes[0] = Wire.read();
      hum1.longBytes[1] = Wire.read();
      hum1.longBytes[2] = Wire.read();
      hum1.longBytes[3] = Wire.read();
    }
    else if(x == 'b'){
      temp1.longBytes[0] = Wire.read();
      temp1.longBytes[1] = Wire.read();
      temp1.longBytes[2] = Wire.read();
      temp1.longBytes[3] = Wire.read();
    }
    else if(x == 'c'){
      hum1.longBytes[0] = Wire.read();
      hum1.longBytes[1] = Wire.read();
      hum1.longBytes[2] = Wire.read();
      hum1.longBytes[3] = Wire.read();
    }
    else if(x == 'd'){
      temp2.longBytes[0] = Wire.read();
      temp2.longBytes[1] = Wire.read();
      temp2.longBytes[2] = Wire.read();
      temp2.longBytes[3] = Wire.read();
    }
    else if(x == 'e'){
      hum2.longBytes[0] = Wire.read();
      hum2.longBytes[1] = Wire.read();
      hum2.longBytes[2] = Wire.read();
      hum2.longBytes[3] = Wire.read();
    }
    else if(x == 'f'){
      temp3.longBytes[0] = Wire.read();
      temp3.longBytes[1] = Wire.read();
      temp3.longBytes[2] = Wire.read();
      temp3.longBytes[3] = Wire.read();
    }
    else if(x == 'g'){
      hum3.longBytes[0] = Wire.read();
      hum3.longBytes[1] = Wire.read();
      hum3.longBytes[2] = Wire.read();
      hum3.longBytes[3] = Wire.read();
    }
    else if(x == 'h'){
      temp4.longBytes[0] = Wire.read();
      temp4.longBytes[1] = Wire.read();
      temp4.longBytes[2] = Wire.read();
      temp4.longBytes[3] = Wire.read();
    }
    else if(x == 'i'){
      hum4.longBytes[0] = Wire.read();
      hum4.longBytes[1] = Wire.read();
      hum4.longBytes[2] = Wire.read();
      hum4.longBytes[3] = Wire.read();
    }
    else if(x == 'j'){
      temp5.longBytes[0] = Wire.read();
      temp5.longBytes[1] = Wire.read();
      temp5.longBytes[2] = Wire.read();
      temp5.longBytes[3] = Wire.read();
    }
    else if(x == 'k'){
      hum5.longBytes[0] = Wire.read();
      hum5.longBytes[1] = Wire.read();
      hum5.longBytes[2] = Wire.read();
      hum5.longBytes[3] = Wire.read();
    }
    else if(x == 'l'){
      temp6.longBytes[0] = Wire.read();
      temp6.longBytes[1] = Wire.read();
      temp6.longBytes[2] = Wire.read();
      temp6.longBytes[3] = Wire.read();
    }
    else if(x == 'm'){
      hum6.longBytes[0] = Wire.read();
      hum6.longBytes[1] = Wire.read();
      hum6.longBytes[2] = Wire.read();
      hum6.longBytes[3] = Wire.read();
    }
    else if(x == '1'){
      f1.hour_begin = Wire.read();
      f1.minute_begin = Wire.read();
      f1.hour_end = Wire.read();
      f1.minute_end = Wire.read();
    }
    else if(x == '2'){
      f2.hour_begin = Wire.read();
      f2.minute_begin = Wire.read();
      f2.hour_end = Wire.read();
      f2.minute_end = Wire.read();
    }
    else if(x == '3'){
      f3.hour_begin = Wire.read();
      f3.minute_begin = Wire.read();
      f3.hour_end = Wire.read();
      f3.minute_end = Wire.read();
    }
    else if(x == '4'){
      f4.hour_begin = Wire.read();
      f4.minute_begin = Wire.read();
      f4.hour_end = Wire.read();
      f4.minute_end = Wire.read();
    }
    else if(x == '5'){
      f5.hour_begin = Wire.read();
      f5.minute_begin = Wire.read();
      f5.hour_end = Wire.read();
      f5.minute_end = Wire.read();
    }
    else if(x == '6'){
      f6.hour_begin = Wire.read();
      f6.minute_begin = Wire.read();
      f6.hour_end = Wire.read();
      f6.minute_end = Wire.read();
    }
    else if(x == '7'){
      f7.hour_begin = Wire.read();
      f7.minute_begin = Wire.read();
      f7.hour_end = Wire.read();
      f7.minute_end = Wire.read();
    }
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
     Serial.print(F("Temp1 = ")); Serial.println(temp1.number); 
    }
    else if(x == 'c') {
     Serial.print(F("Hum1 =  ")); Serial.println(hum1.number); 
    }
    else if(x == 'd') {
     Serial.print(F("Temp2 = ")); Serial.println(temp2.number); 
    }
    else if(x == 'e') {
     Serial.print(F("Hum2 =  ")); Serial.println(hum2.number); 
    }
    else if(x == 'f') {
     Serial.print(F("Temp3 = ")); Serial.println(temp3.number); 
    }
    else if(x == 'g') {
     Serial.print(F("Hum3 =  ")); Serial.println(hum3.number); 
    }
    else if(x == 'h') {
     Serial.print(F("Temp4 = ")); Serial.println(temp4.number); 
    }
    else if(x == 'i') {
     Serial.print(F("Hum4 =  ")); Serial.println(hum4.number); 
    }
    else if(x == 'j') {
     Serial.print(F("Temp5 = ")); Serial.println(temp5.number); 
    }
    else if(x == 'k') {
     Serial.print(F("Hum5 =  ")); Serial.println(hum5.number); 
    }
    else if(x == 'l') {
     Serial.print(F("Temp6 = ")); Serial.println(temp6.number); 
    }
    else if(x == 'm') {
     Serial.print(F("Hum6 =  ")); Serial.println(hum6.number); 
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
