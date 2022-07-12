#!/usr/bin/env python3

# Import modules
import os
import re
import sys
import json
from time import sleep
from datetime import datetime
sys.path.insert(0, './src/')
import utils
import parseClient
from iHP import logiHP
from dataLogger import logDataLogger
from irrigationConsumption import logIrrigationConsumption
sys.path.insert(0, './../src/')
from credentials import parse 

# Define config variables
with open("../config.json") as f:
    data = json.load(f)
    ID = data["ID"]
    
# Parse client
client = parseClient.parseClient(parse['server'], parse['appId'], parse['restKey'], mountPath = parse['mountPath'])

# Get ubication
ubication = None
try: ubication = client.query("ProductionSystems", {"systemId": ID})['results'][0]['ubication']
except: print("Ubication cannot be found")

# Control variables
checkFile = False
checkLine = False

# State variables
compressor = False
pump = False
mux1 = False
myiHP = logiHP()
myDataLogger = logDataLogger()
myIrrigation = logIrrigationConsumption()

# Log list
logDict = list()
# Env list
envDict = list()
# LED list
ledDict = list()
# Sensor list
sensorDict = list()
# Electrical list
electricalDict = list()
# dataLogger list
datLoggerDict = list()
# Irrigation list
irrigationDict = list()

def line2logDict(line):
    dt1, dev1, typo1, msg1 = utils.getInfo(line[:-1])
    return {"realDate": {"__type": "Date", "iso": dt1.isoformat()}, "device": dev1, "type": typo1, "message": msg1, "systemId": ID, "ubication": ubication}

def line2envDict(line):
    dt1, dev1, typo1, msg1 = utils.getInfo(line[:-1])
    obj = {}
    obj["realDate"] = {"__type": "Date", "iso": dt1.isoformat()}
    obj["infoFrom"] = dev1
    obj["systemId"] = ID
    obj["ubication"] = ubication
    if(dev1.startswith("grower")): 
        values = re.findall(r"[-+]?(?:\d*\.\d+|\d+)", msg1)
        if(len(values) == 3):
            values = [float(values[i]) for i in range(len(values))]
            obj["hum"] = values[0]
            obj["temp"] = values[1]
            obj["co2"] = values[2]
            return obj
    elif(dev1.startswith("master")): 
        values = re.findall(r"[-+]?(?:\d*\.\d+|\d+)", msg1)
        if(len(values) == 3):
            values = [float(values[i]) for i in range(len(values))]
            obj["temp"] = values[0]
            obj["hum"] = values[1]
            obj["pressure"] = values[2]
            return obj

def line2ledDict(line):
    dt1, dev1, typo1, msg1 = utils.getInfo(line[:-1])
    obj = {}
    obj["realDate"] = {"__type": "Date", "iso": dt1.isoformat()}
    obj["systemId"] = ID
    obj["ubication"] = ubication
    values = re.findall(r"[-+]?(?:\d*\.\d+|\d+)", msg1)
    if(len(values) == 2):
        values = [float(values[i]) for i in range(len(values))]
        obj["floor"] = int(values[0])
        obj["intensity"] = values[1]
        obj["intensityPercent"] = values[1]/37.5*100
        return obj

def line2sensorDict(line, sensorType):
    dt1, dev1, typo1, msg1 = utils.getInfo(line[:-1])
    obj = {}
    obj["realDate"] = {"__type": "Date", "iso": dt1.isoformat()}
    obj["systemId"] = ID
    obj["ubication"] = ubication
    obj["type"] = sensorType
    
    values = re.findall(r"[-+]?(?:\d*\.\d+|\d+)", msg1)
    if(len(values) == 2):
        values = [float(values[i]) for i in range(len(values))]
        obj["number"] = int(values[0])
        obj["value"] = values[1]
        return obj

def add2electricalDict():
    obj = {}
    obj["realDate"] = {"__type": "Date", "iso": myiHP.firstDateLine.isoformat()}
    obj["systemId"] = ID
    obj["ubication"] = ubication
    obj["infoFrom"] = "iHP"
    obj["voltage"] = myiHP.getVoltage()
    obj["current"] = myiHP.getCurrent()
    myiHP.reset()
    electricalDict.append(obj)

