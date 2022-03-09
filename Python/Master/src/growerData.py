#!/usr/bin/env python3

# Import directories
from time import time
from numpy import isnan
from datetime import datetime

class grower:
    def __init__(self, fl, logger): # Constructor
        self.floor = fl
        self.inRoutine = False
        self.startRoutine = False
        self.count = 1
        self.serialRequest = ""
        self.mqttRequest = ""
        self.actualTime = time()
        
        self.Temp = 0
        self.Hum = 0
        self.CO2 = 0
        self.connected = False
        self.failedConnection = 0
        self.log = logger
        
        self.xSeq = 155
        self.ySeq = 200
        
    def serialReq(self, req):
        self.serialRequest = req
        self.actualTime = time()
        
    def mqttReq(self, req):
        self.mqttRequest = req
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
        if(self.failedConnection>=6):
            self.failedConnection = 1
            self.log.error("Device disconnected")
            
    def connectionSuccess(self):
        self.failedConnection = 0
    
    def time2Move(self, fl = None):
        if fl is None: fl = self.floor
        self.startRoutine = True
        msg = "available,{}".format(fl)
        self.serialReq(msg)
    
    def getSequenceParameters(self):
        return self.xSeq, self.ySeq
    
class multiGrower:
    def __init__(self, logger, data = None):
        if data is not None: self.data = data
        else:
            self.data = {}
            for i in range(len(logger.logger_grower)): self.data["{}".format(i+1)] = "{}".format(i+1)
        
        self.Gr =[]
        for i in range(len(logger.logger_grower)): self.Gr.append(grower(i+1, logger.logger_grower[i]))
    
    def updateStatus(self):
        for i in range(len(self.Gr)):
            if(self.Gr[i].connected == False and self.data["{}".format(i+1)]!="disconnected"): self.Gr[i].connectionFailed()
            else: self.Gr[i].connectionSuccess()
            self.Gr[i].connected = False
    
    def upload2DB(self, dbConnector):
        # Create cursor
        c = dbConnector.cursor()
        # Create table if not exist
        t_aux = "CREATE TABLE IF NOT EXISTS cozirData(datetime TIMESTAMP,"
        for i in range(len(self.Gr)): t_aux += " T_{0} REAL, H_{0} REAL, CO2_{0} REAL,".format(i+1)
        t_aux = t_aux[:-1] + ")"
        c.execute(t_aux)
        
        # Insert values
        t_aux = "INSERT INTO cozirData ( datetime,"
        v_aux = (datetime.now(),)
        for i in range(len(self.Gr)): 
            t_aux += " T_{0}, H_{0}, CO2_{0},".format(i+1)
            v_aux += (self.Gr[i].Temp, self.Gr[i].Hum, self.Gr[i].CO2)
        t_aux = t_aux[:-1] + ") VALUES (?," + " ?,"*(len(self.Gr)*3)
        t_aux = t_aux[:-1] + ")"
        
        c.execute(t_aux, v_aux)
        # Commit changes
        dbConnector.commit()
        # Erase cursor
        c.close()