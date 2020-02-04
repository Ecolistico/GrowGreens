#!/usr/bin/env python3

# Import modules
import os
import sys
import csv
import json
import sqlite3
import colored_traceback
from datetime import datetime
import paho.mqtt.client as mqtt
import paho.mqtt.publish as publish
from time import time, sleep, strftime, localtime
sys.path.insert(0, './src/')
import security
import EnvControl
from gui import GUI
from smtp import Mail
from logger import logger
from sensor import BME680
from asciiART import asciiArt
from Calendar import Calendar
from growerData import multiGrower
from inputHandler import inputHandler
from mqttCallback import mqttController
from serialCallback import serialController

# Colored traceback useful for raise exception with colors in terminal
colored_traceback.add_hook()

art = asciiArt()
print("\033[1;32;40m", end='')
print(" Welcome to GrowGreens ".center(80,'*'))
art.img_print('./img/GrowGreens1_Web.png')
print("\033[0;37;40m")

# Check if temp dir exists, if not then create it
if not os.path.exists('temp/'): os.makedirs('temp/')
# Check if data dir exists, if not then create it
if not os.path.exists('data/'): os.makedirs('data/')

# Charge logger parameters
log = logger()

# Charge calendar parameters
growCal = Calendar()

# From communication
mGrower = multiGrower(log.logger_grower1, log.logger_grower2, log.logger_grower3, log.logger_grower4)

# From Serial Callback
serialControl = serialController(mGrower,
                                 log.logger,
                                 log.logger_generalControl,
                                 log.logger_motorsGrower,
                                 log.logger_solutionMaker,
                                 "state.json")

# Charge GUI parameters and connect logger and serialControl
gui = GUI(log.logger, serialControl)

# Define functions
def mainClose(): # When program is finishing
    # Close devices when finished
    log.logger.info("Closing devices")
    conn.close() # Database pointer
    serialControl.close()
    log.logger.info("GrowGreens Finished")
    if run: mail.sendMail("Ecolistico Alerta", "GrowGreens se detuvo")
    else: mail.sendMail("Ecolistico Alerta", "GrowGreens fue detenido por el operador")

def mqttDisconnect(cliente, mqttObj):
    cliente.disconnect()
    mqttObj.logMain.warning("Client MQTT Disconnected")
    mqttObj.clientConnected = False
    mqttObj.actualTime = time()

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
    log.logger.debug("Permission to start GrowGreens accepted")

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
                                 mGrower,
                                 log.logger,
                                 log.logger_grower1,
                                 log.logger_grower2,
                                 log.logger_grower3,
                                 log.logger_grower4,
                                 log.logger_esp32front,
                                 log.logger_esp32center,
                                 log.logger_esp32back)

    # From inputHandler
    inputControl = inputHandler(log.logger, serialControl, mqttControl, gui)

    try:
        # Define MQTT communication
        client = mqtt.Client()
        client.on_connect = mqttControl.on_connect  # Specify on_connect callback
        client.on_message = mqttControl.on_message  # Specify on_message callback
        #client.on_publish = mqttController.on_publish  # Specify on_publish callback
        client.on_disconnect = mqttControl.on_disconnect  # Specify on_disconnect callback
        # Connect to MQTT broker. Paremeters (IP direction, Port, Seconds Alive)
        if(client.connect(brokerIP, 1883, 60)==0): mqttControl.clientConnected = True
        else: log.logger.error("Cannot connect with MQTT Broker")
    except: log.logger.error("Cannot connect with MQTT Broker")

    # Setting up
    bme = BME680(log.logger) # Start bme680 sensor
    day = 0
    hour = 0
    minute = 0
    boot = False
    log.logger.info("Setting up devices...")
    serialControl.open()
    log.logger.info("Devices ready")
    mail.sendMail("Ecolistico Alerta", "GrowGreens acaba de iniciar")
    sub, msg = growCal.getEmail()
    mail.sendMail(sub, msg)
    gui.begin()
else:
    run = False
    log.logger.warning("Permission to start GrowGreens refused")

