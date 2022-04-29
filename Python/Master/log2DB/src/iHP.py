#!/usr/bin/env python3

# Import modules
import re
from datetime import datetime

class logiHP:
    def __init__(self):
        self.voltage = [0, 0, 0]
        self.current = [0, 0, 0]
        self.update = [False, False, False, False, False, False]
        self.dateTime = None
        self.firstDateLine = None

    def line2value(self, line):
        # Get important data
        dTime = datetime.strptime(line[0:19], '%Y-%m-%d %H:%M:%S')
        msg = line[45:].strip()
        
        # Set dateTime if it is the first value
        if(self.voltage==[0, 0, 0] and self.current==[0, 0, 0]): 
            self.dateTime = datetime.now()
            self.firstDateLine = dTime
        
        # Get voltage
        if("Voltage" in msg):
            values = re.findall(r"[-+]?(?:\d*\.\d+|\d+)", msg)
            if(len(values) == 2):
                index =  int(values[0])
                if(self.update[index-1]):
                    self.reset()
                    return False
                else:
                    self.voltage[index-1] = float(values[1])
                    self.update[index-1] = True
        # Get current
        elif("Current" in msg):
            values = re.findall(r"[-+]?(?:\d*\.\d+|\d+)", msg)
            if(len(values) == 2): 
                index =  int(values[0])
                if(self.update[index+2]):
                    self.reset()
                    return False
                else:
                    self.current[index-1] = float(values[1])
                    self.update[index+2] = True
        
        # Restart if late response
        seconds = (datetime.now() - self.dateTime).total_seconds()
        if(seconds > 10):
            self.reset()
            return False

        # Check if the data is complete
        if(self.update == [True, True, True, True, True, True]): 
            return True
        else: return False
    
    def getVoltage(self):
        return {"v1": self.voltage[0], "v2": self.voltage[1], "v3": self.voltage[2]}

    def getCurrent(self):
        return {"i1": self.current[0], "i2": self.current[1], "i3": self.current[2]}

    def reset(self):
        self.voltage = [0, 0, 0]
        self.current = [0, 0, 0]
        self.update = [False, False, False, False, False, False]
        self.dateTime = None
        self.firstDateLine = None
    
    def setData(self, data):
        self.voltage = data["voltage"]
        self.current = data["current"]
        self.update = data["update"]
        if(data["dateTime"]==None): self.dateTime = data["dateTime"]
        else: self.dateTime = datetime.strptime(data["dateTime"], '%Y-%m-%d %H:%M:%S')
        if(data["firstDateLine"]==None): self.firstDateLine = data["firstDateLine"]
        else: self.firstDateLine = datetime.strptime(data["firstDateLine"], '%Y-%m-%d %H:%M:%S')
    
    def getData(self):
        data = {}
        data["voltage"] = self.voltage
        data["current"] = self.current
        data["update"] = self.update
        if(self.dateTime==None): data["dateTime"] = self.dateTime
        else: data["dateTime"] = self.dateTime.strftime("'%Y-%m-%d %H:%M:%S'")
        if(self.firstDateLine==None): data["firstDateLine"] = self.firstDateLine
        else: data["firstDateLine"] = self.firstDateLine.strftime("'%Y-%m-%d %H:%M:%S'")
        return data

