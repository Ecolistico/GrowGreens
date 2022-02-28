#!/usr/bin/env python3

# Import Directories
from streamClient_copy import *
from mqttClient import *

# Data pre-defined
data = {"host": "192.168.6.87", "port": "8001"}
# Host can potentially change once the program is open is better to define it
inside mqtt instance

stream = streamClient(data)
stream.cameraSetup()
mqtt = mqttClient(stream)
mqtt.mqttInit()

while True:
    mqtt.client.loop()
        
