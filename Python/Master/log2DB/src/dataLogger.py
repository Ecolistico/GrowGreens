#!/usr/bin/env python3

# Import modules
import re
from datetime import datetime

class logDataLogger:
    def __init__(self):
        self.values = {}
        self.dev = ""
        self.dTime = None

    def line2value(self, line):
        # Get important data
        dev = line.split(" ")[2].strip()

        # Check if the device is the same
        if(self.dev==""): 
            self.dev = dev
        elif(self.dev!=dev): 
            self.reset()
            self.dev = dev

        self.dTime = datetime.strptime(line[0:19], '%Y-%m-%d %H:%M:%S')
        msg = line[45:].strip()
        values = re.findall(r"[-+]?(?:\d*\.\d+|\d+)", msg)

        if(dev.endswith("1")):
            if("M1" in msg):
                values = re.findall(r"(OPEN)|(CLOSE)", msg)
                if(len(values) == 4):
                    values = [True if val[0]=="OPEN" else False for val in values]
                    self.values["M1"] = values[0]
                    self.values["M2"] = values[1]
                    self.values["M3"] = values[2]
                    self.values["M4"] = values[3]
                    return True
            elif(len(values)==8):
                if("T1R" in msg):
                    self.values["T1R"] = float(values[1])
                    self.values["H1R"] = float(values[3])
                    self.values["T1L"] = float(values[5])
                    self.values["H1L"] = float(values[7])
                elif("T2R" in msg):
                    self.values["T2R"] = float(values[1])
                    self.values["H2R"] = float(values[3])
                    self.values["T2L"] = float(values[5])
                    self.values["H2L"] = float(values[7])
                elif("T3R" in msg):
                    self.values["T3R"] = float(values[1])
                    self.values["H3R"] = float(values[3])
                    self.values["T3L"] = float(values[5])
                    self.values["H3L"] = float(values[7])
                elif("T4R" in msg):
                    self.values["T4R"] = float(values[0])
                    self.values["H4R"] = float(values[1])
                    self.values["T4L"] = float(values[2])
                    self.values["H4L"] = float(values[3])
            
        elif(dev.endswith("2")):
            if("M5" in msg):
                values = re.findall(r"(OPEN)|(CLOSE)", msg)
                if(len(values) == 4):
                    values = [True if val[0]=="OPEN" else False for val in values]
                    self.values["M5"] = values[0]
                    self.values["M6"] = values[1]
                    self.values["M7"] = values[2]
                    self.values["M8"] = values[3]
                    return True
            elif(len(values)==8):
                if("T5R" in msg):
                    self.values["T5R"] = float(values[1])
                    self.values["H5R"] = float(values[3])
                    self.values["T5L"] = float(values[5])
                    self.values["H5L"] = float(values[7])
                elif("T6R" in msg):
                    self.values["T6R"] = float(values[1])
                    self.values["H6R"] = float(values[3])
                    self.values["T6L"] = float(values[5])
                    self.values["H6L"] = float(values[7])
                elif("T7R" in msg):
                    self.values["T7R"] = float(values[1])
                    self.values["H7R"] = float(values[3])
                    self.values["T7L"] = float(values[5])
                    self.values["H7L"] = float(values[7])
                elif("T8R" in msg):
                    self.values["T8R"] = float(values[1])
                    self.values["H8R"] = float(values[3])
                    self.values["T8L"] = float(values[5])
                    self.values["H8L"] = float(values[7])
        return False
        
    def reset(self):
        self.values = {}
        self.type = ""
        self.dTime = None