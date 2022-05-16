// Begin touchscreen
void beginTFT(){
  pinMode(updatePin, OUTPUT);
  digitalWrite(updatePin, LOW);
  uint16_t ID = tft.readID();
  if (ID == 0xD3D3) ID = 0x9486; // write-only shield
  tft.begin(ID);
  tft.setRotation(Orientation);
  splashscreen();
  home_screen(led_s);
  screens = 0;
  updateTimer = millis();
}

// Get XY touch
bool Touch_getXY(void) { 
  TSPoint tp = ts.getPoint();
  pinMode(YP, OUTPUT); //restore shared pins
  pinMode(XM, OUTPUT);
  digitalWrite(YP, HIGH); // because TFT control pins
  digitalWrite(XM, HIGH);
  bool pressed = (tp.z > MINPRESSURE && tp.z < MAXPRESSURE);
  if (pressed) {
    switch (Orientation) {
      case 0:
        xpos = map(tp.x, TS_LEFT, TS_RT, 0, tft.width());
        ypos = map(tp.y, TS_TOP, TS_BOT, 0, tft.height());
        break;
      case 1:
        xpos = map(tp.y, TS_TOP, TS_BOT, 0, tft.width());
        ypos = map(tp.x, TS_RT, TS_LEFT, 0, tft.height());
        break;
      case 2:
        xpos = map(tp.x, TS_RT, TS_LEFT, 0, tft.width());
        ypos = map(tp.y, TS_BOT, TS_TOP, 0, tft.height());
        break;
      case 3:
        xpos = map(tp.y, TS_BOT, TS_TOP, 0, tft.width());
        ypos = map(tp.x, TS_LEFT, TS_RT, 0, tft.height());
        break;
    }
  }
  return pressed;
}

// Splashscreen
void splashscreen()
{
  tft.fillScreen(BLACK); 
  tft.setTextColor(GREEN);
  tft.setTextSize(2);
  tft.setCursor(60,30);
  tft.print(F("BABY CENTER"));
  tft.setTextSize(2);
  tft.setTextColor(GREEN);
  tft.setCursor(20,200);
  tft.print(F("POWERED BY ECOLISTICO"));
  tft.setTextSize(4);
  tft.setTextColor(WHITE);
  tft.setCursor(30,100);
  tft.print(F("GROW GREENS")); 
  delay(2000);
}

// Home screen
void home_screen(led_status led_st){   
  uint16_t color = 0;
  tft.fillScreen(BLACK);
  tft.drawLine(0, 80, 400, 80, YELLOW);
  tft.setCursor(60,40);
  tft.setTextSize(3);
  tft.setTextColor(WHITE);
  tft.print(F("BABY CENTER"));   

  // Indicator 1
  if(led_st.f1) color = GREEN;
  else color = RED;
  tft.drawCircle(50, 120, 10, color); 
  tft.fillCircle(50, 120, 10, color);
  tft.setTextSize(1.5);
  tft.setTextColor(WHITE);
  tft.setCursor(45,140);
  tft.print(F("F1"));
    
  // Indicator 2
  if(led_st.f2) color = GREEN;
  else color = RED;
  tft.drawCircle(80, 120, 10, color);
  tft.fillCircle(80, 120, 10, color);
  tft.setCursor(75,140);
  tft.print(F("F2")); 

  // Indicator 3
  if(led_st.f3) color = GREEN;
  else color = RED;
  tft.drawCircle(110, 120, 10, color);
  tft.fillCircle(110, 120, 10, color);
  tft.setCursor(105,140);
  tft.print(F("F3")); 

  // Indicator 4
  if(led_st.f4) color = GREEN;
  else color = RED;
  tft.drawCircle(140, 120, 10, color);
  tft.fillCircle(140, 120, 10, color);
  tft.setCursor(135,140);
  tft.print(F("F4")); 

  // Indicator 5
  if(led_st.f5) color = GREEN;
  else color = RED;
  tft.drawCircle(170, 120, 10, color);
  tft.fillCircle(170, 120, 10, color);
  tft.setCursor(165,140);
  tft.print(F("F5")); 

  // Indicator 6
  if(led_st.f6) color = GREEN;
  else color = RED;
  tft.drawCircle(200, 120, 10, color);
  tft.fillCircle(200, 120, 10, color);
  tft.setCursor(195,140);
  tft.print(F("F6")); 

  // Indicator 7
  if(led_st.f7) color = GREEN;
  else color = RED;
  tft.drawCircle(230, 120, 10, color);
  tft.fillCircle(230, 120, 10, color);
  tft.setCursor(225,140);
  tft.print(F("F7")); 

  // Indicator 8
  if(led_st.f8) color = GREEN;
  else color = RED;
  tft.drawCircle(260, 120, 10, color);
  tft.fillCircle(260, 120, 10, color);  
  tft.setCursor(255,140);
  tft.print(F("F8")); 

  // Temperature
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.setCursor(20,170);
  tft.print(F("Temp= "));

  // Humidity
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.setCursor(180,170);
  tft.print(F("Hum= "));

  // CO2
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.setCursor(20,210);
  tft.print(F("CO2= "));

  // Hour display
  tft.setTextColor(GRAY);
  tft.setCursor(20, 10);
  tft.println(F("00:00"));

  // Settings button
  tft.setTextColor(GRAY);
  tft.setCursor(200, 10);
  tft.println(F("CONFIG"));
}

