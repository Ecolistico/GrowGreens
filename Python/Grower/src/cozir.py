from serial import Serial

"""
Functions resume:
    * Cozir()- Constructor for the class
    * opMode(mode) - set Operation Mode
    * setData_output() - set data output format
    * autocalibration() - set autocalibration parameters
    * get_Autocalibration() - get autocalibration parameters
    * altitude_calibration() - calibrate sensor with altitude
    * get_altitude() - returns altitude saved in sensor
    * set_filter() - set filter parameter
    * get_filter() - get filter parameter
    * temperature() - returns temperature
    * humidity() - returns humidity
    * co2() - returns co2
    * getData() - returns data setted in setData_Output
    * setBackground() - set background minimun co2 ppm
    * calibrate() - calibrate with one of the 3 modes pre-stablished
    * recalibrate() - calibrate knowing the actual co2 concentration
"""

class Cozir:
    
    # Operation Mode Constants
    command   = 0  # Stops the sensor. Wait for a command. Low Power Consumption 3.5mW
    streaming = 1  # Sensor reports measurements twice per second
    polling   = 2  # Sensor take measurements in background but only reports when requested
    
    # Calibration Mode Constants
    gas_concentration = 0  # Used when you know the actual concentration of your gas
    zero_nitrogen     = 1  # Used when gas has no CO2 (usually nitrogen)
    fresh_air         = 2  # When no gas concentration available aassumed fresh air at 400 ppm
    
    # Parameters request constants
    Hum            = 4096  # Reports humidity
    D_filt         = 2048  # Reports value related to the normalized LED signal strength (smoothed)
    D_unfilt       = 1024  # Reports value related to the normalized LED signal strength
    Zero_setPoint  = 256   # Reports value related to the normalized LED signal strength (smoothed)
    Sens_Temp      = 128   # Reports value which varies inversely with the sensor temperature
    Temp           = 64    # Reports temperature
    LED_filt       = 32    # Reports a which gives indication of the LED signal strength (smoothed)
    LED_unfilt     = 16    # Reports a which gives indication of the LED signal strength
    Sens_Temp_Filt = 8     # Reports value which varies inversely with the sensor temperature (smoothed)
    CO2_filt       = 4     # Reports CO2 filtered
    CO2            = 2     # Reports CO2
    
    # Actual Mode
    act_OpMode = -1
    act_OutMode = -1
    
    # Constructor
    def __init__(self, logger, port = "/dev/ttyS0", TIME = 1):
        self.ser = Serial(port, baudrate = 9600, timeout = TIME)
        self.log = logger
        
    # Set Operation Mode
    def opMode(self, mode):
        if(mode>=0 and mode<=2):
            self.ser.write(bytes("K {0}\r\n".format(mode),'utf-8'))
            self.ser.flush()
            resp = str(self.ser.readline(),'utf-8')
            
            if(resp == " K 0000{0}\r\n".format(mode)):
                self.act_OpMode = mode
                return True
            else: return False
            
        else: # Parameter wrong
            self.log.error("Cozir: Mode Parameter not recognized")
            return False
    
    # Request info
    def setData_output(self, val):
        if(val<10): str_comp = "0000{0}".format(val)
        elif(val<100): str_comp = "000{0}".format(val)
        elif(val<1000): str_comp = "00{0}".format(val)
        elif(val<10000): str_comp = "0{0}".format(val)
        elif(val<100000): str_comp = "{0}".format(val)
        
        self.ser.write(bytes("M {0}\r\n".format(val),'utf-8'))
        self.ser.flush()
        resp = str(self.ser.readline(),'utf-8')
        
        if(resp == " M "+ str_comp +"\r\n"):
            self.act_OutMode = val
            return True
        else: return False
    
    # Autocalibration
    def autocalibration(self, a, b):
        if(a>0 and b>0):
            self.ser.write(bytes("@ {0}.0 {1}.0\r\n".format(a, b),'utf-8'))
            self.ser.flush()
            if(str(self.ser.readline(),'utf-8') == " @ {0}.0 {1}.0\r\n".format(a, b)):
                self.log.info("Cozir: Autocalibration parameters saved")
                return 2
            else:
                self.log.error("Cozir: Cannot saved autocalibration parameters")
                return 0
        
        else:
            self.ser.write(bytes("@ 0\r\n",'utf-8'))
            self.ser.flush()
            if(str(self.ser.readline(),'utf-8') == " @ 0\r\n"):
                self.log.info("Cozir: Disable Autocalibration")
                return 1
            else:
                self.log.error("Cozir: Cannot disable Autocalibration")
                return 0
    
    # Get Autocalibration setting
    def get_Autocalibration(self):
        self.ser.write(bytes("@\r\n",'utf-8'))
        self.ser.flush()
        resp = str(self.ser.readline(),'utf-8')
        return resp
    
    # Pressure Comprensantion
    def altitude_calibration(self, altitude): # Altitude in meters
        code = int(0.9262*altitude+8193.7) # Calibration line get it from official Documentation
        
        self.ser.write(bytes("s\r\n",'utf-8'))
        self.ser.flush()
        resp = str(self.ser.readline(),'utf-8')
        resp = int(resp[3:8])
        
        if(abs(code-resp)>10):
            self.log.info("Cozir: Altitude difference is considerable recalibrating... ")
            self.ser.write(bytes("S {0}\r\n".format(code),'utf-8'))
            self.ser.flush()
            resp = str(self.ser.readline(),'utf-8')
            resp = int(resp[3:8])
            if(code == resp):
                self.log.info("Cozir: Sensor is calibrated")
                return True
            else:
                self.log.warning("Cozir: Sensor is not calibrated")
                return False
            
        else:
            self.log.info("Cozir: Altitude difference is not important")
            return True
    
    # Get Actual Altitude Parameter
    def get_altitude(self):
        self.ser.write(bytes("s\r\n",'utf-8'))
        self.ser.flush()
        resp = str(self.ser.readline(),'utf-8')
        resp = int(resp[3:8])
        altitude = (resp-8193.7)/0.9262 # Calibration line get it from official Documentation
        return round(altitude,2)
    
    # Set digital filter
    def set_filter(self, filt):
        self.ser.write(bytes("A {0}\r\n".format(filt),'utf-8'))
        self.ser.flush()
        resp = str(self.ser.readline(),'utf-8')
        resp = int(resp[3:8])
        if(resp == filt):
            self.log.info("Cozir: Filter correctly setted")
            return True
        else:
            self.log.error("Cozir: Filter is not setted")
            return False
    
    # Get digital filter
    def get_filter(self):
        self.ser.write(bytes("a\r\n",'utf-8'))
        self.ser.flush()
        resp = str(self.ser.readline(),'utf-8')
        resp = int(resp[3:8])
        return resp
    
    # Get temperature
    def temperature(self):
        self.ser.write(bytes("T\r\n",'utf-8'))
        self.ser.flush()
        resp = str(self.ser.readline(),'utf-8')
        resp = float((int(resp[3:8])-1000))/10
        return resp
    
    # Get humidity
    def humidity(self):
        self.ser.write(bytes("H\r\n",'utf-8'))
        self.ser.flush()
        resp = str(self.ser.readline(),'utf-8')
        resp = float(resp[3:8])/10
        return resp
    
    # Get CO2
    def co2(self, filter = 0):
        if(filter == 0):
            self.ser.write(bytes("Z\r\n",'utf-8'))
            self.ser.flush()
            resp = str(self.ser.readline(),'utf-8')
            resp = int(resp[3:8])
            return resp
        elif(1):
            self.ser.write(bytes("z\r\n",'utf-8'))
            self.ser.flush()
            resp = str(self.ser.readline(),'utf-8')
            resp = int(resp[3:8])
            return resp
        else:
            self.log.error("Cozir: Parameter Incorrect")
            return None
    
    # Get Data Outputs setted
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
    
    # Set Background Concentration
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
        
        if(i == 2): return True
        else: return False

    # General Calibration
    def calibrate(self, mode, ppm = -1):
        # Known Gas Concentration
        if(mode == 0 and ppm > 0):
            self.ser.write(bytes("X {0}\r\n".format(ppm),'utf-8'))
            self.ser.flush()
            resp = str(self.ser.readline(),'utf-8')
            if(resp[3] != "?" and resp.startswith(" X ")): return True
            else: return False
        
        # 0 CO2 (Nitrogen)
        elif(mode == 1):
            self.ser.write(bytes("U\r\n",'utf-8'))
            self.ser.flush()
            resp = str(self.ser.readline(),'utf-8')
            if(resp[3] != "?" and resp.startswith(" U ")): return True
            else: return False
    
        # Fresh Air (400 ppm)
        elif(mode == 2):
            self.ser.write(bytes("G\r\n",'utf-8'))
            self.ser.flush()
            resp = str(self.ser.readline(),'utf-8')
            if(resp[3] != "?" and resp.startswith(" G ")): return True
            else: return False
            
        else:
            self.log.error("Cozir: Some Parameter wrong")
            return False
    
    # Calibration when you know the value reported by the sensor and the actual correct value
    def recalibrate(self, sens, real):
        if(sens>0 and sens<2000 and real>0 and real<2000):
            self.ser.write(bytes("F {0} {1}\r\n".format(sens, real),'utf-8'))
            self.ser.flush()
            resp = str(self.ser.readline(),'utf-8')
            self.log.debug(resp)
            if(resp[3] != "?" and resp.startswith(" F ")): return True
            else: return False
            
        else:
            self.log.error("Cozir: Some Parameter wrong")
            return False