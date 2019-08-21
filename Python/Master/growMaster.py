#!/usr/bin/env python3

# Import modules
import os
import sys
import csv
import time
import sqlite3
from time import strftime, localtime
from datetime import datetime
import paho.mqtt.client as mqtt
import paho.mqtt.publish as publish
sys.path.insert(0, './src/')
import EnvControl
import mqttCallback
import serialCallback

# Define paths
logPath = "./log"

# Define variables imported form other files
# From MQTT Callback
ID = mqttCallback.ID
brokerIP = mqttCallback.brokerIP
DataBase = mqttCallback.DataBase
conn = mqttCallback.conn
ESP32 = mqttCallback.ESP32
# From Serial Callback
generalControl = serialCallback.generalControl
motorsGrower = serialCallback.motorsGrower
solutionMaker = serialCallback.solutionMaker

# Define MQTT communication
client = mqtt.Client()
client.on_connect = mqttCallback.on_connect  # Specify on_connect callback
client.on_message = mqttCallback.on_message  # Specify on_message callback
#client.on_publish = mqttCallback.on_publish  # Specify on_publish callback
client.on_disconnect = mqttCallback.on_disconnect  # Specify on_publish callback
client.connect(brokerIP, 1883, 60)  # Connect to MQTT broker. Paremeters (IP direction, Port, Seconds Alive)

# Setting up
day = 0
hour = 0
minute = 0
run = False
boot = False
print("Setting up devices...")
generalControl.open()
motorsGrower.open()
solutionMaker.open()
print("Devices ready")
print("\033[1;32;40m\n***** Welcome to GrowGreens *****\n")
print("\033[0;37;40mWith this window you can check and control your production system," +
      " but before you must have completed your training. If you have not taked this" +
      " training could cause severe damage to yourself and to the system.")
start = input("Do you want to continue? y/n\n")

if(start.startswith("y") or start.startswith("Y")):
    run = True

else:
    run = False

actualTime = time.time()
# Main program
while run:
    serialCallback.loop()
    client.loop()
    now = datetime.now()
    
    # When it is a new day
    if day!=now.day:
        # Update day
        day = now.day
        # Change database file
        splitPath = DataBase.split("/")
        DataBase = ""
        for i,spl in enumerate(splitPath):
            if i==len(splitPath)-1: DataBase += strftime("%Y-%m-%d", localtime()) + ".db"
            else: DataBase += spl + "/"
        conn.close()
        conn = sqlite3.connect(DataBase)
    
    # When it is a new hour
    if hour!=now.hour:
        hour = now.hour
    
    # When it is a new minute
    if minute!=now.minute:
        # Update Minute
        minute = now.minute
        # Save last ESP32 info and request an update
        if(boot): ESP32.upload2DB(conn)
        else: boot = True
        publish.single("{}/esp32front".format(ID), "sendData", hostname = brokerIP)
        publish.single("{}/esp32center".format(ID), "sendData", hostname = brokerIP)
        publish.single("{}/esp32back".format(ID), "sendData", hostname = brokerIP)
        # Send to generalControl new time info
        generalControl.write(bytes("updateHour,{0},{1}".format(now.hour, now.minute),"utf-8"))
        
    # Testing codes

# Close devices when finished
print("Closing devices")
conn.close() # Database pointer
generalControl.close()
motorsGrower.close()
solutionMaker.close()
print("Finished")
