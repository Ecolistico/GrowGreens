#!/usr/bin/env python3

# Import directories
from numpy import isnan
from datetime import datetime

# def class to get ESP32 sensor data
class dataESP32:
    def __init__(self, level, logger): # Constructor
        self.T1R = 0
        self.H1R = 0
        self.T1L = 0
        self.H1L = 0
        self.T2R = 0
        self.H2R = 0
        self.T2L = 0
        self.H2L = 0
        self.T3R = 0
        self.H3R = 0
        self.T3L = 0
        self.H3L = 0
        self.T4R = 0
        self.H4R = 0
        self.T4L = 0
        self.H4L = 0
        self.M1 = 0
        self.M2 = 0
        self.M3 = 0
        self.M4 = 0
        self.missingData = []
        self.connected = False
        self.failedConnection = 0
        self.level = level
        self.log = logger
        
    def append(self, data, key):
        if not key in data: data.append(key)
    
    def remove(self, data, key):
        if key in data: data.remove(key)
        
    def checkValue(self, val, variable, data, key):
        if(isnan(val)):
            self.append(data, key)
            return variable
        else:
            self.remove(data, key)
            return val
            
    def str2array(self, rawString): # Process string array
        strSplit = rawString.split(",")
        
        self.T1R = self.checkValue(float(strSplit[0]), self.T1R, self.missingData, "T1R")
        self.H1R = self.checkValue(float(strSplit[1]), self.H1R, self.missingData, "H1R")
        self.T1L = self.checkValue(float(strSplit[2]), self.T1L, self.missingData, "T1L")
        self.H1L = self.checkValue(float(strSplit[3]), self.H1L, self.missingData, "H1L")
        
        self.T2R = self.checkValue(float(strSplit[4]), self.T2R, self.missingData, "T2R")
        self.H2R = self.checkValue(float(strSplit[5]), self.H2R, self.missingData, "H2R")
        self.T2L = self.checkValue(float(strSplit[6]), self.T2L, self.missingData, "T2L")
        self.H2L = self.checkValue(float(strSplit[7]), self.H2L, self.missingData, "H2L")
        
        self.T3R = self.checkValue(float(strSplit[8]), self.T3R, self.missingData, "T3R")
        self.H3R = self.checkValue(float(strSplit[9]), self.H3R, self.missingData, "H3R")
        self.T3L = self.checkValue(float(strSplit[10]), self.T3L, self.missingData, "T3L")
        self.H3L = self.checkValue(float(strSplit[11]), self.H3L, self.missingData, "H3L")
        
        self.T4R = self.checkValue(float(strSplit[12]), self.T4R, self.missingData, "T4R")
        self.H4R = self.checkValue(float(strSplit[13]), self.H4R, self.missingData, "H4R")
        self.T4L = self.checkValue(float(strSplit[14]), self.T4L, self.missingData, "T4L")
        self.H4L = self.checkValue(float(strSplit[15]), self.H4L, self.missingData, "H4L")
        
        self.M1 = strSplit[16]
        self.M2 = strSplit[17]
        self.M3 = strSplit[18]
        self.M4 = strSplit[19]
        
        self.logValues()
        
    def isDataComplete(self):
        if len(self.missingData)==0: return True
        else: return False
    
    def printMissingData(self):
        mssg = "The missing values are: "
        for i,miss in enumerate(self.missingData):
            if i<len(self.missingData)-1: mssg += "{}, ".format(miss)
            else: mssg += "{}".format(miss)
        self.log.warning(mssg)
    
    def logValues(self):
        lv = self.level
        self.log.debug("T{0}R={1}, H{2}R={3}, T{4}L={5}, H{6}L={7}".format(1+lv*4, self.T1R, 1+lv*4, self.H1R, 1+lv*4, self.T1L, 1+lv*4, self.H1L))
        self.log.debug("T{0}R={1}, H{2}R={3}, T{4}L={5}, H{6}L={7}".format(2+lv*4, self.T2R, 2+lv*4, self.H2R, 2+lv*4, self.T2L, 2+lv*4, self.H2L))
        self.log.debug("T{0}R={1}, H{2}R={3}, T{4}L={5}, H{6}L={7}".format(3+lv*4, self.T3R, 3+lv*4, self.H3R, 3+lv*4, self.T3L, 3+lv*4, self.H3L))
        self.log.debug("T{0}R={1}, H{2}R={3}, T{4}L={5}, H{6}L={7}".format(4+lv*4, self.T4R, 4+lv*4, self.H4R, 4+lv*4, self.T4L, 4+lv*4, self.H4L))
        self.log.debug("M{0}={1}, M{2}={3}, M{4}={5}, M{6}={7}".format(1+lv*4, self.isOpen(self.M1), 2+lv*4, self.isOpen(self.M2), 3+lv*4, self.isOpen(self.M3), 4+lv*4, self.isOpen(self.M4)))
        
    def isOpen(self, val): return "OPEN" if (val=="1") else "CLOSE"
    
    def connectionFailed(self):
        self.failedConnection += 1
        if(self.failedConnection>=5):
            self.failedConnection = 0
            self.log.error("Device disconnected")
            
    def connectionSuccess(self):
        self.failedConnection = 0

    def averageTemp(self):
        data = [self.T1R, self.T1L, self.T2R, self.T2L, self.T3R, self.T3L, self.T4R, self.T4L]
        data = [ele for ele in data if ele not in {0}]
        if len(data)==0: return 0
        else: return sum(data)/len(data)
    
    def averageHum(self):
        data = [self.H1R, self.H1L, self.H2R, self.H2L, self.H3R, self.H3L, self.H4R, self.H4L]
        data = [ele for ele in data if ele not in {0}]
        if len(data)==0: return 0
        else: return sum(data)/len(data)
        
