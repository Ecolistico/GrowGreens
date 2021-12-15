#!/usr/bin/env python3

# Import directories
from serial import Serial
from time import time, sleep
from systemState import systemState

class serialController:
    def __init__(self, multiGrower, loggerMain, loggerGC, loggerMG1, loggerMG2, stateFile):
        # Define loggers
        self.logMain = loggerMain
        self.logGC = loggerGC
        self.logMG1 = loggerMG1
        self.logMG2 = loggerMG2

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
        try:
            self.motorsGrower1 = Serial('/dev/motorsGrower1', 115200, timeout=0)
            self.motorsGrower1.dtr = False # Reset
            self.motorsGrower1.close()
            self.mg1IsConnected = True
        except Exception as e:
            self.mg1IsConnected = False
            self.logMain.error("Communication with motorsGrower1 device cannot be stablished. [{}]".format(e))
        try:
            self.motorsGrower2 = Serial('/dev/motorsGrower2', 115200, timeout=0)
            self.motorsGrower2.dtr = False # Reset
            self.motorsGrower2.close()
            self.mg2IsConnected = True
        except Exception as e:
            self.mg2IsConnected = False
            self.logMain.error("Communication with motorsGrower2 device cannot be stablished. [{}]".format(e))

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
        if self.mg1IsConnected and not self.motorsGrower1.is_open:
            self.motorsGrower1.open()
            sleep(0.33)
            self.motorsGrower1.reset_input_buffer()
            self.motorsGrower1.dtr = True
        if self.mg2IsConnected and not self.motorsGrower2.is_open:
            self.motorsGrower2.open()
            sleep(0.33)
            self.motorsGrower2.reset_input_buffer()
            self.motorsGrower2.dtr = True

    def close(self):
        if self.gcIsConnected and self.generalControl.is_open: self.generalControl.close()
        if self.mg1IsConnected and self.motorsGrower1.is_open: self.motorsGrower1.close()
        if self.mg2IsConnected and self.motorsGrower2.is_open: self.motorsGrower2.close()

    def Msg2Log(self, logger, mssg):
        if(mssg.startswith("debug,")): logger.debug(mssg.split(",")[1])
        elif(mssg.startswith("info,")): logger.info(mssg.split(",")[1])
        elif(mssg.startswith("warning,")): logger.warning(mssg.split(",")[1])
        elif(mssg.startswith("error,")): logger.error(mssg.split(",")[1])
        elif(mssg.startswith("critical,")): logger.critical(mssg.split(",")[1])
        else: logger.debug(mssg)

    def write(self, serialObject, mssg):
        if serialObject == self.generalControl and self.gcIsConnected: aux = True
        elif serialObject == self.motorsGrower1 and self.mg1IsConnected: aux = True
        elif serialObject == self.motorsGrower2 and self.mg2IsConnected: aux = True
        if aux:
            serialObject.write(bytes(mssg, "utf-8"))
            serialObject.flush()
        else: self.logMain.error("Cannot write to serial device. It is disconnected.")

    def cleanLine(self, line):
        resp = line.split(",")
        if len(resp)>1: return resp[1]
        else: return resp[0]

    def detectGrower(self, line):
        if(line.startswith("Grower1")): return 1
        elif(line.startswith("Grower2")): return 2
        elif(line.startswith("Grower3")): return 3
        elif(line.startswith("Grower4")): return 4
        else: return 0

    def cleanGrowerLine(self, line):
        resp = line.split(":")[1][1:]
        return resp

    def getGrowerLine(self, line):
        resp = self.cleanLine(line)
        num = self.detectGrower(resp)
        return resp, num

    def startGrowerSequence(self, fl):
        if fl==1:
            self.mGrower.Gr1.serialReq("")
            x, y = self.mGrower.Gr1.getSequenceParameters()
        elif fl==2:
            self.mGrower.Gr2.serialReq("")
            x, y = self.mGrower.Gr2.getSequenceParameters()
        elif fl==3:
            self.mGrower.Gr3.serialReq("")
            x, y = self.mGrower.Gr3.getSequenceParameters()
        elif fl==4:
            self.mGrower.Gr4.serialReq("")
            x, y = self.mGrower.Gr4.getSequenceParameters()
        self.write(self.motorsGrower, "sequence,{},{},{}".format(fl,x,y))
        self.logMain.info("Grower{} sending request to start sequence".format(fl))

    def stopGrower(self, fl):
        self.write(self.motorsGrower1, "stop,{}".format(fl))
        self.logMain.warning("Grower{} is busy, sending request to stop".format(fl))

    def decideStartOrStopGrower(self, resp):
        auxBool = False
        num = self.detectGrower(resp)
        if(num==1): auxBool = self.mGrower.Gr1.startRoutine
        elif(num==2): auxBool = self.mGrower.Gr2.startRoutine
        elif(num==3): auxBool = self.mGrower.Gr3.startRoutine
        elif(num==4): auxBool = self.mGrower.Gr4.startRoutine

        if(num>=1 and num<=4): resp = self.cleanGrowerLine(resp)
        if resp.startswith("Available") and auxBool:
            self.startGrowerSequence(num)
            return True
        elif resp.startswith("Unavailable") and auxBool:
            self.stopGrower(num)
            return True

        return False

    def GrowerInRoutine(self, fl):
        if(fl==1):
            self.mGrower.Gr1.startRoutine = False
            self.mGrower.Gr1.inRoutine = True
            self.mGrower.Gr1.count = 0
        elif(fl==2):
            self.mGrower.Gr2.startRoutine = False
            self.mGrower.Gr2.inRoutine = True
            self.mGrower.Gr2.count = 0
        elif(fl==3):
            self.mGrower.Gr3.startRoutine = False
            self.mGrower.Gr3.inRoutine = True
            self.mGrower.Gr3.count = 0
        elif(fl==4):
            self.mGrower.Gr4.startRoutine = False
            self.mGrower.Gr4.inRoutine = True
            self.mGrower.Gr4.count = 0
        self.logMain.info("Grower{} sequence started".format(fl))

    def GrowerInPosition(self, fl):
        # request Grower to take pictures
        if(fl==1):
            photoName = self.mGrower.Gr1.count
            self.mGrower.Gr1.count += 1
            self.mGrower.Gr1.serialReq("")
            self.mGrower.Gr1.mqttReq("photoSequence,{}".format(photoName))
            self.mGrower.Gr1.actualTime = time()-20
        elif(fl==2):
            photoName = self.mGrower.Gr2.count
            self.mGrower.Gr2.count += 1
            self.mGrower.Gr2.serialReq("")
            self.mGrower.Gr2.mqttReq("photoSequence,{}".format(photoName))
            self.mGrower.Gr2.actualTime = time()-20
        elif(fl==3):
            photoName = self.mGrower.Gr3.count
            self.mGrower.Gr3.count += 1
            self.mGrower.Gr3.serialReq("")
            self.mGrower.Gr3.mqttReq("photoSequence,{}".format(photoName))
            self.mGrower.Gr3.actualTime = time()-20
        elif(fl==4):
            photoName = self.mGrower.Gr4.count
            self.mGrower.Gr4.count += 1
            self.mGrower.Gr4.serialReq("")
            self.mGrower.Gr4.mqttReq("photoSequence,{}".format(photoName))
            self.mGrower.Gr4.actualTime = time()-20

        self.logMain.debug("Grower{} in position to take photo sequence".format(fl))

    def GrowerRoutineFinish(self, fl):
        if(fl==1):
            self.mGrower.Gr1.count = 1
            self.mGrower.Gr1.mqttReq("routineFinish")
            self.mGrower.Gr1.serialReq("")
            self.mGrower.Gr1.inRoutine = False
            self.mGrower.Gr1.actualTime = time()-20
        elif(fl==2):
            self.mGrower.Gr2.count = 1
            self.mGrower.Gr2.mqttReq("routineFinish")
            self.mGrower.Gr2.serialReq("")
            self.mGrower.Gr2.inRoutine = False
            self.mGrower.Gr2.actualTime = time()-20
        elif(fl==3):
            self.mGrower.Gr3.count = 1
            self.mGrower.Gr3.mqttReq("routineFinish")
            self.mGrower.Gr3.serialReq("")
            self.mGrower.Gr3.inRoutine = False
            self.mGrower.Gr3.actualTime = time()-20
        elif(fl==4):
            self.mGrower.Gr4.count = 1
            self.mGrower.Gr4.mqttReq("routineFinish")
            self.mGrower.Gr4.serialReq("")
            self.mGrower.Gr4.inRoutine = False
            self.mGrower.Gr4.actualTime = time()-20
        self.logMain.info("Grower{} finished its routine".format(fl))

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
        if not self.mg1IsConnected or self.motorsGrower1.in_waiting==0: motorG = True
        else: motorG = False
        if not self.mg2IsConnected or self.motorsGrower2.in_waiting==0: sMaker = True
        else: sMaker = False
        if(time()-self.respTime>2 and gControl and motorG and sMaker and len(self.resp)>0):
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

        if self.mg1IsConnected:
            # If bytes available in motorsGrower
            while self.motorsGrower1.in_waiting>0:
                line2 = str(self.motorsGrower1.readline(), "utf-8")[0:-1]
                self.Msg2Log(self.logMG1, line2)

                decition = False
                # If we are waiting for a particular response from Grower1
                if(self.mGrower.Gr1.serialRequest!="" and not decition):
                    decition = self.decideStartOrStopGrower(self.cleanLine(line2))

                # If we are waiting for a particular response from Grower2
                if(self.mGrower.Gr2.serialRequest!="" and not decition):
                    decition = self.decideStartOrStopGrower(self.cleanLine(line2))

                # If we are waiting for a particular response from Grower3
                if(self.mGrower.Gr3.serialRequest!="" and not decition):
                    decition = self.decideStartOrStopGrower(self.cleanLine(line2))

                # If we are waiting for a particular response from Grower4
                if(self.mGrower.Gr4.serialRequest!="" and not decition):
                    decition = self.decideStartOrStopGrower(self.cleanLine(line2))

                # If we are waiting Gr1 to reach home and start the sequence
                if(self.mGrower.Gr1.serialRequest=="" and self.mGrower.Gr1.startRoutine and not decition):
                    resp, num = self.getGrowerLine(line2)
                    if(num==1):
                        resp = self.cleanGrowerLine(resp)
                        if resp.startswith("Starting Routine Stage 2"):
                            decition = True
                            self.GrowerInRoutine(num)

                # If we are waiting Gr2 to reach home and start the sequence
                if(self.mGrower.Gr2.serialRequest=="" and self.mGrower.Gr2.startRoutine and not decition):
                    resp, num = self.getGrowerLine(line2)
                    if(num==2):
                        resp = self.cleanGrowerLine(resp)
                        if resp.startswith("Starting Routine Stage 2"):
                            decition = True
                            self.GrowerInRoutine(num)

                # If we are waiting Gr3 to reach home and start the sequence
                if(self.mGrower.Gr3.serialRequest=="" and self.mGrower.Gr3.startRoutine and not decition):
                    resp, num = self.getGrowerLine(line2)
                    if(num==3):
                        resp = self.cleanGrowerLine(resp)
                        if resp.startswith("Starting Routine Stage 2"):
                            decition = True
                            self.GrowerInRoutine(num)

                # If we are waiting Gr4 to reach home and start the sequence
                if(self.mGrower.Gr4.serialRequest=="" and self.mGrower.Gr4.startRoutine and not decition):
                    resp, num = self.getGrowerLine(line2)
                    if(num==4):
                        resp = self.cleanGrowerLine(resp)
                        if resp.startswith("Starting Routine Stage 2"):
                            decition = True
                            self.GrowerInRoutine(num)

                # If we are waiting Gr1 to reach next sequence position
                if(self.mGrower.Gr1.inRoutine and not decition):
                    resp, num = self.getGrowerLine(line2)
                    if(num==1):
                        resp = self.cleanGrowerLine(resp)
                        if resp.startswith("In Position"):
                            decition = True
                            self.GrowerInPosition(num)
                        elif resp.startswith("Routine Finished"):
                            decition = True
                            self.GrowerRoutineFinish(num)

                # If we are waiting Gr2 to reach next sequence position
                if(self.mGrower.Gr2.inRoutine and not decition):
                    resp, num = self.getGrowerLine(line2)
                    if(num==2):
                        resp = self.cleanGrowerLine(resp)
                        if resp.startswith("In Position"):
                            decition = True
                            self.GrowerInPosition(num)
                        elif resp.startswith("Routine Finished"):
                            decition = True
                            self.GrowerRoutineFinish(num)

                # If we are waiting Gr3 to reach next sequence position
                if(self.mGrower.Gr3.inRoutine  and not decition):
                    resp, num = self.getGrowerLine(line2)
                    if(num==3):
                        resp = self.cleanGrowerLine(resp)
                        if resp.startswith("In Position"):
                            decition = True
                            self.GrowerInPosition(num)
                        elif resp.startswith("Routine Finished"):
                            decition = True
                            self.GrowerRoutineFinish(num)

                # If we are waiting Gr4 to reach next sequence position
                if(self.mGrower.Gr4.inRoutine and not decition):
                    resp, num = self.getGrowerLine(line2)
                    if(num==4):
                        resp = self.cleanGrowerLine(resp)
                        if resp.startswith("In Position"):
                            decition = True
                            self.GrowerInPosition(num)
                        elif resp.startswith("Routine Finished"):
                            decition = True
                            self.GrowerRoutineFinish(num)

        if self.mg2IsConnected:
            # If bytes available in motorsGrower
            while self.motorsGrower2.in_waiting>0:
                line2 = str(self.motorsGrower2.readline(), "utf-8")[0:-1]
                self.Msg2Log(self.logMG2, line2)

        # Send all responses
        self.response()