// Config Screen
void ConfigFloor() {
  tft.fillScreen(BLACK);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.drawRoundRect(3, 3, 314, 234, 6, GREEN);
  
  tft.drawRoundRect(10, 10, 150, 30, 6, GREEN);
  tft.setCursor(15, 15);
  tft.println(F("FLOOR     1")); 
  tft.drawRoundRect(10, 50, 150, 30, 6, GREEN);
  tft.setCursor(15, 55);
  tft.println(F("FLOOR     2"));
  tft.drawRoundRect(10, 90, 150, 30, 6, GREEN);
  tft.setCursor(15, 95);
  tft.println(F("FLOOR     3"));
  tft.drawRoundRect(10, 130, 150, 30, 6, GREEN);
  tft.setCursor(15, 135);
  tft.println(F("FLOOR     4"));
  tft.drawRoundRect(10, 170, 150, 30, 6, GREEN);
  tft.setCursor(15, 175);
  tft.println(F("RELAY     1"));

  tft.drawRoundRect(162, 10, 150, 30, 6, GREEN);
  tft.setCursor(167, 15);
  tft.println(F("FLOOR     5"));
  tft.drawRoundRect(162, 50, 150, 30, 6, GREEN);
  tft.setCursor(167, 55);
  tft.println(F("FLOOR     6"));
  tft.drawRoundRect(162, 90, 150, 30, 6, GREEN);
  tft.setCursor(167, 95);
  tft.println(F("FLOOR     7"));
  tft.drawRoundRect(162, 130, 150, 30, 6, GREEN);
  tft.setCursor(167, 135);
  tft.println(F("FLOOR     8"));
  tft.drawRoundRect(162, 170, 150, 30, 6, GREEN);
  tft.setCursor(167, 175);
  tft.println(F("RELAY     2"));
  
  //tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.setCursor(30, 210);
  tft.println(F("Atras"));
}

// Keyboard Screen
void keyBoard() {
  tft.fillScreen(BLACK);
  tft.drawRoundRect(3, 3, 314, 234, 6, GREEN);
  tft.fillRoundRect(43, 43, 314-80, 30, 6, WHITE);
  tft.drawRoundRect(43, 43, 314-80, 30, 6, GREEN);
  String txt = "";
  if(conf_number==1) txt = "Hora Inicio";
  else if(conf_number==2) txt = "Minuto Inicio";
  else if(conf_number==3) txt = "Hora Fin";
  else if(conf_number==4) txt = "Minuto Fin";
  tft.setCursor(floor((320-((txt.length()+1)*5*2))/2), 15);
  tft.setTextSize(2);
  tft.println(txt);
  
  for (int i=0;i<5;i++){
    tft.setTextColor(WHITE);
    
    tft.drawRoundRect(8+i*(tft.width()-16)/5+i,tft.height()/3,(tft.width()-16)/5-3,(tft.width()-16)/5-3,6,GREEN);
    tft.setCursor(8+i*(tft.width()-16)/5+i+floor(((tft.width()-16)/5-3)/2)-5, tft.height()/3+ floor((tft.width()-16)/5-3-16)/2);
    tft.println(i);
    
    tft.drawRoundRect(8+i*(tft.width()-16)/5+i,tft.height()/3+(tft.width()-16)/5,(tft.width()-16)/5-3,(tft.width()-16)/5-3,6,GREEN);
    tft.setCursor(8+i*(tft.width()-16)/5+i+floor(((tft.width()-16)/5-3)/2)-5, tft.height()/3 + 4*floor((tft.width()-16)/5-3-16)/2-3);
    tft.println(i+5);
  }

  tft.setTextColor(BLACK);
  tft.setCursor(floor((320-((String(conf_value).length()+1)*5*2))/2), 51);
  
  tft.println(conf_value);
  
  tft.setTextColor(WHITE);
  tft.setCursor(30, 205);
  tft.println(F("Atras        Continuar"));
  
  tft.setCursor(285,51);
  tft.println(F("x"));
}

