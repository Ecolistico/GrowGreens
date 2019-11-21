#!/usr/bin/env python3

# Import directories
from time import time

class growerStatus:
    def __init__(self):
        self.inRoutine = False
        self.startRoutine = False
        self.count = 0
        self.serialRequest = ""
        self.mqttRequest = ""
        self.actualTime = time()
    
    def serialReq(self, req):
        self.serialRequest = ""
        self.actualTime = time()
        
    def mqttReq(self, req):
        self.mqttRequest = ""
        self.actualTime = time()
        
class share:
    def __init__(self):
        self.Gr1 = growerStatus()
        self.Gr2 = growerStatus()
        self.Gr3 = growerStatus()
        self.Gr4 = growerStatus()