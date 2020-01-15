#!/usr/bin/env python3

# Import directories
import os
import json

class systemState:
    def __init__(self, file):
        # Default State
        self.file = file
        self.state = {}
        self.state["solution"] = 1
        self.state["volumenNut"] = 0
        self.state["volumenH2O"] = 0
        self.state["consumptionNut"] = 0
        self.state["consumptionH2O"] = 0
        self.state["pumpIn"] = 0
        self.state["IPC"] = 0
        self.state["MPC"] = 0
        self.state["missedNut"] = 0
        self.state["missedH2O"] = 0
        
    def load(self):
        if(os.path.isfile(self.file)):
            with open(self.file) as f:
                data = json.load(f)
                self.state["solution"] = data["solution"]
                self.state["volumenNut"] = data["volumenNut"]
                self.state["volumenH2O"] = data["volumenH2O"]
                self.state["consumptionNut"] = data["consumptionNut"]
                self.state["consumptionH2O"] = data["consumptionH2O"]
                self.state["pumpIn"] = data["pumpIn"]
                self.state["IPC"] = data["IPC"]
                self.state["MPC"] = data["MPC"]
                self.state["missedNut"] = data["missedNut"]
                self.state["missedH2O"] = data["missedH2O"]                
            return True
        else: return False
    
    def update(self, key, val):
        save = False
        if(key.startswith("solution") and val>=0 and val<4):
            self.state["solution"] = val
            save = True 
        elif(key.startswith("volumenNut") and val>=0):
            self.state["volumenNut"] = val
            save = True
        elif(key.startswith("volumenH2O") and val>=0):
            self.state["volumenH2O"] = val
            save = True
        elif(key.startswith("consumptionNut") and val>=0):
            self.state["consumptionNut"] = val
            save = True
        elif(key.startswith("consumptionH2O") and val>=0):
            self.state["consumptionH2O"] = val
            save = True
        elif(key.startswith("pumpIn")):
            self.state["pumpIn"] = val
            save = True
        elif(key.startswith("IPC")):
            self.state["IPC"] = val
            save = True
        elif(key.startswith("MPC")):
            self.state["MPC"] = val
            save = True
        elif(key.startswith("missedNut")):
            self.state["missedNut"] = val
            save = True
        elif(key.startswith("missedH2O")):
            self.state["missedH2O"] = val
            save = True
        
        if save:
            self.save()
            
        return save
        
    def save(self):
        with open(self.file, "w") as f:
            json.dump(self.state, f)
    
    def print(self):
        print(self.state)