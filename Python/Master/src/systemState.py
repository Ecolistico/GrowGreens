#!/usr/bin/env python3

# Import directories
import os
import json

class systemState:
    def __init__(self, file):
        # Default State
        self.file = file
        self.state = {}
        self.state["controlState"] = 0
        self.state["consumptionH2O"] = 0
        
    def load(self):
        if(os.path.isfile(self.file)):
            with open(self.file) as f:
                data = json.load(f)
                self.state["controlState"] = data["controlState"]
                self.state["consumptionH2O"] = data["consumptionH2O"]                
            return True
        else: return False
    
    def update(self, key, val):
        save = False
        if(key.startswith("controlState") and val>=0):
            self.state["controlState"] = val
            save = True
        elif(key.startswith("consumptionH2O") and val>=0):
            self.state["consumptionH2O"] = val
            save = True
        if save:
            self.save()
            
        return save
        
    def save(self):
        with open(self.file, "w") as f:
            json.dump(self.state, f)
    
    def print(self):
        print(self.state)