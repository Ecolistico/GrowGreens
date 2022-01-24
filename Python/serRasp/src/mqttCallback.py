#!/usr/bin/env python3

# Import SDirectories
from time import time
import paho.mqtt.publish as publish

class mqttController:
    def __init__(self, logger, data):
        self.log = logger
        self.containerID = data["ID"]
        self.number = data["number"]
        self.brokerIP = data["IP"]
        self.log.info("Setting up serRasp..." )
        self.clientConnected = False
        self.actualTime = time()

    def sendLog(self, mssg, logType = 0):
        logTopic = "{}/serRasp{}/log".format(self.containerID, self.number)
        # Debug
        if(logType==0):
            self.log.info(mssg)
            mssg += ",debug"
        # Info
        elif(logType==1):
            self.log.info(mssg)
            mssg += ",info"
        # Warning
        elif(logType==2):
            self.log.warning(mssg)
            mssg += ",warning"
        # Error
        elif(logType==3):
            self.log.error(mssg)
            mssg += ",error"
        # Critical
        elif(logType==4):
            self.log.critical(mssg)
            mssg += ",critical"
        # Any other case
        else:
            self.log.info(mssg)
            mssg += ",debug"
            
        publish.single(logTopic, mssg, hostname = self.brokerIP)
        
    # On Connect Callback for MQTT
    def on_connect(self, client, userdata, flags, rc):
        Topic = "{}/serRasp{}".format(self.containerID, self.number)
        message = "MQTT"
        if(rc == 0):
            message += " Connection succesful"
            mssg = "serRasp connected"
            client.subscribe(Topic)
            self.sendLog(mssg, 1)
            self.log.info(message)
            self.log.info("Subscribed topic= {}".format(Topic))
        else:
            message += " Connection refused"
            if(rc == 1): message += " - incorrect protocol version"
            elif(rc == 2): message += " - invalid client identifier"
            elif(rc == 3): message += " - server unavailable"
            elif(rc == 4): message += " - bad username or password"
            elif(rc == 5): message += " - not authorised"
            else: message += " - currently unused"
            self.log.error(message)

    # On Message Callback for MQTT
    def on_message(self, client, userdata, msg):        
        logTopic = "{}/serRasp{}/log".format(self.containerID, self.number) # Output Topic
        top = msg.topic # Input Topic
        message = msg.payload.decode("utf-8") # Input message
        
        if(message == "OnOut1"):
            self.grower.turnOn(self.grower.OUT1)
            self.sendLog("Out1 On")

        elif(message == "OnOut2"):
            self.grower.turnOn(self.grower.OUT2)
            self.sendLog("Out2 On")
        
        elif(message == "whatIsMyIP"):
            mssg = "IP={}".format(self.grower.whatIsMyIP())
            self.sendLog(mssg, 1)
            
        elif(message == "cozirData"):
            if(self.grower.coz != None):
                hum, temp, co2 = self.grower.coz.getData()
                mssg = "cozir,{},{},{}".format(hum, temp, co2)
                self.sendLog(mssg)
            else: self.sendLog("Cozir disconnected: ignore data request", 3)
        
    def on_publish(self, client, userdata, mid):
        self.log.info("Message delivered")
        
    def on_disconnect(self, client, userdata, msg):
        self.log.warning("Client MQTT Disconnected")
        self.clientConnected = False
        self.actualTime = time()
        