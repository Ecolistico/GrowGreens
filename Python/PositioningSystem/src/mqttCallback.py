#!/usr/bin/env python3

# Import directories
from time import time
import paho.mqtt.publish as publish

class mqttController:
    def __init__(self, ID, logger = None):
        # Define loggers
        self.log = logger.logger
        
        # ID system
        self.ID = ID

        # Define aux variables
        self.clientConnected = False
        self.actualTime = time()
        
    def Msg2Log(self, mssg):
        if self.log != None:
            if(mssg.split(",")[1]=="debug"): self.log.debug(mssg.split(",")[0])
            elif(mssg.endswith(",debug")): self.log.debug(mssg.replace(",debug", ""))
            elif(mssg.split(",")[1]=="info"): self.log.info(mssg.split(",")[0])
            elif(mssg.endswith(",info")): self.log.info(mssg.replace(",info", ""))
            elif(mssg.split(",")[1]=="warning"): self.log.warning(mssg.split(",")[0])
            elif(mssg.endswith(",warning")): self.log.warning(mssg.replace(",warning", ""))
            elif(mssg.split(",")[1]=="error"): self.log.error(mssg.split(",")[0])
            elif(mssg.endswith(",error")): self.log.error(mssg.replace(",error", ""))
            elif(mssg.split(",")[1]=="critical"): self.log.critical(mssg.split(",")[0])
            elif(mssg.endswith(",critical")): self.log.critical(mssg.replace(",critical", ""))
            else: self.log.debug(mssg)
        else: print(mssg)
    
    # Callback fires when conected to MQTT broker.
    def on_connect(self, client, userdata, flags, rc):
        Topic = "positioningSystem/{}/#".format(self.ID)
        message = "MQTT"
        if(rc == 0):
            message += " Connection succesful"
            mssg = "Master connected"
            client.subscribe(Topic)
            self.log.info(message)
            self.log.debug("Subscribed topic= {}".format(Topic))
        else:
            message += " Connection refused"
            if(rc == 1): message += " - incorrect protocol version"
            elif(rc == 2): message += " - invalid client identifier"
            elif(rc == 3): message += " - server unavailable"
            elif(rc == 4): message += " - bad username or password"
            elif(rc == 5): message += " - not authorised"
            else: message += " - currently unused"
            self.log.warning(message)

    # Callback fires when a published message is received.
    def on_message(self, client, userdata, msg):
        top = msg.topic # Input Topic
        message = msg.payload.decode("utf-8") # Input message
        device = top.split("/")[1] # Device where come
        
        # Get MQTT logs from all the devices
        if(top.endswith("updateDevices")):
            pass
        
        # Get MQTT errors from ESP32´s
        elif(top.endswith("deleteDevice")):
            pass
        
        elif(message.startswith('updateInfo')):
            pass

        elif(message.startswith('updateDevice')):
            pass
                
 
    def on_publish(self, client, userdata, mid):
        self.log.debug("MQTT Message delivered")

    def on_disconnect(self, client, userdata, rc):
        self.log.warning("Client MQTT Disconnected")
        self.clientConnected = False
        self.actualTime = time()