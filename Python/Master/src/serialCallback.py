#!/usr/bin/env python3

# Import directories
from time import time, sleep
from serial import Serial
from systemState import systemState

class serialController:
    def __init__(self, loggerMain, loggerGC, loggerMG, loggerSM, stateFile):
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
        else: logger.info(mssg)

    def write(self, serialObject, mssg):
        serialObject.write(bytes(mssg, "utf-8"))
        #serialObject.flush()
        
    def sendBootParams(self):
        self.write(self.generalControl, "boot,{0},{1},{2},{3},{4}".format(
            self.irrigation.state["solution"], self.irrigation.state["volumenNut"],
            self.irrigation.state["volumenH2O"], self.irrigation.state["consumptionNut"],
            self.irrigation.state["consumptionH2O"]))
        
        
    def updateIrrigationState(self, index):
        param = self.respLine[index].split(",")
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
    
    def requestSolution(self, index):
        # Form -> "?solutionMaker,float[liters],int[sol],float[ph],int[ec]"
        param = self.respLine[index].split(",")
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
                        self.write(self.solutionMaker, "prepare,{0},{1},{2},{3}".format(
                            liters, solution, ph, ec))
                    else: self.logGC.error("solutionMaker ec out of range [0-5000]")
                else: self.logGC.error("solutionMaker ph out of range [0-14]")
            else: self.logGC.error("solutionMaker solution out of range [0-3]")
        else: self.logGC.error("solutionMaker liters has to be positive")
        
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
                self.logMain.warning("Sending response for request {}".format(resp))
                
                # generalControl is requesting the necessary booting parameters
                if(resp == "boot"): self.sendBootParams()            
                # Update irrigation state
                elif(self.resp == "updateIrrigationState"): self.updateIrrigationState(i)
                # generalControl is requesting to prepare a solution
                elif(self.resp == "requestSolution"): self.requestSolution(i)
                # solutionMaker accepts to prepare a new solution
                elif(self.resp == "requestAccepted"):
                    self.write(self.generalControl, "solutionMaker,accept")
                # solutionMaker finished to prepare the solution
                elif(self.resp == "solutionFinished"):
                    self.write(self.generalControl, "solutionMaker,finished")
                    
                self.logMain.warning("Request {} was answered".format(resp))
              
            self.resp = []
            self.respLine = []           
            
    def loop(self):
        # If bytes available in generalControl
        while self.generalControl.in_waiting>0:
            line1 = str(self.generalControl.readline(), "utf-8")[0:-1]
            self.Msg2Log(self.logGC, line1)
            
            if(line1.startswith("?boot")):
                self.concatResp("boot", line1)
            elif(line1.startswith("updateIrrigationState")):
                self.concatResp("updateIrrigationState", line1)
            elif(line1.startswith("?solutionMaker")):
                self.concatResp("requestSolution", line1)
                    
        # If bytes available in motorsGrower
        while self.motorsGrower.in_waiting>0:
            line2 = str(self.motorsGrower.readline(), "utf-8")[0:-1]
            self.Msg2Log(self.logMG, line2)
                
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