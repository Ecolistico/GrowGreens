#!/usr/bin/env python3

# Import modules
import os
import sys
import csv
import time
import json
import sqlite3
from time import strftime, localtime
from datetime import datetime
import paho.mqtt.client as mqtt
import paho.mqtt.publish as publish
sys.path.insert(0, './src/')
import EnvControl
from logger import logger
from smtp import Mail
from mqttCallback import mqttController
from serialCallback import serialController

# Check if temp dir exists, if not then create it
if not os.path.exists('temp/'): os.makedirs('temp/')
    
# Charge logger parameters
log = logger()

# Define database
DataBase = './data/{}.db'.format(strftime("%Y-%m-%d", localtime()))
conn = sqlite3.connect(DataBase)

# Define config variables
with open("config.json") as f:
    data = json.load(f)
    ID = data["ID"]
    brokerIP = data["staticIP"]
    city = data["city"]
    state = data["state"]
    
# Define Mail object
mail = Mail(log.logger, "direccion@sippys.com.mx", city, state, ID) # Main logger, Team Ecolistico

# Define variables imported form other files
# From MQTT Callback
mqttControl = mqttController(ID,
                             brokerIP,
                             conn,
                             log.logger,
                             log.logger_grower1,
                             log.logger_grower2,
                             log.logger_grower3,
                             log.logger_grower4,
                             log.logger_esp32front,
                             log.logger_esp32center,
                             log.logger_esp32back)

# From Serial Callback
serialControl = serialController(log.logger_generalControl,
                                 log.logger_motorsGrower,
                                 log.logger_solutionMaker)

# Define MQTT communication
client = mqtt.Client()
client.on_connect = mqttControl.on_connect  # Specify on_connect callback
client.on_message = mqttControl.on_message  # Specify on_message callback
#client.on_publish = mqttController.on_publish  # Specify on_publish callback
client.on_disconnect = mqttControl.on_disconnect  # Specify on_publish callback
client.connect(brokerIP, 1883, 60)  # Connect to MQTT broker. Paremeters (IP direction, Port, Seconds Alive)

def mainClose():
    # Close devices when finished
    log.logger.info("Closing devices")
    conn.close() # Database pointer
    serialControl.close()
    log.logger.info("GrowGreens Finished")
    mail.sendMail("Error", "GrowGreens se detuvo")
    
# Setting up
day = 0
hour = 0
minute = 0
run = False
boot = False
log.logger.info("Setting up devices...")
serialControl.open()
log.logger.info("Devices ready")

try: param = sys.argv[1]
except: param = ""

if(param==""):
    print("\033[1;32;40m", end='')
    print(" Welcome to GrowGreens ".center(80,'*'))
    print("\033[0;37;40m\nWith this window you can check and control your production system," +
          " but before you must have completed your training. If you have not taked this" +
          " training could cause severe damage to yourself and to the system.")
    start = input("Do you want to continue? y/n\n")

if(start.startswith("y") or start.startswith("Y") or param=="start"):
    run = True
    log.logger.info("Permission to start GrowGreens accepted")

else:
    run = False
    log.logger.info("Permission to start GrowGreens refused")
    
actualTime = time.time()

try:
    # Main program
    while run:
        serialControl.loop()
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
            if(boot): mqttControl.ESP32.upload2DB(conn)
            else: boot = True
            publish.single("{}/esp32front".format(ID), "sendData", hostname = brokerIP)
            publish.single("{}/esp32center".format(ID), "sendData", hostname = brokerIP)
            publish.single("{}/esp32back".format(ID), "sendData", hostname = brokerIP)
            # Send to generalControl new time info
            serialControl.generalControl.write(bytes("updateHour,{0},{1}".format(now.hour, now.minute),"utf-8"))
    
    mainClose() # Finished th program

except:
    log.logger.exception("Exception Raised")
    
finally:
    mainClose() # Finished th program