void relayScreen(String relay, bool intervalo){
  tft.fillScreen(BLACK);
  tft.drawRoundRect(3, 3, 314, 234, 6, GREEN);
  
  tft.setCursor(floor((320-((relay.length()+1)*5*2))/2), 15);
  tft.setTextSize(2);
  tft.println(relay);
  
  if(intervalo){
    tft.drawRoundRect(10, 40, 150, 30, 6, GREEN);
    tft.fillRoundRect(10, 40, 150, 30, 6, GREEN);
    tft.setCursor(25, 50);
    tft.setTextColor(BLACK);
    tft.println(F("Intervalo"));
    
    tft.drawRoundRect(162, 40, 150, 30, 6, GREEN);
    tft.setCursor(200, 48);
    tft.setTextColor(WHITE);
    tft.println(F("Hora"));
  }
  else{
    tft.drawRoundRect(10, 40, 150, 30, 6, GREEN);
    tft.setCursor(25, 50);
    tft.setTextColor(WHITE);
    tft.println(F("Intervalo"));
    
    tft.drawRoundRect(162, 40, 150, 30, 6, GREEN);
    tft.fillRoundRect(162, 40, 150, 30, 6, GREEN);
    tft.setCursor(200, 48);
    tft.setTextColor(BLACK);
    tft.println(F("Hora"));
  }
  
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.setCursor(30, 210);
  tft.println(F("Atras"));
}