try:
    # Main program
    while run:
        if gui.isOpen: gui.run()
        serialControl.loop()
        inputControl.loop()
        now = datetime.now()

        # If mqtt connected check for messages
        if mqttControl.clientConnected: client.loop(0.2)
        else:
            sleep(0.2)
            # Else try to reconnect every 30s
            if(time()-mqttControl.actualTime>30):
                mqttControl.actualTime = time()
                try:
                    # Reconnect client
                    client = mqtt.Client()
                    client.on_connect = mqttControl.on_connect  # Specify on_connect callback
                    client.on_message = mqttControl.on_message  # Specify on_message callback
                    #client.on_publish = mqttController.on_publish  # Specify on_publish callback
                    client.on_disconnect = mqttControl.on_disconnect  # Specify on_disconnect callback
                    # Connect to MQTT broker. Paremeters (IP direction, Port, Seconds Alive)
                    if(client.connect(brokerIP, 1883, 60)==0): mqttControl.clientConnected = True
                    else: log.logger.error("Cannot connect with MQTT Broker")

                except: log.logger.error("Cannot connect with MQTT Broker")

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
                mqttControl.mGrower.upload2DB(conn)
                bme.upload2DB(conn)
                # Send to generalControl new time info
                serialControl.write(serialControl.generalControl, "updateHour,{0},{1}".format(
                    now.hour, now.minute))
            else: boot = True
            # Check if ESP32's and Growers are connected
            mqttControl.mGrower.updateStatus()
            mqttControl.ESP32.updateStatus()

            if(mqttControl.clientConnected):
                try:
                    # Request ESP32's and Growers data
                    publish.single("{}/esp32front".format(ID), "sendData", hostname = brokerIP)
                    publish.single("{}/esp32center".format(ID), "sendData", hostname = brokerIP)
                    publish.single("{}/esp32back".format(ID), "sendData", hostname = brokerIP)
                    publish.single("{}/Grower1".format(ID), "cozirData", hostname = brokerIP)
                    publish.single("{}/Grower2".format(ID), "cozirData", hostname = brokerIP)
                    publish.single("{}/Grower3".format(ID), "cozirData", hostname = brokerIP)
                    publish.single("{}/Grower4".format(ID), "cozirData", hostname = brokerIP)
                except:
                    log.logger.error("LAN/WLAN not found- Impossible use publish() to request ESP&Grower data")
                    mqttDisconnect(client, mqttControl)

            # Request bme data
            if bme.read(): bme.logData()
            else: log.logger.warning("BME680 sensor cannot take reading")

            # Coordinate Grower routines
            if(hour==6 and minute==0): # It is time to move Grower3
                mGrower.Gr1.startRoutine = True
                mssg = "available,1"
                serialControl.write(serialControl.motorsGrower, mssg)
                mGrower.Gr1.serialReq(mssg)
                log.logger.info("Checking Grower1 status to start sequence")
            elif(hour==8 and minute==0): # It is time to move Grower2
                mGrower.Gr2.startRoutine = True
                mssg = "available,2"
                serialControl.write(serialControl.motorsGrower, mssg)
                mGrower.Gr2.serialReq(mssg)
                log.logger.info("Checking Grower2 status to start sequence")
            elif(hour==10 and minute==0): # It is time to move Grower3
                mGrower.Gr3.startRoutine = True
                mssg = "available,3"
                serialControl.write(serialControl.motorsGrower, mssg)
                mGrower.Gr3.serialReq(mssg)
                log.logger.info("Checking Grower3 status to start sequence")
            elif(hour==12 and minute==0): # It is time to move Grower4
                mGrower.Gr4.startRoutine = True
                mssg = "available,4"
                serialControl.write(serialControl.motorsGrower, mssg)
                mGrower.Gr3.serialReq(mssg)
                log.logger.info("Checking Grower4 status to start sequence")
            elif(hour==0 and minute==0): # Update Plant database and restart GUI
                pass
        # Resend serial messages without response in 20s for Growers
        if(mGrower.Gr1.serialRequest!="" and time()-mGrower.Gr1.actualTime>20):
            serialControl.write(serialControl.motorsGrower, mGrower.Gr1.serialRequest)
            mGrower.Gr1.actualTime = time()
        if(mGrower.Gr2.serialRequest!="" and time()-mGrower.Gr2.actualTime>20):
            serialControl.write(serialControl.motorsGrower, mGrower.Gr2.serialRequest)
            mGrower.Gr2.actualTime = time()
        if(mGrower.Gr3.serialRequest!="" and time()-mGrower.Gr3.actualTime>20):
            serialControl.write(serialControl.motorsGrower, mGrower.Gr3.serialRequest)
            mGrower.Gr3.actualTime = time()
        if(mGrower.Gr4.serialRequest!="" and time()-mGrower.Gr4.actualTime>20):
            serialControl.write(serialControl.motorsGrower, mGrower.Gr4.serialRequest)
            mGrower.Gr4.actualTime = time()

        # Resend mqtt messages withouth response in 20s for Growers
        if(mGrower.Gr1.mqttRequest!="" and time()-mGrower.Gr1.actualTime>20):
            if mGrower.Gr1.mqttRequest=="sendPhotos":
                mssg = "{},{},{},{}".format(mGrower.Gr1.mqttRequest, brokerIP,
                    security.decode(security.hostName), security.decode(security.passw))
            else: mssg = mGrower.Gr1.mqttRequest
            if(mqttControl.clientConnected):
                try:
                    publish.single("{}/Grower1".format(ID), mssg, hostname = brokerIP)
                except:
                    log.logger.error("LAN/WLAN not found- Impossible use publish() to resend Grower1 request")
                    mqttDisconnect(client, mqttControl)
            mGrower.Gr1.actualTime = time()
        if(mGrower.Gr2.mqttRequest!="" and time()-mGrower.Gr2.actualTime>20):
            if mGrower.Gr2.mqttRequest=="sendPhotos":
                mssg = "{},{},{},{}".format(mGrower.Gr2.mqttRequest, brokerIP,
                    security.decode(security.hostName), security.decode(security.passw))
            else: mssg = mGrower.Gr2.mqttRequest
            if(mqttControl.clientConnected):
                try:
                    publish.single("{}/Grower2".format(ID), mssg, hostname = brokerIP)
                except:
                    log.logger.error("LAN/WLAN not found- Impossible use publish() to resend Grower2 request")
                    mqttDisconnect(client, mqttControl)
            mGrower.Gr2.actualTime = time()
        if(mGrower.Gr3.mqttRequest!="" and time()-mGrower.Gr3.actualTime>20):
            if mGrower.Gr3.mqttRequest=="sendPhotos":
                mssg = "{},{},{},{}".format(mGrower.Gr3.mqttRequest, brokerIP,
                    security.decode(security.hostName), security.decode(security.passw))
            else: mssg = mGrower.Gr3.mqttRequest
            if(mqttControl.clientConnected):
                try:
                    publish.single("{}/Grower3".format(ID), mssg, hostname = brokerIP)
                except:
                    log.logger.error("LAN/WLAN not found- Impossible use publish() to resend Grower3 request")
                    mqttDisconnect(client, mqttControl)
            mGrower.Gr3.actualTime = time()
        if(mGrower.Gr4.mqttRequest!="" and time()-mGrower.Gr4.actualTime>20):
            if mGrower.Gr4.mqttRequest=="sendPhotos":
                mssg = "{},{},{},{}".format(mGrower.Gr4.mqttRequest, brokerIP,
                    security.decode(security.hostName), security.decode(security.passw))
            else: mssg = mGrower.Gr4.mqttRequest
            if(mqttControl.clientConnected):
                try:
                    publish.single("{}/Grower4".format(ID), mssg, hostname = brokerIP)
                except:
                    log.logger.error("LAN/WLAN not found- Impossible use publish() to resend Grower4 request")
                    mqttDisconnect(client, mqttControl)
            mGrower.Gr4.actualTime = time()

        if inputControl.exit:
            ex = input("Are you sure? y/n\n")
            if (ex.startswith("y") or start.startswith("Y")):
                run = False
                mainClose() # Finished th program
                log.logger.warning("Program finished by operator")
            else:
                inputControl.exit = False
                log.logger.warning("Exit canceled")

except Exception as e:
    log.logger.critical("Exception Raised", exc_info=True)
    raise e

finally:
    if run: mainClose() # Finished the program
