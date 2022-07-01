#!/usr/bin/env python3

# Import Directories
import os
import sys
import json
import colored_traceback
from time import time, sleep
import paho.mqtt.publish as publish
import paho.mqtt.client as mqtt
sys.path.insert(0, './src/')
from gui import GUI
from logger import logger
from asciiART import asciiArt
from sysCloud import getIPaddr
from mqttCallback import mqttController
from streamCallback import streamController

# Colored traceback useful for raise exception with colors in terminal
colored_traceback.add_hook()

art = asciiArt()
print("\033[1;32;40m", end='')
print(" Welcome to GrowGreens (Cloud)".center(75,'*'))
art.img_print('./img/GrowGreens1_Web.png')
print("\033[0;37;40m")

# Check if temp dir exists, if not then create it
if not os.path.exists('temp/'): os.makedirs('temp/')
# Check if data dir exists, if not then create it
if not os.path.exists('data/'): os.makedirs('data/')
# Check if capture dir exists, if not then create it
if not os.path.exists('captures/'): os.makedirs('captures/')

# Charge logger parameters
log = logger()

# Define config variables
with open("config.json") as f: data = json.load(f)

# Check if capture/ID dir exists, if not then create it
if not os.path.exists('captures/{}/'.format(data["ID"])): os.makedirs('captures/{}/'.format(data["ID"]))
# Do the same with all the floors in the system
for i in range(int(data["floors"])):
    # Check if capture/ID/floor dir exists, if not then create it
    if not os.path.exists('captures/{}/floor{}/'.format(data["ID"], i+1)): os.makedirs('captures/{}/floor{}/'.format(data["ID"], i+1))

# From streamCallback
streamControl = streamController(data["ID"], log.logger)
data['port'] = streamControl.port

# From mqttCallback
mqttControl = mqttController(data, log.logger)

client = None
run = True
waitNextMove = False

def mainClose():
    # Close devices when finished
    if(client!=None):
        client.disconnect() # Disconnect MQTT
    streamControl.end() # Close stream
    log.shutdown()

try:
    # Define MQTT communication
    client = mqtt.Client()
    client.on_connect = mqttControl.on_connect  # Specify on_connect callback
    client.on_message = mqttControl.on_message  # Specify on_message callback
    #client.on_publish = mqttController.on_publish  # Specify on_publish callback
    client.on_disconnect = mqttControl.on_disconnect  # Specify on_disconnect callback
    # Connect to MQTT broker. Paremeters (IP direction, Port, Seconds Alive)
    if(client.connect(data["IP"], 1883, 60)==0): mqttControl.clientConnected = True
    else: log.logger.error("Cannot connect with MQTT Broker")
except Exception as e: log.logger.error("Cannot connect with MQTT Broker [{}]".format(e))

try:
    gui = GUI(data["ID"], data["IP"], mqttControl, client)
    gui.begin()
    
    while run:
        if gui.isOpen: gui.run()
        # Update MQTT variables with stream variables
        if mqttControl.takePicture:
            mqttControl.takePicture = False
            streamControl.inCapture = True
            waitNextMove = True
        elif mqttControl.routineStarted and not streamControl.inCapture and waitNextMove:
            waitNextMove = False
            publish.single("{}/Master".format(data["ID"]), 'continueSequence,{}'.format(mqttControl.inRoutine), hostname=data["IP"])

        if streamControl.floor != mqttControl.inRoutine: streamControl.floor = mqttControl.inRoutine

        # Stream loop
        streamControl.streaming()

        # If mqtt connected check for messages
        if mqttControl.clientConnected:
            if(mqttControl.inRoutine!=0 and time()-mqttControl.routineTimer>=15*60):
                mqttControl.finishRoutine() # TimeOut for the routine
                log.logger.error("Routine Error: Timeout reached")
            client.loop(0.2)
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
                    if(client.connect(data["IP"], 1883, 60)==0): mqttControl.clientConnected = True
                    else: log.logger.error("Cannot connect with MQTT Broker")

                except Exception as e: log.logger.error("Cannot connect with MQTT Broker [{}]".format(e))

except:
    log.logger.critical("Exception Raised", exc_info=True)

finally:
    mainClose()
