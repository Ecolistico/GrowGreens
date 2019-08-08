import sys
import os
from threading import Thread
import paho.mqtt.publish as publish
import paho.mqtt.client as mqtt

data = ""

# Get an Input Line
def GetLine():
    global data
    x = False
    while not x:
        data = sys.stdin.readline()

def mainThread():
    global data
    containerID = ""
    floor = 0
    broker = ""
    quit = False
    while(containerID == "" or floor==0 or broker==''):
        param = data.split(',')
        if(param[0]=="setContainerID" and param[1]!=""):
            containerID = param[1]
            data = ""
            print("New containerID = " + containerID)
        elif(param[0]=="setFloor" and int(param[1])>0 and int(param[1])<=4):
            floor = int(param[1])
            data = ""
            print("New floor = " + str(floor))
        elif(param[0]=="setBroker" and param[1]!=""):
            broker = param[1]
            data = ""
            print("New Broker IP = " + broker)

Thread(target=GetLine).start()
Thread(target=mainThread).start()
