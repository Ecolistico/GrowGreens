#!/usr/bin/env python3

# Import Directories
from streamClient import *
from mqttClient import *

# Initialize MQTT connection
mqtt = mqttClient()
mqtt.mqttInit()

tucan = streamClient()
tucan.clientConnect()

try:
    tucan.streaming()
    while True:
        if mqtt.messg == 'close':
            print('closing...')
            publish.single(mqtt.pub,
                           "closing mqtt",
                           hostname=mqtt.server_ip)
            break
        
        elif mqtt.messg == 'activate cameras':
            tucan.cameraSetup()
            
    
    mqtt.client.loop()


finally:
    tucan.connection.close()
    tucan.sock.close()
    
"""    
try:
    tucan.streaming()
    
finally:
    tucan.connection.close()
    tucan.sock.close()
"""