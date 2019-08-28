#!/usr/bin/env python3

# Import directories
import os
import json
from datetime import datetime
from sysMaster import runShellCommand

class irrigationControl:
    def __init__(self):
        if not os.path.isfile("irrigation.json"): runShellCommand("./setupIrrigation.py")
        with open("irrigation.json") as f:
            data = json.load(f)
            order = data["order"]
            beginHour = data["beginHour"]
            percent = data["percent"]
        self.beginHour = []
        for i in range(len(beginHour)):
            self.beginHour.append(beginHour[str(i+1)])
        self.order = []
        for i in range(len(order)):
            self.order.append(order[str(i+1)])
        self.percentage = []
        for i in range(len(order)):
            self.percentage.append(percent[str(i+1)])
        print(self.percentage)
        print(self.order)
        print(self.beginHour)
        self.init = True
    
    def begin(self, beginHour, percentages, order):
        if(sum(percentages)==100):
            if(0 in order and 1 in order and 2 in order and 3 in order):
                self.beginHour = beginHour
                self.per = percentages
                self.order = order
                return True
        return False
                
    def sort_list(self, list1, list2): 
        zipped_pairs = zip(list2, list1) 
        z = [x for _, x in sorted(zipped_pairs)] 
        return z

    def nextSolution(self):
        now = datetime.now()
        # Get the actual time as float
        actualTime = float(now.hour) + float(now.minute)/60
        
        # Get actualTime in the cycle and the time gap
        if(actualTime>self.beginHour[0] and actualTime<=self.beginHour[1]):
            actualTime -= self.beginHour[0]
            timeGap = self.beginHour[1]-self.beginHour[0]
        elif(actualTime>self.beginHour[1] and actualTime<=self.beginHour[2]):
            actualTime -= self.beginHour[1]
            timeGap = self.beginHour[2]-self.beginHour[1]
        elif(actualTime>self.beginHour[2] and actualTime<=self.beginHour[3]):
            actualTime -= self.beginHour[2]
            timeGap = self.beginHour[3]-self.beginHour[2]
        elif(actualTime>self.beginHour[3] or actualTime<=self.beginHour[0]):
            timeGap = 24-self.beginHour[3]+self.beginHour[2]
            if(actualTime>self.beginHour[3]): actualTime -= self.beginHour[3]
            else: actualTime += 24-self.beginHour[3]
        
        # Get the percentage of the time that have already passed
        percentageGap = actualTime/timeGap*100
        # Ger percentages list in order
        perOrder = self.sort_list(self.per, self.order)
        print(perOrder)
        # Check what solution is the next one
        if(percentageGap<=perOrder[0]): return 0
        elif(percentageGap<=perOrder[0]+perOrder[1]): return 1
        elif(percentageGap<=perOrder[0]+perOrder[1]+perOrder[2]): return 2
        else: return 3