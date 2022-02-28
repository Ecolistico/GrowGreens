void i2c_updateData() {
  if(millis()-i2c_timer>60000){ // Send data each minute
    if(NTP_flag) epoch += 60; // Add one minute if we did not recieve a NTP response
    i2c_updateParameters();
    
    Wire.beginTransmission(i2c_dir);
    Wire.write('a');
    i2c_write4Byte(myTime.longBytes);
    Wire.endTransmission();
    delay(5);

    Wire.beginTransmission(i2c_dir);
    Wire.write('b');
    i2c_write4Byte(temp1.longBytes);
    Wire.endTransmission();
    delay(5);

    Wire.beginTransmission(i2c_dir);
    Wire.write('c');
    i2c_write4Byte(hum1.longBytes);
    Wire.endTransmission();
    delay(5);

    Wire.beginTransmission(i2c_dir);
    Wire.write('d');
    i2c_write4Byte(temp2.longBytes);
    Wire.endTransmission();
    delay(5);

    Wire.beginTransmission(i2c_dir);
    Wire.write('e');
    i2c_write4Byte(hum2.longBytes);
    Wire.endTransmission();
    delay(5);

    Wire.beginTransmission(i2c_dir);
    Wire.write('f');
    i2c_write4Byte(temp3.longBytes);
    Wire.endTransmission();
    delay(5);

    Wire.beginTransmission(i2c_dir);
    Wire.write('g');
    i2c_write4Byte(hum3.longBytes);
    Wire.endTransmission();
    delay(5);

    Wire.beginTransmission(i2c_dir);
    Wire.write('h');
    i2c_write4Byte(temp4.longBytes);
    Wire.endTransmission();
    delay(5);

    Wire.beginTransmission(i2c_dir);
    Wire.write('i');
    i2c_write4Byte(hum4.longBytes);
    Wire.endTransmission();
    delay(5);

    Wire.beginTransmission(i2c_dir);
    Wire.write('j');
    i2c_write4Byte(temp5.longBytes);
    Wire.endTransmission();
    delay(5);

    Wire.beginTransmission(i2c_dir);
    Wire.write('k');
    i2c_write4Byte(hum5.longBytes);
    Wire.endTransmission();
    delay(5);

    Wire.beginTransmission(i2c_dir);
    Wire.write('l');
    i2c_write4Byte(temp6.longBytes);
    Wire.endTransmission();
    delay(5);

    Wire.beginTransmission(i2c_dir);
    Wire.write('m');
    i2c_write4Byte(hum6.longBytes);
    Wire.endTransmission();
    delay(5);
    
    i2c_timer = millis();
    udpWrite();
    NTP_flag = true;
    Serial.println(F("Data sended over i2c"));
  }
}

void i2c_updateParameters(){
  temp1.number = data_1.temperature;
  hum1.number = data_1.humidity;
  temp2.number = data_2.temperature;
  hum2.number = data_2.humidity;
  temp3.number = data_3.temperature;
  hum3.number = data_3.humidity;
  temp4.number = data_4.temperature;
  hum4.number = data_4.humidity;
  temp5.number = data_5.temperature;
  hum5.number = data_5.humidity;
  temp6.number = data_6.temperature;
  hum6.number = data_6.humidity;
  myTime.epoch = epoch;
}

void i2c_write4Byte(byte longBytes[4]){
  Wire.write(longBytes[0]); 
  Wire.write(longBytes[1]); 
  Wire.write(longBytes[2]); 
  Wire.write(longBytes[3]);
}

