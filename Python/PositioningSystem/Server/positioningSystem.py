#!/usr/bin/env python3
import os
import sys
import json
from time import time, sleep
import paho.mqtt.client as mqtt
import paho.mqtt.publish as publish
sys.path.insert(0, './src/')
from logger import logger
from client import ClientManager
from mqttCallback import mqttController

# Check if temp dir exists, if not then create it
if not os.path.exists('temp/'): os.makedirs('temp/')

# Define config variables
with open("config.json") as f:
    data = json.load(f)
    bluetoothDevices = data["bluetoothDevices"]
    clients = data["clients"]
    brokerIP = data["brokerIP"]

# Define logger
log = logger()

# Define mqtt callbacks
mqttControl = mqttController(log)

# Define Clients
myClients = ClientManager(clients)

# Define Bluetooth devices

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

while True:
     # If mqtt connected check for messages
    if mqttControl.clientConnected: client.loop(0.1)
    else:
        sleep(0.1)
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
                if(client.connect(data["brokerIP"], 1883, 60)==0): mqttControl.clientConnected = True
                else: log.logger.error("Cannot connect with MQTT Broker")

            except Exception as e: log.logger.error("Cannot connect with MQTT Broker [{}]".format(e))