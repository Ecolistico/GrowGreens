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
        
    def load(self):
        if(os.path.isfile(self.file)):
            with open(self.file) as f:
                data = json.load(f)
                self.state["solution"] = data["solution"]
                self.state["volumenNut"] = data["volumenNut"]
                self.state["volumenH2O"] = data["volumenH2O"]
                self.state["consumptionNut"] = data["consumptionNut"]
                self.state["consumptionH2O"] = data["consumptionH2O"]
            return True
        else: return False
    
    def update(self, key, val):
        save = False
        if(key.startswith("s") and val>0 and val<=4):
            self.state["solution"] = val
            save = True
        else:
            keySplit = key.lower().split(",")
            print(keySplit)
            if(keySplit[0].startswith("v") and keySplit[1].startswith("n") and val>=0):
                self.state["volumenNut"] = val
                save = True
            elif(keySplit[0].startswith("v") and keySplit[1].startswith("h") and val>=0):
                self.state["volumenH2O"] = val
                save = True
            if(keySplit[0].startswith("c") and keySplit[1].startswith("n") and val>=0):
                self.state["consumptionNut"] = val
                save = True
            elif(keySplit[0].startswith("c") and keySplit[1].startswith("h") and val>=0):
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