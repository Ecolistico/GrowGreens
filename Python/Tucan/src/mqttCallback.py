#!/usr/bin/env python3

# Import Directories
from time import time
import paho.mqtt.publish as publish

class mqttController:
    def __init__(self, data, logger = None, stream = None):
        # Log variables
        self.log = logger
        self.containerID = data["ID"]
        self.brokerIP = data["IP"]
        self.log.info("Setting up Tucan..." )

        # MQTT Variables
        self.clientConnected = False
        self.actualTime = time()

        # Streaming variables
        self.pictureCounter = 1
        self.stream = stream
        self.inStream = False
        self.streamTimer = time()

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
    
    # Function to end streaming
    def endStreaming(self):
        self.sendLog('Closing socket') 
        self.pictureCounter = 1
        self.inStream = False
        if self.stream != None:
            self.stream.endStreaming()
        else: self.sendLog("Stream not configured")

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
        
        if(message.startswith("startStreaming")):
            self.sendLog('Opening socket to start streaming')
            mssgSplit = message.split(",")
            if len(mssgSplit) >= 3:
                if self.stream != None:
                    self.stream.clientConnect(mssgSplit[1], int(mssgSplit[2]))
                    publish.single("{}/Cloud".format(self.containerID), "StreamReady", hostname=self.brokerIP)
                    self.inStream = True
                    self.streamTimer = time()
                else: 
                    publish.single("{}/Cloud".format(self.containerID), "StreamNotReady", hostname=self.brokerIP) 
            else: self.sendLog('startStreaming parameters are wrong', 3)
                
        elif (message.startswith("takePicture")): 
            if self.stream != None:
                self.sendLog('Taking picture {}'.format(self.pictureCounter))
                self.pictureCounter += 1
                self.stream.captureStreaming()
                self.streamTimer = time()
            else: self.sendLog("Stream not configured")
            
        elif (message.startswith("endStreaming")): self.endStreaming()
        
        elif (message.startswith("StartRoutine")):
            # Here goes the bluetooth validation with Grower for now just ignore it
            publish.single("{}/Cloud".format(self.containerID), "TucanReady", hostname = self.brokerIP)
        
    def on_publish(self, client, userdata, mid):
        self.log.info("Message delivered")
        
    def on_disconnect(self, client, userdata, msg):
        self.log.warning("Client MQTT Disconnected")
        self.clientConnected = False
        self.actualTime = time()
        