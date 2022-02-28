#!/usr/bin/env python3

# Import directories
from streamServer import *
from mqttClient import *


mqtt = mqttClient()
# Initialize MQTT connection
mqtt.mqttInit()

publish.single(mqtt.pub,
                'begin',
                hostname=mqtt.server_ip)

publish.single(mqtt.pub,
                'activate cameras',
                hostname=mqtt.server_ip)

# Start listening for stream
cloud = streamServer()
cloud.serverListen()

try:
    cloud.streaming()
    while True:
        if mqtt.message == 'closing mqtt':
            break
        mqtt.client.loop()

finally:       
    cloud.connection.close()
    cloud.sock.close()


publish.single(mqtt.pub,
                'close',
                hostname=mqtt.server_ip)



"""
# Streaming
    
cloud = streamServer()
cloud.serverListen()

try:
    cloud.streaming()

finally:       
    cloud.connection.close()
    cloud.sock.close()
"""

