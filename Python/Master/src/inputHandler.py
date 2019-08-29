#!/usr/bin/env python3

# Import directories
import sys
import select
import paho.mqtt.publish as publish

class inputHandler:
    def __init__(self, logger, serialController):
        # Define logger
        self.log = logger
        # Define communication controllers
        self.serialControl = serialController
        
    # Get an Input Line it consumes between 20-30% of the processor capacity
    def getLine(self, Block=False):
      if Block or select.select([sys.stdin], [], [], 0) == ([sys.stdin], [], []):
          return input()
    
    def writeGC(self, mssg): # Write in generalControl
        self.serialControl.write(self.serialControl.generalControl, mssg)
    
    def writeMG(self, mssg): # Write in motorsGrower
        self.serialControl.write(self.serialControl.motorsGrower, mssg)
        
    def writeSM(self, mssg): # Write in solutionMaker
        self.serialControl.write(self.serialControl.solutionMaker, mssg)
    
    def valInteger(self, integer):
        try:
            val = int(integer)
            return True
        except:
            self.log.error("inputHandler- {} is not an integer".format(integer))
            return False
    
    def valGreater0(self, number):
        if(number>0): return True
        else:
            self.log.error("inputHandler- {} is not greater than 0".format(number))
            return False
    
    def valLenList(self, myList, reqLen):
        if(len(myList)==reqLen): return True
        else:
            self.log.error("inputHandler- {} has not the correct length".format(myList))
            return False
    
    def valSplit(self, strLine):
        splitLine = strLine.split(",")
        if(len(splitLine)>1):
            return splitLine
        else: self.log.error("inputHandler- {} needs more arguments".format(strLine))
            
    def handleInput(self, line):
        if(line=="exit"):
            self.log.warning("inputMode exit")
        elif(line.startswith("irr")):
            param = self.valSplit(line)
            if(param!=None):
                if(param[1].startswith("en")):
                    self.writeGC("solenoid,enableGroup,1")
                    self.log.warning("Irrigation - Enable")
                elif(param[1].startswith("dis")):
                    self.writeGC("solenoid,enableGroup,0")
                    self.log.warning("Irrigation - Disable")
                elif(param[1].startswith("setCyc")):
                    if(self.valLenList(param,3) and self.valInteger(param[2]) and
                       self.valGreater0(int(param[2]))):
                        cycleTime = int(param[2])
                        self.writeGC("solenoid,setCycleTime,{}".format(cycleTime))
                        self.log.warning("Irrigation- Change cycle time to {} min".format(cycleTime))
                elif(param[1].startswith("setTim")):
                    if(self.valLenList(param,6) and self.valInteger(param[2]) and
                       self.valInteger(param[3]) and self.valInteger(param[4]) and
                       self.valInteger(param[5])):
                        fl = int(param[2])
                        reg = int(param[3])
                        sol = int(param[4])
                        time_s = int(param[5])
                        self.writeGC("solenoid,setTimeOn,{},{},{},{}".format(fl, reg, sol, time_s))
                        self.log.warning("Irrigation- Solenoid of fl{}, reg{} change time to {}s with sol{}".format(
                            fl, reg, time_s, sol))
                else: self.log.warning("inputHandler- {} does not match a type".format(line))
        if(line.startswith("debug")):
            param = self.valSplit(line)
            if(param!=None):
                if(param[1].startswith("whatSol")):
                    self.writeGC("debug,irrigation,whatSolution")
                    self.log.warning("Debug - Asking for next solution")
                elif(param[1].startswith("getEC")):
                    self.writeGC("debug,irrigation,getEC")
                    self.log.warning("Debug - Asking for the actual EC parameter")
        else: self.log.warning("inputHandler- {} does not match a type".format(line))
            
    def loop(self):
        line = self.getLine()
        if line!=None:
            self.handleInput(line)
    
        
