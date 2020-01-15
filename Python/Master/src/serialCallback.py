#!/usr/bin/env python3

# Import directories
from serial import Serial
from time import time, sleep
from systemState import systemState

class serialController:
    def __init__(self, multiGrower, loggerMain, loggerGC, loggerMG, loggerSM, stateFile):
        # Define microcontrolers
        self.generalControl = Serial('/dev/generalControl', 115200, timeout=0)
        self.generalControl.dtr = False # Reset
        self.generalControl.close()
        self.motorsGrower = Serial('/dev/motorsGrower', 115200, timeout=0)
        self.motorsGrower.dtr = False # Reset
        self.motorsGrower.close()
        self.solutionMaker = Serial('/dev/solutionMaker', 115200, timeout=0)
        self.solutionMaker.dtr = False # Reset
        self.solutionMaker.close()
        # Define multiGrower variables with mqtt module
        self.mGrower = multiGrower
        # Define responses auxVariables
        self.resp = []
        self.respLine = []
        self.respTime = time()
        # Define loggers
        self.logMain = loggerMain
        self.logGC = loggerGC
        self.logMG = loggerMG
        self.logSM = loggerSM
        self.system = systemState(stateFile)
        if(self.system.load()): self.logMain.info("System State charged")
        else: self.logMain.error("[ERROR-1] Cannot charge System State because it does not exist")
        
    def open(self):
        if not self.generalControl.is_open:
            self.generalControl.open()
            sleep(0.33)
            self.generalControl.reset_input_buffer()
            self.generalControl.dtr = True
        if not self.motorsGrower.is_open:
            self.motorsGrower.open()
            sleep(0.33)
            self.motorsGrower.reset_input_buffer()
            self.motorsGrower.dtr = True
        if not self.solutionMaker.is_open:
            self.solutionMaker.open()
            sleep(0.33)
            self.solutionMaker.reset_input_buffer()
            self.solutionMaker.dtr = True
    
    def close(self):
        if self.generalControl.is_open: self.generalControl.close()
        if self.motorsGrower.is_open: self.motorsGrower.close()
        if self.solutionMaker.is_open:self.solutionMaker.close()
    
    def Msg2Log(self, logger, mssg):
        if(mssg.startswith("debug,")): logger.debug(mssg.split(",")[1])
        elif(mssg.startswith("info,")): logger.info(mssg.split(",")[1])
        elif(mssg.startswith("warning,")): logger.warning(mssg.split(",")[1])
        elif(mssg.startswith("error,")): logger.error(mssg.split(",")[1])
        elif(mssg.startswith("critical,")): logger.critical(mssg.split(",")[1])
        else: logger.debug(mssg)

    def write(self, serialObject, mssg):
        serialObject.write(bytes(mssg, "utf-8"))
        serialObject.flush()
        
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
        resp = line.split(":")[1]
        return resp[1:]
    
    def sendBootParams(self):
        self.write(self.generalControl, "boot,{0},{1},{2},{3},{4},{5},{6},{7},{8},{9}".format(
            self.system.state["solution"], self.system.state["volumenNut"],
            self.system.state["volumenH2O"], self.system.state["consumptionNut"],
            self.system.state["consumptionH2O"], self.system.state["pumpIn"],
            self.system.state["IPC"], self.system.state["MPC"],
            self.system.state["missedNut"], self.system.state["missedH2O"]))
        
    def updateSystemState(self, index):
        param = self.respLine[index].split(",")
        if(len(param)>=11):
            if(self.system.update("solution", int(param[1]))):
                self.logMain.debug("System Solution Updated")
            else: self.logMain.error("Cannot Update Solution State")
            if(self.system.update("volumenNut", float(param[2]))):
                self.logMain.debug("System volNut Updated")
            else: self.logMain.error("Cannot Update volNut State")
            if(self.system.update("volumenH2O", float(param[3]))):
                self.logMain.debug("System volH2O Updated")
            else: self.logMain.error("Cannot Update volH2O State")
            if(self.system.update("consumptionNut", float(param[4]))):
                self.logMain.debug("System consNut Updated")
            else: self.logMain.error("Cannot Update consNut State")
            if(self.system.update("consumptionH2O", float(param[5]))):
                self.logMain.debug("System consH2O Updated")
            else: self.logMain.error("Cannot Update consH2O State")
            if(self.system.update("pumpIn", int(param[6]))):
                self.logMain.debug("System pumpIn Updated")
            else: self.logMain.error("Cannot Update pumpIn State")
            if(self.system.update("IPC", int(param[7]))):
                self.logMain.debug("System IPC Updated")
            else: self.logMain.error("Cannot Update IPC State")
            if(self.system.update("MPC", int(param[8]))):
                self.logMain.debug("System MPC Updated")
            else: self.logMain.error("Cannot Update MPC State")
            if(self.system.update("missedNut", float(param[9]))):
                self.logMain.debug("System missedNut Updated")
            else: self.logMain.error("Cannot Update missedNut State")
            if(self.system.update("missedH2O", float(param[10]))):
                self.logMain.debug("System missedH2O Updated")
            else: self.logMain.error("Cannot Update missedH2O State")
            
        else: self.logMain.error("Line incomplete - {}".format(self.respLine[index]))
    
    def requestSolution(self, index):
        # Form -> "?solutionMaker,float[liters],int[sol],float[ph],int[ec]"
        param = self.respLine[index].split(",")
        if(len(param)>=5):
            liters = float(param[1])
            solution = int(param[2])
            ph = float(param[3])
            ec = int(param[4])
            self.logMain.debug("Prepare Solution line: {}".format(self.respLine[index]))
            # Check parameters
            if(liters>0):
                if(solution>=0 and solution<4):
                    if(ph>0 and ph<14):
                        if(ec>0 and ec<5000):
                            # If parameters correct then request a solution
                            self.write(self.solutionMaker, "prepare,{0},{1},{2},{3}".format(
                                liters, solution, ph, ec))
                        else: self.logGC.error("solutionMaker ec out of range [0-5000]")
                    else: self.logGC.error("solutionMaker ph out of range [0-14]")
                else: self.logGC.error("solutionMaker solution out of range [0-3]")
            else: self.logGC.error("solutionMaker liters has to be positive")
        else: self.logMain.error("Line incomplete - {}".format(self.respLine[index]))
        
    def concatResp(self, resp, line):
        # If that request is not save
        if not resp in self.resp:
            self.resp.append(resp) # Add the request
            self.respLine.append(line) # Add the line
            self.respTime = time() # Restart timer
            
    def response(self):
        if(time()-self.respTime>1 and
           self.generalControl.in_waiting==0 and
           self.motorsGrower.in_waiting==0 and
           self.solutionMaker.in_waiting==0 and
           len(self.resp)>0):
            for i, resp in enumerate(self.resp):
                # generalControl is requesting the necessary booting parameters
                if(resp == "boot"): self.sendBootParams()            
                # Update system state
                elif(resp == "updateSystemState"): self.updateSystemState(i)
                # generalControl is requesting to prepare a solution
                elif(resp == "requestSolution"): self.requestSolution(i)
                # generalControl ask if sMaker finished to prepare the solution
                elif(resp == "askSolFinished"): self.write(self.solutionMaker, "?solutionFinished")
                # solutionMaker accepts to prepare a new solution
                elif(resp == "requestAccepted"):
                    self.write(self.generalControl, "solutionMaker,accept")
                # solutionMaker finished to prepare the solution
                elif(resp == "solutionFinished"): self.write(self.generalControl, "solutionMaker,finished")
                    
                self.logMain.debug("Request {} was answered".format(resp))
              
            self.resp = []
            self.respLine = []           
            
    def loop(self):
        # If bytes available in generalControl
        while self.generalControl.in_waiting>0:
            line1 = str(self.generalControl.readline(), "utf-8")[0:-1]
            self.Msg2Log(self.logGC, line1)
            
            if(line1.startswith("?boot")):
                self.concatResp("boot", line1)
            elif(line1.startswith("updateSystemState")):
                self.concatResp("updateSystemState", line1)
            elif(line1.startswith("?solutionMaker")):
                self.concatResp("requestSolution", line1)
            elif(line1.startswith("?solutionFinished")):
                self.concatResp("askSolFinished", line1)
                    
        # If bytes available in motorsGrower
        while self.motorsGrower.in_waiting>0:
            line2 = str(self.motorsGrower.readline(), "utf-8")[0:-1]
            self.Msg2Log(self.logMG, line2)
            
            decition = False
            # If we are waiting for a particular response from Grower1
            if(self.mGrower.Gr1.serialRequest!="" and not decition):
                resp = self.cleanLine(line2)
                if(self.detectGrower(resp)==1):
                    resp = self.cleanGrowerLine(resp)
                    if resp == "Available" and self.mGrower.Gr1.startRoutine:
                        decition = True
                        self.write(self.motorsGrower, "sequence,1,35,20")
                        self.mGrower.Gr1.serialReq("")
                        self.self.logMain.info("Grower1 sending request to start sequence")
                    elif resp == "Unavailable" and self.mGrower.Gr1.startRoutine:
                        decition = True
                        self.write(self.motorsGrower, "stop,1")
                        self.self.logMain.warning("Grower1 is busy, sending request to stop")
                        
            # If we are waiting for a particular response from Grower2
            if(self.mGrower.Gr2.serialRequest!="" and not decition):
                resp = self.cleanLine(line2)
                if(self.detectGrower(resp)==2):
                    resp = self.cleanGrowerLine(resp)
                    if resp == "Available" and self.mGrower.Gr2.startRoutine:
                        decition = True
                        self.write(self.motorsGrower, "sequence,2,35,20")
                        self.mGrower.Gr2.serialReq("")
                        self.self.logMain.info("Grower2 sending request to start sequence")
                    elif resp == "Unavailable" and self.mGrower.Gr2.startRoutine:
                        decition = True
                        self.write(self.motorsGrower, "stop,2")
                        self.self.logMain.warning("Grower2 is busy, sending request to stop")
                        
            # If we are waiting for a particular response from Grower3
            if(self.mGrower.Gr3.serialRequest!="" and not decition):
                resp = self.cleanLine(line2)
                if(self.detectGrower(resp)==3):
                    resp = self.cleanGrowerLine(resp)
                    if resp == "Available" and self.mGrower.Gr3.startRoutine:
                        decition = True
                        self.write(self.motorsGrower, "sequence,3,35,20")
                        self.mGrower.Gr3.serialReq("")
                        self.self.logMain.info("Grower3 sending request to start sequence")
                    elif resp == "Unavailable" and self.mGrower.Gr3.startRoutine:
                        decition = True
                        self.write(self.motorsGrower, "stop,3")
                        self.self.logMain.warning("Grower3 is busy, sending request to stop")
                    
            # If we are waiting for a particular response from Grower4
            if(self.mGrower.Gr4.serialRequest!="" and not decition):
                resp = self.cleanLine(line2)
                if(self.detectGrower(resp)==4):
                    resp = self.cleanGrowerLine(resp)
                    if resp == "Available" and self.mGrower.Gr4.startRoutine:
                        decition = True
                        self.write(self.motorsGrower, "sequence,4,35,20")
                        self.mGrower.Gr4.serialReq("")
                        self.self.logMain.info("Grower4 sending request to start sequence")
                    elif resp == "Unavailable" and self.mGrower.Gr4.startRoutine:
                        decition = True
                        self.write(self.motorsGrower, "stop,4")
                        self.self.logMain.warning("Grower4 is busy, sending request to stop")
            
            # If we are waiting Gr1 to reach home and start the sequence
            if(self.mGrower.Gr1.serialRequest=="" and self.mGrower.Gr1.startRoutine and not decition):
                resp = self.cleanLine(line2)
                if(self.detectGrower(resp)==1):
                    resp = self.cleanGrowerLine(resp)
                    if resp=="Starting Routine Stage 2":
                        decition = True
                        self.mGrower.Gr1.startRoutine = False
                        self.mGrower.Gr1.inRoutine = True
                        self.mGrower.Gr1.count = 0
                        self.self.logMain.info("Grower1 sequence started")
                        
            # If we are waiting Gr2 to reach home and start the sequence
            if(self.mGrower.Gr2.serialRequest=="" and self.mGrower.Gr2.startRoutine and not decition):
                resp = self.cleanLine(line2)
                if(self.detectGrower(resp)==2):
                    resp = self.cleanGrowerLine(resp)
                    if resp=="Starting Routine Stage 2":
                        decition = True
                        self.mGrower.Gr2.startRoutine = False
                        self.mGrower.Gr2.inRoutine = True
                        self.mGrower.Gr2.count = 0
                        self.self.logMain.info("Grower2 sequence started")
                        
            # If we are waiting Gr3 to reach home and start the sequence
            if(self.mGrower.Gr3.serialRequest=="" and self.mGrower.Gr3.startRoutine and not decition):
                resp = self.cleanLine(line2)
                if(self.detectGrower(resp)==3):
                    resp = self.cleanGrowerLine(resp)
                    if resp=="Starting Routine Stage 2":
                        decition = True
                        self.mGrower.Gr3.startRoutine = False
                        self.mGrower.Gr3.inRoutine = True
                        self.mGrower.Gr3.count = 0
                        self.self.logMain.info("Grower3 sequence started")
                        
            # If we are waiting Gr4 to reach home and start the sequence
            if(self.mGrower.Gr4.serialRequest=="" and self.mGrower.Gr4.startRoutine and not decition):
                resp = self.cleanLine(line2)
                if(self.detectGrower(resp)==4):
                    resp = self.cleanGrowerLine(resp)
                    if resp=="Starting Routine Stage 2":
                        decition = True
                        self.mGrower.Gr4.startRoutine = False
                        self.mGrower.Gr4.inRoutine = True
                        self.mGrower.Gr4.count = 0
                        self.self.logMain.info("Grower4 sequence started")
                
            # If we are waiting Gr1 to reach next sequence position
            if(self.mGrower.Gr1.inRoutine and not decition):
                resp = self.cleanLine(line2)
                if(self.detectGrower(resp)==1):
                    resp = self.cleanGrowerLine(resp)
                    if resp=="In Position":
                        decition = True
                        # request Grower1 to take pictures
                        photoName = self.mGrower.Gr1.count
                        self.mGrower.Gr1.count += 1
                        self.mGrower.Gr1.mqttReq("photoSequence,{}".format(photoName))
                        self.mGrower.Gr1.actualTime = time()-20
                        self.self.logMain.debug("Grower1 in position to take photo sequence")
                    elif resp=="Routine Finished":
                        decition = True
                        self.mGrower.Gr1.inRoutine = False
                        self.mGrower.Gr1.mqttReq("sendPhotos")
                        self.mGrower.Gr1.actualTime = time()-20
                        self.self.logMain.info("Grower1 finished its routine")
                        
            # If we are waiting Gr2 to reach next sequence position
            if(self.mGrower.Gr2.inRoutine and not decition):
                resp = self.cleanLine(line2)
                if(self.detectGrower(resp)==2):
                    resp = self.cleanGrowerLine(resp)
                    if resp=="In Position":
                        decition = True
                        # request Grower2 to take pictures
                        photoName = self.mGrower.Gr2.count
                        self.mGrower.Gr2.count += 1
                        self.mGrower.Gr2.mqttReq("photoSequence,{}".format(photoName))
                        self.mGrower.Gr2.actualTime = time()-20
                        self.self.logMain.debug("Grower2 in position to take photo sequence")
                    elif resp=="Routine Finished":
                        decition = True
                        self.mGrower.Gr2.inRoutine = False
                        self.mGrower.Gr2.mqttReq("sendPhotos")
                        self.mGrower.Gr2.actualTime = time()-20
                        self.self.logMain.info("Grower2 finished its routine")
                        
            # If we are waiting Gr3 to reach next sequence position
            if(self.mGrower.Gr3.inRoutine  and not decition):
                resp = self.cleanLine(line2)
                if(self.detectGrower(resp)==3):
                    resp = self.cleanGrowerLine(resp)
                    if resp=="In Position":
                        decition = True
                        # request Grower3 to take pictures
                        photoName = self.mGrower.Gr3.count
                        self.mGrower.Gr3.count += 1
                        self.mGrower.Gr3.mqttReq("photoSequence,{}".format(photoName))
                        self.mGrower.Gr3.actualTime = time()-20
                        self.self.logMain.debug("Grower3 in position to take photo sequence")
                    elif resp=="Routine Finished":
                        decition = True
                        self.mGrower.Gr3.inRoutine = False
                        self.mGrower.Gr3.mqttReq("sendPhotos")
                        self.mGrower.Gr3.actualTime = time()-20
                        self.self.logMain.info("Grower3 finished its routine")
                        
            # If we are waiting Gr4 to reach next sequence position
            if(self.mGrower.Gr4.inRoutine and not decition):
                resp = self.cleanLine(line2)
                if(self.detectGrower(resp)==4):
                    resp = self.cleanGrowerLine(resp)
                    if resp=="In Position":
                        decition = True
                        # request Grower4 to take pictures
                        photoName = self.mGrower.Gr4.count
                        self.mGrower.Gr4.count += 1
                        self.mGrower.Gr4.mqttReq("photoSequence,{}".format(photoName))
                        self.mGrower.Gr4.actualTime = time()-20
                        self.self.logMain.debug("Grower4 in position to take photo sequence")
                    elif resp=="Routine Finished":
                        decition = True
                        self.mGrower.Gr4.inRoutine = False
                        self.mGrower.Gr4.mqttReq("sendPhotos")
                        self.mGrower.Gr4.actualTime = time()-20
                        self.self.logMain.info("Grower4 finished its routine")
                        
        # If bytes available in solutionMaker
        while self.solutionMaker.in_waiting>0:
            line3 = str(self.solutionMaker.readline(), "utf-8")[0:-1]
            self.Msg2Log(self.logSM, line3)
            self.respTime = time()
            if(line3.startswith("Request accepted")):
                self.concatResp("requestAccepted", line3)
            elif(line3.startswith("Solution Finished")):
                self.concatResp("solutionFinished", line3)
        
        # Send all responses
        self.response()