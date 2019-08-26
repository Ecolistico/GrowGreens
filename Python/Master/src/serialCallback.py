#!/usr/bin/env python3

# Import directories
from serial import Serial
        
class serialController:
    def __init__(self, loggerGC, loggerMG, loggerSM):
        # Define microcontrolers
        self.generalControl = Serial('/dev/generalControl', 115200, timeout=1)
        self.generalControl.close()
        self.motorsGrower = Serial('/dev/motorsGrower', 115200, timeout=1)
        self.motorsGrower.close()
        self.solutionMaker = Serial('/dev/solutionMaker', 115200, timeout=1)
        self.solutionMaker.close()
        # Define loggers
        self.logGC = loggerGC
        self.logMG = loggerMG
        self.logSM = loggerSM
    
    def open(self):
        self.generalControl.open()
        self.motorsGrower.open()
        self.solutionMaker.open()
    
    def close(self):
        self.generalControl.close()
        self.motorsGrower.close()
        self.solutionMaker.close()
        
    def loop(self):
        # If bytes available in generalControl
        while self.generalControl.inWaiting()>0:
            line1 = str(self.generalControl.readline(), "utf-8")
            self.logGC.info(line1[0:-1])
            
        # If bytes available in motorsGrower
        while self.motorsGrower.inWaiting()>0:
            line2 = str(self.motorsGrower.readline(), "utf-8")
            self.logMG.info(line2[0:-1])

        # If bytes available in solutionMaker
        while self.solutionMaker.inWaiting()>0:
            line3 = str(self.solutionMaker.readline(), "utf-8")
            self.logSM.info(line3[0:-1])