def add2datLoggerDict():
    obj = {}
    obj["realDate"] = {"__type": "Date", "iso": myDataLogger.dTime.isoformat()}
    obj["systemId"] = ID
    obj["ubication"] = ubication
    obj["infoFrom"] = myDataLogger.dev
    obj["value"] = myDataLogger.values
    myDataLogger.reset()
    datLoggerDict.append(obj)

def add2irrigationDict():
    obj = {}
    obj["realDate"] = {"__type": "Date", "iso": myIrrigation.dTime.isoformat()}
    obj["systemId"] = ID
    obj["ubication"] = ubication
    obj["consumption"] = myIrrigation.values
    obj["totalConsumption"] = myIrrigation.totalWater
    myIrrigation.reset()
    irrigationDict.append(obj)

# Process the program between seconds 15 and 30 to be sure log registered correctly the information requested each minute
second = datetime.now().second
while (second<15 or second>30):
    second = datetime.now().second
    sleep(1)
    
# Get lastLine, dateTime and important variables from conf file
if (os.path.exists('log2DB.conf')):
    with open('log2DB.conf', "r") as f:
        data = json.load(f)
        lastLine = data["lastLine"]
        compressor = data["compressor"]
        pump = data["pump"]
        mux1 = data["mux1"]
        myiHP.setData(data["iHP"])
        myDataLogger.setData(data["dataLoggers"])
        myIrrigation.setData(data["irrigationConsumption"])
        lastDatetime = utils.getDateTime(lastLine)
else:
    lastLine = None
    lastDatetime = datetime(1,1,1)
    checkFile = True
    checkLine = True

