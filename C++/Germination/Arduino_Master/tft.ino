// Begin touchscreen
void beginTFT(){
  pinMode(updatePin, OUTPUT);
  digitalWrite(updatePin, LOW);
  uint16_t ID = tft.readID();
  if (ID == 0xD3D3) ID = 0x9486; // write-only shield
  tft.begin(ID);
  tft.setRotation(Orientation);
  splashscreen();
  home_screen();
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
void home_screen(){   
  tft.fillScreen(BLACK);
  tft.drawLine(0, 80, 400, 80, YELLOW);
  tft.setCursor(60,40);
  tft.setTextSize(3);
  tft.setTextColor(WHITE);
  tft.print(F("BABY CENTER"));   

  // Indicator 1
  tft.drawCircle(50, 120, 10, RED); 
  tft.fillCircle(50, 120, 10, RED);
  tft.setTextSize(1.5);
  tft.setTextColor(WHITE);
  tft.setCursor(45,140);
  tft.print(F("F1"));
    
  // Indicator 2
  tft.drawCircle(80, 120, 10, GREEN);
  tft.fillCircle(80, 120, 10, GREEN);
  tft.setCursor(75,140);
  tft.print(F("F2")); 

  // Indicator 3
  tft.drawCircle(110, 120, 10, RED);
  tft.fillCircle(110, 120, 10, RED);
  tft.setCursor(105,140);
  tft.print(F("F3")); 

  // Indicator 4
  tft.drawCircle(140, 120, 10, GREEN);
  tft.fillCircle(140, 120, 10, GREEN);
  tft.setCursor(135,140);
  tft.print(F("F4")); 

  // Indicator 5
  tft.drawCircle(170, 120, 10, RED);
  tft.fillCircle(170, 120, 10, RED);
  tft.setCursor(165,140);
  tft.print(F("F5")); 

  // Indicator 6
  tft.drawCircle(200, 120, 10, GREEN);
  tft.fillCircle(200, 120, 10, GREEN);
  tft.setCursor(195,140);
  tft.print(F("F6")); 

  // Indicator 7
  tft.drawCircle(230, 120, 10, RED);
  tft.fillCircle(230, 120, 10, RED);
  tft.setCursor(225,140);
  tft.print(F("F7")); 

  // Indicator 8
  tft.drawCircle(260, 120, 10, GREEN);
  tft.fillCircle(260, 120, 10, GREEN);  
  tft.setCursor(255,140);
  tft.print(F("F8")); 

  // Temperature
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.setCursor(20,200);
  tft.print(F("Temp: "));

  // Humidity
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.setCursor(200,200);
  tft.print(F("Hum: "));

  // Settings button
  tft.setTextColor(GRAY);
  tft.setCursor(25, 10);
  tft.println(F("                SETTINGS"));
}

// Config Screen
void ConfigFloor() {
  tft.fillScreen(BLACK);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.drawRoundRect(3, 3, 314, 234, 6, CYAN);
  
  tft.drawRoundRect(10, 20, 150, 30, 6, CYAN);
  tft.setCursor(15, 25);
  tft.println(F("FLOOR     1")); 
  tft.drawRoundRect(10, 60, 150, 30, 6, CYAN);
  tft.setCursor(15, 65);
  tft.println(F("FLOOR     2"));
  tft.drawRoundRect(10, 100, 150, 30, 6, CYAN);
  tft.setCursor(15, 105);
  tft.println(F("FLOOR     3"));
  tft.drawRoundRect(10, 140, 150, 30, 6, CYAN);
  tft.setCursor(15, 145);
  tft.println(F("FLOOR     4"));

  tft.drawRoundRect(162, 20, 150, 30, 6, CYAN);
  tft.setCursor(167, 25);
  tft.println(F("FLOOR     5"));
  tft.drawRoundRect(162, 60, 150, 30, 6, CYAN);
  tft.setCursor(167, 65);
  tft.println(F("FLOOR     6"));
  tft.drawRoundRect(162, 100, 150, 30, 6, CYAN);
  tft.setCursor(167, 105);
  tft.println(F("FLOOR     7"));
  tft.drawRoundRect(162, 140, 150, 30, 6, CYAN);
  tft.setCursor(167, 145);
  tft.println(F("FLOOR     8"));
  
  //tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.setCursor(30, 205);
  tft.println(F(" Back"));
}

// Start Hour Screen
void hourDisplay1() {
  tft.fillScreen(BLACK);
  tft.drawRoundRect(3, 3, 314, 234, 6, CYAN);
  tft.fillRoundRect(43, 43, 314-80, 30, 6, WHITE);
  tft.drawRoundRect(43, 43, 314-80, 30, 6, CYAN);
  String txt = "Start Hour";
  tft.setCursor(floor((320-((txt.length()+1)*5*2))/2), 15);
  tft.setTextSize(2);
  tft.println(txt);
  for (int i=0;i<5;i++){
    tft.setTextColor(WHITE);
    
    tft.drawRoundRect(8+i*(tft.width()-16)/5+i,tft.height()/3,(tft.width()-16)/5-3,(tft.width()-16)/5-3,6,CYAN);
    tft.setCursor(8+i*(tft.width()-16)/5+i+floor(((tft.width()-16)/5-3)/2)-5, tft.height()/3+ floor((tft.width()-16)/5-3-16)/2);
    tft.println(i);
    
    tft.drawRoundRect(8+i*(tft.width()-16)/5+i,tft.height()/3+(tft.width()-16)/5,(tft.width()-16)/5-3,(tft.width()-16)/5-3,6,CYAN);
    tft.setCursor(8+i*(tft.width()-16)/5+i+floor(((tft.width()-16)/5-3)/2)-5, tft.height()/3 + 4*floor((tft.width()-16)/5-3-16)/2-3);
    tft.println(i+5);
  }

  tft.setTextColor(BLACK);
  tft.setCursor(floor((320-((String(hour).length()+1)*5*2))/2), 51);
  if(first_update){
    if(screens == 2) {
      hour = f1.hour_begin;
      minute = f1.minute_begin;
      hour1 = f1.hour_end;
      minute1 = f1.minute_end;
    }
    else if(screens == 6) {
      hour = f2.hour_begin;
      minute = f2.minute_begin;
      hour1 = f2.hour_end;
      minute1 = f2.minute_end;
    }
    else if(screens == 10) {
      hour = f3.hour_begin;
      minute = f3.minute_begin;
      hour1 = f3.hour_end;
      minute1 = f3.minute_end;
    }
    else if(screens == 14) {
      hour = f4.hour_begin;
      minute = f4.minute_begin;
      hour1 = f4.hour_end;
      minute1 = f4.minute_end;
    }
    else if(screens == 18) {
      hour = f5.hour_begin;
      minute = f5.minute_begin;
      hour1 = f5.hour_end;
      minute1 = f5.minute_end;
    }
    else if(screens == 22) {
      hour = f6.hour_begin;
      minute = f6.minute_begin;
      hour1 = f6.hour_end;
      minute1 = f6.minute_end;
    }
    else if(screens == 26) {
      hour = f7.hour_begin;
      minute = f7.minute_begin;
      hour1 = f7.hour_end;
      minute1 = f7.minute_end;
    }
    else if(screens == 30) {
      hour = f8.hour_begin;
      minute = f8.minute_begin;
      hour1 = f8.hour_end;
      minute1 = f8.minute_end;
    }
    first_update = false;
  }
  tft.println(hour);
  
  tft.setTextColor(WHITE);
  tft.setCursor(30, 205);
  tft.println(F(" Back        Continue"));
  
  tft.setCursor(285,51);
  tft.println(F("x"));
}

// Start Minute Screen
void minuteDisplay1() {
  tft.fillScreen(BLACK);
  tft.drawRoundRect(3, 3, 314, 234, 6, CYAN);
  tft.fillRoundRect(43, 43, 314-80, 30, 6, WHITE);
  tft.drawRoundRect(43, 43, 314-80, 30, 6, CYAN);
  String txt = "Start Minute";
  tft.setCursor(floor((320-((txt.length()+1)*5*2))/2), 15);
  tft.setTextSize(2);
  tft.println(txt);
  for (int i=0;i<5;i++){
    tft.setTextColor(WHITE);
    
    tft.drawRoundRect(8+i*(tft.width()-16)/5+i,tft.height()/3,(tft.width()-16)/5-3,(tft.width()-16)/5-3,6,CYAN);
    tft.setCursor(8+i*(tft.width()-16)/5+i+floor(((tft.width()-16)/5-3)/2)-5, tft.height()/3+ floor((tft.width()-16)/5-3-16)/2);
    tft.println(i);
    
    tft.drawRoundRect(8+i*(tft.width()-16)/5+i,tft.height()/3+(tft.width()-16)/5,(tft.width()-16)/5-3,(tft.width()-16)/5-3,6,CYAN);
    tft.setCursor(8+i*(tft.width()-16)/5+i+floor(((tft.width()-16)/5-3)/2)-5, tft.height()/3 + 4*floor((tft.width()-16)/5-3-16)/2-3);
    tft.println(i+5);
  }
  
  tft.setTextColor(BLACK);
  tft.setCursor(floor((320-((String(minute).length()+1)*5*2))/2), 51);
  tft.println(minute);

  tft.setTextColor(WHITE);
  tft.setCursor(25, 205);
  tft.println(F(" Back        Continue"));

  tft.setCursor(285,51);
  tft.println(F("x"));
}

// End hour screen
void hourDisplay2() { 
  tft.fillScreen(BLACK);
  tft.drawRoundRect(3, 3, 314, 234, 6, CYAN);
  tft.fillRoundRect(43, 43, 314-80, 30, 6, WHITE);
  tft.drawRoundRect(43, 43, 314-80, 30, 6, CYAN);
  String txt = "End Hour";
  tft.setCursor(floor((320-((txt.length()+1)*5*2))/2), 15);
  tft.setTextSize(2);
  tft.println(txt);
  for (int i=0;i<5;i++){
    tft.setTextColor(WHITE);
    
    tft.drawRoundRect(8+i*(tft.width()-16)/5+i,tft.height()/3,(tft.width()-16)/5-3,(tft.width()-16)/5-3,6,CYAN);
    tft.setCursor(8+i*(tft.width()-16)/5+i+floor(((tft.width()-16)/5-3)/2)-5, tft.height()/3+ floor((tft.width()-16)/5-3-16)/2);
    tft.println(i);
    
    tft.drawRoundRect(8+i*(tft.width()-16)/5+i,tft.height()/3+(tft.width()-16)/5,(tft.width()-16)/5-3,(tft.width()-16)/5-3,6,CYAN);
    tft.setCursor(8+i*(tft.width()-16)/5+i+floor(((tft.width()-16)/5-3)/2)-5, tft.height()/3 + 4*floor((tft.width()-16)/5-3-16)/2-3);
    tft.println(i+5);
  }

  tft.setTextColor(BLACK);
  tft.setCursor(floor((320-((String(hour1).length()+1)*5*2))/2), 51);
  tft.println(hour1);
  
  tft.setTextColor(WHITE);
  tft.setCursor(30, 205);
  tft.println(F(" Back        Continue"));
  
  tft.setCursor(285,51);
  tft.println(F("x"));
}

// End minute screen
void minuteDisplay2() {
  tft.fillScreen(BLACK);
  tft.drawRoundRect(3, 3, 314, 234, 6, CYAN);
  tft.fillRoundRect(43, 43, 314-80, 30, 6, WHITE);
  tft.drawRoundRect(43, 43, 314-80, 30, 6, CYAN);
  String txt = "End Minute";
  tft.setCursor(floor((320-((txt.length()+1)*5*2))/2), 15);
  tft.setTextSize(2);
  tft.println(txt);
  for (int i=0;i<5;i++){
    tft.setTextColor(WHITE);
    
    tft.drawRoundRect(8+i*(tft.width()-16)/5+i,tft.height()/3,(tft.width()-16)/5-3,(tft.width()-16)/5-3,6,CYAN);
    tft.setCursor(8+i*(tft.width()-16)/5+i+floor(((tft.width()-16)/5-3)/2)-5, tft.height()/3+ floor((tft.width()-16)/5-3-16)/2);
    tft.println(i);
    
    tft.drawRoundRect(8+i*(tft.width()-16)/5+i,tft.height()/3+(tft.width()-16)/5,(tft.width()-16)/5-3,(tft.width()-16)/5-3,6,CYAN);
    tft.setCursor(8+i*(tft.width()-16)/5+i+floor(((tft.width()-16)/5-3)/2)-5, tft.height()/3 + 4*floor((tft.width()-16)/5-3-16)/2-3);
    tft.println(i+5);
  }
  
  tft.setTextColor(BLACK);
  tft.setCursor(floor((320-((String(minute1).length()+1)*5*2))/2), 51);
  tft.println(minute1);

  tft.setTextColor(WHITE);
  tft.setCursor(25, 205);
  tft.println(F(" Back        Continue"));

  tft.setCursor(285,51);
  tft.println(F("x"));
}

void updateTFT(){
  bool down = Touch_getXY();
  
  switch (screens){ // Move between screens
    case 0: // If Config button is pressed
      if (down == 1 && xpos<300 && xpos>130 && ypos<70 && ypos>5) {
        ConfigFloor();
        screens = 1;
      }    
      break;
      
    case 1: // Config Floor Display
      if (down == 1 && xpos<140 && xpos>5 && ypos<50 && ypos>5) {
        Serial.println(F("TFT: Floor 1 selected"));
        screens = 2;
        first_update = true;
        hourDisplay1();
       }

      if (down == 1 && xpos<140 && xpos>5 && ypos<100 && ypos>51) {
        Serial.println(F("TFT: Floor 2 selected"));
        screens = 6;
        first_update = true; 
        hourDisplay1();
      }

      if (down == 1 && xpos<140 && xpos>5 && ypos<150 && ypos>101) {
        Serial.println(F("TFT: Floor 3 selected"));
        screens = 10; 
        first_update = true;
        hourDisplay1();
      }

      if (down == 1 && xpos<140 && xpos>5 && ypos<200 && ypos>151) {
        Serial.println(F("TFT: Floor 4 selected"));
        screens = 14; 
        first_update = true;
        hourDisplay1();
      }

      if (down == 1 && xpos<300 && xpos>141 && ypos<50 && ypos>5) {
        Serial.println(F("TFT: Floor 5 selected"));
        screens = 18;
        first_update = true;
        hourDisplay1();
      }

      if (down == 1 && xpos<300 && xpos>141 && ypos<100 && ypos>51) {
        Serial.println(F("TFT: Floor 6 selected"));
        screens = 22;
        first_update = true;
        hourDisplay1();
      }

      if (down == 1 && xpos<300 && xpos>141 && ypos<150 && ypos>101) {
        Serial.println(F("TFT: Floor 7 selected"));
        screens = 26;
        first_update = true;
        hourDisplay1();
      }

       if (down == 1 && xpos<300 && xpos>141 && ypos<200 && ypos>151) {
        Serial.println(F("TFT: Floor 8 selected"));
        screens = 30;
        first_update = true;
        hourDisplay1();
      }

      if (down == 1 && 0<xpos && xpos<107 && 210<ypos && ypos<240) {
        home_screen();
        screens = 0;
      } 
      break;

    // Floor 1
    case 2: 
      if (down == 1 && xpos<60 && xpos>5 && ypos<136 && ypos>81) {
        hour = hour*10+0;
        hourDisplay1();
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<136 && ypos>81) {
        hour = hour*10+1;
        hourDisplay1();
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<136 && ypos>81) {
        hour = hour*10+2;
        hourDisplay1();
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<136 && ypos>81) {
        hour = hour*10+3;
        hourDisplay1();
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<136 && ypos>81) {
        hour = hour*10+4;
        hourDisplay1();
      }
      if (down == 1 && xpos<60 && xpos>5 && ypos<199 && ypos>144) {
        hour = hour*10+5;
        hourDisplay1();
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<199 && ypos>144) {
        hour = hour*10+6;
        hourDisplay1();
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<199 && ypos>144) {
        hour = hour*10+7;
        hourDisplay1();
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<199 && ypos>144) {
        hour = hour*10+8;
        hourDisplay1();
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<199 && ypos>144) {
        hour = hour*10+9;
        hourDisplay1();
      }
      if (down == 1 && xpos<312 && xpos>280 && ypos<70 && ypos>46) {
        hour = floor(hour/10);
        hourDisplay1();
      }
      if (hour>24){
        hour = 24;
        hourDisplay1();
      }
      if (down == 1 && 0<xpos && xpos<107 && 210<ypos && ypos<240) {
        ConfigFloor();
        screens = 1; 
      }

      if (down == 1 && 214<xpos && xpos<320 && 210<ypos && ypos<240) {
        minuteDisplay1();
        f1.hour_begin = hour;
        hour = 0;
        Serial.println(f1.hour_begin);
        screens = 3;
      }      
      break;
   
    case 3: 
      if (down == 1 && xpos<60 && xpos>5 && ypos<136 && ypos>81) {
        minute = minute*10+0;
        minuteDisplay1();
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<136 && ypos>81) {
        minute = minute*10+1;
        minuteDisplay1();
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<136 && ypos>81) {
        minute = minute*10+2;
        minuteDisplay1();
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<136 && ypos>81) {
        minute = minute*10+3;
        minuteDisplay1();
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<136 && ypos>81) {
        minute = minute*10+4;
        minuteDisplay1();
      }
      if (down == 1 && xpos<60 && xpos>5 && ypos<199 && ypos>144) {
        minute = minute*10+5;
        minuteDisplay1();
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<199 && ypos>144) {
        minute = minute*10+6;
        minuteDisplay1();
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<199 && ypos>144) {
        minute = minute*10+7;
        minuteDisplay1();
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<199 && ypos>144) {
        minute = minute*10+8;
        minuteDisplay1();
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<199 && ypos>144) {
        minute = minute*10+9;
        minuteDisplay1();
      }
      if (down == 1 && xpos<312 && xpos>280 && ypos<70 && ypos>46) {
        minute = floor(minute/10);
        minuteDisplay1();
      }
      if (minute>60){
        minute = 59;
        minuteDisplay1();
      }
      if (down == 1 && 0<xpos && xpos<107 && 210<ypos && ypos<240) {
        hourDisplay1();
        screens = 2;
      }

      if (down == 1 && 214<xpos && xpos<320 && 210<ypos && ypos<240) {
        hourDisplay2();
        f1.minute_begin = minute;
        minute = 0;
        Serial.println(f1.minute_begin);
        screens = 4;
      }
      break;

    case 4:
      if (down == 1 && xpos<60 && xpos>5 && ypos<136 && ypos>81) {
        hour1 = hour1*10+0;
        hourDisplay2();
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<136 && ypos>81) {
        hour1 = hour1*10+1;
        hourDisplay2();
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<136 && ypos>81) {
        hour1 = hour1*10+2;
        hourDisplay2();
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<136 && ypos>81) {
        hour1 = hour1*10+3;
        hourDisplay2();
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<136 && ypos>81) {
        hour1 = hour1*10+4;
        hourDisplay2();
      }
      if (down == 1 && xpos<60 && xpos>5 && ypos<199 && ypos>144) {
        hour1 = hour1*10+5;
        hourDisplay2();
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<199 && ypos>144) {
        hour1 = hour1*10+6;
        hourDisplay2();
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<199 && ypos>144) {
        hour1 = hour1*10+7;
        hourDisplay2();
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<199 && ypos>144) {
        hour1 = hour1*10+8;
        hourDisplay2();
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<199 && ypos>144) {
        hour1 = hour1*10+9;
        hourDisplay2();
      }
      if (down == 1 && xpos<312 && xpos>280 && ypos<70 && ypos>46) {
        hour1 = floor(hour1/10);
        hourDisplay2();
      }
      if (hour1>25){
        hour1 = 24;
        hourDisplay2();
      }
      if (down == 1 && 0<xpos && xpos<107 && 210<ypos && ypos<240) {
        minuteDisplay1();
        screens = 3; 
      }

      if (down == 1 && 214<xpos && xpos<320 && 210<ypos && ypos<240) {
        minuteDisplay2();
        f1.hour_end = hour1;
        hour1 = 0;
        Serial.println(f1.hour_end);
        screens = 5;
      }      
      break;


    case 5: 
      if (down == 1 && xpos<60 && xpos>5 && ypos<136 && ypos>81) {
        minute1 = minute1*10+0;
        minuteDisplay2();
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<136 && ypos>81) {
        minute1 = minute1*10+1;
        minuteDisplay2();
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<136 && ypos>81) {
        minute1 = minute1*10+2;
        minuteDisplay2();
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<136 && ypos>81) {
        minute1 = minute1*10+3;
        minuteDisplay2();
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<136 && ypos>81) {
        minute1 = minute1*10+4;
        minuteDisplay2();
      }
      if (down == 1 && xpos<60 && xpos>5 && ypos<199 && ypos>144) {
        minute1 = minute1*10+5;
        minuteDisplay2();
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<199 && ypos>144) {
        minute1 = minute1*10+6;
        minuteDisplay2();
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<199 && ypos>144) {
        minute1 = minute1*10+7;
        minuteDisplay2();
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<199 && ypos>144) {
        minute1 = minute1*10+8;
        minuteDisplay2();
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<199 && ypos>144) {
        minute1 = minute1*10+9;
        minuteDisplay2();
      }
      if (down == 1 && xpos<312 && xpos>280 && ypos<70 && ypos>46) {
        minute1 = floor(minute1/10);
        minuteDisplay2();
      }
      if (minute1>60){
        minute1 = 59;
        minuteDisplay2();
      }
      if (down == 1 && 0<xpos && xpos<107 && 210<ypos && ypos<240) {
        hourDisplay2();
        screens = 4;
      }

      if (down == 1 && 214<xpos && xpos<320 && 210<ypos && ypos<240) {
        ConfigFloor();
        f1.minute_end = minute1;
        Serial.println(f1.minute_end);
        updateTimer = millis();
        updateFlag = true;
        screens = 1;
      }       
      break;    
    
    // Floor 2
    case 6: 
      if (down == 1 && xpos<60 && xpos>5 && ypos<136 && ypos>81) {
        hour = hour*10+0;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<136 && ypos>81) {
        hour = hour*10+1;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<136 && ypos>81) {
        hour = hour*10+2;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<136 && ypos>81) {
        hour = hour*10+3;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<136 && ypos>81) {
        hour = hour*10+4;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<60 && xpos>5 && ypos<199 && ypos>144) {
        hour = hour*10+5;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<199 && ypos>144) {
        hour = hour*10+6;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<199 && ypos>144) {
        hour = hour*10+7;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<199 && ypos>144) {
        hour = hour*10+8;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<199 && ypos>144) {
        hour = hour*10+9;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<312 && xpos>280 && ypos<70 && ypos>46) {
        hour = floor(hour/10);
        hourDisplay1();
        
      }
      if (hour>25){
        hour = 24;
        hourDisplay1();
      }
      if (down == 1 && 0<xpos && xpos<107 && 210<ypos && ypos<240) {
        ConfigFloor();
        screens = 1; 
        
      }

      if (down == 1 && 214<xpos && xpos<320 && 210<ypos && ypos<240) {
        minuteDisplay1();
        f2.hour_begin = hour;
        hour = 0;
        Serial.println(f2.hour_begin);
        screens = 7;
        
      }      
      break;
   
    case 7: 
      if (down == 1 && xpos<60 && xpos>5 && ypos<136 && ypos>81) {
        minute = minute*10+0;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<136 && ypos>81) {
        minute = minute*10+1;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<136 && ypos>81) {
        minute = minute*10+2;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<136 && ypos>81) {
        minute = minute*10+3;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<136 && ypos>81) {
        minute = minute*10+4;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<60 && xpos>5 && ypos<199 && ypos>144) {
        minute = minute*10+5;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<199 && ypos>144) {
        minute = minute*10+6;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<199 && ypos>144) {
        minute = minute*10+7;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<199 && ypos>144) {
        minute = minute*10+8;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<199 && ypos>144) {
        minute = minute*10+9;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<312 && xpos>280 && ypos<70 && ypos>46) {
        minute=floor(minute/10);
        minuteDisplay1();
        
      }
      if (minute>60){
        minute = 59;
        minuteDisplay1();
      }
      if (down == 1 && 0<xpos && xpos<107 && 210<ypos && ypos<240) {
        hourDisplay1();
        screens = 6; 
        
      }

      if (down == 1 && 214<xpos && xpos<320 && 210<ypos && ypos<240) {
        hourDisplay2();
        f2.minute_begin = minute;
        minute = 0;
        Serial.println(f2.minute_begin);
        screens = 8;
        
      }       
      break;

    case 8:
      if (down == 1 && xpos<60 && xpos>5 && ypos<136 && ypos>81) {
        hour1 = hour1*10+0;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<136 && ypos>81) {
        hour1 = hour1*10+1;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<136 && ypos>81) {
        hour1 = hour1*10+2;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<136 && ypos>81) {
        hour1 = hour1*10+3;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<136 && ypos>81) {
        hour1 = hour1*10+4;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<60 && xpos>5 && ypos<199 && ypos>144) {
        hour1 = hour1*10+5;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<199 && ypos>144) {
        hour1 = hour1*10+6;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<199 && ypos>144) {
        hour1 = hour1*10+7;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<199 && ypos>144) {
        hour1 = hour1*10+8;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<199 && ypos>144) {
        hour1 = hour1*10+9;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<312 && xpos>280 && ypos<70 && ypos>46) {
        hour1 = floor(hour1/10);
        hourDisplay2();
        
      }
      if (hour1>25){
        hour1 = 24;
        hourDisplay2();
      }
      if (down == 1 && 0<xpos && xpos<107 && 210<ypos && ypos<240) {
        minuteDisplay1();
        screens = 7; 
        
      }

      if (down == 1 && 214<xpos && xpos<320 && 210<ypos && ypos<240) {
        minuteDisplay2();
        f2.hour_end = hour1;
        hour1 = 0;
        Serial.println(f2.hour_end);
        screens = 9;
        
      }      
      break;


    case 9: 
      if (down == 1 && xpos<60 && xpos>5 && ypos<136 && ypos>81) {
        minute1 = minute1*10+0;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<136 && ypos>81) {
        minute1 = minute1*10+1;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<136 && ypos>81) {
        minute1 = minute1*10+2;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<136 && ypos>81) {
        minute1 = minute1*10+3;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<136 && ypos>81) {
        minute1 = minute1*10+4;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<60 && xpos>5 && ypos<199 && ypos>144) {
        minute1 = minute1*10+5;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<199 && ypos>144) {
        minute1 = minute1*10+6;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<199 && ypos>144) {
        minute1 = minute1*10+7;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<199 && ypos>144) {
        minute1 = minute1*10+8;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<199 && ypos>144) {
        minute1 = minute1*10+9;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<312 && xpos>280 && ypos<70 && ypos>46) {
        minute1=floor(minute1/10);
        minuteDisplay2();
        
      }
      if (minute1>60){
        minute1 = 59;
        minuteDisplay2();
      }
      if (down == 1 && 0<xpos && xpos<107 && 210<ypos && ypos<240) {
        hourDisplay2();
        screens = 4; 
        
      }

      if (down == 1 && 214<xpos && xpos<320 && 210<ypos && ypos<240) {
        ConfigFloor();
        f2.minute_end = minute1;
        minute1 = 0;
        Serial.println(f2.minute_end);
        updateTimer = millis();
        updateFlag = true;
        screens = 1;
        
      }      
      break;

    // Floor 3
    case 10: 
      if (down == 1 && xpos<60 && xpos>5 && ypos<136 && ypos>81) {
        hour = hour*10+0;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<136 && ypos>81) {
        hour = hour*10+1;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<136 && ypos>81) {
        hour = hour*10+2;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<136 && ypos>81) {
        hour = hour*10+3;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<136 && ypos>81) {
        hour = hour*10+4;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<60 && xpos>5 && ypos<199 && ypos>144) {
        hour = hour*10+5;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<199 && ypos>144) {
        hour = hour*10+6;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<199 && ypos>144) {
        hour = hour*10+7;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<199 && ypos>144) {
        hour = hour*10+8;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<199 && ypos>144) {
        hour = hour*10+9;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<312 && xpos>280 && ypos<70 && ypos>46) {
        hour=floor(hour/10);
        hourDisplay1();
        
      }
      if (hour>25){
        hour = 24;
        hourDisplay1();
      }
      if (down == 1 && 0<xpos && xpos<107 && 210<ypos && ypos<240) {
        ConfigFloor();
        screens = 1; 
        
      }

      if (down == 1 && 214<xpos && xpos<320 && 210<ypos && ypos<240) {
        minuteDisplay1();
        f3.hour_begin = hour;
        hour = 0;
        Serial.println(f3.hour_begin);
        screens = 11;
        
      }      
      break;
   
    case 11: 
      if (down == 1 && xpos<60 && xpos>5 && ypos<136 && ypos>81) {
        minute = minute*10+0;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<136 && ypos>81) {
        minute = minute*10+1;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<136 && ypos>81) {
        minute = minute*10+2;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<136 && ypos>81) {
        minute = minute*10+3;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<136 && ypos>81) {
        minute = minute*10+4;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<60 && xpos>5 && ypos<199 && ypos>144) {
        minute = minute*10+5;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<199 && ypos>144) {
        minute = minute*10+6;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<199 && ypos>144) {
        minute = minute*10+7;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<199 && ypos>144) {
        minute = minute*10+8;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<199 && ypos>144) {
        minute = minute*10+9;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<312 && xpos>280 && ypos<70 && ypos>46) {
        minute=floor(minute/10);
        minuteDisplay1();
        
      }
      if (minute>60){
        minute = 59;
        minuteDisplay1();
      }
      if (down == 1 && 0<xpos && xpos<107 && 210<ypos && ypos<240) {
        hourDisplay1();
        screens = 10; 
        
      }

      if (down == 1 && 214<xpos && xpos<320 && 210<ypos && ypos<240) {
        hourDisplay2();
        f3.minute_begin = minute;
        minute = 0;
        Serial.println(f3.minute_begin);
        screens = 12;
        
      }      
      break;

    case 12:
      if (down == 1 && xpos<60 && xpos>5 && ypos<136 && ypos>81) {
        hour1 = hour1*10+0;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<136 && ypos>81) {
        hour1 = hour1*10+1;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<136 && ypos>81) {
        hour1 = hour1*10+2;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<136 && ypos>81) {
        hour1 = hour1*10+3;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<136 && ypos>81) {
        hour1 = hour1*10+4;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<60 && xpos>5 && ypos<199 && ypos>144) {
        hour1 = hour1*10+5;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<199 && ypos>144) {
        hour1 = hour1*10+6;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<199 && ypos>144) {
        hour1 = hour1*10+7;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<199 && ypos>144) {
        hour1 = hour1*10+8;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<199 && ypos>144) {
        hour1 = hour1*10+9;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<312 && xpos>280 && ypos<70 && ypos>46) {
        hour1 = floor(hour1/10);
        hourDisplay2();
        
      }
      if (hour1>25){
        hour1 = 24;
        hourDisplay2();
      }
      if (down == 1 && 0<xpos && xpos<107 && 210<ypos && ypos<240) {
        minuteDisplay1();
        screens = 11; 
        
      }

      if (down == 1 && 214<xpos && xpos<320 && 210<ypos && ypos<240) {
        minuteDisplay2();
        f3.hour_end = hour1;
        hour1 = 0;
        Serial.println(f3.hour_end);
        screens = 13;
        
      }      
      break;


    case 13: 
      if (down == 1 && xpos<60 && xpos>5 && ypos<136 && ypos>81) {
        minute1 = minute1*10+0;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<136 && ypos>81) {
        minute1 = minute1*10+1;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<136 && ypos>81) {
        minute1 = minute1*10+2;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<136 && ypos>81) {
        minute1 = minute1*10+3;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<136 && ypos>81) {
        minute1 = minute1*10+4;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<60 && xpos>5 && ypos<199 && ypos>144) {
        minute1 = minute1*10+5;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<199 && ypos>144) {
        minute1 = minute1*10+6;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<199 && ypos>144) {
        minute1 = minute1*10+7;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<199 && ypos>144) {
        minute1 = minute1*10+8;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<199 && ypos>144) {
        minute1 = minute1*10+9;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<312 && xpos>280 && ypos<70 && ypos>46) {
        minute1=floor(minute1/10);
        minuteDisplay2();
        
      }
      if (minute1>60){
        minute1 = 59;
        minuteDisplay2();
      }
      if (down == 1 && 0<xpos && xpos<107 && 210<ypos && ypos<240) {
        hourDisplay2();
        screens = 12; 
        
      }

      if (down == 1 && 214<xpos && xpos<320 && 210<ypos && ypos<240) {
        ConfigFloor();
        f3.minute_end = minute1;
        minute1 = 0;
        Serial.println(f3.minute_end);
        updateTimer = millis();
        updateFlag = true;
        screens = 1;
        
      }             
      break;

    // Floor 4
    case 14: 
      if (down == 1 && xpos<60 && xpos>5 && ypos<136 && ypos>81) {
        hour = hour*10+0;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<136 && ypos>81) {
        hour = hour*10+1;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<136 && ypos>81) {
        hour = hour*10+2;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<136 && ypos>81) {
        hour = hour*10+3;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<136 && ypos>81) {
        hour = hour*10+4;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<60 && xpos>5 && ypos<199 && ypos>144) {
        hour = hour*10+5;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<199 && ypos>144) {
        hour = hour*10+6;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<199 && ypos>144) {
        hour = hour*10+7;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<199 && ypos>144) {
        hour = hour*10+8;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<199 && ypos>144) {
        hour = hour*10+9;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<312 && xpos>280 && ypos<70 && ypos>46) {
        hour=floor(hour/10);
        hourDisplay1();
        
      }
      if (hour>25){
        hour = 24;
        hourDisplay1();
      }
      if (down == 1 && 0<xpos && xpos<107 && 210<ypos && ypos<240) {
        ConfigFloor();
        screens = 1; 
        
      }

      if (down == 1 && 214<xpos && xpos<320 && 210<ypos && ypos<240) {
        minuteDisplay1();
        f4.hour_begin = hour;
        hour = 0;
        Serial.println(f4.hour_begin);
        screens = 15;
        
      }      
      break;
   
    case 15: 
      if (down == 1 && xpos<60 && xpos>5 && ypos<136 && ypos>81) {
        minute = minute*10+0;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<136 && ypos>81) {
        minute = minute*10+1;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<136 && ypos>81) {
        minute = minute*10+2;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<136 && ypos>81) {
        minute = minute*10+3;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<136 && ypos>81) {
        minute = minute*10+4;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<60 && xpos>5 && ypos<199 && ypos>144) {
        minute = minute*10+5;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<199 && ypos>144) {
        minute = minute*10+6;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<199 && ypos>144) {
        minute = minute*10+7;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<199 && ypos>144) {
        minute = minute*10+8;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<199 && ypos>144) {
        minute = minute*10+9;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<312 && xpos>280 && ypos<70 && ypos>46) {
        minute=floor(minute/10);
        minuteDisplay1();
        
      }
      if (minute>60){
        minute = 59;
        minuteDisplay1();
      }
      if (down == 1 && 0<xpos && xpos<107 && 210<ypos && ypos<240) {
        hourDisplay1();
        screens = 14; 
        
      }

      if (down == 1 && 214<xpos && xpos<320 && 210<ypos && ypos<240) {
        hourDisplay2();
        f4.minute_begin = minute;
        minute = 0;
        Serial.println(f4.minute_begin);
        screens = 16;
        
      }      
      break;

    case 16:
      if (down == 1 && xpos<60 && xpos>5 && ypos<136 && ypos>81) {
        hour1 = hour1*10+0;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<136 && ypos>81) {
        hour1 = hour1*10+1;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<136 && ypos>81) {
        hour1 = hour1*10+2;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<136 && ypos>81) {
        hour1 = hour1*10+3;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<136 && ypos>81) {
        hour1 = hour1*10+4;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<60 && xpos>5 && ypos<199 && ypos>144) {
        hour1 = hour1*10+5;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<199 && ypos>144) {
        hour1 = hour1*10+6;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<199 && ypos>144) {
        hour1 = hour1*10+7;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<199 && ypos>144) {
        hour1 = hour1*10+8;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<199 && ypos>144) {
        hour1 = hour1*10+9;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<312 && xpos>280 && ypos<70 && ypos>46) {
        hour1 = floor(hour1/10);
        hourDisplay2();
        
      }
      if (hour1>25){
        hour1 = 24;
        hourDisplay2();
      }
      if (down == 1 && 0<xpos && xpos<107 && 210<ypos && ypos<240) {
        minuteDisplay1();
        screens = 15; 
        
      }

      if (down == 1 && 214<xpos && xpos<320 && 210<ypos && ypos<240) {
        minuteDisplay2();
        f4.hour_end = hour1;
        hour1 = 0;
        Serial.println(f4.hour_end);
        screens = 17;
        
      }      
      break;


    case 17: 
      if (down == 1 && xpos<60 && xpos>5 && ypos<136 && ypos>81) {
        minute1 = minute1*10+0;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<136 && ypos>81) {
        minute1 = minute1*10+1;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<136 && ypos>81) {
        minute1 = minute1*10+2;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<136 && ypos>81) {
        minute1 = minute1*10+3;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<136 && ypos>81) {
        minute1 = minute1*10+4;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<60 && xpos>5 && ypos<199 && ypos>144) {
        minute1 = minute1*10+5;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<199 && ypos>144) {
        minute1 = minute1*10+6;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<199 && ypos>144) {
        minute1 = minute1*10+7;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<199 && ypos>144) {
        minute1 = minute1*10+8;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<199 && ypos>144) {
        minute1 = minute1*10+9;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<312 && xpos>280 && ypos<70 && ypos>46) {
        minute1=floor(minute1/10);
        minuteDisplay2();
        
      }
      if (minute1>60){
        minute1 = 59;
        minuteDisplay2();
      }
      if (down == 1 && 0<xpos && xpos<107 && 210<ypos && ypos<240) {
        hourDisplay2();
        screens = 16; 
        
      }

      if (down == 1 && 214<xpos && xpos<320 && 210<ypos && ypos<240) {
        ConfigFloor();
        f4.minute_end = minute1;
        minute1 = 0;
        Serial.println(f4.minute_end);
        updateTimer = millis();
        updateFlag = true;
        screens = 1;
        
      }      
      break;

    // Floor 5
    case 18: 
      if (down == 1 && xpos<60 && xpos>5 && ypos<136 && ypos>81) {
        hour = hour*10+0;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<136 && ypos>81) {
        hour = hour*10+1;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<136 && ypos>81) {
        hour = hour*10+2;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<136 && ypos>81) {
        hour = hour*10+3;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<136 && ypos>81) {
        hour = hour*10+4;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<60 && xpos>5 && ypos<199 && ypos>144) {
        hour = hour*10+5;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<199 && ypos>144) {
        hour = hour*10+6;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<199 && ypos>144) {
        hour = hour*10+7;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<199 && ypos>144) {
        hour = hour*10+8;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<199 && ypos>144) {
        hour = hour*10+9;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<312 && xpos>280 && ypos<70 && ypos>46) {
        hour=floor(hour/10);
        hourDisplay1();
        
      }
      if (hour>25){
        hour = 24;
        hourDisplay1();
      }
      if (down == 1 && 0<xpos && xpos<107 && 210<ypos && ypos<240) {
        ConfigFloor();
        screens = 1; 
        
      }

      if (down == 1 && 214<xpos && xpos<320 && 210<ypos && ypos<240) {
        minuteDisplay1();
        f5.hour_begin = hour;
        hour = 0;
        Serial.println(f5.hour_begin);
        screens = 19;
        
      }      
      break;
   
    case 19: 
      if (down == 1 && xpos<60 && xpos>5 && ypos<136 && ypos>81) {
        minute = minute*10+0;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<136 && ypos>81) {
        minute = minute*10+1;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<136 && ypos>81) {
        minute = minute*10+2;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<136 && ypos>81) {
        minute = minute*10+3;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<136 && ypos>81) {
        minute = minute*10+4;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<60 && xpos>5 && ypos<199 && ypos>144) {
        minute = minute*10+5;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<199 && ypos>144) {
        minute = minute*10+6;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<199 && ypos>144) {
        minute = minute*10+7;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<199 && ypos>144) {
        minute = minute*10+8;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<199 && ypos>144) {
        minute = minute*10+9;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<312 && xpos>280 && ypos<70 && ypos>46) {
        minute=floor(minute/10);
        minuteDisplay1();
        
      }
      if (minute>60){
        minute = 59;
        minuteDisplay1();
      }
      if (down == 1 && 0<xpos && xpos<107 && 210<ypos && ypos<240) {
        hourDisplay1();
        screens = 18; 
        
      }

      if (down == 1 && 214<xpos && xpos<320 && 210<ypos && ypos<240) {
        hourDisplay2();
        f5.minute_begin = minute;
        minute = 0;
        Serial.println(f5.minute_begin);
        screens = 20;
        
      }      
      break;

    case 20:
      if (down == 1 && xpos<60 && xpos>5 && ypos<136 && ypos>81) {
        hour1 = hour1*10+0;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<136 && ypos>81) {
        hour1 = hour1*10+1;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<136 && ypos>81) {
        hour1 = hour1*10+2;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<136 && ypos>81) {
        hour1 = hour1*10+3;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<136 && ypos>81) {
        hour1 = hour1*10+4;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<60 && xpos>5 && ypos<199 && ypos>144) {
        hour1 = hour1*10+5;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<199 && ypos>144) {
        hour1 = hour1*10+6;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<199 && ypos>144) {
        hour1 = hour1*10+7;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<199 && ypos>144) {
        hour1 = hour1*10+8;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<199 && ypos>144) {
        hour1 = hour1*10+9;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<312 && xpos>280 && ypos<70 && ypos>46) {
        hour1 = floor(hour1/10);
        hourDisplay2();
        
      }
      if (hour1>25){
        hour1 = 24;
        hourDisplay2();
      }
      if (down == 1 && 0<xpos && xpos<107 && 210<ypos && ypos<240) {
        minuteDisplay1();
        screens = 19; 
        
      }

      if (down == 1 && 214<xpos && xpos<320 && 210<ypos && ypos<240) {
        minuteDisplay2();
        f5.hour_end = hour1;
        hour1 = 0;
        Serial.println(f5.hour_end);
        screens = 21;
        
      }      
      break;


    case 21: 
      if (down == 1 && xpos<60 && xpos>5 && ypos<136 && ypos>81) {
        minute1 = minute1*10+0;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<136 && ypos>81) {
        minute1 = minute1*10+1;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<136 && ypos>81) {
        minute1 = minute1*10+2;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<136 && ypos>81) {
        minute1 = minute1*10+3;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<136 && ypos>81) {
        minute1 = minute1*10+4;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<60 && xpos>5 && ypos<199 && ypos>144) {
        minute1 = minute1*10+5;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<199 && ypos>144) {
        minute1 = minute1*10+6;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<199 && ypos>144) {
        minute1 = minute1*10+7;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<199 && ypos>144) {
        minute1 = minute1*10+8;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<199 && ypos>144) {
        minute1 = minute1*10+9;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<312 && xpos>280 && ypos<70 && ypos>46) {
        minute1=floor(minute1/10);
        minuteDisplay2();
        
      }
      if (minute1>60){
        minute1 = 59;
        minuteDisplay2();
      }
      if (down == 1 && 0<xpos && xpos<107 && 210<ypos && ypos<240) {
        hourDisplay2();
        screens = 20; 
        
      }

      if (down == 1 && 214<xpos && xpos<320 && 210<ypos && ypos<240) {
        ConfigFloor();
        f5.minute_end = minute1;
        minute1 = 0;
        Serial.println(f5.minute_end);
        updateTimer = millis();
        updateFlag = true;
        screens = 1;
        
      }         
      break;

    // Floor 6
    case 22: 
      if (down == 1 && xpos<60 && xpos>5 && ypos<136 && ypos>81) {
        hour = hour*10+0;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<136 && ypos>81) {
        hour = hour*10+1;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<136 && ypos>81) {
        hour = hour*10+2;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<136 && ypos>81) {
        hour = hour*10+3;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<136 && ypos>81) {
        hour = hour*10+4;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<60 && xpos>5 && ypos<199 && ypos>144) {
        hour = hour*10+5;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<199 && ypos>144) {
        hour = hour*10+6;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<199 && ypos>144) {
        hour = hour*10+7;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<199 && ypos>144) {
        hour = hour*10+8;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<199 && ypos>144) {
        hour = hour*10+9;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<312 && xpos>280 && ypos<70 && ypos>46) {
        hour=floor(hour/10);
        hourDisplay1();
        
      }
      if (hour>25){
        hour = 24;
        hourDisplay1();
      }
      if (down == 1 && 0<xpos && xpos<107 && 210<ypos && ypos<240) {
        ConfigFloor();
        screens = 1; 
        
      }

      if (down == 1 && 214<xpos && xpos<320 && 210<ypos && ypos<240) {
        minuteDisplay1();
        f6.hour_begin = hour;
        hour = 0;
        Serial.println(f6.hour_begin);
        screens = 23;
        
      }      
      break;
   
    case 23: 
      if (down == 1 && xpos<60 && xpos>5 && ypos<136 && ypos>81) {
        minute = minute*10+0;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<136 && ypos>81) {
        minute = minute*10+1;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<136 && ypos>81) {
        minute = minute*10+2;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<136 && ypos>81) {
        minute = minute*10+3;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<136 && ypos>81) {
        minute = minute*10+4;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<60 && xpos>5 && ypos<199 && ypos>144) {
        minute = minute*10+5;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<199 && ypos>144) {
        minute = minute*10+6;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<199 && ypos>144) {
        minute = minute*10+7;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<199 && ypos>144) {
        minute = minute*10+8;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<199 && ypos>144) {
        minute = minute*10+9;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<312 && xpos>280 && ypos<70 && ypos>46) {
        minute=floor(minute/10);
        minuteDisplay1();
        
      }
      if (minute>60){
        minute = 59;
        minuteDisplay1();
      }
      if (down == 1 && 0<xpos && xpos<107 && 210<ypos && ypos<240) {
        hourDisplay1();
        screens = 22; 
        
      }

      if (down == 1 && 214<xpos && xpos<320 && 210<ypos && ypos<240) {
        hourDisplay2();
        f6.minute_begin = minute;
        minute = 0;
        Serial.println(f6.minute_begin);
        screens = 24;
        
      }      
      break;

    case 24:
      if (down == 1 && xpos<60 && xpos>5 && ypos<136 && ypos>81) {
        hour1 = hour1*10+0;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<136 && ypos>81) {
        hour1 = hour1*10+1;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<136 && ypos>81) {
        hour1 = hour1*10+2;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<136 && ypos>81) {
        hour1 = hour1*10+3;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<136 && ypos>81) {
        hour1 = hour1*10+4;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<60 && xpos>5 && ypos<199 && ypos>144) {
        hour1 = hour1*10+5;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<199 && ypos>144) {
        hour1 = hour1*10+6;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<199 && ypos>144) {
        hour1 = hour1*10+7;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<199 && ypos>144) {
        hour1 = hour1*10+8;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<199 && ypos>144) {
        hour1 = hour1*10+9;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<312 && xpos>280 && ypos<70 && ypos>46) {
        hour1 = floor(hour1/10);
        hourDisplay2();
        
      }
      if (hour1>25){
        hour1 = 24;
        hourDisplay2();
      }
      if (down == 1 && 0<xpos && xpos<107 && 210<ypos && ypos<240) {
        minuteDisplay1();
        screens = 23; 
        
      }

      if (down == 1 && 214<xpos && xpos<320 && 210<ypos && ypos<240) {
        minuteDisplay2();
        f6.hour_end = hour1;
        hour1 = 0;
        Serial.println(f6.hour_end);
        screens = 25;
        
      }      
      break;


    case 25: 
      if (down == 1 && xpos<60 && xpos>5 && ypos<136 && ypos>81) {
        minute1 = minute1*10+0;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<136 && ypos>81) {
        minute1 = minute1*10+1;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<136 && ypos>81) {
        minute1 = minute1*10+2;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<136 && ypos>81) {
        minute1 = minute1*10+3;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<136 && ypos>81) {
        minute1 = minute1*10+4;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<60 && xpos>5 && ypos<199 && ypos>144) {
        minute1 = minute1*10+5;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<199 && ypos>144) {
        minute1 = minute1*10+6;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<199 && ypos>144) {
        minute1 = minute1*10+7;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<199 && ypos>144) {
        minute1 = minute1*10+8;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<199 && ypos>144) {
        minute1 = minute1*10+9;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<312 && xpos>280 && ypos<70 && ypos>46) {
        minute1=floor(minute1/10);
        minuteDisplay2();
        
      }
      if (minute1>60){
        minute1 = 59;
        minuteDisplay2();
      }
      if (down == 1 && 0<xpos && xpos<107 && 210<ypos && ypos<240) {
        hourDisplay2();
        screens = 24; 
        
      }

      if (down == 1 && 214<xpos && xpos<320 && 210<ypos && ypos<240) {
        ConfigFloor();
        f6.minute_end = minute1;
        minute1 = 0;
        Serial.println(f6.minute_end);
        updateTimer = millis();
        updateFlag = true;
        screens = 1;
        
      }      
      break;

    // Floor 7
    case 26: 
      if (down == 1 && xpos<60 && xpos>5 && ypos<136 && ypos>81) {
        hour = hour*10+0;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<136 && ypos>81) {
        hour = hour*10+1;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<136 && ypos>81) {
        hour = hour*10+2;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<136 && ypos>81) {
        hour = hour*10+3;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<136 && ypos>81) {
        hour = hour*10+4;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<60 && xpos>5 && ypos<199 && ypos>144) {
        hour = hour*10+5;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<199 && ypos>144) {
        hour = hour*10+6;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<199 && ypos>144) {
        hour = hour*10+7;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<199 && ypos>144) {
        hour = hour*10+8;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<199 && ypos>144) {
        hour = hour*10+9;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<312 && xpos>280 && ypos<70 && ypos>46) {
        hour=floor(hour/10);
        hourDisplay1();
        
      }
      if (hour>25){
        hour = 24;
        hourDisplay1();
      }
      if (down == 1 && 0<xpos && xpos<107 && 210<ypos && ypos<240) {
        ConfigFloor();
        screens = 1; 
        
      }

      if (down == 1 && 214<xpos && xpos<320 && 210<ypos && ypos<240) {
        minuteDisplay1();
        f7.hour_begin = hour;
        hour = 0;
        Serial.println(f7.hour_begin);
        screens = 27;
        
      }      
      break;
   
    case 27: 
      if (down == 1 && xpos<60 && xpos>5 && ypos<136 && ypos>81) {
        minute = minute*10+0;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<136 && ypos>81) {
        minute = minute*10+1;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<136 && ypos>81) {
        minute = minute*10+2;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<136 && ypos>81) {
        minute = minute*10+3;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<136 && ypos>81) {
        minute = minute*10+4;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<60 && xpos>5 && ypos<199 && ypos>144) {
        minute = minute*10+5;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<199 && ypos>144) {
        minute = minute*10+6;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<199 && ypos>144) {
        minute = minute*10+7;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<199 && ypos>144) {
        minute = minute*10+8;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<199 && ypos>144) {
        minute = minute*10+9;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<312 && xpos>280 && ypos<70 && ypos>46) {
        minute=floor(minute/10);
        minuteDisplay1();
        
      }
      if (minute>60){
        minute = 59;
        minuteDisplay1();
      }
      if (down == 1 && 0<xpos && xpos<107 && 210<ypos && ypos<240) {
        hourDisplay1();
        screens = 26; 
        
      }

      if (down == 1 && 214<xpos && xpos<320 && 210<ypos && ypos<240) {
        hourDisplay2();
        f7.minute_begin = minute;
        minute = 0;
        Serial.println(f7.minute_begin);
        screens = 28;
        
      }      
      break;

    case 28:
      if (down == 1 && xpos<60 && xpos>5 && ypos<136 && ypos>81) {
        hour1 = hour1*10+0;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<136 && ypos>81) {
        hour1 = hour1*10+1;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<136 && ypos>81) {
        hour1 = hour1*10+2;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<136 && ypos>81) {
        hour1 = hour1*10+3;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<136 && ypos>81) {
        hour1 = hour1*10+4;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<60 && xpos>5 && ypos<199 && ypos>144) {
        hour1 = hour1*10+5;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<199 && ypos>144) {
        hour1 = hour1*10+6;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<199 && ypos>144) {
        hour1 = hour1*10+7;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<199 && ypos>144) {
        hour1 = hour1*10+8;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<199 && ypos>144) {
        hour1 = hour1*10+9;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<312 && xpos>280 && ypos<70 && ypos>46) {
        hour1 = floor(hour1/10);
        hourDisplay2();
        
      }
      if (hour1>25){
        hour1 = 24;
        hourDisplay2();
      }
      if (down == 1 && 0<xpos && xpos<107 && 210<ypos && ypos<240) {
        minuteDisplay1();
        screens = 27; 
        
      }

      if (down == 1 && 214<xpos && xpos<320 && 210<ypos && ypos<240) {
        minuteDisplay2();
        f7.hour_end = hour1;
        hour1 = 0;
        Serial.println(f7.hour_end);
        screens = 29;
        
      }      
      break;


    case 29: 
      if (down == 1 && xpos<60 && xpos>5 && ypos<136 && ypos>81) {
        minute1 = minute1*10+0;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<136 && ypos>81) {
        minute1 = minute1*10+1;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<136 && ypos>81) {
        minute1 = minute1*10+2;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<136 && ypos>81) {
        minute1 = minute1*10+3;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<136 && ypos>81) {
        minute1 = minute1*10+4;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<60 && xpos>5 && ypos<199 && ypos>144) {
        minute1 = minute1*10+5;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<199 && ypos>144) {
        minute1 = minute1*10+6;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<199 && ypos>144) {
        minute1 = minute1*10+7;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<199 && ypos>144) {
        minute1 = minute1*10+8;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<199 && ypos>144) {
        minute1 = minute1*10+9;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<312 && xpos>280 && ypos<70 && ypos>46) {
        minute1=floor(minute1/10);
        minuteDisplay2();
        
      }
      if (minute1>60){
        minute1 = 59;
        minuteDisplay2();
      }
      if (down == 1 && 0<xpos && xpos<107 && 210<ypos && ypos<240) {
        hourDisplay2();
        screens = 28; 
        
      }

      if (down == 1 && 214<xpos && xpos<320 && 210<ypos && ypos<240) {
        ConfigFloor();
        f7.minute_end = minute1;
        minute1 = 0;
        Serial.println(f7.minute_end);
        updateTimer = millis();
        updateFlag = true;
        screens = 1;
        
      }      
      break;

    // Floor 8
    case 30: 
      if (down == 1 && xpos<60 && xpos>5 && ypos<136 && ypos>81) {
        hour = hour*10+0;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<136 && ypos>81) {
        hour = hour*10+1;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<136 && ypos>81) {
        hour = hour*10+2;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<136 && ypos>81) {
        hour = hour*10+3;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<136 && ypos>81) {
        hour = hour*10+4;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<60 && xpos>5 && ypos<199 && ypos>144) {
        hour = hour*10+5;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<199 && ypos>144) {
        hour = hour*10+6;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<199 && ypos>144) {
        hour = hour*10+7;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<199 && ypos>144) {
        hour = hour*10+8;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<199 && ypos>144) {
        hour = hour*10+9;
        hourDisplay1();
        
      }
      if (down == 1 && xpos<312 && xpos>280 && ypos<70 && ypos>46) {
        hour=floor(hour/10);
        hourDisplay1();
        
      }
      if (hour>25){
        hour = 24;
        hourDisplay1();
      }
      if (down == 1 && 0<xpos && xpos<107 && 210<ypos && ypos<240) {
        ConfigFloor();
        screens = 1; 
        
      }

      if (down == 1 && 214<xpos && xpos<320 && 210<ypos && ypos<240) {
        minuteDisplay1();
        f8.hour_begin = hour;
        hour = 0;
        Serial.println(f8.hour_begin);
        screens = 31;
        
      }      
      break;
   
    case 31: 
      if (down == 1 && xpos<60 && xpos>5 && ypos<136 && ypos>81) {
        minute = minute*10+0;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<136 && ypos>81) {
        minute = minute*10+1;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<136 && ypos>81) {
        minute = minute*10+2;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<136 && ypos>81) {
        minute = minute*10+3;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<136 && ypos>81) {
        minute = minute*10+4;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<60 && xpos>5 && ypos<199 && ypos>144) {
        minute = minute*10+5;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<199 && ypos>144) {
        minute = minute*10+6;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<199 && ypos>144) {
        minute = minute*10+7;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<199 && ypos>144) {
        minute = minute*10+8;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<199 && ypos>144) {
        minute = minute*10+9;
        minuteDisplay1();
        
      }
      if (down == 1 && xpos<312 && xpos>280 && ypos<70 && ypos>46) {
        minute=floor(minute/10);
        minuteDisplay1();
        
      }
      if (minute>60){
        minute = 59;
        minuteDisplay1();
      }
      if (down == 1 && 0<xpos && xpos<107 && 210<ypos && ypos<240) {
        hourDisplay1();
        screens = 30; 
        
      }

      if (down == 1 && 214<xpos && xpos<320 && 210<ypos && ypos<240) {
        hourDisplay2();
        f8.minute_begin = minute;
        minute = 0;
        Serial.println(f8.minute_begin);
        screens = 32;
        
      }      
      break;

    case 32:
      if (down == 1 && xpos<60 && xpos>5 && ypos<136 && ypos>81) {
        hour1 = hour1*10+0;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<136 && ypos>81) {
        hour1 = hour1*10+1;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<136 && ypos>81) {
        hour1 = hour1*10+2;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<136 && ypos>81) {
        hour1 = hour1*10+3;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<136 && ypos>81) {
        hour1 = hour1*10+4;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<60 && xpos>5 && ypos<199 && ypos>144) {
        hour1 = hour1*10+5;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<199 && ypos>144) {
        hour1 = hour1*10+6;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<199 && ypos>144) {
        hour1 = hour1*10+7;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<199 && ypos>144) {
        hour1 = hour1*10+8;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<199 && ypos>144) {
        hour1 = hour1*10+9;
        hourDisplay2();
        
      }
      if (down == 1 && xpos<312 && xpos>280 && ypos<70 && ypos>46) {
        hour1 = floor(hour1/10);
        hourDisplay2();
        
      }
      if (hour1>25){
        hour1 = 24;
        hourDisplay2();
      }
      if (down == 1 && 0<xpos && xpos<107 && 210<ypos && ypos<240) {
        minuteDisplay1();
        screens = 31; 
        
      }

      if (down == 1 && 214<xpos && xpos<320 && 210<ypos && ypos<240) {
        minuteDisplay2();
        f8.hour_end = hour1;
        hour1 = 0;
        Serial.println(f8.hour_end);
        screens = 33;
        
      }      
      break;


    case 33: 
      if (down == 1 && xpos<60 && xpos>5 && ypos<136 && ypos>81) {
        minute1 = minute1*10+0;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<136 && ypos>81) {
        minute1 = minute1*10+1;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<136 && ypos>81) {
        minute1 = minute1*10+2;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<136 && ypos>81) {
        minute1 = minute1*10+3;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<136 && ypos>81) {
        minute1 = minute1*10+4;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<60 && xpos>5 && ypos<199 && ypos>144) {
        minute1 = minute1*10+5;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<123 && xpos>68 && ypos<199 && ypos>144) {
        minute1 = minute1*10+6;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<186 && xpos>131 && ypos<199 && ypos>144) {
        minute1 = minute1*10+7;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<249 && xpos>194 && ypos<199 && ypos>144) {
        minute1 = minute1*10+8;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<312 && xpos>257 && ypos<199 && ypos>144) {
        minute1 = minute1*10+9;
        minuteDisplay2();
        
      }
      if (down == 1 && xpos<312 && xpos>280 && ypos<70 && ypos>46) {
        minute1=floor(minute1/10);
        minuteDisplay2();
        
      }
      if (minute1>60){
        minute1 = 59;
        minuteDisplay2();
      }
      if (down == 1 && 0<xpos && xpos<107 && 210<ypos && ypos<240) {
        hourDisplay2();
        screens = 32; 
        
      }

      if (down == 1 && 214<xpos && xpos<320 && 210<ypos && ypos<240) {
        ConfigFloor();
        f8.minute_end = minute1;
        minute1 = 0;
        Serial.println(f8.minute_end);
        updateTimer = millis();
        updateFlag = true;
        screens = 1;
        
      }      
      break;      
 }

 if(millis() - updateTimer > 300000 && updateFlag){
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
