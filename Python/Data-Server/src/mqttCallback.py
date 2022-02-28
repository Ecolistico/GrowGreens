#!/usr/bin/env python3

# Import Directories
from time import time
from sysDataServer import getIPaddr
import paho.mqtt.publish as publish

class mqttController:
    def __init__(self, logger, data, streamer):
        # Logging
        self.log = logger
        self.log.info("Setting up Data Server..." )
        # Data and MQTT variables
        self.containerID = data["ID"]
        self.number = data["number"]
        self.brokerIP = data["IP"]
        self.actualTime = time()
        # Stream object
        self.streamer = streamer
        self.clientConnected = False
        # Routine variables
        self.inRoutine = 0
        self.routineStarted = False
        self.masterReady = False
        self.ambientalReady = False
        self.phenotypeReady = False
        self.lightsReady = False
        self.streamReady = False

    def sendLog(self, mssg, logType = 0):
        logTopic = "{}/DataServer{}/log".format(self.containerID, self.number)
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
        
    def isRoutineReady(self):
        if self.masterReady and self.ambientalReady and self.phenotypeReady:
            # Prepare all devices
            msgs = [{"topic": "{}/Grower{}".format(self.containerID, self.inRoutine), "payload": "OnOut1"},
                    {"topic": "{}/Grower{}".format(self.containerID, self.inRoutine), "payload": "OnOut2"},
                    {"topic": "{}/Phenotype".format(self.containerID), "payload": "streamIP,{}".format(getIPaddr())}]
            publish.multiple(msgs, hostname = self.brokerIP)
            self.streamer.openSocket()

    def askConfirmation(self):
        if self.lightsReady and self.streamReady: self.log.info("All devices are ready to start routine. Please confirm if you want to start it.")

    # On Connect Callback for MQTT
    def on_connect(self, client, userdata, flags, rc):
        Topic = "{}/DataServer{}".format(self.containerID, self.number)
        message = "MQTT"
        if(rc == 0):
            message += " Connection succesful"
            mssg = "Data Server connected"
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
        logTopic = "{}/DataServer{}/log".format(self.containerID, self.number) # Output Topic
        top = msg.topic # Input Topic
        message = msg.payload.decode("utf-8") # Input message
        
        # Changes messages and communication
        if(message.startswith("StartRoutine")):
            if self.inRoutine == 0:
                self.inRoutine = int(message.split(",")[1])
                if self.inRoutine>0 and self.inRoutine<=8:
                    # Ask to all devices if they are ready
                    msgs = [{"topic": "{}/Master".format(self.containerID), "payload": message},
                            {"topic": "{}/Grower{}".format(self.containerID, self.inRoutine), "payload": message},
                            {"topic": "{}/Phenotype".format(self.containerID), "payload": message}]
                    publish.multiple(msgs, hostname = self.brokerIP)
                    self.sendLog("Checking status to start routine in floor {}".format(self.inRoutine), 1)
                else:
                    self.inRoutine = 0
                    publish.single("{}/Master".format(self.containerID), "unblockMotors", hostname = self.brokerIP)
                    self.sendLog("Invalid floor number to start the routine", 3)
            elif self.inRoutine == int(message.split(",")[1]):
                if self.masterReady and self.ambientalReady and self.phenotypeReady and self.lightsReady and self.streamReady: 
                    publish.single("{}/Master".format(self.containerID), message, hostname = self.brokerIP)
                    self.routineStarted = True

        elif(message.startswith("MasterReady")):
            # Master is ready when motors are ready and blocked
            self.masterReady = True
            self.isRoutineReady()
            
        elif(message.startswith("AmbientalReady")):
            # Ambiental is ready when detects the Ambiental module and 
            # Ambiental module could talk with Phenotype module via Bluetooth
            self.ambientalReady = True
            self.isRoutineReady()
            
        elif(message.startswith("PhenotypeReady")):
            # Phenotype is ready when detects the Phenotype module and
            # Phenotype module could talk with Ambiental module via Bluetooth
            self.phenotypeReady = True
            self.isRoutineReady()
            
        elif(message.startswith("LightsReady")):
            # Light are ready when Ambiental module turns on both of them
            self.lightsReady = True
            self.askConfirmation()

        elif(message.startswith("StreamReady")):
            # Stream is ready when Phenotype module stablish communication with Data Server
            self.streamReady = True
            self.askConfirmation()

        elif(message.startswith("TakePicture")):
            if self.streamer.isStreaming:
                self.streamer.savePicture = True
                self.sendLog("Taking picture number {}".format(self.streamer.captures), 0)
            else: self.sendLog("Cannot take a picture if Streaming is not working", 3)

        elif(message.startswith("RoutineFinished")):
            self.inRoutine = 0
            self.routineStarted = False
            self.masterReady = False
            self.ambientalReady = False
            self.phenotypeReady = False
            self.lightsReady = False
            self.streamReady = False
            # Send messages to turn off lights and streaming
            
        # This is usefull for debugging without making a routine
        elif(message.startswith("OpenStreaming")):
            floor = int(message.split(",")[1])
            topic = "{}/Grower{}".format(self.containerID, floor)
            mssg = "OpenStreaming"
            publish.single(topic, mssg, hostname = self.brokerIP)
            self.streamer.openSocket()
            self.sendLog("Starting streaming in floor {}".format(floor), 1)
        
        """""
        elif(message == "whatIsMyIP"):
            mssg = "IP={}".format(self.grower.whatIsMyIP())
            self.sendLog(mssg, 1)
            
        elif(message == "cozirData"):
            pass
        
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
        