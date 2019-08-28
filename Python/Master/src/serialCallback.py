#!/usr/bin/env python3

# Import directories
from time import time
from serial import Serial
from systemState import systemState

class serialController:
    def __init__(self, loggerMain, loggerGC, loggerMG, loggerSM, stateFile):
        # Define microcontrolers
        self.generalControl = Serial('/dev/generalControl', 115200, timeout=1)
        self.generalControl.close()
        self.motorsGrower = Serial('/dev/motorsGrower', 115200, timeout=1)
        self.motorsGrower.close()
        self.solutionMaker = Serial('/dev/solutionMaker', 115200, timeout=1)
        self.solutionMaker.close()
        # Define responses auxVariables
        self.resp = []
        self.respLine = []
        self.respTime = time()
        # Define loggers
        self.logMain = loggerMain
        self.logGC = loggerGC
        self.logMG = loggerMG
        self.logSM = loggerSM
        self.irrigation = systemState(stateFile)
        if(self.irrigation.load()): self.logMain.info("Irrigation State charged")
        else: self.logMain.error("Cannot charge Irrigation State because it does not exist")
        
    def open(self):
        self.generalControl.open()
        self.motorsGrower.open()
        self.solutionMaker.open()
    
    def close(self):
        self.generalControl.close()
        self.motorsGrower.close()
        self.solutionMaker.close()
    
    def Msg2Log(self, logger, mssg):
        if(mssg.startswith("debug,")): logger.debug(mssg.split(",")[1])
        elif(mssg.startswith("info,")): logger.info(mssg.split(",")[1])
        elif(mssg.startswith("warning,")): logger.warning(mssg.split(",")[1])
        elif(mssg.startswith("error,")): logger.error(mssg.split(",")[1])
        else: logger.info(mssg)
    
    def response(self):
        if(time()-self.respTime>1 and
           self.generalControl.inWaiting()==0 and
           self.motorsGrower.inWaiting()==0 and
           self.solutionMaker.inWaiting()==0 and
           len(self.resp)>0):
            for i, resp in enumerate(self.resp):
                self.logMain.warning("Sending response for request {}".format(resp))
                
                # generalControl is requesting the necessary booting parameters
                if(resp == "boot"):
                    self.generalControl.write(bytes("boot,{0},{1},{2},{3},{4}".format(
                                            self.irrigation.state["solution"],
                                            self.irrigation.state["volumenNut"],
                                            self.irrigation.state["volumenH2O"],
                                            self.irrigation.state["consumptionNut"],
                                            self.irrigation.state["consumptionH2O"]),
                                             'utf8'))
                    self.generalControl.flush()
                    
                # Update irrigation state
                elif(self.resp == "updateIrrigationState"):
                    param = self.respLine[i].split(",")
                    if(self.irrigation.update("solution", int(param[1]))):
                        self.logMain.info("Irrigation Solution Updated")
                    else: self.logMain.error("Cannot Update Solution State")
                    if(self.irrigation.update("vol,nut", float(param[2]))):
                        self.logMain.info("Irrigation volNut Updated")
                    else: self.logMain.error("Cannot Update volNut State")
                    if(self.irrigation.update("vol,h2o", float(param[3]))):
                        self.logMain.info("Irrigation volH2O Updated")
                    else: self.logMain.error("Cannot Update volH2O State")
                    if(self.irrigation.update("cons,nut", float(param[4]))):
                        self.logMain.info("Irrigation consNut Updated")
                    else: self.logMain.error("Cannot Update consNut State")
                    if(self.irrigation.update("cons,h2o", float(param[5]))):
                        self.logMain.info("Irrigation consH2O Updated")
                    else: self.logMain.error("Cannot Update consH2O State")
                
                # generalControl is requesting to prepare a solution
                # Form -> "?solutionMaker,float[liters],int[sol],float[ph],int[ec]"
                elif(self.resp == "requestSolution"):
                    param = self.respLine[i].split(",")
                    liters = float(param[1])
                    solution = int(param[2])
                    ph = float(param[3])
                    ec = int(param[4])
                    # Check parameters
                    if(liters>0):
                        if(solution>=0 and solution<4):
                            if(ph>0 and ph<14):
                                if(ec>0 and ec<5000):
                                    # If parameters correct then request a solution
                                    self.solutionMaker.write(bytes("prepare,{0},{1},{2},{3}".format(
                                                                    liters,
                                                                    solution,
                                                                    ph,
                                                                    ec),
                                                                   'utf8'))
                                    self.solutionMaker.flush()
                                else: self.logGC.error("solutionMaker ec out of range [0-5000]")
                            else: self.logGC.error("solutionMaker ph out of range [0-14]")
                        else: self.logGC.error("solutionMaker solution out of range [0-3]")
                    else: self.logGC.error("solutionMaker liters has to be positive")
                
                elif(self.resp == "requestAccepted"):
                    self.generalControl.write(bytes("solutionMaker,accept", "utf-8"))
                    self.generalControl.flush()
                
                elif(self.resp == "solutionFinished"):
                    self.generalControl.write(bytes("solutionMaker,finished", "utf-8"))
                    self.generalControl.flush()
                    
                self.logMain.warning("Request {} was answered".format(resp))
                
            self.resp = []
            self.respLine = []           
            
    def loop(self):
        # If bytes available in generalControl
        while self.generalControl.inWaiting()>0:
            line1 = str(self.generalControl.readline(), "utf-8")[0:-1]
            self.Msg2Log(self.logGC, line1)
            
            if(line1.startswith("?boot")):
                self.resp.append("boot")
                self.respLine.append(line1)
                self.respTime = time()
            elif(line1.startswith("updateIrrigationState")):
                self.resp.append("updateIrrigationState")
                self.respLine.append(line1)
                self.respTime = time()
            elif(line1.startswith("?solutionMaker")):
                self.response.append("requestSolution")
                self.respLine.append(line1)
                self.respTime = time()
                    
        # If bytes available in motorsGrower
        while self.motorsGrower.inWaiting()>0:
            line2 = str(self.motorsGrower.readline(), "utf-8")[0:-1]
            self.Msg2Log(self.logMG, line2)
            self.respTime = time()
                
        # If bytes available in solutionMaker
        while self.solutionMaker.inWaiting()>0:
            line3 = str(self.solutionMaker.readline(), "utf-8")[0:-1]
            self.Msg2Log(self.logSM, line3)
            self.respTime = time()
            
            if(line3.startswith("Request accepted")):
                self.resp.append("requestAccepted")
                self.respLine.append(line3)
                self.respTime = time()
            elif(line3.startswith("Solution Finished")):
                self.resp.append("solutionFinished")
                self.respLine.append(line3)
                self.respTime = time()
        
        # Send all responses
        self.response()