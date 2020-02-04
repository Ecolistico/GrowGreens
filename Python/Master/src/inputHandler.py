#!/usr/bin/env python3

# Import directories
import sys
import select
import paho.mqtt.publish as publish

class inputHandler:
    def __init__(self, logger, serialController, mqttController, gui):
        # Define logger
        self.log = logger
        # Define communication controllers
        self.serialControl = serialController
        self.mqttControl = mqttController
        # Define connection with gui
        self.gui = gui
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
    
    def handleGUI(self, command, parameters):
        if self.gui.isOpen:
            if(command.startswith("solenoid,setCycleTime,")):
                try:
                    val = parameters[-1]
                    self.gui.total = self.gui.getTotalIrrigationTime(self.gui.data)
                    if self.gui.total<=(int(val)*60):
                        self.gui.cycleTime = int(val)
                        self.gui.data[-1][1] = str(self.gui.cycleTime)
                        self.gui.window['Table'].Update(values=self.gui.data)
                        self.gui.window['cycleTime'].Update(value=self.gui.cycleTime)
                        self.gui.window['Actualizar Ciclo'].Update(button_color=self.gui.disable_color, disabled=True)
                        self.gui.rewriteCSV(self.gui.filename, self.gui.header_list, self.gui.data)
                        if self.gui.piso!='' and self.gui.lado!='' and self.gui.etapa!='' and self.gui.solucion!='':
                            evMin, evMax = self.gui.getEVlimits(self.gui.cycleTime, int(self.gui.etapa))
                            self.gui.window['evTime'].Update(range=(evMin, evMax))
                    else: self.log.error("inputHandler Error: GUI not allow changes")
                except Exception as e: self.log.error("inputHandler Error: Connecction with GUI failed [{}]".format(e))
                        
            elif(command.startswith("solenoid,setTimeOn,") and len(parameters)>=8):
                try:
                    floor = int(parameters[4])+1
                    region = int(parameters[5])
                    if(region<4):
                        side = 'A'
                        region += 1
                    else:
                        side = 'B'
                        region -= 3
                    sol = int(parameters[6])
                    if sol == 0: solution = 'H2O'
                    else: solution = str(sol)
                    val = parameters[7]
                    resp = self.gui.changeEVvalue(floor, side, region, solution,
                                                  self.gui.data, self.gui.cycleTime, int(val), factor=1)
                    if(resp):
                        if(floor==int(self.gui.piso) and side==self.gui.lado and region==int(self.gui.etapa) and solution==self.gui.solucion):
                            self.gui.evTime = int(val)
                            self.gui.window['evTime'].Update(value=self.gui.evTime)
                        self.gui.window['Table'].Update(values=self.gui.data)
                        self.gui.window['Actualizar'].Update(button_color=self.gui.disable_color, disabled=True)
                        self.gui.total = self.gui.getTotalIrrigationTime(self.gui.data)
                        self.gui.window['cycleTime'].Update(range=(int(self.gui.total/30)+1, 20))
                        self.gui.rewriteCSV(self.gui.filename, self.gui.header_list, self.gui.data)
                    else: self.log.error("inputHandler Error: GUI not allow changes")
                except Exception as e: self.log.error("inputHandler Error: Connecction with GUI failed [{}]".format(e))
    
    def handleInput(self, line):
        if(line.lower()=="exit"):
            self.log.debug("Exit command activated")
            self.exit = True
        elif(line.startswith("raw")):
            param = self.valSplit(line)
            if(param!=None):
                if(param[1]=="generalControl" and self.valLenList1(param, 3)):
                    cmd = ",".join(param[2:])
                    self.writeGC(cmd)
                    self.log.info("inputHandler-[generalControl] Raw Command={}".format(cmd))
                    self.handleGUI(cmd, param)
                             
                elif(param[1]=="motorsGrower" and self.valLenList1(param, 3)):
                    cmd = ",".join(param[2:])
                    self.writeMG(cmd)
                    self.log.info("inputHandler-[motorsGrower] Raw Command={}".format(cmd)) 
                elif(param[1]=="solutionMaker" and self.valLenList1(param, 3)):
                    cmd = ",".join(param[2:])
                    self.writeSM(cmd)
                    self.log.info("inputHandler-[solutionMaker] Raw Command={}".format(cmd))
                elif( (param[1]=="esp32" or param[1]=="Grower") and self.valLenList1(param, 4)):
                    cmd = ",".join(param[3:])
                    topic = "{}/{}{}".format(self.mqttControl.ID, param[1], param[2])
                    publish.single(topic, "{}".format(cmd), hostname = self.mqttControl.brokerIP)
                    self.log.info("inputHandler-[mqtt] Raw Command Topic={} Message={}".format(
                        topic,cmd))
                else: self.log.info("inputHandler- {} Command Unknown".format(line))
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
                else: self.log.error("inputHandler- {} Command Unknown".format(line))
        
        elif(line.startswith("debug")):
            param = self.valSplit(line)
            if(param!=None):
                if(param[1].startswith("whatSol")):
                    self.writeGC("debug,irrigation,whatSolution")
                    self.log.debug("Asking for next solution")
                elif(param[1].startswith("getEC")):
                    self.writeGC("debug,irrigation,getEC")
                    self.log.debug("Asking for the actual EC parameter")
                else: self.log.error("inputHandler- {} Command Unknown".format(line))
        else: self.log.error("inputHandler- {} Command Unknown".format(line))
            
    def loop(self):
        line = self.getLine()
        if line!=None:
            self.handleInput(line)
