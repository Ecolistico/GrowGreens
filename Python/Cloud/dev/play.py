from streamServer_copy import *
from mqttClient import *
import time

stream = streamServer()
stream.serverListen()
mqtt = mqttClient()
# Initialize MQTT connection
mqtt.mqttInit()
counter = 0
timer = time.time()

def end():     
    stream.connection.close()
    stream.sock.close()
    
while True:
    mqtt.client.loop()
    stream.streaming()
    
    if (time.time() - timer > 3):
        timer = time.time()
        if counter == 0:
            counter += 1
            publish.single(mqtt.pub,
                        'begin',
                        hostname=mqtt.server_ip)
        elif counter>0 and counter <11 and :
            counter += 1
            publish.single(mqtt.pub,
                        'takePicture',
                        hostname=mqtt.server_ip)
        elif counter==11:
            counter += 1
            publish.single(mqtt.pub,
                        'close',
                        hostname=mqtt.server_ip)
        else:
            end()
            brea