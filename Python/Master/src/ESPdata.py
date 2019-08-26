from numpy import isnan
from datetime import datetime

# def class to get ESP32 sensor data
class dataESP32:
    def __init__(self, logger): # Constructor
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
        self.missingData = []
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
        
    def isDataComplete(self):
        if len(self.missingData)==0: return True
        else: return False
    
    def printMissingData(self):
        mssg = "The missing values are: "
        for i,miss in enumerate(self.missingData):
            if i<len(self.missingData)-1: mssg += "{}, ".format(miss)
            else: mssg += "{}".format(miss)
        self.log.warning(mssg)

# Define class for the 3 ESP
class multiESP:
    def __init__(self, loggerFront, loggerCenter, loggerBack):
        self.front = dataESP32(loggerFront)
        self.center = dataESP32(loggerCenter)
        self.back = dataESP32(loggerBack)
    
    def isDataComplete(self):
        if(self.front.isDataComplete() and self.center.isDataComplete() and self.back.isDataComplete()):
            return True
        else: return False
    
    def printMissingData(self):
        if not self.front.isDataComplete():
            self.front.printMissingData()
                
        if not self.center.isDataComplete():
            self.center.printMissingData()
                
        if not self.back.isDataComplete():
            self.back.printMissingData()
        
    def upload2DB(self, dbConnector):
        # Create cursor
        c = dbConnector.cursor()
        # Create table if not exist
        c.execute("CREATE TABLE IF NOT EXISTS espData(datetime TIMESTAMP," +
                  " F_T1R REAL, F_H1R REAL, F_T1L REAL, F_H1L REAL," +
                  " F_T2R REAL, F_H2R REAL, F_T2L REAL, F_H2L REAL," +
                  " F_T3R REAL, F_H3R REAL, F_T3L REAL, F_H3L REAL," +
                  " F_T4R REAL, F_H4R REAL, F_T4L REAL, F_H4L REAL," +
                  " C_T1R REAL, C_H1R REAL, C_T1L REAL, C_H1L REAL," +
                  " C_T2R REAL, C_H2R REAL, C_T2L REAL, C_H2L REAL," +
                  " C_T3R REAL, C_H3R REAL, C_T3L REAL, C_H3L REAL," +
                  " C_T4R REAL, C_H4R REAL, C_T4L REAL, C_H4L REAL," +
                  " B_T1R REAL, B_H1R REAL, B_T1L REAL, B_H1L REAL," +
                  " B_T2R REAL, B_H2R REAL, B_T2L REAL, B_H2L REAL," +
                  " B_T3R REAL, B_H3R REAL, B_T3L REAL, B_H3L REAL," +
                  " B_T4R REAL, B_H4R REAL, B_T4L REAL, B_H4L REAL)")
        # Insert values
        c.execute("INSERT INTO espData ( datetime," +
                  " F_T1R, F_H1R, F_T1L, F_H1L, F_T2R, F_H2R, F_T2L, F_H2L," +
                  " F_T3R, F_H3R, F_T3L, F_H3L, F_T4R, F_H4R, F_T4L, F_H4L," +
                  " C_T1R, C_H1R, C_T1L, C_H1L, C_T2R, C_H2R, C_T2L, C_H2L," +
                  " C_T3R, C_H3R, C_T3L, C_H3L, C_T4R, C_H4R, C_T4L, C_H4L," +
                  " B_T1R, B_H1R, B_T1L, B_H1L, B_T2R, B_H2R, B_T2L, B_H2L," +
                  " B_T3R, B_H3R, B_T3L, B_H3L, B_T4R, B_H4R, B_T4L, B_H4L)" +
                  " VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?," +
                  " ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?," +
                  " ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)",
                  ( datetime.now(), self.front.T1R, self.front.H1R, self.front.T1L, self.front.H1L,
                   self.front.T2R, self.front.H2R, self.front.T2L, self.front.H2L,
                   self.front.T3R, self.front.H3R, self.front.T3L, self.front.H3L,
                   self.front.T4R, self.front.H4R, self.front.T4L, self.front.H4L,
                   self.center.T1R, self.center.H1R, self.center.T1L, self.center.H1L,
                   self.center.T2R, self.center.H2R, self.center.T2L, self.center.H2L,
                   self.center.T3R, self.center.H3R, self.center.T3L, self.center.H3L,
                   self.center.T4R, self.center.H4R, self.center.T4L, self.center.H4L,
                   self.back.T1R, self.back.H1R, self.back.T1L, self.back.H1L,
                   self.back.T2R, self.back.H2R, self.back.T2L, self.back.H2L,
                   self.back.T3R, self.back.H3R, self.back.T3L, self.back.H3L,
                   self.back.T4R, self.back.H4R, self.back.T4L, self.back.H4L))
        # Commit changes
        dbConnector.commit()
        # Erase cursor
        c.close()
