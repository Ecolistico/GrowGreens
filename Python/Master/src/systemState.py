#!/usr/bin/env python3

# Import directories
import os
import json

class systemState:
    def __init__(self, file):
        # Default State
        self.file = file
        self.state = {}
        self.state["volumen"] = 0
        self.state["consumption"] = 0
        self.state["IPC"] = 0
        self.state["missedH2O"] = 0
        
    def load(self):
        if(os.path.isfile(self.file)):
            with open(self.file) as f:
                data = json.load(f)
                self.state["volumen"] = data["volumen"]
                self.state["consumption"] = data["consumption"]
                self.state["IPC"] = data["IPC"]
                self.state["missedH2O"] = data["missedH2O"]                
            return True
        else: return False
    
    def update(self, key, val):
        save = False
        if(key.startswith("volumen") and val>=0):
            self.state["volumen"] = val
            save = True
        elif(key.startswith("consumption") and val>=0):
            self.state["consumption"] = val
            save = True
        elif(key.startswith("IPC")):
            self.state["IPC"] = val
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