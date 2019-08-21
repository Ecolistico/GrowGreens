#!/usr/bin/env python3

# Import directories
import os
import json
import sqlite3
from time import strftime, localtime
import paho.mqtt.publish as publish
import ESPdata

# Define files paths
actualDirectory = os.getcwd()
if(actualDirectory.endswith('src')):
    file = "../config.json"
    logPath = "../log/"
    DataBase = '../data/{}.db'.format(strftime("%Y-%m-%d", localtime()))
else:
    file = "config.json"
    logPath = "./log/"
    DataBase = './data/{}.db'.format(strftime("%Y-%m-%d", localtime()))

# Get config data
with open(file) as f:
    data = json.load(f)
    f.close()

ID = data["ID"]
brokerIP = data["staticIP"]

# Create ESP32 objects and connected it with database
conn = sqlite3.connect(DataBase)
ESP32 = ESPdata.multiESP()

def saveLog(device, line):
    logP = logPath + device
    logF = logP + "/{}.log".format(strftime("%Y-%m-%d", localtime()))
    # Check if directory exists
    if not os.path.exists(logP): os.makedirs(logP)
    # Save log
    with open(logF, "a+") as f:
        f.write("\n{} - ".format(strftime("%H:%M:%S", localtime())) + line)
        f.close()
        
# Callback fires when conected to MQTT broker.
def on_connect(client, userdata, flags, rc):
    Topic = "{}/#".format(ID)
    logTopic = "{}/Master/log".format(ID)
    
    message = "MQTT"
    if(rc == 0):
        message += " Connection succesful"
        mssg = "Master connected"
        client.subscribe(Topic)
        publish.single(logTopic, mssg, hostname = brokerIP)
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

# Callback fires when a published message is received.
def on_message(client, userdata, msg):
    top = msg.topic # Input Topic
    message = msg.payload.decode("utf-8") # Input message
    device = top.split("/")[1] # Device where come
    
    # Get MQTT logs from all the devices
    if(top.endswith("log")):
        saveLog(device, message)
        # Debug
        #print("Device={}, Topic={},\tMessage={}".format(device, top, message))
        
    # Get data from ESP32 front, center and back
    elif(top.endswith("esp32front") and message!="sendData"):
        ESP32.front.str2array(message)
        # Ask again for the data if not complete
    elif(top.endswith("esp32center") and message!="sendData"):
        ESP32.center.str2array(message)
        # Ask again for the data if not complete
    elif(top.endswith("esp32back") and message!="sendData"):
        ESP32.back.str2array(message)
        # Ask again for the data if not complete
        
    # Debug esp32 database
    elif top.endswith("esp32front"):
        ESP32.front.str2array("1,100,2,200,30,300,4,400,5,500,6,600,7,700,8,800")
    elif top.endswith("esp32center"):
        ESP32.center.str2array("9,900,10,1000,11,1100,12,1200,13,1300,14,1400,15,1500,16,1600")
    elif top.endswith("esp32back"):
        ESP32.back.str2array("17,1700,18,1800,19,1900,20,2000,21,2100,22,2200,23,2300,24,2400")    
    # For debug purposes
    #else: print(message)

def on_publish(client, userdata, mid):
    print("Message delivered")


def on_disconnect(client, userdata, rc):
    print("Client MQTT Disconnected")
    