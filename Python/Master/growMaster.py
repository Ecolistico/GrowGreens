#!/usr/bin/env python3

# Import modules
import os
import sys
import csv
import json
import sqlite3
from datetime import datetime
import paho.mqtt.client as mqtt
import paho.mqtt.publish as publish
from time import time, sleep, strftime, localtime
sys.path.insert(0, './src/')
import security
import EnvControl
from smtp import Mail
from logger import logger
from sensor import BME680
from communication import share
from inputHandler import inputHandler
from mqttCallback import mqttController
from serialCallback import serialController

print("\033[1;32;40m", end='')
print(" Welcome to GrowGreens ".center(80,'*'))
print("\033[0;37;40m")
    
# Check if temp dir exists, if not then create it
if not os.path.exists('temp/'): os.makedirs('temp/')
    
# Charge logger parameters
log = logger()

# From communication
sh = share()

# From Serial Callback
serialControl = serialController(sh,
                                 log.logger,
                                 log.logger_generalControl,
                                 log.logger_motorsGrower,
                                 log.logger_solutionMaker,
                                 "irrigation.json")

# Define functions
def mainClose(): # When program is finishing
    # Close devices when finished
    log.logger.info("Closing devices")
    conn.close() # Database pointer
    serialControl.close()
    log.logger.info("GrowGreens Finished")
    if run: mail.sendMail("Ecolistico Alerta", "GrowGreens se detuvo")
    else: mail.sendMail("Ecolistico Alerta", "GrowGreens fue detenido por el operador")
    
# Aux Variables
try: param = sys.argv[1]
except: param = ""
start = ""
run = False

if(param==""):
    print("With this window you can check and control your production system," +
          " but before you must have completed your training. If you have not taked this" +
          " training could cause severe damage to yourself and to the system.")
    start = input("Do you want to continue? y/n\n")

if(start.startswith("y") or start.startswith("Y") or param=="start"):
    run = True
    log.logger.info("Permission to start GrowGreens accepted")

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
    #mail = Mail(log.logger, "direccion@sippys.com.mx", city, state, ID) # Main logger, Team Ecolistico
    # Main logger, me and @IFTTT
    mail = Mail(log.logger, ["jmcasimar@sippys.com.mx", "trigger@applet.ifttt.com"], city, state, ID) 

    # Define variables imported form other files
    # From MQTT Callback
    mqttControl = mqttController(ID,
                                 brokerIP,
                                 conn,
                                 sh,
                                 log.logger,
                                 log.logger_grower1,
                                 log.logger_grower2,
                                 log.logger_grower3,
                                 log.logger_grower4,
                                 log.logger_esp32front,
                                 log.logger_esp32center,
                                 log.logger_esp32back)

    # From inputHandler
    inputControl = inputHandler(log.logger, serialControl, mqttControl)

    try:
        # Define MQTT communication
        client = mqtt.Client()
        client.on_connect = mqttControl.on_connect  # Specify on_connect callback
        client.on_message = mqttControl.on_message  # Specify on_message callback
        #client.on_publish = mqttController.on_publish  # Specify on_publish callback
        client.on_disconnect = mqttControl.on_disconnect  # Specify on_publish callback
        # Connect to MQTT broker. Paremeters (IP direction, Port, Seconds Alive)
        if(client.connect(brokerIP, 1883, 60)==0): mqttControl.clientConnected = True
        else: log.logger.warning("Cannot connect with MQTT Broker")
    except: log.logger.warning("Cannot connect with MQTT Broker")

    # Setting up
    bme = BME680() # Start bme680 sensor
    day = 0
    hour = 0
    minute = 0
    boot = False
    log.logger.info("Setting up devices...")
    serialControl.open()
    log.logger.info("Devices ready")

else:
    run = False
    log.logger.info("Permission to start GrowGreens refused")
    
