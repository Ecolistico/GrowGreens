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
import EnvControl
from gui import GUI
from iHP import IHP
from smtp import Mail
from logger import logger
from asciiART import asciiArt
from artificialDay import Day
from sensor import BMP280, BME680
#from Calendar import Calendar
from credentials import broker, sensor
from EnvControl import EnvControl
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
# Check if eeprom config file exists, if not then create it
if not os.path.exists('eeprom.config'):
    with open('eeprom.config', 'w') as f: f.write('')

# Define config variables
with open("config.json") as f:
    data = json.load(f)
    ID = data["ID"]
    brokerIP = data["staticIP"]
    city = data["city"]
    state = data["state"]
    ihp_data = data["ihp"]              # Must include 'MAC', 'ip_range' and 'port'
    artDay_data = data["day"]           # Include all the configuration to control the artifitial light
    env_data = data["env"]              # Include all the configuration to control the environment
    floors = int(artDay_data['floors']) # Number of floors
    growerData = data["growers"]

# Charge logger parameters
log = logger(floors)

# Charge calendar parameters
#growCal = Calendar()

# From communication
mGrower = multiGrower(log, growerData)

# From Serial Callback
serialControl = serialController(mGrower, log, "state.json")

# Define functions
def mainClose(): # When program is finishing
    # Close devices when finished
    log.logger.info("Closing devices")
    conn.close() # Database pointer
    serialControl.close()
    log.logger.info("GrowGreens Finished")
    if run: mail.sendMail("ALERTA", "GrowGreens se detuvo")
    else: mail.sendMail("ALERTA", "GrowGreens fue detenido por el operador")

def mqttDisconnect(cliente, mqttObj):
    cliente.disconnect()
    mqttObj.logMain.warning("Client MQTT Disconnected")
    mqttObj.clientConnected = False
    mqttObj.actualTime = time()

def checkSerialMsg(grower):
    # Resend serial messages without response for Growers
    serialFloor = mGrower.data[str(grower.floor)]
    if(serialFloor != "disconnected" and grower.serialRequest!="" and time()-grower.actualTime>120):
        serialDevice = int(int(serialFloor)/4)
        serialControl.write(serialControl.motorsGrower[serialDevice], grower.serialRequest)
        grower.actualTime = time()
        if grower.serialRequestCounter == 0: log.logger.debug("Sending Grower{} serialRequest: {}".format(grower.floor, grower.serialRequest))
        else: log.logger.warning("Resending Grower{} serialRequest: {}".format(grower.floor, grower.serialRequest))
        grower.serialRequestCounter += 1
    else: pass