def checkLog(filePath, number = 0):  
    # Control aux
    global checkFile
    global checkLine

    # State variables
    global compressor
    global pump
    global mux1
    
    if number != 0: filePath += ".{}".format(number)
    
    # Check if filePath dir exists
    if (os.path.exists(filePath)):
        # Get first and last line of file
        with open(filePath, "rb") as f:
            first = f.readline().decode()[:-1]
            last = utils.readlastline(f).decode()[:-1]
        # Get dateTimes from lines
        firstDT = datetime.strptime(first[0:19], '%Y-%m-%d %H:%M:%S')
        lastDT = datetime.strptime(last[0:19], '%Y-%m-%d %H:%M:%S')
        
        # Check if last dateTime in file is between start and end dateTime of log
        if((firstDT<=lastDatetime and lastDT>=lastDatetime) or checkFile):
            print('We have to check database from log file {}'.format(number))
            checkFile = True
            
            # Check line by line
            with open(filePath, "r") as f:
                for line in f:
                    if(checkLine):
                        if(line[0] >= '0' and line[0] <= '9'): # Line has to start as dateTime format
                            lineUpload = False # Control line to know which line has already taken into account
                            dt, dev, typo, msg = utils.getInfo(line[:-1])
                            
                            # Filter master lines
                            if(dev.startswith("master")):
                                # DEBUG lines
                                if(typo.startswith("DEBUG")):
                                    if(msg.startswith("Subscribed topic= {}/#".format(ID))):
                                        #print("INIT MASTER found") # Initialization line
                                        logDict.append(line2logDict(line))
                                        lineUpload = True
                                    elif("C," in msg and "%RH," in msg and "m" in msg):
                                        #(msg) # External conditions line
                                        #logDict.append(line2logDict(line))
                                        envDict.append(line2envDict(line))
                                        lineUpload = True
                                # INFO lines
                                elif(typo.startswith("INFO")):
                                    if(msg.startswith("inputHandler-")):
                                        #print("New Input") # manual inputs to the system
                                        logDict.append(line2logDict(line))
                                        lineUpload = True

                            # Filter generalControl lines
                            # It needs extra clean to avoid incomplete lines
                            elif(dev.startswith("generalControl")):
                                # DEBUG lines
                                if(typo.startswith("DEBUG")):
                                    if((msg.startswith("Solenoid Valve") and "Water Volume" in msg) or (msg.startswith("Solenoid System: Water Consumption"))):
                                        #print(msg) # Gives the last water consumption per solenoid
                                        #logDict.append(line2logDict(line))
                                        lineUpload = True
                                        if(myIrrigation.line2value(line)): add2irrigationDict()
                                    elif(compressor and msg.startswith("(Irrigation) Compressor Controller:") and "Turn Off" in msg):
                                        #print("Compressor off") # Compressor off
                                        compressor = False
                                        logDict.append(line2logDict(line))
                                        lineUpload = True
                                        
                                # INFO lines
                                elif(typo.startswith("INFO")):
                                    if(not compressor and msg.startswith("(Irrigation) Compressor Controller:") and "Pressurize" in msg):
                                        #print("Compressor on") # Compressor on
                                        compressor = True
                                        logDict.append(line2logDict(line))
                                        lineUpload = True
                                    elif(msg.startswith("Sensor: Analog number") and "value" in msg):
                                        #print("New lecture analog sensor") # Number: 0 air, 1 water
                                        #logDict.append(line2logDict(line))
                                        sensorDict.append(line2sensorDict(line, "analog"))
                                        lineUpload = True
                                    elif(msg.startswith("Sensor: Scale number") and "weight" in msg):
                                        #print("New lecture scale") # Number: 0 only scale
                                        #logDict.append(line2logDict(line))
                                        sensorDict.append(line2sensorDict(line, "scale"))
                                        lineUpload = True
                                    elif(msg.startswith("Sensor: Flowmeter number") and "water" in msg):
                                        dt_1, dev_1, typo_1, msg_1 = utils.getInfo(line[:-1])
                                        values = re.findall(r"[-+]?(?:\d*\.\d+|\d+)", msg_1)
                                        if(len(values) == 2 and float(values[1])!=0):
                                            #print("New lecture flowmeter") # Number: 0 only flowmeter
                                            #logDict.append(line2logDict(line))
                                            sensorDict.append(line2sensorDict(line, "flowmeter"))
                                            lineUpload = True
                                        #Sensor:  0 water= 0.00 liters
                                    elif(not pump and msg.startswith("(Irrigation) Recirculation Controller: Pump was turn") and "on" in msg):
                                        #print("Pump on") # Pump on
                                        pump = True
                                        logDict.append(line2logDict(line))
                                        lineUpload = True
                                    elif(pump and msg.startswith("(Irrigation) Recirculation Controller: Pump was turn") and "off" in msg):
                                        #print("Pump off") # Pump off
                                        pump = False
                                        logDict.append(line2logDict(line))
                                        lineUpload = True
                                    elif(not mux1 and msg.startswith("Mux 1: Enabled")):
                                        #print("Mux 1 enabled") # Mux1 enable
                                        mux1 = True
                                        logDict.append(line2logDict(line))
                                        lineUpload = True
                                    elif(mux1 and msg.startswith("Mux 1: Disabled")):
                                        #print("Mux 1 disabled") # Mux1 disable
                                        mux1 = False
                                        logDict.append(line2logDict(line))
                                        lineUpload = True
                            
                            # Filter iHP lines
                            elif(dev.startswith("iHP")):
                                # DEBUG lines
                                if(typo.startswith("DEBUG")):
                                    if(msg.startswith("(iHP PS) Change current from module")):
                                        #print("LED intensity change") # Led intensity change detect module and current
                                        #logDict.append(line2logDict(line))
                                        ledDict.append(line2ledDict(line))
                                        lineUpload = True
                                    elif(msg.startswith("(iHP PS) Current Line")):
                                        #print(msg) # Current input from iHP get lines 1-3
                                        #logDict.append(line2logDict(line))
                                        if(myiHP.line2value(line)): add2electricalDict()
                                        lineUpload = True
                                    elif(msg.startswith("(iHP PS) Voltage Line")):
                                        #print(msg) # Voltage input from iHP get lines 1-3
                                        #logDict.append(line2logDict(line))
                                        if(myiHP.line2value(line)): add2electricalDict()
                                        lineUpload = True
                            
                            # Filter Grower lines
                            elif(dev.startswith("grower")):
                                # DEBUG lines
                                if(typo.startswith("DEBUG")):
                                    if("cozir" in msg):
                                        #print(msg) # Info grower # NOT TESTED
                                        #logDict.append(line2logDict(line))
                                        envDict.append(line2envDict(line))
                                        lineUpload = True
                            # Filter ESP32 lines
                            elif(dev.startswith("esp32")):
                                # DEBUG lines
                                if(typo.startswith("DEBUG")):
                                    if("R=" in msg and "L=" in msg):
                                        #print(msg) # Static sensors environmental conditions
                                        #logDict.append(line2logDict(line))
                                        if(myDataLogger.line2value(line)): add2datLoggerDict()
                                        lineUpload = True
                                    elif(re.search(r"([M][1-9][=])", msg)):
                                        #print(msg) # Door state from sensors
                                        #logDict.append(line2logDict(line))
                                        if(myDataLogger.line2value(line)): add2datLoggerDict()
                                        lineUpload = True
                            
                            # Filter AirConditioner Principal or Return lines
                            elif(dev.startswith("AirPrincipal") or dev.startswith("AirReturn")):
                                # DEBUG lines
                                if(typo.startswith("DEBUG")):
                                    if("Turning on" in msg):
                                        #print(msg) # Turn on air conditioner
                                        logDict.append(line2logDict(line))
                                        lineUpload = True
                                    elif("Turning off" in msg):
                                        #print(msg) # Turn off air conditioner
                                        logDict.append(line2logDict(line))
                                        lineUpload = True
                            
                            if(not lineUpload and typo.startswith("WARNING")):
                                #print(msg) # Warning
                                logDict.append(line2logDict(line))
                                lineUpload = True
                            
                            elif(not lineUpload and typo.startswith("ERROR")):
                                #print(msg) # Warning
                                logDict.append(line2logDict(line))
                                lineUpload = True
                            
                            elif(not lineUpload and typo.startswith("CRITICAL")):
                                #print(msg) # Warning
                                logDict.append(line2logDict(line))
                                lineUpload = True
                                        
                    elif(line[:-1]==lastLine): checkLine = True
                    
            # Save the last data variables before closing the file
            data = {}
            data["lastLine"] = line[:-1]
            data["compressor"] = compressor
            data["pump"] = pump
            data["mux1"] = mux1
            data["iHP"] = myiHP.getData()
            data["dataLoggers"] = myDataLogger.getData()
            data["irrigationConsumption"] = myIrrigation.getData()
            with open('log2DB.conf', "w") as f: json.dump(data, f)
        # Send error
        else: print('Our dateTime is not in log file {}'.format(number))