void i2c_requestData(){
  if(digitalRead(update_pin)){
    Wire.requestFrom(i2c_dir, 32); // Request 4 bytes of information for each floor (h_begin, m_begin, h_end, m_end) 8 floors
    if(Wire.available()){
      f1.hour_begin = Wire.read(); f1.minute_begin = Wire.read(); f1.hour_end = Wire.read(); f1.minute_end = Wire.read();
      f2.hour_begin = Wire.read(); f2.minute_begin = Wire.read(); f2.hour_end = Wire.read(); f2.minute_end = Wire.read();
      f3.hour_begin = Wire.read(); f3.minute_begin = Wire.read(); f3.hour_end = Wire.read(); f3.minute_end = Wire.read();
      f4.hour_begin = Wire.read(); f4.minute_begin = Wire.read(); f4.hour_end = Wire.read(); f4.minute_end = Wire.read();
      f5.hour_begin = Wire.read(); f5.minute_begin = Wire.read(); f5.hour_end = Wire.read(); f5.minute_end = Wire.read();
      f6.hour_begin = Wire.read(); f6.minute_begin = Wire.read(); f6.hour_end = Wire.read(); f6.minute_end = Wire.read();
      f7.hour_begin = Wire.read(); f7.minute_begin = Wire.read(); f7.hour_end = Wire.read(); f7.minute_end = Wire.read();
      f8.hour_begin = Wire.read(); f8.minute_begin = Wire.read(); f8.hour_end = Wire.read(); f8.minute_end = Wire.read();
      // Save new parameters in memory
      for(int i=4; i<12; i++){
        memorySave(i);
      }
    }
    else Serial.println(F("i2c_requestData() does not recieve any parameter from slave"));
  }
}

void i2c_updateConfigData(){
  byte longBytes[4];

  longBytes[0] = f1.hour_begin; longBytes[1] = f1.minute_begin; longBytes[2] = f1.hour_end; longBytes[3] = f1.minute_end;
  Wire.beginTransmission(i2c_dir);
  Wire.write('1');
  i2c_write4Byte(longBytes);
  Wire.endTransmission();
  delay(5);

  longBytes[0] = f2.hour_begin; longBytes[1] = f2.minute_begin; longBytes[2] = f2.hour_end; longBytes[3] = f2.minute_end;
  Wire.beginTransmission(i2c_dir);
  Wire.write('2');
  i2c_write4Byte(longBytes);
  Wire.endTransmission();
  delay(5);

  longBytes[0] = f3.hour_begin; longBytes[1] = f3.minute_begin; longBytes[2] = f3.hour_end; longBytes[3] = f3.minute_end;
  Wire.beginTransmission(i2c_dir);
  Wire.write('3');
  i2c_write4Byte(longBytes);
  Wire.endTransmission();
  delay(5);

  longBytes[0] = f4.hour_begin; longBytes[1] = f4.minute_begin; longBytes[2] = f4.hour_end; longBytes[3] = f4.minute_end;
  Wire.beginTransmission(i2c_dir);
  Wire.write('4');
  i2c_write4Byte(longBytes);
  Wire.endTransmission();
  delay(5);

  longBytes[0] = f5.hour_begin; longBytes[1] = f5.minute_begin; longBytes[2] = f5.hour_end; longBytes[3] = f5.minute_end;
  Wire.beginTransmission(i2c_dir);
  Wire.write('5');
  i2c_write4Byte(longBytes);
  Wire.endTransmission();
  delay(5);

  longBytes[0] = f6.hour_begin; longBytes[1] = f6.minute_begin; longBytes[2] = f6.hour_end; longBytes[3] = f6.minute_end;
  Wire.beginTransmission(i2c_dir);
  Wire.write('6');
  i2c_write4Byte(longBytes);
  Wire.endTransmission();
  delay(5);

  longBytes[0] = f7.hour_begin; longBytes[1] = f7.minute_begin; longBytes[2] = f7.hour_end; longBytes[3] = f7.minute_end;
  Wire.beginTransmission(i2c_dir);
  Wire.write('7');
  i2c_write4Byte(longBytes);
  Wire.endTransmission();
  delay(5);

  longBytes[0] = f8.hour_begin; longBytes[1] = f8.minute_begin; longBytes[2] = f8.hour_end; longBytes[3] = f8.minute_end;
  Wire.beginTransmission(i2c_dir);
  Wire.write('8');
  i2c_write4Byte(longBytes);
  Wire.endTransmission();
  delay(5);

  Serial.println(F("Config Data sended over i2c"));
}
