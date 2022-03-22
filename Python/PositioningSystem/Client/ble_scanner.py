#!/usr/bin/env python3

import os
import sys
import json
import struct
from time import time, sleep
import paho.mqtt.client as mqtt
import paho.mqtt.publish as publish
from ctypes import (CDLL, get_errno)
from ctypes.util import find_library
from socket import (
    socket,
    AF_BLUETOOTH,
    SOCK_RAW,
    BTPROTO_HCI,
    SOL_HCI,
    HCI_FILTER,
)
sys.path.insert(0, './src/')
import utils
from beacon import Beacon
from logger import logger
from mqttCallback import mqttController

utils.runShellCommand("sudo hciconfig hci0 down")
utils.runShellCommand("sudo hciconfig hci0 up")

# Check if temp dir exists, if not then create it
if not os.path.exists('temp/'): os.makedirs('temp/')

if not os.geteuid() == 0:
    sys.exit("script only works as root")

btlib = find_library("bluetooth")
if not btlib:
    raise Exception(
        "Can't find required bluetooth libraries"
        " (need to install bluez)"
    )
bluez = CDLL(btlib, use_errno=True)

dev_id = bluez.hci_get_route(None)

sock = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI)
sock.bind((dev_id,))

err = bluez.hci_le_set_scan_parameters(sock.fileno(), 0, 0x10, 0x10, 0, 0, 1000);
if err < 0:
    raise Exception("Set scan parameters failed")
    # occurs when scanning is still enabled from previous call

# allows LE advertising events
hci_filter = struct.pack(
    "<IQH", 
    0x00000010, 
    0x4000000000000000, 
    0
)
sock.setsockopt(SOL_HCI, HCI_FILTER, hci_filter)

err = bluez.hci_le_set_scan_enable(
    sock.fileno(),
    1,  # 1 - turn on;  0 - turn off
    0, # 0-filtering disabled, 1-filter out duplicates
    1000  # timeout
)
if err < 0:
    errnum = get_errno()
    raise Exception("{} {}".format(
        errno.errorcode[errnum],
        os.strerror(errnum)
    ))

# Define config variables
with open("config.json") as f:
    data = json.load(f)
    bluetoothMac = data["bluetoothDevices"]
    ID = data["ID"]
    brokerIP = data["brokerIP"]

# Define logger
log = logger()

# Define beacons to track
myBeacons = []
for dev in bluetoothMac: myBeacons.append(Beacon(dev, bluetoothMac[dev], log.logger))

# Define mqtt callbacks
mqttControl = mqttController(ID, log)

# Define auxiliar variables
reportTime = time()

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
    data = sock.recv(1024)

    filterMac = ':'.join("{0:02x}".format(x) for x in data[12:6:-1])
    if filterMac in [dev.mac for dev in myBeacons]:
        for i in range(len(myBeacons)): myBeacons[i].decode(data)

    if time() - reportTime > 10: # Each 10 seconds reports devices position
        reportTime = time()
        mssg = []
        for i in range(len(myBeacons)):
            dist = 0
            # If beacon update timer is greater than 60 seconds, then it is considered as out of range
            if time() - myBeacons[i].updateTimer > 60: myBeacons[i].distance = 0
            if myBeacons[i].distance != 0: dist = myBeacons[i].distance
            mssg.append({"topic": "positioningSystem/{}/{}".format(ID, myBeacons[i].mac), "payload": str(dist)})
        publish.multiple(mssg, hostname=brokerIP)

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