try:
    # Main program
    while run:
        serialControl.loop()
        inputControl.loop()
        now = datetime.now()
        
        # If mqtt connected check for messages
        if mqttControl.clientConnected: client.loop()
        # Else try to reconnect every 30s
        elif(time()-mqttControl.actualTime):
            mqttControl.actualTime = time()
            try:
                # Reconnect client
                client = mqtt.Client()
                client.on_connect = mqttControl.on_connect  # Specify on_connect callback
                client.on_message = mqttControl.on_message  # Specify on_message callback
                #client.on_publish = mqttController.on_publish  # Specify on_publish callback
                client.on_disconnect = mqttControl.on_disconnect  # Specify on_publish callback
                # Connect to MQTT broker. Paremeters (IP direction, Port, Seconds Alive)
                if(client.connect(brokerIP, 1883, 60)==0): mqttControl.clientConnected = True
                else: log.logger.warning("Cannot connect with MQTT Broker")
            except: log.logger.warning("Cannot connect with MQTT Broker")
            
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
            if(boot):
                # Upload sensor data
                mqttControl.ESP32.upload2DB(conn)
                bme.upload2DB(conn)
                # Send to generalControl new time info
                serialControl.write(serialControl.generalControl, "updateHour,{0},{1}".format(
                    now.hour, now.minute))
            else: boot = True
            # Request ESP32 data
            publish.single("{}/esp32front".format(ID), "sendData", hostname = brokerIP)
            publish.single("{}/esp32center".format(ID), "sendData", hostname = brokerIP)
            publish.single("{}/esp32back".format(ID), "sendData", hostname = brokerIP)
            # Request bme data
            if bme.read(): log.logger.debug("BME680 reading succes")
            else: log.logger.warning("BME680 sensor cannot take reading")
            
            # Coordinate Grower routines
            if(hour==6 and minute==0): # It is time to move Grower3
                sh.Gr1.startRoutine = True
                mssg = "available,1"
                serialControl.write(serialControl.motorsGrower, mssg)
                sh.Gr1.serialReq(mssg)
                log.logger.info("Checking Grower1 status to start sequence")
            elif(hour==8 and minute==0): # It is time to move Grower2
                sh.Gr2.startRoutine = True
                mssg = "available,2"
                serialControl.write(serialControl.motorsGrower, mssg)
                sh.Gr2.serialReq(mssg)
                log.logger.info("Checking Grower2 status to start sequence")
            elif(hour==10 and minute==0): # It is time to move Grower3
                sh.Gr3.startRoutine = True
                mssg = "available,3"
                serialControl.write(serialControl.motorsGrower, mssg)
                sh.Gr3.serialReq(mssg)
                log.logger.info("Checking Grower3 status to start sequence")
            elif(hour==12 and minute==0): # It is time to move Grower4
                sh.Gr4.startRoutine = True
                mssg = "available,4"
                serialControl.write(serialControl.motorsGrower, mssg)
                sh.Gr3.serialReq(mssg)
                log.logger.info("Checking Grower4 status to start sequence")
        
        # Resend serial messages without response in 20s for Growers
        if(sh.Gr1.serialRequest!="" and time()-sh.Gr1.actualTime>20):
            serialControl.write(serialControl.motorsGrower, sh.Gr1.serialRequest)
            sh.Gr1.actualTime = time()
        if(sh.Gr2.serialRequest!="" and time()-sh.Gr2.actualTime>20):
            serialControl.write(serialControl.motorsGrower, sh.Gr2.serialRequest)
            sh.Gr2.actualTime = time()
        if(sh.Gr3.serialRequest!="" and time()-sh.Gr3.actualTime>20):
            serialControl.write(serialControl.motorsGrower, sh.Gr3.serialRequest)
            sh.Gr3.actualTime = time()
        if(sh.Gr4.serialRequest!="" and time()-sh.Gr4.actualTime>20):
            serialControl.write(serialControl.motorsGrower, sh.Gr4.serialRequest)
            sh.Gr4.actualTime = time()
        
        # Resend mqtt messages withouth response in 20s for Growers
        if(sh.Gr1.mqttRequest!="" and time()-sh.Gr1.actualTime>20):
            if sh.Gr1.mqttRequest=="sendPhotos":
                mssg = "{},{},{},{}".format(sh.Gr1.mqttRequest, brokerIP,
                    security.decode(security.hostName), security.decode(security.passw))
            else: mssg = sh.Gr1.mqttRequest
            publish.single("{}/Grower1".format(ID), mssg, hostname = brokerIP)
            sh.Gr1.actualTime = time()
        if(sh.Gr2.mqttRequest!="" and time()-sh.Gr2.actualTime>20):
            if sh.Gr2.mqttRequest=="sendPhotos":
                mssg = "{},{},{},{}".format(sh.Gr2.mqttRequest, brokerIP,
                    security.decode(security.hostName), security.decode(security.passw))
            else: mssg = sh.Gr2.mqttRequest
            publish.single("{}/Grower2".format(ID), mssg, hostname = brokerIP)
            sh.Gr2.actualTime = time()
        if(sh.Gr3.mqttRequest!="" and time()-sh.Gr3.actualTime>20):
            if sh.Gr3.mqttRequest=="sendPhotos":
                mssg = "{},{},{},{}".format(sh.Gr3.mqttRequest, brokerIP,
                    security.decode(security.hostName), security.decode(security.passw))
            else: mssg = sh.Gr3.mqttRequest
            publish.single("{}/Grower3".format(ID), mssg, hostname = brokerIP)
            sh.Gr3.actualTime = time()
        if(sh.Gr4.mqttRequest!="" and time()-sh.Gr4.actualTime>20):
            if sh.Gr4.mqttRequest=="sendPhotos":
                mssg = "{},{},{},{}".format(sh.Gr4.mqttRequest, brokerIP,
                    security.decode(security.hostName), security.decode(security.passw))
            else: mssg = sh.Gr4.mqttRequest
            publish.single("{}/Grower4".format(ID), mssg, hostname = brokerIP)
            sh.Gr4.actualTime = time()
            
        if inputControl.exit:
            ex = input("Are you sure? y/n\n")
            if (ex.startswith("y") or start.startswith("Y")):
                run = False
                mainClose() # Finished th program
                log.logger.info("Program finished by operator")
            else:
                inputControl.exit = False
                log.logger.info("Exit aborted")
                
except:
    log.logger.exception("Exception Raised")
    
finally:
    if run: mainClose() # Finished th program