checkLog('../temp/growMaster.log', 2)
checkLog('../temp/growMaster.log', 1)
checkLog('../temp/growMaster.log', 0)
if(not checkFile): print('No log file to check')

print("We found {}\tupdates for the database Log".format(len(logDict)))
print("We found {}\tupdates for the database Environmental".format(len(envDict)))
print("We found {}\tupdates for the database LedIntensity".format(len(ledDict)))
print("We found {}\tupdates for the database SystemSensor".format(len(sensorDict)))
print("We found {}\tupdates for the database ElectricalStatus".format(len(electricalDict)))
print("We found {}\tupdates for the database DataLoggers".format(len(datLoggerDict)))
print("We found {}\tupdates for the database IrrigationConsumption".format(len(irrigationDict)))

def upload2DB(myList, myClass):
    for i in range(0, len(myList), 100):
        if(i+100>len(myList)):
            r = client.createObject(myClass, myList[i:])
            print(r)
        else: client.createObject(myClass, myList[i:i+100])

upload2DB(logDict, "Log")
upload2DB(envDict, "Environmental")
upload2DB(ledDict, "LedIntensity")
upload2DB(sensorDict, "SystemSensor")
upload2DB(electricalDict, "ElectricalStatus")
upload2DB(datLoggerDict, "DataLoggers")
upload2DB(irrigationDict, "IrrigationConsumption")