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
    def __init__(self, loggerFront1, loggerCenter1, loggerBack1, loggerFront2, loggerCenter2, loggerBack2):
        self.front1 = dataESP32(0, loggerFront1)
        self.center1 = dataESP32(0, loggerCenter1)
        self.back1 = dataESP32(0, loggerBack1)
        self.front2 = dataESP32(1, loggerFront2)
        self.center2 = dataESP32(1, loggerCenter2)
        self.back2 = dataESP32(1, loggerBack2)
    
    def updateStatus(self):
        if(self.front1.connected == False): self.front1.connectionFailed()
        else: self.front1.connectionSuccess()
        self.front1.connected = False
        if(self.center1.connected == False): self.center1.connectionFailed()
        else: self.center1.connectionSuccess()
        self.center1.connected = False
        if(self.back1.connected == False): self.back1.connectionFailed()
        else: self.back1.connectionSuccess()
        self.back1.connected = False
        
        if(self.front2.connected == False): self.front2.connectionFailed()
        else: self.front2.connectionSuccess()
        self.front2.connected = False
        if(self.center2.connected == False): self.center2.connectionFailed()
        else: self.center2.connectionSuccess()
        self.center2.connected = False
        if(self.back2.connected == False): self.back2.connectionFailed()
        else: self.back2.connectionSuccess()
        self.back2.connected = False
        
    def isDataComplete(self):
        if(self.front1.isDataComplete() and self.center1.isDataComplete() and self.back1.isDataComplete() and self.front2.isDataComplete() and self.center2.isDataComplete() and self.back2.isDataComplete()):
            return True
        else: return False
    
    def printMissingData(self):
        if not self.front1.isDataComplete():
            self.front1.printMissingData()
                
        if not self.center1.isDataComplete():
            self.center1.printMissingData()
                
        if not self.back1.isDataComplete():
            self.back1.printMissingData()
            
        if not self.front2.isDataComplete():
            self.front2.printMissingData()
                
        if not self.center2.isDataComplete():
            self.center2.printMissingData()
                
        if not self.back2.isDataComplete():
            self.back2.printMissingData()

    def averageTemp(self):
        data = [self.front1.averageTemp(), self.center1.averageTemp(), self.back1.averageTemp(), self.front2.averageTemp(), self.center2.averageTemp(), self.back2.averageTemp()]
        data = [ele for ele in data if ele not in {0}]
        if len(data)==0: return 0
        else: return sum(data)/len(data)
    
    def averageHum(self):
        data = [self.front1.averageHum(), self.center1.averageHum(), self.back1.averageHum(), self.front2.averageHum(), self.center2.averageHum(), self.back2.averageHum()]
        data = [ele for ele in data if ele not in {0}]
        if len(data)==0: return 0
        else: return sum(data)/len(data)

    def upload2DB(self, dbConnector):
        # Create cursor
        c = dbConnector.cursor()
        # Create table if not exist
        c.execute("CREATE TABLE IF NOT EXISTS espData(datetime TIMESTAMP," +
                  " F_T1R REAL, F_H1R REAL, F_T1L REAL, F_H1L REAL," +
                  " F_T2R REAL, F_H2R REAL, F_T2L REAL, F_H2L REAL," +
                  " F_T3R REAL, F_H3R REAL, F_T3L REAL, F_H3L REAL," +
                  " F_T4R REAL, F_H4R REAL, F_T4L REAL, F_H4L REAL," +
                  " F_T5R REAL, F_H5R REAL, F_T5L REAL, F_H5L REAL," +
                  " F_T6R REAL, F_H6R REAL, F_T6L REAL, F_H6L REAL," +
                  " F_T7R REAL, F_H7R REAL, F_T7L REAL, F_H7L REAL," +
                  " F_T8R REAL, F_H8R REAL, F_T8L REAL, F_H8L REAL," +
                  " C_T1R REAL, C_H1R REAL, C_T1L REAL, C_H1L REAL," +
                  " C_T2R REAL, C_H2R REAL, C_T2L REAL, C_H2L REAL," +
                  " C_T3R REAL, C_H3R REAL, C_T3L REAL, C_H3L REAL," +
                  " C_T4R REAL, C_H4R REAL, C_T4L REAL, C_H4L REAL," +
                  " C_T5R REAL, C_H5R REAL, C_T5L REAL, C_H5L REAL," +
                  " C_T6R REAL, C_H6R REAL, C_T6L REAL, C_H6L REAL," +
                  " C_T7R REAL, C_H7R REAL, C_T7L REAL, C_H7L REAL," +
                  " C_T8R REAL, C_H8R REAL, C_T8L REAL, C_H8L REAL," +
                  " B_T1R REAL, B_H1R REAL, B_T1L REAL, B_H1L REAL," +
                  " B_T2R REAL, B_H2R REAL, B_T2L REAL, B_H2L REAL," +
                  " B_T3R REAL, B_H3R REAL, B_T3L REAL, B_H3L REAL," +
                  " B_T4R REAL, B_H4R REAL, B_T4L REAL, B_H4L REAL," +
                  " B_T5R REAL, B_H5R REAL, B_T5L REAL, B_H5L REAL," +
                  " B_T6R REAL, B_H6R REAL, B_T6L REAL, B_H6L REAL," +
                  " B_T7R REAL, B_H7R REAL, B_T7L REAL, B_H7L REAL," +
                  " B_T8R REAL, B_H8R REAL, B_T8L REAL, B_H8L REAL)")
        # Insert values
        c.execute("INSERT INTO espData ( datetime," +
                  " F_T1R, F_H1R, F_T1L, F_H1L, F_T2R, F_H2R, F_T2L, F_H2L," +
                  " F_T3R, F_H3R, F_T3L, F_H3L, F_T4R, F_H4R, F_T4L, F_H4L," +
                  " F_T5R, F_H5R, F_T5L, F_H5L, F_T6R, F_H6R, F_T6L, F_H6L," +
                  " F_T7R, F_H7R, F_T7L, F_H7L, F_T8R, F_H8R, F_T8L, F_H8L," +
                  " C_T1R, C_H1R, C_T1L, C_H1L, C_T2R, C_H2R, C_T2L, C_H2L," +
                  " C_T3R, C_H3R, C_T3L, C_H3L, C_T4R, C_H4R, C_T4L, C_H4L," +
                  " C_T5R, C_H5R, C_T5L, C_H5L, C_T6R, C_H6R, C_T6L, C_H6L," +
                  " C_T7R, C_H7R, C_T7L, C_H7L, C_T8R, C_H8R, C_T8L, C_H8L," +
                  " B_T1R, B_H1R, B_T1L, B_H1L, B_T2R, B_H2R, B_T2L, B_H2L," +
                  " B_T3R, B_H3R, B_T3L, B_H3L, B_T4R, B_H4R, B_T4L, B_H4L," +
                  " B_T5R, B_H5R, B_T5L, B_H5L, B_T6R, B_H6R, B_T6L, B_H6L," +
                  " B_T7R, B_H7R, B_T7L, B_H7L, B_T8R, B_H8R, B_T8L, B_H8L)" +
                  " VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?," +
                  " ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?," +
                  " ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?," +
                  " ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?," +
                  " ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?," +
                  " ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)",
                  ( datetime.now(),
                   self.front1.T1R, self.front1.H1R, self.front1.T1L, self.front1.H1L,
                   self.front1.T2R, self.front1.H2R, self.front1.T2L, self.front1.H2L,
                   self.front1.T3R, self.front1.H3R, self.front1.T3L, self.front1.H3L,
                   self.front1.T4R, self.front1.H4R, self.front1.T4L, self.front1.H4L,
                   self.front2.T1R, self.front2.H1R, self.front2.T1L, self.front2.H1L,
                   self.front2.T2R, self.front2.H2R, self.front2.T2L, self.front2.H2L,
                   self.front2.T3R, self.front2.H3R, self.front2.T3L, self.front2.H3L,
                   self.front2.T4R, self.front2.H4R, self.front2.T4L, self.front2.H4L,
                   self.center1.T1R, self.center1.H1R, self.center1.T1L, self.center1.H1L,
                   self.center1.T2R, self.center1.H2R, self.center1.T2L, self.center1.H2L,
                   self.center1.T3R, self.center1.H3R, self.center1.T3L, self.center1.H3L,
                   self.center1.T4R, self.center1.H4R, self.center1.T4L, self.center1.H4L,
                   self.center2.T1R, self.center2.H1R, self.center2.T1L, self.center2.H1L,
                   self.center2.T2R, self.center2.H2R, self.center2.T2L, self.center2.H2L,
                   self.center2.T3R, self.center2.H3R, self.center2.T3L, self.center2.H3L,
                   self.center2.T4R, self.center2.H4R, self.center2.T4L, self.center2.H4L,
                   self.back1.T1R, self.back1.H1R, self.back1.T1L, self.back1.H1L,
                   self.back1.T2R, self.back1.H2R, self.back1.T2L, self.back1.H2L,
                   self.back1.T3R, self.back1.H3R, self.back1.T3L, self.back1.H3L,
                   self.back1.T4R, self.back1.H4R, self.back1.T4L, self.back1.H4L,
                   self.back2.T1R, self.back2.H1R, self.back2.T1L, self.back2.H1L,
                   self.back2.T2R, self.back2.H2R, self.back2.T2L, self.back2.H2L,
                   self.back2.T3R, self.back2.H3R, self.back2.T3L, self.back2.H3L,
                   self.back2.T4R, self.back2.H4R, self.back2.T4L, self.back2.H4L))
        # Commit changes
        dbConnector.commit()
        # Erase cursor
        c.close()
