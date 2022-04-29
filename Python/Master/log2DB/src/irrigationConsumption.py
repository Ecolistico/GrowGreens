#!/usr/bin/env python3

# Import modules
import re
from datetime import datetime

class logIrrigationConsumption:
    def __init__(self):
        self.values = {}
        self.dTime = None
        self.totalWater = 0

    def line2value(self, line):
        # Get important data
        self.dTime = datetime.strptime(line[0:19], '%Y-%m-%d %H:%M:%S')
        msg = line[45:].strip()

        if(msg.startswith("Solenoid Valve") and "Water Volume" in msg):
            ev = re.findall(r"([1-9][A|B][1-9])", msg)
            values = re.findall(r"[-+]?(?:\d*\.\d+|\d+)", msg)
            if(len(ev)==1 and len(values)==3): self.values[ev[0]] = float(values[2])
        elif(msg.startswith("Solenoid System: Water Consumption")):
            values = re.findall(r"[-+]?(?:\d*\.\d+|\d+)", msg)
            if(len(values)==1): self.totalWater = float(values[0])
            return True
            
        return False

    def reset(self):
        self.values = {}
        self.totalWater = 0
        self.dTime = None
    
    def setData(self, data):
        self.values = data["values"]
        if(data["dTime"] == None): self.dTime = data["dTime"]
        else: self.dTime = datetime.strptime(data["dTime"], '%Y-%m-%d %H:%M:%S')
        self.totalWater = data["totalWater"]  
    
    def getData(self):
        data = {}
        data["values"] = self.values
        if(self.dTime == None): data["dTime"] = self.dTime
        else: data["dTime"] = self.dTime.strftime("'%Y-%m-%d %H:%M:%S'")
        data["totalWater"] = self.totalWater
        return data
        