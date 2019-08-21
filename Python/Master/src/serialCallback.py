#!/usr/bin/env python3

# Import directories
import os
from serial import Serial
from time import strftime, localtime

# Define files paths
actualDirectory = os.getcwd()
if(actualDirectory.endswith('src')): logPath = "../log/"
else: logPath = "./log/"
    
# Define microcontrolers
generalControl = Serial('/dev/generalControl', 115200, timeout=1)
generalControl.close()
motorsGrower = Serial('/dev/motorsGrower', 115200, timeout=1)
motorsGrower.close()
solutionMaker = Serial('/dev/solutionMaker', 115200, timeout=1)
solutionMaker.close()

def saveLog(device, line):
    logP = logPath + device
    logF = logP + "/{}.log".format(strftime("%Y-%m-%d", localtime()))
    # Check if directory exists
    if not os.path.exists(logP): os.makedirs(logP)
    # Save log
    with open(logF, "a+") as f:
        f.write("\n{} - ".format(strftime("%H:%M:%S", localtime())) + line)
        f.close()
        
def loop():
    # If bytes available in generalControl
    while generalControl.inWaiting()>0:
        line1 = str(generalControl.readline(), "utf-8")
        saveLog("generalControl", line1[0:-1])
        
        # Debug
        #print("generalControl:", line1, end="")
        
    # If bytes available in motorsGrower
    while motorsGrower.inWaiting()>0:
        line2 = str(motorsGrower.readline(), "utf-8")
        saveLog("motorsGrower", line2[0:-1])
            
        # Debug
        #print("motorsGrower:", line2, end="")
        
    # If bytes available in solutionMaker
    while solutionMaker.inWaiting()>0:
        line3 = str(solutionMaker.readline(), "utf-8")
        saveLog("solutionMaker", line3[0:-1])
            
        # Debug
        #print("solutionMaker:", line, end="")