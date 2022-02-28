#!/usr/bin/env python3

# Import Directories
from time import time
import paho.mqtt.publish as publish

class mqttController:
    def __init__(self, data, logger = None, stream = None):
        self.log = logger
        self.containerID = data["ID"]
        self.brokerIP = data["IP"]
        self.log.info("Setting up Tucan..." )
        self.clientConnected = False
        self.actualTime = time()
        self.stream = stream

    def sendLog(self, mssg, logType = 0):
        logTopic = "{}/Tucan/log".format(self.containerID)
        # Debug
        if(logType==0):
            if self.log is not None: self.log.info(mssg)
            else: print(mssg)
            mssg += ",debug"
        # Info
        elif(logType==1):
            if self.log is not None: self.log.info(mssg)
            else: print(mssg)
            mssg += ",info"
        # Warning
        elif(logType==2):
            if self.log is not None: self.log.warning(mssg)
            else: print(mssg)
            mssg += ",warning"
        # Error
        elif(logType==3):
            if self.log is not None: self.log.error(mssg)
            else: print(mssg)
            mssg += ",error"
        # Critical
        elif(logType==4):
            if self.log is not None: self.log.critical(mssg)
            else: print(mssg)
            mssg += ",critical"
        # Any other case
        else:
            if self.log is not None: self.log.info(mssg)
            else: print(mssg)
            mssg += ",debug"
            
        publish.single(logTopic, mssg, hostname = self.brokerIP)
        
    # On Connect Callback for MQTT
    def on_connect(self, client, userdata, flags, rc):
        Topic = "{}/Tucan".format(self.containerID)
        message = "MQTT"
        if(rc == 0):
            message += " Connection succesful"
            mssg = "Tucan connected"
            client.subscribe(Topic)
            self.sendLog(mssg, 1)
            if self.log is not None:
                self.log.info(message)
                self.log.info("Subscribed topic= {}".format(Topic))
            else:
                print(message)
                print("Subscribed topic= {}".format(Topic))
        else:
            message += " Connection refused"
            if(rc == 1): message += " - incorrect protocol version"
            elif(rc == 2): message += " - invalid client identifier"
            elif(rc == 3): message += " - server unavailable"
            elif(rc == 4): message += " - bad username or password"
            elif(rc == 5): message += " - not authorised"
            else: message += " - currently unused"
            if self.log is not None: self.log.error(message)
            else: print(message)

    # On Message Callback for MQTT
    def on_message(self, client, userdata, msg):        
        logTopic = "{}/Tucan/log".format(self.containerID) # Output Topic
        top = msg.topic # Input Topic
        message = msg.payload.decode("utf-8") # Input message
        
        # Change messages from the ones needed to be changed

        if(message.startswith("startStreaming")):
            self.sendLog('Opening socket to start streaming')
            mssgSplit = message.split(",")
            if len(mssgSplit) >= 3:
                if self.stream != None:
                    self.stream.clientConnect(mssgSplit[1], int(mssgSplit[2]))
                    publish.single("{}/cloud".format(self.containerID), "Ready to go!", hostname=self.brokerIP)
                else: 
                    publish.single("{}/cloud".format(self.containerID), "Stream not configured", hostname=self.brokerIP) 
            else: self.sendLog('startStreaming parameters are wrong', 3)
                
        elif (message.startswith("takePicture")):
            self.sendLog('Taking picture...') 
            if self.stream != None:
                self.stream.captureStreaming()
            else: self.sendLog("Stream not configured")
            
        elif (message.startswith("endStreaming")):
            self.sendLog('Closing socket') 
            if self.stream != None:
                self.stream.endStreaming()
            else: self.sendLog("Stream not configured")

        """""
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
        """""
        
    def on_publish(self, client, userdata, mid):
        self.log.info("Message delivered")
        
    def on_disconnect(self, client, userdata, msg):
        self.log.warning("Client MQTT Disconnected")
        self.clientConnected = False
        self.actualTime = time()
        