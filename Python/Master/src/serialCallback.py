#!/usr/bin/env python3

# Import directories
from serial import Serial
from time import time, sleep
from systemState import systemState

class serialController:
    def __init__(self, multiGrower, logger, stateFile):
        # Define loggers
        self.logMain = logger.logger
        self.logGC = logger.logger_generalControl
        self.logMG = []
        for i in range(len(logger.logger_motorsGrower)): self.logMG.append(logger.logger_motorsGrower[i])
        self.logGrower = logger.logger_grower

        # Variable to export Eeprom from GC
        self.exportEeprom = False
        self.importEeprom = 0
        # Define microcontrolers
        try:
            self.generalControl = Serial('/dev/generalControl', 115200, timeout=0)
            self.generalControl.dtr = False # Reset
            self.generalControl.close()
            self.gcIsConnected = True
        except Exception as e:
            self.gcIsConnected = False
            raise Exception("Communication with generalControl device cannot be stablished. [{}]".format(e))
        
        self.motorsGrower = []
        self.mgIsConnected = []
        for i in range(len(self.logMG)):
            try:
                self.motorsGrower.append(Serial('/dev/motorsGrower{}'.format(i+1), 115200, timeout=0))
                self.motorsGrower[i].dtr = False # Reset
                self.motorsGrower[i].close()
                self.mgIsConnected.append(True)
            except Exception as e:
                self.motorsGrower.append(False)
                self.mgIsConnected.append(False)
                self.logMain.error("Communication with motorsGrower{} device cannot be stablished. [{}]".format(i+1, e))
        
        # Define multiGrower variables with mqtt module
        self.mGrower = multiGrower
        # Define responses auxVariables
        self.resp = []
        self.respLine = []
        self.respTime = time()
        # Charge system state
        self.system = systemState(stateFile)
        if(self.system.load()): self.logMain.info("System State charged")
        else: self.logMain.error("Cannot charge System State because it does not exist")

    def open(self):
        if self.gcIsConnected and not self.generalControl.is_open:
            self.generalControl.open()
            sleep(0.33)
            self.generalControl.reset_input_buffer()
            self.generalControl.dtr = True
        for i in range(len(self.motorsGrower)):
            if self.mgIsConnected[i] and not self.motorsGrower[i].is_open:
                self.motorsGrower[i].open()
                sleep(0.33)
                self.motorsGrower[i].reset_input_buffer()
                self.motorsGrower[i].dtr = True

    def close(self):
        if self.gcIsConnected and self.generalControl.is_open: self.generalControl.close()
        for i in range(len(self.motorsGrower)):
            if self.mgIsConnected[i] and self.motorsGrower[i].is_open: self.motorsGrower[i].close()

    def Msg2Log(self, logger, mssg):
        if(mssg.startswith("debug,")): logger.debug(mssg.split(",")[1])
        elif(mssg.startswith("info,")): logger.info(mssg.split(",")[1])
        elif(mssg.startswith("warning,")): logger.warning(mssg.split(",")[1])
        elif(mssg.startswith("error,")): logger.error(mssg.split(",")[1])
        elif(mssg.startswith("critical,")): logger.critical(mssg.split(",")[1])
        else: logger.debug(mssg)

    def write(self, serialObject, mssg):
        if serialObject == self.generalControl and self.gcIsConnected: aux = True
        for i in range(len(self.motorsGrower)):
            if serialObject == self.motorsGrower[i] and self.mgIsConnected[i]: aux = True
        if aux:
            serialObject.write(bytes(mssg, "utf-8"))
            serialObject.flush()
        else: self.logMain.error("Cannot write to serial device. It is disconnected.")

    def cleanLine(self, line):
        resp = line.split(",")
        if len(resp)>1: return resp[1]
        else: return resp[0]

    def detectGrower(self, line):
        for i in range(len(self.logGrower)):
            if(line.startswith("Grower{}".format(i+1))): return i+1
        return 0

    def cleanGrowerLine(self, line):
        try:
            resp = line.split(":")[1][1:]
            return resp
        except Exception as e:
            self.logMain.error("Error cleaning grower line {}. Line that failed is {}".format(e, line))
            return ""
        
    def cleanMaxDistanceLine(self, line):
        try:
            resp1 = line.split(" - ")[0][5:]
            resp2 = line.split(" - ")[1][5:]
            return resp1, resp2
        except Exception as e:
            self.logMain.error("Error cleaning MaxDistanceLine {}. Line that failed is {}".format(e, line))
            return "", ""
            
    def getGrowerLine(self, line):
        resp = self.cleanLine(line)
        num = self.detectGrower(resp)
        return resp, num
    
    def getConfigParameters(self, line):
        resp = self.cleanLine(line)
        respX, respY = self.cleanMaxDistanceLinePos(resp)
        return respX, respY
        
    def stopGrower(self, fl):
        serialFloor = self.mGrower.data[str(fl)]
        if serialFloor != "disconnected":
            serialDevice = int((serialFloor)/4)
            self.write(self.motorsGrower[serialDevice], "stop,{}".format(serialFloor))
            self.logMain.info(("Grower{} is busy, sending request to stop".format(fl)))
        else: self.logMain.warning("Grower{} is disconnected cannot stop sequence in that floor".format(fl))
        
    def decideStartOrStopGrower(self, resp, serialDevice):
        auxBool = False
        num = self.detectGrower(resp) + serialDevice*4
        for i in range(len(self.mGrower.data)):
            if(self.mGrower.data[str(i+1)] == str(num)):
                num = i
                break
        if num > 0: auxBool = self.mGrower.Gr[num].startRoutine
        if(num>0 and num<=len(self.logGrower)): resp = self.cleanGrowerLine(resp)

        if resp.startswith("Available") and auxBool:
            # Send via MQTT Master Ready
            self.mGrower.Gr[num].serialReq("")
            self.mGrower.Gr[num].mqttReq("MasterReady")
            self.mGrower.Gr[num].actualTime = time() - 20
            return True
        elif resp.startswith("Unavailable") and auxBool:
            self.stopGrower(num)
            return True

        return False

    def GrowerInRoutine(self, fl):
        # Start the routine
        if(fl>0 and fl<=len(self.logGrower)):
            self.mGrower.Gr[fl-1].startRoutine = False
            self.mGrower.Gr[fl-1].inRoutine = True
            self.mGrower.Gr[fl-1].count = 0
            self.logMain.info("Grower{} sequence started".format(fl))
        else: self.logMain.error("GrowerInRoutine(): Grower{} does not exist".format(fl))

    def GrowerInPosition(self, fl):
        # Request Grower to take pictures
        if(fl>0 and fl<=len(self.logGrower)):
            self.mGrower.Gr[fl-1].count += 1
            self.mGrower.Gr[fl-1].serialReq("")
            self.mGrower.Gr[fl-1].mqttReq("takePicture")
            self.mGrower.Gr[fl-1].actualTime = time()-20
            self.logMain.debug("Grower{} in position to take photo".format(fl))
        else: self.logMain.error("GrowerInPosition(): Grower{} does not exist".format(fl))

    def GrowerRoutineFinish(self, fl):
        # Finish the routine
        if(fl>0 and fl<=len(self.logGrower)):
            self.mGrower.Gr[fl-1].count = 1
            self.mGrower.Gr[fl-1].mqttReq("RoutineFinished")
            self.mGrower.Gr[fl-1].serialReq("")
            self.mGrower.Gr[fl-1].inRoutine = False
            self.mGrower.Gr[fl-1].startRoutine = False
            self.mGrower.Gr[fl-1].count = 1
            self.mGrower.Gr[fl-1].actualTime = time()-20
            self.logMain.info("Grower{} routine finished".format(fl))
        else: self.logMain.error("GrowerRoutineFinish(): Grower{} does not exist".format(fl))

    def sendBootParams(self):
        self.write(self.generalControl, "boot,{0},{1}".format(self.system.state["controlState"], self.system.state["consumptionH2O"] ))

    def updateSystemState(self, index):
        param = self.respLine[index].split(",")
        if(len(param)>=2 and param[-1]!=''):
            if(self.system.update("controlState", float(param[1]))): pass
            else: self.logMain.error("Cannot Update controlState")
            if(self.system.update("consumptionH2O", float(param[2]))): pass
            else: self.logMain.error("Cannot Update consumptionH2O")

        else: self.logMain.error("Line incomplete - {}".format(self.respLine[index]))

    def writeEeprom(self):
        # open file
        with open("eeprom.config",'r') as conf:
            lines = conf.readlines()
            if(self.importEeprom<len(lines)):
                splitLine = lines[self.importEeprom].split(":")
                self.importEeprom += 1
                aux = "eeprom,write,{},{}".format(splitLine[0], splitLine[1])
                print(aux[:-1])
                self.concatResp("importEeprom", aux)
            else:
                self.importEeprom = 0
                self.logMain.info("Import eeprom to generalControl finished")

    def concatResp(self, resp, line):
        # If that request is not save
        if not resp in self.resp:
            self.resp.append(resp) # Add the request
            self.respLine.append(line) # Add the line
            self.respTime = time() # Restart timer

    def response(self):
        if not self.gcIsConnected or self.generalControl.in_waiting==0: gControl = True
        else: gControl = False
        motorG = []
        for i in range(len(self.mgIsConnected)):
            if not self.mgIsConnected[i] or self.motorsGrower[i].in_waiting==0: motorG.append(True)
            else: motorG.append(False)

        if(time()-self.respTime>2 and gControl and all(motorG) and len(self.resp)>0):
            for i, resp in enumerate(self.resp):
                # generalControl is requesting the necessary booting parameters
                if(resp == "boot"): self.sendBootParams()
                # Update system state
                elif(resp == "updateSystemState"): self.updateSystemState(i)
                # Import EEPROM
                elif(resp == "importEeprom"): self.write(self.generalControl, self.respLine[i])

                self.logMain.debug("Request {} was answered".format(resp))

            self.resp = []
            self.respLine = []

    def loop(self):
        if self.gcIsConnected:
            try:
                # Called the next Eeprom import line
                if (not "importEeprom" in self.resp and self.importEeprom>0 and self.importEeprom<=1000): self.writeEeprom()

                # If bytes available in generalControl
                while self.generalControl.in_waiting>0:
                    line1 = str(self.generalControl.readline(), "utf-8")[0:-1]

                    # Check if we are not exporting eeprom to file
                    if(self.exportEeprom and not line1.startswith("info,EXPORT EEPROM FINISHED")):
                        if(not line1.startswith("info")):
                            with open("eeprom.config", "a") as f:
                                print(line1[:-1])
                                f.write(line1)
                    else:
                        self.Msg2Log(self.logGC, line1)

                    if(line1.startswith("boot")):
                        self.concatResp("boot", line1)
                    elif(line1.startswith("updateSystemState")):
                        self.concatResp("updateSystemState", line1)
                    elif(line1.startswith("info,EXPORT EEPROM STARTED")):
                        open('eeprom.config', 'w').close()
                        self.exportEeprom = True
                    elif(line1.startswith("info,EXPORT EEPROM FINISHED")):
                        self.exportEeprom = False
            
            except UnicodeDecodeError as e: self.logGC.error(e)

        for i in range(len(self.mgIsConnected)):
            if self.mgIsConnected[i]:
                try:
                    # If bytes available in motorsGrower
                    while self.motorsGrower[i].in_waiting>0:
                        line2 = str(self.motorsGrower[i].readline(), "utf-8")[0:-1]
                        self.Msg2Log(self.logMG[i], line2)
    
                        decition = False
                        for j in range(len(self.mGrower.Gr)):
                            # If we are waiting a particular response from GrowerN
                            if(self.mGrower.Gr[j].serialRequest!="" and not decition):
                                decition = self.decideStartOrStopGrower(self.cleanLine(line2), i)

                            # If we are waiting GrowerN to reach home and start the sequence
                            if(self.mGrower.Gr[j].serialRequest.startswith("sequence") and self.mGrower.Gr[j].startRoutine and not decition):
                                resp, num = self.getGrowerLine(line2)
                                num += i*4 # Add 4 for each serialDevice to get the correct growerNumber
                                if(num==int(self.mGrower.data[str(j+1)])):
                                    resp = self.cleanGrowerLine(resp)
                                    if resp.startswith("Starting Routine Stage 2"):
                                        decition = True
                                        self.GrowerInRoutine(j+1)
                            
                            # If we are waiting GrowerN to reach next sequence position
                            if(self.mGrower.Gr[j].inRoutine and not decition):
                                resp, num = self.getGrowerLine(line2)
                                num += i*4 # Add 4 for each serialDevice to get the correct growerNumber
                                if(num==int(self.mGrower.data[str(j+1)])):
                                    resp = self.cleanGrowerLine(resp)
                                    if resp.startswith("In Position"):
                                        decition = True
                                        self.GrowerInPosition(j+1)
                                    elif resp.startswith("Routine Finished"):
                                        decition = True
                                        self.GrowerRoutineFinish(j+1)
                                        
                            # If we are waiting GrowerN RespMax
                            if(line2.startswith("warning")):
                                resp, num = self.getGrowerLine(line2)
                                num += i*4 # Add 4 for each serialDevice to get the correct growerNumber
                                resp1 = self.cleanGrowerLine(resp)
                                if resp1.startswith("max"):
                                    respX, respY = self.cleanMaxDistanceLine(resp1)
                                    print(respX,respY)
                                    self.mGrower.Gr[num-1].maxX = respX
                                    self.mGrower.Gr[num-1].maxY = respY
                                elif resp1.startswith("pos"):
                                    respX, respY = self.cleanMaxDistanceLine(resp1)
                                    print(respX,respY)
                                    self.mGrower.Gr[num-1].posX = respX
                                    self.mGrower.Gr[num-1].posY = respY                                                                            
                                        
                except UnicodeDecodeError as e: self.logMG[i].error(e)
                
        # Send all responses
        self.response()
