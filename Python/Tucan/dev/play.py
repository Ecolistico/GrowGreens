#!/usr/bin/env python3

# Import Directories
from streamClient_copy import *
from mqttClient import *

# Data pre-defined
data = {"host": "192.168.6.87", "port": "8001", "ip": "192.168.6.10", "id": "23-009-006"}
# Host can potentially change once the program is open is better to define it inside mqtt instance

stream = streamClient()
stream.cameraSetup()
mqtt = mqttClient(data, stream)

while True:
    mqtt.client.loop()
        