void updateTFT(){
  bool down = Touch_getXY();
  
  switch (screens){ // Move between screens
    // Home Screen
    case 0: // If Config button is pressed
      if (down == 1 && xpos<300 && xpos>130 && ypos<70 && ypos>5) {
        screens = 1;
        ConfigFloor();
      }    
      break;
      
    // Config Screen
    case 1: // Config Floor Display
      if (down == 1 && xpos<140 && xpos>5 && ypos<45 && ypos>10) {
        Serial.println(F("TFT: Floor 1 selected"));
        screens = 2;
        conf_number = 1;
        floor_selected = 1;
        conf_value = f1.hour_begin;
        keyBoard();
       }

      else if (down == 1 && xpos<140 && xpos>5 && ypos<85 && ypos>50) {
        Serial.println(F("TFT: Floor 2 selected"));
        screens = 2;
        conf_number = 1;
        floor_selected = 2;
        conf_value = f2.hour_begin;
        keyBoard();
      }

      else if (down == 1 && xpos<140 && xpos>5 && ypos<125 && ypos>90) {
        Serial.println(F("TFT: Floor 3 selected"));
        screens = 2;
        conf_number = 1;
        floor_selected = 3;
        conf_value = f3.hour_begin;
        keyBoard();
      }

      else if (down == 1 && xpos<140 && xpos>5 && ypos<165 && ypos>130) {
        Serial.println(F("TFT: Floor 4 selected"));
        screens = 2;
        conf_number = 1;
        floor_selected = 4;
        conf_value = f4.hour_begin;
        keyBoard();
      }

      else if (down == 1 && xpos<140 && xpos>5 && ypos<205 && ypos>170) {
        Serial.println(F("TFT: Relay 1 selected"));
        screens = 3;
        relayScreen("Relay 1", relay1_intervalo);
      }
    
      else if (down == 1 && xpos<300 && xpos>141 && ypos<45 && ypos>10) {
        Serial.println(F("TFT: Floor 5 selected"));
        screens = 2;
        conf_number = 1;
        floor_selected = 5;
        conf_value = f5.hour_begin;
        keyBoard();
      }

      else if (down == 1 && xpos<300 && xpos>141 && ypos<85 && ypos>50) {
        Serial.println(F("TFT: Floor 6 selected"));
        screens = 2;
        conf_number = 1;
        floor_selected = 6;
        conf_value = f6.hour_begin;
        keyBoard();
      }

      else if (down == 1 && xpos<300 && xpos>141 && ypos<125 && ypos>90) {
        Serial.println(F("TFT: Floor 7 selected"));
        screens = 2;
        conf_number = 1;
        floor_selected = 7;
        conf_value = f7.hour_begin;
        keyBoard();
      }

      else if (down == 1 && xpos<300 && xpos>141 && ypos<165 && ypos>130) {
        Serial.println(F("TFT: Floor 8 selected"));
        screens = 2;
        conf_number = 1;
        floor_selected = 8;
        conf_value = f8.hour_begin;
        keyBoard();
      }

      else if (down == 1 && xpos<300 && xpos>141 && ypos<205 && ypos>170) {
        Serial.println(F("TFT: Relay 2 selected"));
        screens = 3;
        relayScreen("Relay 2", relay2_intervalo);
      }

      else if (down == 1 && xpos>0 && xpos<107 && ypos>210 && ypos<240) {
        screens = 0;
        conf_number = 0;
        floor_selected = 0;
        conf_value = 0;
        home_screen(led_s);
      } 
      break;

    // Config Hour_begin, Minute_begin, Hour_end, Minute_end Floor 1-8
    case 2: 
      if (down == 1 && xpos<60 && xpos>5 && ypos<136 && ypos>81) {
        conf_value = conf_value*10+0;
        keyBoard();
      }
      else if (down == 1 && xpos<123 && xpos>68 && ypos<136 && ypos>81) {
        conf_value = conf_value*10+1;
        keyBoard();
      }
      else if (down == 1 && xpos<186 && xpos>131 && ypos<136 && ypos>81) {
        conf_value = conf_value*10+2;
        keyBoard();
      }
      else if (down == 1 && xpos<249 && xpos>194 && ypos<136 && ypos>81) {
        conf_value = conf_value*10+3;
        keyBoard();
      }
      else if (down == 1 && xpos<312 && xpos>257 && ypos<136 && ypos>81) {
        conf_value = conf_value*10+4;
        keyBoard();
      }
      else if (down == 1 && xpos<60 && xpos>5 && ypos<199 && ypos>144) {
        conf_value = conf_value*10+5;
        keyBoard();
      }
      else if (down == 1 && xpos<123 && xpos>68 && ypos<199 && ypos>144) {
        conf_value = conf_value*10+6;
        keyBoard();
      }
      else if (down == 1 && xpos<186 && xpos>131 && ypos<199 && ypos>144) {
        conf_value = conf_value*10+7;
        keyBoard();
      }
      else if (down == 1 && xpos<249 && xpos>194 && ypos<199 && ypos>144) {
        conf_value = conf_value*10+8;
        keyBoard();
      }
      else if (down == 1 && xpos<312 && xpos>257 && ypos<199 && ypos>144) {
        conf_value = conf_value*10+9;
        keyBoard();
      }
      else if (down == 1 && xpos<312 && xpos>280 && ypos<70 && ypos>46) {
        conf_value = floor(conf_value/10);
        keyBoard();
      }
      else if ((conf_number==1 || conf_number==3) && conf_value>23){
        conf_value = 23;
        keyBoard();
      }
      else if ((conf_number==2 || conf_number==4) && conf_value>59){
        conf_value = 59;
        keyBoard();
      }
      else if (down == 1 && 0<xpos && xpos<107 && 210<ypos && ypos<240) {
        conf_number--;
        if(conf_number==0){ // Return to ConfigScreen
          screens = 1; 
          floor_selected = 0;
          conf_value = 0;
          ConfigFloor();
        }
        else {  // Return to Keyboard
          if (floor_selected==1) {
            if(conf_number==1) conf_value = f1.hour_begin;
            else if(conf_number==2) conf_value = f1.minute_begin;
            else if(conf_number==3) conf_value = f1.hour_end;
          }
          else if (floor_selected==2) {
            if(conf_number==1) conf_value = f2.hour_begin;
            else if(conf_number==2) conf_value = f2.minute_begin;
            else if(conf_number==3) conf_value = f2.hour_end;
          }
          else if (floor_selected==3) {
            if(conf_number==1) conf_value = f3.hour_begin;
            else if(conf_number==2) conf_value = f3.minute_begin;
            else if(conf_number==3) conf_value = f3.hour_end;
          }
          else if (floor_selected==4) {
            if(conf_number==1) conf_value = f4.hour_begin;
            else if(conf_number==2) conf_value = f4.minute_begin;
            else if(conf_number==3) conf_value = f4.hour_end;
          }
          else if (floor_selected==5) {
            if(conf_number==1) conf_value = f5.hour_begin;
            else if(conf_number==2) conf_value = f5.minute_begin;
            else if(conf_number==3) conf_value = f5.hour_end;
          }
          else if (floor_selected==6) {
            if(conf_number==1) conf_value = f6.hour_begin;
            else if(conf_number==2) conf_value = f6.minute_begin;
            else if(conf_number==3) conf_value = f6.hour_end;
          }
          else if (floor_selected==7) {
            if(conf_number==1) conf_value = f7.hour_begin;
            else if(conf_number==2) conf_value = f7.minute_begin;
            else if(conf_number==3) conf_value = f7.hour_end;
          }
          else if (floor_selected==8) {
            if(conf_number==1) conf_value = f8.hour_begin;
            else if(conf_number==2) conf_value = f8.minute_begin;
            else if(conf_number==3) conf_value = f8.hour_end;
          }
          else conf_value = 0;
          keyBoard();
        }
      }

      else if (down == 1 && 214<xpos && xpos<320 && 210<ypos && ypos<240) {
        // Save the value
        if (floor_selected==1) {
          if(conf_number==1) f1.hour_begin = conf_value;
          else if(conf_number==2) f1.minute_begin = conf_value;
          else if(conf_number==3) f1.hour_end = conf_value;
          else if(conf_number==4) f1.minute_end = conf_value;
        }
        else if (floor_selected==2) {
          if(conf_number==1) f2.hour_begin = conf_value;
          else if(conf_number==2) f2.minute_begin = conf_value;
          else if(conf_number==3) f2.hour_end = conf_value;
          else if(conf_number==4) f2.minute_end = conf_value;
        }
        else if (floor_selected==3) {
          if(conf_number==1) f3.hour_begin = conf_value;
          else if(conf_number==2) f3.minute_begin = conf_value;
          else if(conf_number==3) f3.hour_end = conf_value;
          else if(conf_number==4) f3.minute_end = conf_value;
        }
        else if (floor_selected==4) {
          if(conf_number==1) f4.hour_begin = conf_value;
          else if(conf_number==2) f4.minute_begin = conf_value;
          else if(conf_number==3) f4.hour_end = conf_value;
          else if(conf_number==4) f4.minute_end = conf_value;
        }
        else if (floor_selected==5) {
          if(conf_number==1) f5.hour_begin = conf_value;
          else if(conf_number==2) f5.minute_begin = conf_value;
          else if(conf_number==3) f5.hour_end = conf_value;
          else if(conf_number==4) f5.minute_end = conf_value;
        }
        else if (floor_selected==6) {
          if(conf_number==1) f6.hour_begin = conf_value;
          else if(conf_number==2) f6.minute_begin = conf_value;
          else if(conf_number==3) f6.hour_end = conf_value;
          else if(conf_number==4) f6.minute_end = conf_value;
        }
        else if (floor_selected==7) {
          if(conf_number==1) f7.hour_begin = conf_value;
          else if(conf_number==2) f7.minute_begin = conf_value;
          else if(conf_number==3) f7.hour_end = conf_value;
          else if(conf_number==4) f7.minute_end = conf_value;
        }
        else if (floor_selected==8) {
          if(conf_number==1) f8.hour_begin = conf_value;
          else if(conf_number==2) f8.minute_begin = conf_value;
          else if(conf_number==3) f8.hour_end = conf_value;
          else if(conf_number==4) f8.minute_end = conf_value;
        }
        
        Serial.println(conf_value);
        conf_number++;

        // Recover next value to display
        if(conf_number<=4){
          if (floor_selected==1) {
            if(conf_number==2) conf_value = f1.minute_begin;
            else if(conf_number==3) conf_value = f1.hour_end;
            else if(conf_number==4) conf_value = f1.minute_end;
          }
          else if (floor_selected==2) {
            if(conf_number==2) conf_value = f2.minute_begin;
            else if(conf_number==3) conf_value = f2.hour_end;
            else if(conf_number==4) conf_value = f2.minute_end;
          }
          else if (floor_selected==3) {
            if(conf_number==2) conf_value = f3.minute_begin;
            else if(conf_number==3) conf_value = f3.hour_end;
            else if(conf_number==4) conf_value = f3.minute_end;
          }
          else if (floor_selected==4) {
            if(conf_number==2) conf_value = f4.minute_begin;
            else if(conf_number==3) conf_value = f4.hour_end;
            else if(conf_number==4) conf_value = f4.minute_end;
          }
          else if (floor_selected==5) {
            if(conf_number==2) conf_value = f5.minute_begin;
            else if(conf_number==3) conf_value = f5.hour_end;
            else if(conf_number==4) conf_value = f5.minute_end;
          }
          else if (floor_selected==6) {
            if(conf_number==2) conf_value = f6.minute_begin;
            else if(conf_number==3) conf_value = f6.hour_end;
            else if(conf_number==4) conf_value = f6.minute_end;
          }
          else if (floor_selected==7) {
            if(conf_number==2) conf_value = f7.minute_begin;
            else if(conf_number==3) conf_value = f7.hour_end;
            else if(conf_number==4) conf_value = f7.minute_end;
          }
          else if (floor_selected==8) {
            if(conf_number==2) conf_value = f8.minute_begin;
            else if(conf_number==3) conf_value = f8.hour_end;
            else if(conf_number==4) conf_value = f8.minute_end;
          }
          else conf_value = 0;
          keyBoard();
        }
        // Return to config screen
        else{
          conf_number = 0;
          conf_value = 0;
          updateTimer = millis();
          updateFlag = true;
          screens = 1;
          ConfigFloor();
        }
      }      
      break;    
    
    // Relay Screen
    case 3: 
      if (down == 1 && xpos>0 && xpos<107 && ypos>210 && ypos<240) {
        screens = 1;
        ConfigFloor();
      } 
      break;
 }

 if(millis() - updateTimer > 60000 && updateFlag){
    updateFlag = false;
    digitalWrite(updatePin, HIGH);
    Serial.println(F("TFT: Variables updated"));
    Serial.print(F("Floor 1 - ")); Serial.print(f1.hour_begin); Serial.print(F(":")); Serial.print(f1.minute_begin); Serial.print(F("\t")); Serial.print(f1.hour_end); Serial.print(F(":")); Serial.println(f1.minute_end);
    Serial.print(F("Floor 2 - ")); Serial.print(f2.hour_begin); Serial.print(F(":")); Serial.print(f2.minute_begin); Serial.print(F("\t")); Serial.print(f2.hour_end); Serial.print(F(":")); Serial.println(f2.minute_end);
    Serial.print(F("Floor 3 - ")); Serial.print(f3.hour_begin); Serial.print(F(":")); Serial.print(f3.minute_begin); Serial.print(F("\t")); Serial.print(f3.hour_end); Serial.print(F(":")); Serial.println(f3.minute_end);
    Serial.print(F("Floor 4 - ")); Serial.print(f4.hour_begin); Serial.print(F(":")); Serial.print(f4.minute_begin); Serial.print(F("\t")); Serial.print(f4.hour_end); Serial.print(F(":")); Serial.println(f4.minute_end);
    Serial.print(F("Floor 5 - ")); Serial.print(f5.hour_begin); Serial.print(F(":")); Serial.print(f5.minute_begin); Serial.print(F("\t")); Serial.print(f5.hour_end); Serial.print(F(":")); Serial.println(f5.minute_end);
    Serial.print(F("Floor 6 - ")); Serial.print(f6.hour_begin); Serial.print(F(":")); Serial.print(f6.minute_begin); Serial.print(F("\t")); Serial.print(f6.hour_end); Serial.print(F(":")); Serial.println(f6.minute_end);
    Serial.print(F("Floor 7 - ")); Serial.print(f7.hour_begin); Serial.print(F(":")); Serial.print(f7.minute_begin); Serial.print(F("\t")); Serial.print(f7.hour_end); Serial.print(F(":")); Serial.println(f7.minute_end);
    Serial.print(F("Floor 8 - ")); Serial.print(f8.hour_begin); Serial.print(F(":")); Serial.print(f8.minute_begin); Serial.print(F("\t")); Serial.print(f8.hour_end); Serial.print(F(":")); Serial.println(f8.minute_end);
 }
}
