/* Definitions */

// Operation Mode Constants
#define COMMAND   0           // Stops the sensor. Wait for a command. Low Power Consumption 3.5mW
#define STREAMING 1           // Sensor reports measurements twice per second
#define POLLING   2           // Sensor take measurements in background but only reports when requested

// Calibration Mode Constants
#define GAS_CONCENTRATION 0  // Used when you know the actual concentration of your gas
#define ZERO_NITROGEN     1  // Used when gas has no CO2 (usually nitrogen)
#define FRESH_AIR         2  // When no gas concentration available aassumed fresh air at 400 ppm

// Parameters request constants
#define Hum            4096  // Reports humidity
#define D_filt         2048  // Reports value related to the normalized LED signal strength (smoothed)
#define D_unfilt       1024  // Reports value related to the normalized LED signal strength
#define Zero_setPoint  256   // Reports value related to the normalized LED signal strength (smoothed)
#define Sens_Temp      128   // Reports value which varies inversely with the sensor temperature
#define Temp           64    // Reports temperature
#define LED_filt       32    // Reports a which gives indication of the LED signal strength (smoothed)
#define LED_unfilt     16    // Reports a which gives indication of the LED signal strength
#define Sens_Temp_Filt 8     // Reports value which varies inversely with the sensor temperature (smoothed)
#define CO2_filt       4     // Reports CO2 filtered
#define CO2            2     // Reports CO2

class Cozir{
  private:
    uint8_t _pinTX, _pinRX;                         // Serial communication pins
    uint16_t _OpMode, _OutMode;                     // Operation mode of the sensor
    float _temp, _hum, _co2;                        // Data variables
    
  public:      
    Cozir();
    void begin();                                   // Initialize sensor
    bool setOpMode(uint16_t opMode);                // Set Operation Mode
    bool setDataOut(uint32_t val);                  // Set Data Output format
    uint8_t autocalibration(uint8_t a, uint8_t b);  // Config autocalibration parameters
    String get_Autocalibration();                   // Get the calibration string
    bool altitude_calibration(float altitude);      // Set altitude in meters
    float get_altitude();                           // Get the altitude in meters
    void set_filter(uint8_t filter);
    uint8_t get_filter();
    float get_temperature();
    float get_humidity();
    float get_co2();
    void get_data();
    void set_background(uint16_t ppm);
    void calibrate(uint8_t _mode, uint16_t ppm);
    void recalibrate(float sens, float real);
    void close();
};

Cozir::Cozir(){
  _pinTX = pinTX;
  _pinRX = pinRX;
  _OpMode = -1;
  _OutMode = -1;
  _temp = 0;
  _hum = 0;
  _co2 = 0;
}

void Cozir:begin(){
  Serial2.begin(9600, SERIAL_8N1, _pinRX, _pinTX);
}

bool Cozir::setOpMode(uint16_t opMode){
  if(opMode>=0 and opMode<=2){
    String cmd = "K " +  opMode;
    Serial2.println(cmd); //Serial2.println("\r");
    Serial2.flush();
    inputstring = Serial2.readStringUntil(13); //read the string until we see a <CR>
    cmd = "K 0000" +  opMode;
    if(inputstring == cmd){
      _OpMode = opMode;
      Serial.print(F("Cozir: Mode set ")); Serial.println(opMode);
      inputstring = "";
      return true
    }
    else{
      Serial.println(F("Cozir: Time limit exceed without response"));
      inputstring = "";
      return false;
    }
  }
  else{ // Parameter wrong
    Serial.println(F("Cozir: Mode Parameter not recognized"));
    return false;
  }
}

bool Cozir::setDataOut(uint32_t val){
  String cmd;
  if(val<10) cmd = "M 0000" + String(val);
  else if(val<100) cmd = "M 000" + String(val);
  else if(val<1000) cmd = "M 00" + String(val);
  else if(val<10000) cmd = "M 0" + String(val);
  else if(val<100000) cmd = "M " + String(val);

  Serial2.println(cmd); //Serial2.println("\r");
  Serial2.flush();
  inputstring = Serial2.readStringUntil(13); //read the string until we see a <CR>  
  if(inputstring == cmd){
    _OutMode = val
    Serial.print(F("Cozir: Data output set ")); Serial.println(val));
    inputstring = "";
    return true;
  }
  else{
    Serial.println(F("Cozir: Data output not recognized"));
    inputstring = "";
    return false;
  }
}