# Define class for the 6 ESP
class multiESP:
    def __init__(self, loggerESP32):
        self.esp32 = []
        for i in range(loggerESP32): 
            self.esp32.append(dataESP32(i, loggerESP32[i][0]), 
                              dataESP32(i, loggerESP32[i][1]), 
                              dataESP32(i, loggerESP32[i][2]))
    
    def updateStatus(self):
        for espLine in self.esp32:
            for esp32 in espLine:
                if(esp32.connected == False): esp32.connectionFailed()
                else: esp32.connectionSuccess()
                esp32.connected = False
        
    def isDataComplete(self):
        for espLine in self.esp32:
            for esp32 in espLine:
                if esp32.isDataComplete() == False: return False
        return True
    
    def printMissingData(self):
        for espLine in self.esp32:
            for esp32 in espLine:
                if not esp32.isDataComplete(): esp32.printMissingData()

    def averageTemp(self):
        data = []
        for espLine in self.esp32:
            for esp32 in espLine: data.append(esp32.averageTemp())
        data = [ele for ele in data if ele not in {0}]
        if len(data)==0: return 0
        else: return sum(data)/len(data)
    
    def averageHum(self):
        data = []
        for espLine in self.esp32:
            for esp32 in espLine: data.append(espLine.averageHum())
        data = [ele for ele in data if ele not in {0}]
        if len(data)==0: return 0
        else: return sum(data)/len(data)

    def upload2DB(self, dbConnector):
        # Create cursor
        c = dbConnector.cursor()
        # Create table if not exist
        t_aux = "CREATE TABLE IF NOT EXISTS espData(datetime TIMESTAMP,"
        for i, espLine in enumerate(self.esp32):
            for j, esp32 in enumerate(espLine):
                prefix = ""
                if j == 0: prefix = "F"
                elif j == 1: prefix = "C"
                elif j == 2: prefix = "B"
                for k in range(4):
                    t_aux += " {1}_T{0}R REAL, {1}_H{0}R REAL, {1}_T{0}L REAL, {1}_H{0}L REAL,".format(k+1+i*4, prefix)
        t_aux = t_aux[:-1] + ")"
        c.execute(t_aux)

        # Insert values
        t_aux = "INSERT INTO espData ( datetime,"
        v_aux = (datetime.now(),)
        for i, espLine in enumerate(self.esp32):
            for j, esp32 in enumerate(espLine):
                prefix = ""
                if j == 0: prefix = "F"
                elif j == 1: prefix = "C"
                elif j == 2: prefix = "B"
                for k in range(4):
                    t_aux += " {1}_T{0}R, {1}_H{0}R, {1}_T{0}L, {1}_H{0}L,".format(k+1+i*4, prefix)
                v_aux += (esp32.T1R, esp32.H1R, esp32.T1L, esp32.H1L, 
                          esp32.T2R, esp32.H2R, esp32.T2L, esp32.H2L, 
                          esp32.T3R, esp32.H3R, esp32.T3L, esp32.H3L, 
                          esp32.T4R, esp32.H4R, esp32.T4L, esp32.H4L)
        t_aux = t_aux[:-1] + ") VALUES (?," + " ?,"*(16*len(self.esp32[0])*len(self.esp32))
        t_aux = t_aux[:-1] + ")"
        c.execute(t_aux, v_aux)
        # Commit changes
        dbConnector.commit()
        # Erase cursor
        c.close()
