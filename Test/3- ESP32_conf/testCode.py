import sys
import os
from threading import Thread
import paho.mqtt.publish as publish
import paho.mqtt.client as mqtt

# Global Variables
CmD = ""
broker = ""
containerID = ""
type = ""
client = None
run = True

# On Conenct Callback for MQTT
def on_connect(client, userdata, flags, rc):
    Topic = "{}/esp32{}/#".format(containerID, type)
    message = "MQTT"

    if(rc == 0):
        message += " Connection succesful"
        mssg = "ESP32 connected"
        client.subscribe(Topic)
        print(message)
        print("Subscribed topic: {}".format(Topic))
    else:
        message += " Connection refused"
        if(rc == 1): message += " - incorrect protocol version"
        elif(rc == 2): message += " - invalid client identifier"
        elif(rc == 3): message += " - server unavailable"
        elif(rc == 4): message += " - bad username or password"
        elif(rc == 5): message += " - not authorised"
        else: message += " - currently unused"

# On Message Callback for MQTT
def on_message(client, userdata, msg):
    top = msg.topic # Input Topic
    message = msg.payload.decode("utf-8") # Input message
    ignoreMsg = ["sendData", "reboot", "hardReset", "updateConstant", "notFilter", "setExponentialFilter", "setKalmanFilter"]
    printAux = True
    for ign in ignoreMsg: if message.startswith(ign): printAux = False
    if(printAux): print("Message enter: Topic = {}\tMessage = {}".format(top, message))

def on_disconnect(client, userdata, msg):
    print("Client MQTT Disconnected")

# Get an Input Line Thread
def GetLine():
    global CmD
    global run
    while run:
        CmD = sys.stdin.readline()
        if(CmD=='quit\n' or CmD=='q\n'): x = False

# Main Thread
def mainThread():
    global CmD
    global run
    global containerID = ""
    global type = ""
    global broker = ""
    config = True

    while( (containerID == "" or type=="" or broker=='' or config) and run):
        param = CmD.split(',')
        if(param[0]=="setContainerID" and param[1]!=""):
            containerID = param[1].strip()
            CmD = ""
            print("New containerID = " + containerID)
        elif(param[0]=="setType"):
            ESPtype = param[1].strip()
            if(ESPtype=="center" or ESPtype=="front" or ESPtype=="back"):
                floor = ESPtype
                CmD = ""
                print("New Type = " + ESPtype)
            else: print("Type parameter incorrect")
        elif(param[0]=="setBroker" and param[1]!=""):
            broker = param[1].strip()
            CmD = ""
            print("New Broker IP = " + broker)
        elif(param[0].strip()=='configFinished'): config = False

    client = mqtt.Client()
    client.on_connect = on_connect # Specify on_connect callback
    client.on_message = on_message # Specify on_message callback
    #client.on_publish = on_publish # Specify on_publish callback
    client.on_disconnect # Specify on_disconnect callback
    client.connect(broker, 1883, 60) # Connect to MQTT Broker Parameters (IP, Port, Seconds Alive)

    while run:
        param = CmD.split(,)
        if()
        client.loop()

t1 = Thread(target=GetLine)
t2 = Thread(target=mainThread)

t1.start()
t2.start()

t1.join()
t2.join()