uint8_t Cozir::autocalibration(uint8_t a, uint8_t b){
  String cmd;
  if(a>0 and b>0){
    cmd = "@ " + String(a) + ".0 " + String(b) + ".0";
    Serial2.println(cmd); //Serial2.println("\r");
    Serial2.flush();
    inputstring = Serial2.readStringUntil(13); //read the string until we see a <CR>  
    if(inputstring == cmd){
      Serial.println(F("Cozir: Autocalibration parameters saved"));
      inputstring = "";
      return 2;
    }
    else{
      Serial.println(F(("Cozir: Cannot saved autocalibration parameters"));
      inputstring = "";
      return 0;
    }        
  }  
  else{
    cmd = "@ 0";
    Serial2.println(cmd); //Serial2.println("\r");
    Serial2.flush();
    inputstring = Serial2.readStringUntil(13); //read the string until we see a <CR>  
    if(inputstring == cmd){
      Serial.println(F("Cozir: Disable Autocalibration"));
      inputstring = "";
      return 1; 
    }
    else{
      Serial.println(F("Cozir: Cannot disable Autocalibration"));
      inputstring = "";
      return 0; 
    }
  }
}

String Cozir:get_Autocalibration(){
  Serial2.println("@");
  Serial2.flush();
  return Serial2.readStringUntil(13);
}

bool Cozir::altitude_calibration(float altitude){
  int code = int(0.9262*altitude+8193.7); // Calibration line get it from official Documentation
        
  Serial2.println("s"); //Serial2.println("\r");
  Serial2.flush();
  inputstring = Serial2.readStringUntil(13); //read the string until we see a <CR>  
  int resp = int(inputstring.substring(3,8));
  
  if(abs(code-resp)>10):
      Serial.println(F("Cozir: Altitude difference is considerable recalibrating... "));
      String cmd = "S " + String(code);
      Serial2.println(cmd); //Serial2.println("\r");
      Serial2.flush();
      inputstring = Serial2.readStringUntil(13); //read the string until we see a <CR>  
      resp = int(inputstring.substring(3,8));
      inputstring = "";
      if(code == resp){
        Serial.println(F("Cozir: Sensor is calibrated"));
        return true;
      }
      else{
        Serial.println(F("Cozir: Sensor is not calibrated"));
        return false;
      }
  else{
    Serial.println(F("Cozir: Altitude difference is not important"));
    return true; 
  }
}

float Cozir::get_altitude(){
  
}

class Cozir:    
    // Get Actual Altitude Parameter
    def get_altitude(self):
        self.ser.write(bytes("s\r\n",'utf-8'))
        self.ser.flush()
        resp = str(self.ser.readline(),'utf-8')
        resp = int(resp[3:8])
        altitude = (resp-8193.7)/0.9262 // Calibration line get it from official Documentation
        return round(altitude,2)
    
    // Set digital filter
    def set_filter(self, filt):
        self.ser.write(bytes("A {0}\r\n".format(filt),'utf-8'))
        self.ser.flush()
        resp = str(self.ser.readline(),'utf-8')
        resp = int(resp[3:8])
        if(resp == filt):
            self.log.debug("Cozir: Filter correctly setted")
            return True
        else:
            self.log.error("Cozir: Filter is not setted")
            return False
    
    // Get digital filter
    def get_filter(self):
        self.ser.write(bytes("a\r\n",'utf-8'))
        self.ser.flush()
        resp = str(self.ser.readline(),'utf-8')
        resp = int(resp[3:8])
        return resp
    
    // Get temperature
    def temperature(self):
        self.ser.write(bytes("T\r\n",'utf-8'))
        self.ser.flush()
        resp = str(self.ser.readline(),'utf-8')
        resp = float((int(resp[3:8])-1000))/10
        return resp
    
    // Get humidity
    def humidity(self):
        self.ser.write(bytes("H\r\n",'utf-8'))
        self.ser.flush()
        resp = str(self.ser.readline(),'utf-8')
        resp = float(resp[3:8])/10
        return resp
    
    // Get CO2
    def co2(self, filter = 0):
        if(filter == 0):
            self.ser.write(bytes("Z\r\n",'utf-8'))
            self.ser.flush()
            resp = str(self.ser.readline(),'utf-8')
            resp = int(resp[3:8])
            return resp
        elif(filter == 1):
            self.ser.write(bytes("z\r\n",'utf-8'))
            self.ser.flush()
            resp = str(self.ser.readline(),'utf-8')
            resp = int(resp[3:8])
            return resp
        else:
            self.log.error("Cozir: Filter Parameter Incorrect")
            return None
    
    // Get Data Outputs setted
    def getData(self):
        self.ser.write(bytes("Q\r\n",'utf-8'))
        self.ser.flush()
        resp = str(self.ser.readline(),'utf-8')
        if(self.act_OutMode == 4164 or self.act_OutMode == 4162):
            hum = float(resp[3:8])/10
            temp = (float(resp[11:16])-1000)/10
            co2 = int(resp[19:24])
            return hum, temp, co2
        
        elif(self.act_OutMode == 4160):
            hum = float(resp[3:8])/10
            temp = (float(resp[11:16])-1000)/10
            return hum, temp
        
        elif(self.act_OutMode == 68 or self.act_OutMode == 66):
            temp = (float(resp[3:8])-1000)/10
            co2 = int(resp[11:16])
            return temp, co2
        
        elif(self.act_OutMode == 4100 or self.act_OutMode == 4098):
            hum = float(resp[3:8])/10
            co2 = int(resp[11:16])
            return hum, co2
        
        else: return resp
    
    // Set Background Concentration
    def setBackground(self, ppm):
        i = 0    
        x = int(ppm/256)
        y = ppm - x*256
          
        if(x<10): x_str = "0000{0}".format(x)
        elif(x<100): x_str = "000{0}".format(x)
        elif(x<1000): x_str = "00{0}".format(x)
        elif(x<10000): x_str = "0{0}".format(x)
        elif(x<100000): x_str = "{0}".format(x)
        
        if(y<10): y_str = "0000{0}".format(y)
        elif(y<100): y_str = "000{0}".format(y)
        elif(y<1000): y_str = "00{0}".format(y)
        elif(y<10000): y_str = "0{0}".format(y)
        elif(y<100000): y_str = "{0}".format(y)
        
        self.ser.write(bytes("P 8 {0}\r\n".format(x),'utf-8'))
        self.ser.flush()
        resp = str(self.ser.readline(),'utf-8')
        if(resp == " P 00008 {}\r\n".format(x_str)):
            i += 1
            
        self.ser.write(bytes("P 9 {0}\r\n".format(y),'utf-8'))
        self.ser.flush()
        resp = str(self.ser.readline(),'utf-8')
        if(resp == " P 00009 {}\r\n".format(y_str)):
            i += 1
        
        if(i == 2):
            self.log.info("Cozir: Background set in {}ppm".format(ppm))
            return True
        else:
            self.log.error("Cozir: Cannot set Bckground")
            return False

    // General Calibration
    def calibrate(self, mode, ppm = -1):
        // Known Gas Concentration
        if(mode == 0 and ppm > 0):
            self.ser.write(bytes("X {0}\r\n".format(ppm),'utf-8'))
            self.ser.flush()
            resp = str(self.ser.readline(),'utf-8')
            if(resp[3] != "?" and resp.startswith(" X ")):
                self.log.info("Cozir: calibrate(gasConcentration) executed correctly")
                return True
            else:
                self.log.error("Cozir: Cannot execute calibrate(gasConcentration)")
                return False
        
        // 0 CO2 (Nitrogen)
        elif(mode == 1):
            self.ser.write(bytes("U\r\n",'utf-8'))
            self.ser.flush()
            resp = str(self.ser.readline(),'utf-8')
            if(resp[3] != "?" and resp.startswith(" U ")):
                self.log.info("Cozir: calibrate(nitrogen) executed correctly")
                return True
            else:
                self.log.error("Cozir: Cannot execute calibrate(nitrogen)")
                return False
    
        // Fresh Air (400 ppm)
        elif(mode == 2):
            self.ser.write(bytes("G\r\n",'utf-8'))
            self.ser.flush()
            resp = str(self.ser.readline(),'utf-8')
            if(resp[3] != "?" and resp.startswith(" G ")):
                self.log.info("Cozir: calibrate(freshAir) executed correctly")
                return True
            else:
                self.log.error("Cozir: Cannot execute calibrate(freshAir)")
                return False
            
        else:
            self.log.error("Cozir: In calibrate() some parameter is wrong")
            return False
    
    // Calibration when you know the value reported by the sensor and the actual correct value
    def recalibrate(self, sens, real):
        if(sens>0 and sens<2000 and real>0 and real<2000):
            self.ser.write(bytes("F {0} {1}\r\n".format(sens, real),'utf-8'))
            self.ser.flush()
            resp = str(self.ser.readline(),'utf-8')
            self.log.debug(resp)
            if(resp[3] != "?" and resp.startswith(" F ")):
                self.log.info("Cozir: recalibrate() executed correctly")
                return True
            else:
                self.log.error("Cozir: Cannot execute recalibrate()")
                return False
            
        else:
            self.log.error("Cozir: In recalibrate() some parameter wrong")
            return False
    
    // Finish serial port
    def close(self):
        if self.ser.is_open: self.ser.close()