def checkMqttMsg(grower):
    # Resend mqtt messages withouth response in 20s for Growers
    if(grower.mqttRequest!="" and time()-grower.actualTime>20):
        req2log = grower.mqttRequest
        if grower.mqttRequest=="RoutineFinished":
            mssg = "RoutineFinished"
            device = "Cloud"
            grower.mqttReq("")
        elif grower.mqttRequest=="MasterReady":
            mssg = "MasterReady"
            device = "Cloud"
            grower.mqttReq("")
        elif grower.mqttRequest=="takePicture":
            mssg = [{"topic": "{}/Cloud".format(ID, grower.floor), "payload": "takePicture"},
                    {"topic": "{}/Tucan".format(ID, grower.floor), "payload": "takePicture"}]
            grower.mqttReq("")
        else: 
            mssg = grower.mqttRequest
            device = "Grower{}".format(grower.floor)
        if(mqttControl.clientConnected):
            try:
                if(type(mssg)==str): publish.single("{}/{}".format(ID, device), mssg, hostname = brokerIP)
                elif(type(mssg)==list): publish.multiple(mssg, hostname = brokerIP)
            except Exception as e:
                log.logger.error("LAN/WLAN not found- Impossible use publish() to resend Grower{} request [{}]".format(grower.floor, e))
                mqttDisconnect(client, mqttControl)
        if grower.mqttRequestCounter == 0: log.logger.debug("Sending Grower{} mqttRequest: {}".format(grower.floor, req2log))
        else: log.logger.warning("Resending Grower{} mqttRequest: {}".format(grower.floor, req2log))
        grower.mqttRequestCounter += 1
        grower.actualTime = time()

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

    # Charge GUI parameters and connect logger and serialControl
    gui = GUI(ID, log.logger, serialControl)

    # Define day object
    myDay = Day()
    myDay.set_function(artDay_data) # Initialize functions
            
    # Define IHP object and connect logger
    ihp = IHP(ihp_data, log)
    # Set all floors OFF by default
    for i in range(myDay.fl): ihp.request(ihp.IREF, {'device': i+1, 'type': 'percentage', 'iref': 0})
    
    # Define Mail object
    #mail = Mail(log.logger, "direccion@sippys.com.mx", city, state, ID) # Main logger, Team Ecolistico
    # Main logger, me and @IFTTT
    mail = Mail(["jmcasimar@sippys.com.mx", "trigger@applet.ifttt.com"], city, state, ID, log.logger)

    # Define variables imported form other files
    # From MQTT Callback
    mqttControl = mqttController(ID,
                                 brokerIP,
                                 conn,
                                 serialControl.mGrower,
                                 log)

    # Define environment controller
    env = EnvControl(env_data, mqttControl)

    # From inputHandler
    inputControl = inputHandler(ID, brokerIP, log.logger, serialControl, mqttControl, gui)

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
    except Exception as e: log.logger.error("Cannot connect with MQTT Broker [{}]".format(e))

    # Setting up
    if sensor['external'] == 'BMP280': bme = BMP280(log.logger) # Start bmp280 sensor
    elif sensor['external'] == 'BME680': bme = BME680(log.logger) # Start bme680 sensor
    else: bme = None

    day = 0
    hour = 0
    minute = 0
    boot = False
    log.logger.info("Setting up devices...")
    serialControl.open()
    log.logger.info("Devices ready")
    mail.sendMail("ALERTA", "GrowGreens acaba de iniciar")
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

                except Exception as e: log.logger.error("Cannot connect with MQTT Broker [{}]".format(e))

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

            # Update day info and send it to iHP
            myDay.get_intensity(hour*60+minute)
            for i in range(1,4,1):
                ihp.request(ihp.READ_VIN, {'line': i})
                ihp.request(ihp.READ_IIN, {'line': i})
            for i in range(myDay.fl):
                if myDay.update[i]: 
                    ihp.request(ihp.IREF, {'device': i+1, 'type': 'percentage', 'iref': myDay.intensity[i]})
                    myDay.update[i] = False

            # Save last ESP32 info and request an update
            if(boot):
                # Upload sensor data
                mqttControl.ESP32.upload2DB(conn)
                mqttControl.mGrower.upload2DB(conn)
                if bme!=None: bme.upload2DB(conn)
                # Send to generalControl new time info
                serialControl.write(serialControl.generalControl, "updateHour,{0},{1}".format(
                    now.hour, now.minute))
            else: boot = True
            
            # Check if ESP32's and Growers are connected
            mqttControl.mGrower.updateStatus()
            mqttControl.ESP32.updateStatus()

            # Update air conditioner controller
            resp = env.update()
            env_msgs = []
            for msg in resp: env_msgs.append({"topic": "{}/{}".format(ID, msg["device"]), "payload": msg["payload"]})
            
            if(mqttControl.clientConnected):
                try:
                    msgs = []
                    for i in range(floors): msgs.append({"topic": "{}/Grower{}".format(ID, i+1), "payload": "cozirData"})
                    for i in range(len(log.logger_esp32)):
                        msgs.append({"topic": "{}/esp32front{}".format(ID, i+1), "payload": "sendData"})
                        msgs.append({"topic": "{}/esp32center{}".format(ID, i+1), "payload": "sendData"})
                        msgs.append({"topic": "{}/esp32back{}".format(ID, i+1), "payload": "sendData"})
                    msgs += env_msgs
                    # Request ESP32's and Growers data and update enviromental Controllers
                    publish.multiple(msgs, hostname = brokerIP)
                except Exception as e:
                    log.logger.error("LAN/WLAN not found- Impossible use publish() to request ESP&Grower data [{}]".format(e))
                    mqttDisconnect(client, mqttControl)

            # Request bme data
            if bme!=None and bme.read(): bme.logData()
            else: log.logger.warning("{} sensor cannot take reading".format(sensor['external']))

            """
            Feature not ready
            elif(hour==7 and minute==0): # At 7am
                # Send Dayly tasks
                sub, msg = growCal.getEmail()
                if(msg!=''): mail.sendMail(sub, msg)
            """
            
        # Check Serial Pending
        for i in range(len(mGrower.Gr)): checkSerialMsg(mGrower.Gr[i])

        # Check MQTT Pending
        for i in range(len(mGrower.Gr)): checkMqttMsg(mGrower.Gr[i])

        # Check iHP pending requests
        ihp.run()
        
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
