#!/usr/bin/env python3
"""
To check:
    * forgetWiFi: not reconnect with MQTT client
    * condition in functions with more than 1 parameter count ","
"""
# Import Directories
import os
import sys
import colored_traceback
from time import time, sleep
import paho.mqtt.publish as publish
import paho.mqtt.client as mqtt
sys.path.insert(0, './src/')
import sysGrower
from logger import logger
from asciiART import asciiArt
from sysGrower import runShellCommand
from mqttCallback import mqttController

# Colored traceback useful for raise exception with colors in terminal
colored_traceback.add_hook()

art = asciiArt()
print("\033[1;32;40m", end='')
print(" Welcome to GrowGreens (Grower Version)".center(75,'*'))
art.img_print('./img/GrowGreens1_Web.png')
print("\033[0;37;40m")

# Check if temp dir exists, if not then create it
if not os.path.exists('temp/'): os.makedirs('temp/')
# Check if data dir exists, if not then create it
if not os.path.exists('data/'): os.makedirs('data/')

# Charge logger parameters
log = logger()

# From mqttCallback
mqttControl = mqttController(log.logger)

# WiFi variables
WiFiState = 0
WiFiTime = time()- 20
WiFiCount = 0
MQTTCount = 0
prevWiFiState = 0

client = None
run = True

def mainInit():
    log.logger.info("Testing Outputs")
    sleep(3)
    log.logger.info("Output 1: ON")
    mqttControl.grower.turnOn(mqttControl.grower.OUT1)
    sleep(3)
    log.logger.info("Output 1: OFF")
    mqttControl.grower.turnOff(mqttControl.grower.OUT1)
    sleep(3)
    log.logger.info("Output 2: ON")
    mqttControl.grower.turnOn(mqttControl.grower.OUT2)
    sleep(3)
    log.logger.info("Output 2: OFF")
    mqttControl.grower.turnOff(mqttControl.grower.OUT2)
    sleep(3)
    log.logger.info("Output IR: ON")
    mqttControl.grower.turnOn(mqttControl.grower.SCL)
    mqttControl.grower.turnOn(mqttControl.grower.SDA)
    sleep(3)
    log.logger.info("Output IR: OFF")
    mqttControl.grower.turnOff(mqttControl.grower.SCL)
    mqttControl.grower.turnOff(mqttControl.grower.SDA)
    sleep(3)
    log.logger.info("Test Finished")


def mainClose():
    # Close devices when finished
    mqttControl.grower.close() # Clean GPIO
    if(client!=None):
        client.disconnect() # Disconnect MQTT
    log.shutdown()

try:
    # mainInit() # DEBUG only
    while run:
        # Check WiFi Loop
        if(time()- WiFiTime > 20):
            # If there is WiFi
            if(sysGrower.checkInterface('eth0')):
                prevWiFiState = WiFiState
                if(sysGrower.isAP()): WiFiState = 3 # Conected to Eth0 and AP
                else: WiFiState = 1 # Conected to Eth0
                WiFiCount = 0
            elif(sysGrower.isAP()):
                prevWiFiState = WiFiState
                prevAPState = 1
                WiFiState = 2 # AP created
                WiFiCount = 0
            else:
                prevWiFiState = WiFiState
                WiFiState = 0 # Not AP or Eth0
                WiFiCount += 1
            WiFiTime = time()

        # If not WiFi and not AP while 5*20 seconds
        if(WiFiCount>=5 or MQTTCount>=5):
            # Configure AP
            WiFiCount = 0
            MQTTCount = 0
            log.logger.warning("Cannot detect WiFi network ")
            log.logger.info("Configuring AP...")
            sysGrower.runShellCommand('sudo python ./src/APconfig.py')

        # MQTT Loop
        if(WiFiState!=prevWiFiState):
            # If WiFi Available
            if(WiFiState==1):
                # Update parameters
                mqttControl.update()
                try:
                    # Reconnect client
                    client = mqtt.Client()
                    client.on_connect = mqttControl.on_connect # Specify on_connect callback
                    client.on_message = mqttControl.on_message # Specify on_message callback
                    #client.on_publish = mqttControl.on_publish # Specify on_publish callback
                    client.on_disconnect = mqttControl.on_disconnect # Specify on_disconnect callback
                    # Connect to MQTT Broker Parameters (IP, Port, Seconds Alive)
                    if(client.connect(mqttControl.brokerIP, 1883, 60)==0):
                        mqttControl.clientConnected = True
                        MQTTCount = 0
                    else:
                        log.logger.warning("Cannot connect with MQTT Broker")
                        MQTTCount += 1
                except Exception as e:
                    log.logger.error("Cannot connect with MQTT Broker. Exception raise: {}".format(e))
                    MQTTCount += 1
            prevWiFiState = WiFiState

        # If WiFi and client connected wait for messages
        if(WiFiState==1 and client!=None):
            # If client connected
            if mqttControl.clientConnected:
                #mqttControl.turnOff_TIMEOUT()
                client.loop(0.1)
            # Else try to reconnect every 30s
            elif(time()-mqttControl.actualTime>30):
                mqttControl.actualTime = time()
                try:
                    # Reconnect client
                    client = mqtt.Client()
                    client.on_connect = mqttControl.on_connect # Specify on_connect callback
                    client.on_message = mqttControl.on_message # Specify on_message callback
                    #client.on_publish = mqttControl.on_publish # Specify on_publish callback
                    client.on_disconnect = mqttControl.on_disconnect # Specify on_disconnect callback
                    # Connect to MQTT Broker Parameters (IP, Port, Seconds Alive)
                    if(client.connect(mqttControl.brokerIP, 1883, 60)==0):
                        mqttControl.clientConnected = True
                        MQTTCount = 0
                    else:
                        log.logger.warning("Cannot connect with MQTT Broker")
                        MQTTCount += 1
                except Exception as e:
                    log.logger.error("Cannot connect with MQTT Broker. Exception raise: {}".format(e))
                    MQTTCount += 1
            else: sleep(0.1) # Avoid HIGH CPU usage
        else: sleep(0.1) # Avoid HIGH CPU usage
    mainClose()

except Exception as e:
    log.logger.critical("Exception Raised", exc_info=True)
    raise e

finally:
    mainClose()
