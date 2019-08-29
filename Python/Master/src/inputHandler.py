#!/usr/bin/env python3

# Import directories
import os
import tty
import sys
import time
import select
import termios
import paho.mqtt.publish as publish

class inputHandler:
    def __init__(self, logger, serialController):
        # By default we are not into input mode
        self.inputMode = 0
        # Define logger
        self.log = logger
        # Define communication controllers
        self.serialControl = serialController
        
    # Get a single character without too much processor effort
    def getch(self):
        
        fd = sys.stdin.fileno()
        
        old_settings = termios.tcgetattr(fd)
        try:
            print("OK")
            tty.setraw(sys.stdin.fileno())
            print("OK1")
            ch = sys.stdin.read(1)
        finally:
            termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
        return ch
    
    # Get an Input Line it consumes between 20-30% of the processor capacity
    def getLine(self, Block=False):
      if Block or select.select([sys.stdin], [], [], 0) == ([sys.stdin], [], []):
          return input()
    
    def handle(self, line):
        if(line=="exit"):
            self.log.warning("inputMode exit")
            self.inputMode = False
        elif(line.startswith("setCycleTime")):
            try:
                cycleTime = int(line.split(",")[1])
                if(cycleTime>0): self.log.info("setCycleTime works")
                else: self.log.warning("setCycleTime parameter must be greater than 0")
            except: self.log.error("setCycleTime need 1 argument")
        else: self.log.error("inputHandler - command does not match a type")
            
    def loop(self):
        if self.inputMode == 0:
            char = self.getch()
            if(char!=None):
                if(char == "9"):
                    self.log.warning("Enter into inputMode")
                    self.inputMode = 1
                pass
        elif self.inputMode == 1:
            line = self.getLine()
            if line!=None:
                self.handle(line)
        
        
