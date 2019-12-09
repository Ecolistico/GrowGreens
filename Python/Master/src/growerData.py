#!/usr/bin/env python3

# Import directories
from time import time
from numpy import isnan
from datetime import datetime

class grower:
    def __init__(self, logger): # Constructor
        self.inRoutine = False
        self.startRoutine = False
        self.count = 0
        self.serialRequest = ""
        self.mqttRequest = ""
        self.actualTime = time()
        
        self.Temp = 0
        self.Hum = 0
        self.CO2 = 0
        self.connected = False
        self.failedConnection = 0
        self.IP = ""
        self.log = logger
        
    def serialReq(self, req):
        self.serialRequest = ""
        self.actualTime = time()
        
    def mqttReq(self, req):
        self.mqttRequest = ""
        self.actualTime = time()
    
    def checkValue(self, val, variable):
        if(isnan(val)):
            return variable
        else:
            return val
        
    def str2array(self, rawString): # Process string array
        strSplit = rawString.split(",")
        
        self.Hum = self.checkValue(float(strSplit[1]), self.Hum)
        self.Temp = self.checkValue(float(strSplit[2]), self.Temp)
        self.CO2 = self.checkValue(float(strSplit[3]), self.CO2)
        
        self.logValues()
        
    def logValues(self):
        self.log.debug("Temp={0}°C, Hum={1}%, CO2={2}ppm".format(self.Temp, self.Hum, self.CO2))
        
    def connectionFailed(self):
        self.failedConnection += 1
        if(self.failedConnection>=5):
            self.failedConnection = 0
            self.log.error("Device disconnected")
            
    def connectionSuccess(self):
        self.failedConnection = 0
        
class multiGrower:
    def __init__(self, logger1, logger2, logger3, logger4):
        self.Gr1 = grower(logger1)
        self.Gr2 = grower(logger2)
        self.Gr3 = grower(logger3)
        self.Gr4 = grower(logger4)
    
    def updateStatus(self):
        if(self.Gr1.connected == False): self.Gr1.connectionFailed()
        else: self.Gr1.connectionSuccess()
        self.Gr1.connected = False
        if(self.Gr2.connected == False): self.Gr2.connectionFailed()
        else: self.Gr2.connectionSuccess()
        self.Gr2.connected = False
        if(self.Gr3.connected == False): self.Gr3.connectionFailed()
        else: self.Gr3.connectionSuccess()
        self.Gr3.connected = False
        if(self.Gr4.connected == False): self.Gr4.connectionFailed()
        else: self.Gr4.connectionSuccess()
        self.Gr4.connected = False
        
    def upload2DB(self, dbConnector):
        # Create cursor
        c = dbConnector.cursor()
        # Create table if not exist
        c.execute("CREATE TABLE IF NOT EXISTS cozirData(datetime TIMESTAMP," +
                  " T_1 REAL, H_1 REAL, CO2_1 REAL," +
                  " T_2 REAL, H_2 REAL, CO2_2 REAL," +
                  " T_3 REAL, H_3 REAL, CO2_3 REAL," +
                  " T_4 REAL, H_4 REAL, CO2_4 REAL)")
        
        # Insert values
        c.execute("INSERT INTO cozirData ( datetime," +
                  " T_1, H_1, CO2_1, T_2, H_2, CO2_2," +
                  " T_3, H_3, CO2_3, T_4, H_4, CO2_4)" 
                  " VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)",
                  (datetime.now(), self.Gr1.Temp, self.Gr1.Hum, self.Gr1.CO2,
                   self.Gr2.Temp, self.Gr2.Hum, self.Gr2.CO2,
                   self.Gr3.Temp, self.Gr3.Hum, self.Gr3.CO2,
                   self.Gr4.Temp, self.Gr4.Hum, self.Gr4.CO2))
        # Commit changes
        dbConnector.commit()
        # Erase cursor
        c.close()