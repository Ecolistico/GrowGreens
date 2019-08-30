#!/usr/bin/env python3

# Import directories
import sys
import select
import paho.mqtt.publish as publish

class inputHandler:
    def __init__(self, logger, serialController, mqttController):
        # Define logger
        self.log = logger
        # Define communication controllers
        self.serialControl = serialController
        self.mqttControl = mqttController
        # Aux Variables
        self.exit = False
        
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
            self.log.error("inputHandler- {} does not have the correct length".format(myList))
            return False
    
    def valLenList1(self, myList, minLen):
        if(len(myList)>=minLen): return True
        else:
            self.log.error("inputHandler- {} does not have the minimun length".format(myList))
            return False
        
    def valSplit(self, strLine):
        splitLine = strLine.split(",")
        if(len(splitLine)>1):
            return splitLine
        else: self.log.error("inputHandler- {} needs more arguments".format(strLine))
            
    def handleInput(self, line):
        if(line.lower()=="exit"):
            self.log.info("Exit command activated")
            self.exit = True
        elif(line.startswith("raw")):
            param = self.valSplit(line)
            if(param!=None):
                if(param[1]=="generalControl" and self.valLenList1(param, 3)):
                    cmd = ",".join(param[2:])
                    self.writeGC(cmd)
                    self.log.warning("inputHandler-[generalControl] Raw Command={}".format(cmd))          
                elif(param[1]=="motorsGrower" and self.valLenList1(param, 3)):
                    cmd = ",".join(param[2:])
                    self.writeMG(cmd)
                    self.log.warning("inputHandler-[motorsGrower] Raw Command={}".format(cmd)) 
                elif(param[1]=="solutionMaker" and self.valLenList1(param, 3)):
                    cmd = ",".join(param[2:])
                    self.writeSM(cmd)
                    self.log.warning("inputHandler-[solutionMaker] Raw Command={}".format(cmd))
                elif( (param[1]=="esp32" or param[1]=="Grower") and self.valLenList1(param, 4)):
                    cmd = ",".join(param[3:])
                    topic = "{}/{}{}".format(self.mqttControl.ID, param[1], param[2])
                    publish.single(topic, "{}".format(cmd), hostname = self.mqttControl.brokerIP)
                    self.log.warning("inputHandler-[mqtt] Raw Command Topic={} Message={}".format(
                        topic,cmd))
                else: self.log.warning("inputHandler- {} Command Unknown".format(line))
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
                        self.writeGC("solenoid,setTimeOn,{},{},{},{}".format(fl-1, reg-1, sol, time_s))
                        self.log.warning("Irrigation- Solenoid of fl {}, reg {} change time to {}s with sol {}".format(
                            fl, reg, time_s, sol))
                else: self.log.warning("inputHandler- {} Command Unknown".format(line))
        
        elif(line.startswith("debug")):
            param = self.valSplit(line)
            if(param!=None):
                if(param[1].startswith("whatSol")):
                    self.writeGC("debug,irrigation,whatSolution")
                    self.log.warning("Debug - Asking for next solution")
                elif(param[1].startswith("getEC")):
                    self.writeGC("debug,irrigation,getEC")
                    self.log.warning("Debug - Asking for the actual EC parameter")
                else: self.log.warning("inputHandler- {} Command Unknown".format(line))
        else: self.log.warning("inputHandler- {} Command Unknown".format(line))
            
    def loop(self):
        line = self.getLine()
        if line!=None:
            self.handleInput(line)
    
        
