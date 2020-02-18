#!/usr/bin/env python3

# Import directories
import sys
import select
import paho.mqtt.publish as publish

class inputHandler:
    def __init__(self, systemID, brokerIP, logger, serialController, mqttController, gui):
        # Publish require varaibles
        self.ID = systemID
        self.IP = brokerIP
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
        if self.serialControl.gcIsConnected: self.serialControl.write(self.serialControl.generalControl, mssg)
        else: self.log.error("Cannot write to serial device [generalControl]. It is disconnected.")
            
    def writeMG(self, mssg): # Write in motorsGrower
        if self.serialControl.mgIsConnected: self.serialControl.write(self.serialControl.motorsGrower, mssg)
        else: self.log.error("Cannot write to serial device [motorsGrower]. It is disconnected.")
            
    def writeSM(self, mssg): # Write in solutionMaker
        if self.serialControl.smIsConnected: self.serialControl.write(self.serialControl.solutionMaker, mssg)
        else: self.log.error("Cannot write to serial device [solutionMaker]. It is disconnected.")
            
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
                        self.gui.window['cycleTime'].Update(value=self.gui.cycleTime)
                        self.gui.window['Actualizar Ciclo'].Update(button_color=self.gui.disable_color, disabled=True)
                        self.gui.rewriteCSV(self.gui.filename, self.gui.header_list, self.gui.data)
                        if self.gui.piso!='' and self.gui.lado!='' and self.gui.etapa!='' and self.gui.solucion!='':
                            evMin, evMax = self.gui.getEVlimits(self.gui.cycleTime, int(self.gui.etapa))
                            self.gui.window['evTime'].Update(range=(evMin, evMax))
                            self.gui.window['TiempoCiclo'].Update(self.gui.data[-1][1]+' min')
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
                        self.gui.window['Actualizar'].Update(button_color=self.gui.disable_color, disabled=True)
                        self.gui.total = self.gui.getTotalIrrigationTime(self.gui.data)
                        self.gui.window['cycleTime'].Update(range=(int(self.gui.total/30)+1, 20))
                        self.gui.rewriteCSV(self.gui.filename, self.gui.header_list, self.gui.data)
                        self.gui.updateCurrentTimeValues()
                    else: self.log.error("inputHandler Error: GUI not allow changes")
                except Exception as e: self.log.error("inputHandler Error: Connecction with GUI failed [{}]".format(e))
    
    def handleInput(self, line):
        if(line.lower()=="exit"):
            self.log.debug("Exit command activated")
            self.exit = True
        elif(line.startswith("raw")):
            param = self.valSplit(line)
            if(param!=None):
                if(param[1]=="generalControl" and self.valLenList1(param, 3) and self.serialControl.gcIsConnected):
                    cmd = ",".join(param[2:])
                    self.writeGC(cmd)
                    self.log.info("inputHandler-[generalControl] Raw Command={}".format(cmd))
                    self.handleGUI(cmd, param)
                elif(param[1]=="motorsGrower" and self.valLenList1(param, 3) and self.serialControl.mgIsConnected):
                    cmd = ",".join(param[2:])
                    self.writeMG(cmd)
                    self.log.info("inputHandler-[motorsGrower] Raw Command={}".format(cmd)) 
                elif(param[1]=="solutionMaker" and self.valLenList1(param, 3) and self.serialControl.smIsConnected):
                    cmd = ",".join(param[2:])
                    self.writeSM(cmd)
                    self.log.info("inputHandler-[solutionMaker] Raw Command={}".format(cmd))
                elif( (param[1]=="esp32" or param[1]=="Grower") and self.valLenList1(param, 4)):
                    cmd = ",".join(param[3:])
                    topic = "{}/{}{}".format(self.mqttControl.ID, param[1], param[2])
                    try:
                        publish.single(topic, "{}".format(cmd), hostname = self.mqttControl.brokerIP)
                        self.log.info("inputHandler-[mqtt] Raw Command Topic={} Message={}".format(
                            topic,cmd))
                    except Exception as e:
                        self.log.error("LAN/WLAN not found- Impossible use publish() [{}]".format(e))
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
        
        elif(line.startswith("startRoutine")):
            if self.serialControl.mgIsConnected:
                param = self.valSplit(line)
                if(param!=None and len(param)>=2):
                    fl = param[1]
                    if(len(param)==4):
                        x = param[2]
                        y = param[3]
                    elif(len(param)==3): x = y = param[2]
                    else: x = y = 0
                    
                    if(fl=='1'):
                        if(x!=0 and y!=0):
                            self.serialControl.mGrower.Gr1.xSeq = x
                            self.serialControl.mGrower.Gr1.ySeq = y
                        mssg = self.serialControl.mGrower.Gr1.time2Move()
                    elif(fl=='2'):
                        if(x!=0 and y!=0):
                            self.serialControl.mGrower.Gr2.xSeq = x
                            self.serialControl.mGrower.Gr2.ySeq = y
                        mssg = self.serialControl.mGrower.Gr2.time2Move()
                    elif(fl=='3'):
                        if(x!=0 and y!=0):
                            self.serialControl.mGrower.Gr3.xSeq = x
                            self.serialControl.mGrower.Gr3.ySeq = y
                        mssg = self.serialControl.mGrower.Gr3.time2Move()
                    elif(fl=='4'):
                        if(x!=0 and y!=0):
                            self.serialControl.mGrower.Gr4.xSeq = x
                            self.serialControl.mGrower.Gr4.ySeq = y
                        mssg = self.serialControl.mGrower.Gr4.time2Move()
                    else:
                        mssg = ''
                        self.log.error("Please provide a valid floor to start Grower sequence")
                    if mssg != '':
                        try:
                            top = "{}/Grower{}".format(self.ID,fl)
                            msgs = [{"topic": top, "payload": "OnLED1"},
                                    {"topic": top, "payload": "OnLED2"},
                                    {"topic": top, "payload": "DisableStream"}]
                            publish.multiple(msgs, hostname = self.IP)
                            self.log.info("Checking Grower{} status to start sequence".format(fl))
                        except Exception as e:
                            self.log.error("LAN/WLAN not found- Impossible use publish() [{}]".format(e))
                    else: self.log.error("Please provide a valid floor to start Grower sequence")
                else: self.log.error("Please provide valid arguments to start Grower sequence")
            else: self.log.error("motorsGrower device is disconnected. It is impossible to start a routine or sequence.")
        else: self.log.error("inputHandler- {} Command Unknown".format(line))
            
    def loop(self):
        line = self.getLine()
        if line!=None:
            self.handleInput(line)
